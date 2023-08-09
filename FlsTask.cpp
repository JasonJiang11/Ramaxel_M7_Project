#include "stdafx.h"
#include "FlsTask.h"
#include "FlsStringStream.h"

//Handle to the Global FLS Task Manager
FlsTaskManager* FTM;
//thread_local FlsTaskManager::StreamInfo* p_FlsTMgr;
#ifdef PRE_BETA4_STREAMID
BOOL flsTasksCallback(DWORD128* data, DWORD total_components, void* user);
#else
BOOL flsTasksCallback(NXTFlsTargetStreamNumber stream_id, DWORD128* data, DWORD total_components, void* cb_data);
#endif
BOOL flsTasksCallback_startup(NXTFlsTargetStreamNumber nstream_id);
void flsTasksCallback_shutdown(NXTFlsTargetStreamNumber stream_id, NXTFlsStreamShutdownReason nreason);

thread_local static std::vector<s_queue_history> qpop_history;
thread_local uint64_t qpop_cnt;
thread_local uint64_t qnpop_cnt;

void output_qpop_history(int stream_id)
{
	std::stringstream ss;
	ss << "Queue Pop Count = " << std::dec << qpop_cnt << ",  Queue No Pop Count = " << qnpop_cnt;
	for (const auto& queues : qpop_history) {
		ss << "Fstrm" << (int)stream_id << ": Queue Pop - InStructStreamID=" << std::dec << queues.stream_id << ", TaskID=" << queues.tid << ", ptr=" << std::hex << queues.ptr << "\n";
	}
	output("%s", ss.str().c_str());
}

std::string get_pattern_state()
{
	PatternState ps = pattern_state();
	std::string ps_str;

	switch (ps) {
	case PATTERN_RUNNING: ps_str = "PATTERN_RUNNING"; break;
	case PATTERN_PAUSED: ps_str = "PATTERN_PAUSED"; break;
	case PATTERN_DONE: ps_str = "PATTERN_DONE"; break;
	case PATTERN_STOPPED: ps_str = "PATTERN_STOPPED"; break;
	case PATTERN_STEPPING: ps_str = "PATTERN_STEPPING"; break;
	}
	return ps_str;
}

void FlsTask::queue()
{
	//output("FStrm%d: %s: ", g_stream_id + 1, __func__);
	//Get this task ready to send to the FLS streams
	{
		std::lock_guard<std::mutex> lck(mtx);
		for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
		{
			TaskState[int(stream_id)].state = FlsTaskState::queued;
		}
		cv.notify_all();
	}

	//Push this object into the FLS Task Queue to be picked up by the callback for processing
	//FlsTaskManager &FlsTaskManager = FlsTaskManager.getInstance();
	//FlsTaskManager.queue_push(this);
	FTM->queue_push(this);
}

void FlsTask::fcb_take_ownership(int stream_id, DWORD PatternID, DWORD FlsTaskID)
{
	//output("FStrm%d: %s: ", stream_id + 1, __func__);
	//Block until main thread changes state to ready

	//Find this stream's data
	sFlsTaskState* p_local_stream = &TaskState[stream_id]; // cache pointer to local stream state

	auto msgtime = std::chrono::steady_clock::now();
	std::unique_lock<std::mutex> lck(mtx);
	while (p_local_stream->state != FlsTaskState::queued) {
		auto now = std::chrono::steady_clock::now();
		if (now - msgtime > std::chrono::seconds(10)) {
			warning("fcb_take_ownership: callback is waiting for seq/bin thread to make object state=ready.");
			msgtime = now;
		}
		cv.wait_for(lck, std::chrono::seconds(1)); //wait up to 1 second for any change of state
	}
	p_local_stream->state = FlsTaskState::callback;
	p_local_stream->PatternID = PatternID;
	p_local_stream->FlsTaskID = FlsTaskID;
	cv.notify_all();
}

//inline void FlsTask::fcb_add(COMP128 &comp)
//{
//	if (state == callback) {
//		//derived implementation should do work here
//	}
//	else {
//		state = error;
//	}
//	return;
//}
//
//inline void FlsTask::fcb_add_block(DWORD128* data, int length)
//{
//	if (state == callback) {
//		for (int i = 0; i < length; i++)
//		{
//			//derived implementation should do work here
//		}
//	}
//	else {
//		state = error;
//	}
//	return;
//}


void FlsTask::fcb_release_ownership(int stream_id, FlsTaskState release_state)
{
	//output("FStrm%d: %s: ", stream_id + 1, __func__);
	//Block until main thread changes state to filled
	std::lock_guard<std::mutex> lck(mtx);
	//Find this stream's data
	sFlsTaskState* p_local_stream = &TaskState[stream_id]; // cache pointer to local stream state
	p_local_stream->state = release_state;
	p_local_stream = NULL;
	cv.notify_all();
}

//This is polling the local Task's state. This is NOT the streams state.
bool FlsTask::any_streams_running() {
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
	{
		if (TaskState[int(stream_id)].state == FlsTaskState::callback || TaskState[int(stream_id)].state == FlsTaskState::queued) { return true; }
	}
	return false;
}

void FlsTask::wait_for_taskstop()
{
	//Block until fls callback changes state to filled
	auto ntime = std::chrono::steady_clock::now() + std::chrono::seconds(10);
	std::unique_lock<std::mutex> lck(mtx);

	while (any_streams_running()) {
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed = now - ntime;

		if (elapsed > std::chrono::seconds(2)) {
			std::stringstream ss;
			ss << "<color=red>wait_until_filled: seq / bin thread is waiting for fls callbacks to release current FlsTask Object.</color> Debug data : \n";
			ss << "PatternState=" << get_pattern_state() << "\n";
			get_task_state(ss);
			FTM->get_ftm_status(ss);
			get_fls_status(ss);
			output("%s\\", ss.str().c_str());
			ntime = now;
		}

		if (should_stop()) {
			warning("Exiting wait_for_taskstop() due to Stop button being pressed.");
		}
		cv.wait_for(lck, std::chrono::milliseconds(250));
	}
}

void FlsTask::set_taskid(DWORD id)
{
	seqbin_FlsTaskID = id;
}

std::string FlsTask::taskstate_to_string(FlsTaskState state)
{
	switch (state)
	{
	case FlsTaskState::callback: return "callback";
	case FlsTaskState::error: return "error";
	case FlsTaskState::filled: return "filled";
	case FlsTaskState::init: return "init";
	case FlsTaskState::queued: return "queued";
	default: return "unknown";
	}
}

FlsTask::FlsTaskState FlsTask::stream_state(enFlsTargetStreamNumberMag7 stream_id)
{
	std::unique_lock<std::mutex> lck(mtx);
	return TaskState[(int)stream_id].state;
}

bool FlsTask::all_streams_ok()
{
	std::unique_lock<std::mutex> lck(mtx);
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
		if (TaskState[(int)stream_id].state != FlsTaskState::filled) return false;
	}
	return true;
}

bool FlsTask::any_streams_error()
{
	std::unique_lock<std::mutex> lck(mtx);
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
		if (TaskState[(int)stream_id].state != FlsTaskState::filled) return false;
	}
	return true;

}

//This is outputting the Task's state for each stream
void FlsTask::get_task_state(std::stringstream& ss)
{
	//std::unique_lock<std::mutex> lck(mtx);
	ss << "<i>Current FLS Task's State:</i> ";
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
		ss << "<b>[Stream" + std::to_string((int)stream_id) + "</b>: state = " + taskstate_to_string(TaskState[(int)stream_id].state);
		ss << ", taskID=" + std::to_string(this->TaskState[(int)stream_id].FlsTaskID);
		ss << ", patID=" + std::to_string(this->TaskState[(int)stream_id].PatternID);
		ss << "<b>]</b>   ";
	}
	ss << "\n";
}

void FlsTask::output_task_state()
{
	std::stringstream ss;
	get_task_state(ss);
	output("%s\\", ss.str().c_str());
}

/*
typedef concurrency::concurrent_queue < FlsTask* > FlsTaskQueueType;
FlsTaskQueueType FlsTaskQueue;

FlsTask* receive_fls_task(DWORD& icomp, DWORD& PatID, DWORD& TaskID)
{
	auto ntime = std::chrono::steady_clock::now();
	FlsTask* FlsTask = NULL;
	while (FlsTask == NULL)
	{
		if (FlsTaskQueue.try_pop(FlsTask)) { //Got a new task, take ownership of the task
			FlsTask->fcb_take_ownership(PatID, TaskID);
			// We should exit the while loop from here as curFlsTask is active
			return FlsTask;
		}
		else {                                  //No task is queued. Begin to wait.
			FlsTask = NULL;
			if (std::chrono::steady_clock::now() - ntime > std::chrono::seconds(10))
			{
				//output a warning if we are stuck waiting for a task for a long time
				q_output("flsTasksCallback: Waiting for a FLS Task to be queued. Pattern ID = %d, Task ID = %d", PatID, TaskID);
				q_flush();
				ntime = std::chrono::steady_clock::now();
			}
#undef Yield
			concurrency::Context::Yield(); //make sure seq/bin thread has time to queue a task
		}
	}
	return NULL;
}
*/

void FlsTaskManager::queue_push(FlsTask* FlsTask)
{
	//Queue task to all streams
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
		auto& ftmgr = FlsTMgr[int(stream_id)];

		ftmgr.FlsTaskQueue.push(FlsTask);
		//GWB Debug
		ftmgr.qpush_cnt++;
#ifdef TASKMGR_DEBUG_LOGGING
		ftmgr.qpush_history.emplace_back((int)stream_id, FlsTask->seqbin_FlsTaskID, (void*)FlsTask, std::chrono::high_resolution_clock::now());
#endif
	}
}

//Pop from FLS Task Queue
bool FlsTaskManager::fcb_queue_pop(int stream_id, FlsTask*& FlsTask, DWORD& PatID, DWORD& TaskID)
{
	//Pull a task from the local queue
	if (FlsTask != NULL) {
		warning("%s: A FlsTask was active when a new one was being pulled from the queue. Pulling a new task and continuing... but this shouldn't have happened.", __func__);
		FlsTask = NULL;
	}

	//Get the next task from the queue. Give a wanring message if we have been waiting for a new task for too long
	auto btime = std::chrono::steady_clock::now();
	while (FlsTask == NULL) {
		if (FlsTMgr[stream_id].thread_being_interrupted) { return false; }  //This needs to be added to each while loop
		if (FlsTMgr[stream_id].FlsTaskQueue.try_pop(FlsTask)) {
			//A task was waiting in the queue for us. Activate it.
			qpop_cnt++;
			FlsTask->fcb_take_ownership(stream_id, PatID, TaskID);
#ifdef TASKMGR_DEBUG_LOGGING
			qpop_history.emplace_back(stream_id, FlsTask->TaskState[stream_id].FlsTaskID, FlsTask, std::chrono::high_resolution_clock::now());
#endif
		}
		else {
			//We did not see a task. Yield the thread and then try again. Warn user if we seem stuck.
			qnpop_cnt++;
			auto ntime = std::chrono::steady_clock::now();
			if ((ntime - btime) > std::chrono::seconds(10))
			{
				output("FStrm%d %s: Waiting for FLS task to be queued. PatternID=%d, TaskID=%d", stream_id + 1, __func__, PatID, TaskID);
#ifdef TASKMGR_DEBUG_LOGGING
				output_qpop_history(stream_id);
#endif
				q_flush();
				btime = ntime;
			}
#undef Yield
			concurrency::Context::Yield();	//TODO make this a condition_variable
		}
	}
	return true;
}

//Check if the User type
inline bool FlsTaskManager::is_task_framing_component(COMP128* comp, const std::set<DWORD>& types) {
	if (comp->id.cid == CID_USER1) {
		DWORD type = user_component_type(comp);
		return(types.find(type) != types.end());  //would like to use .compare(), but need C++20
	}
	return false;
}

//Check all the framing component types
bool FlsTaskManager::is_framing_component(COMP128* comp) {
	if (comp->id.cid == CID_FLUSH || comp->id.cid == CID_HALTED) { return true; }
	if (comp->id.cid == CID_USER1) {
#ifndef USE_SHARED_USER1
		//This will make User Tasks process faster as they won't have to look for actual prefix's, but this means that every USER1 will be a framing component.
		return true;
#else
		//WARNING: There is a possibility that USER1 counts in the FLS Task Manager may be wrong. This section of code would have to increment it's count if the USER1 was NOT a framing component
		return(is_task_framing_component(comp, task_prefix_types) ||
			is_task_framing_component(comp, task_suffix_types) ||
			is_task_framing_component(comp, pattern_prefix_types) ||
			is_task_framing_component(comp, pattern_suffix_types));
#endif
	}
	return false;
}

inline bool FlsTaskManager::is_init_component(COMP128* comp) {
	if (comp->id.cid == init_component.id.cid &&
		comp->user.forced == init_component.user.forced &&
		comp->user.ureg == init_component.user.ureg) {
		return true;
	}
	else { return false; }
}

inline bool FlsTaskManager::is_shutdown_component(COMP128* comp) {
	if (comp->id.cid == shutdown_component.id.cid) {
		if (comp->user.forced && comp->user.ureg == shutdown_component.user.ureg) { return true; }
		if ((!(comp->user.forced)) && (comp->user.xaddr == 0xCAFE) && (comp->user.yaddr == 0xBAD)) { return true; }
	}
	return false;
}

void get_fls_status(std::stringstream& ss)
{
	ss << "<i>Fls Status:</i> ";
	auto gTF = [&](bool i) {return (i ? "T" : "F"); };
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
	{
		FLS_STATUS status;
		fls_status_get(&status, stream_id);//GWB DEBUG
		ss << "<b>[Stream" + std::to_string((int)stream_id) << "</b>: ";
		ss << "patStrt,patDone,cmdPend,compList=" << gTF(status.pat_started) << gTF(status.pat_done) << gTF(status.cmd_pending) << gTF(status.component_in_list) << ", ";
		ss << "Overflows(cmd,dram,event)=" << gTF(status.command_overflow) << gTF(status.dram_overflow) << gTF(status.event_overflow) << ", ";
		ss << "Comps(User1;Flush)=" << status.num_user1_cmds << ";" << status.num_flush_cmds;
		ss << "<b>]</b>   ";
#if 0
		fls_status_get(&status, stream_id);//GWB DEBUG
		CStr.AppendFormat("<b>FLS Status for Stream ID%d</b>\n", (int)stream_id);
		CStr.AppendFormat("Pat_started=%s, Pat_done=%s, Cmd_pending=%s, Component_in_list=%s, ",
			status.pat_started ? "TRUE" : "FALSE", status.pat_done ? "TRUE" : "FALSE",
			status.cmd_pending ? "TRUE" : "FALSE", status.component_in_list ? "TRUE" : "FALSE");
		CStr.AppendFormat("Num_user1_cmd=%d, Num_flush_cmd=%d, Overflows (cmd,dram,event)=(%d,%d,%d)\n",
			status.num_user1_cmds, status.num_flush_cmds,
			status.command_overflow, status.dram_overflow, status.event_overflow);
#endif
	}
	q_flush();
}

void output_fls_status()
{
	std::stringstream ss;
	get_fls_status(ss);
	output("%s\\", ss.str().c_str());
	q_flush();
}

void FlsTaskManager::config(FlsConfig* thisFlsConfig)
{
	//Save the active config for use by FlsTasks
	ActiveFlsConfig = thisFlsConfig;

	//Configure the shutdown flush component
	shutdown_component.user.cid = CID_FLUSH;
	shutdown_component.user.ureg = shutdown_ureg_id;
	shutdown_component.user.forced = true;

	//Use the FLS Task Manager's callback 
	fls_config_struct fls_config;
	thisFlsConfig->getConfig(fls_config);
	fls_config.cb_func = flsTasksCallback;
	fls_config.cb_data = this;
	fls_config.on_fls_stream_startup = flsTasksCallback_startup;
	fls_config.on_fls_stream_shutdown = flsTasksCallback_shutdown;

	//Allocate per-stream FLS Manager data stores. The startup routine will use these.
	if (SimulationMode()) {
		warning("FlsTaskManager: Simulation Mode: Not configuring FLS.");
	}
	else {
		FlsTMgr.clear();
		int num_streams = fls_max_stream_count_get();
		FlsTMgr.resize(num_streams);

		//Configure FLS
		//This will also call the flsTasksCallback_startup() function on the FLS Thread so it can init itself
		fls_config_set(&fls_config);

		//Wait for all streams to start
		bool all_streams_running = false;
		while (!all_streams_running) {
			concurrency::Context::Yield();
			all_streams_running = true;
			for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
				StreamInfo::StreamState cur_state = FlsTMgr[(int)stream_id].state;
				if (cur_state != StreamInfo::StreamState::running) {
					all_streams_running = false;
				}
			}
		}

		//output_fls_status();
	}

}

void FlsTaskManager::update_stream_status()
{
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
		FLS_STATUS fs;
		fls_status_get(&fs, stream_id);
		if (fs.command_overflow || fs.dram_overflow || fs.event_overflow) {
			FlsTMgr[int(stream_id)].state = StreamInfo::StreamState::error;
		}
	}
}

bool FlsTaskManager::any_fls_streams_active()
{
	for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
	{
		if (FlsTMgr[int(stream_id)].state == StreamInfo::StreamState::running) { return true; }
	}
	return (false);
}

void FlsTaskManager::output_ftm_status(bool detailed_history)
{
	std::stringstream ss;
	get_ftm_status(ss, detailed_history);
	output("%s\\", ss.str().c_str());
}

void FlsTaskManager::get_ftm_status(std::stringstream& ss, bool detailed_history)
{
	if (detailed_history) {
		for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
		{
			ss << "<b>FTMgr Stream" + std::to_string((int)stream_id) + " Status: </b>";
			auto& ftmgr = this->FlsTMgr[(int)stream_id];
			ss << " num_user1=" << std::dec << (int)ftmgr.cb_num_user1;
			ss << ", num_flush=" << std::dec << (int)ftmgr.cb_num_flush;
			ss << ", state=";
			switch (ftmgr.state) {
			case FlsTaskManager::StreamInfo::StreamState::error:  ss << "<COLOR=RED>error</COLOR>"; break;
			case FlsTaskManager::StreamInfo::StreamState::running:  ss << "running"; break;
			case FlsTaskManager::StreamInfo::StreamState::shutdown: ss << "<COLOR=RED>shutdown</COLOR>"; break;
			default: ss << "unknown (should never happen)";
			}
			ss << "\n";
			for (const auto& prefix : ftmgr.framing_prefix_history) {
				ss << "Fstrm" << (int)stream_id << ": Prefix - ";
				fls_to_ss(ss, prefix.comp, false);
			}
			for (const auto& suffix : ftmgr.framing_suffix_history) {
				ss << "Fstrm" << (int)stream_id << ": Suffix - ";
				fls_to_ss(ss, suffix.comp, false);
			}
			for (const auto& queues : ftmgr.qpush_history) {
				ss << "Fstrm" << (int)stream_id << ": Queue Push - InStructStreamID=" << std::dec << queues.stream_id << ", TaskID=" << queues.tid << ", ptr=" << std::hex << queues.ptr << "\n";
			}
		}
	}
	else {
		ss << "<i>FTMgr Status:</i> ";
		for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
		{
			ss << "<b>[Stream" << (int)stream_id << "</b> ";
			auto& ftmgr = this->FlsTMgr[(int)stream_id];
			ss << " num_user1=" << std::dec << (int)ftmgr.cb_num_user1;
			ss << ", num_flush=" << std::dec << (int)ftmgr.cb_num_flush;
			ss << ", state=";
			switch (ftmgr.state) {
			case FlsTaskManager::StreamInfo::StreamState::error:  ss << "<COLOR=RED>error</COLOR>"; break;
			case FlsTaskManager::StreamInfo::StreamState::running:  ss << "running"; break;
			case FlsTaskManager::StreamInfo::StreamState::shutdown: ss << "<COLOR=RED>shutdown</COLOR>"; break;
			default: ss << "unknown (should never happen)";
			}
			ss << ", numcb=" << ftmgr.numcbs;
			ss << ", cbTime=" << (std::chrono::duration_cast<std::chrono::microseconds>(ftmgr.lastcb_time - ftmgr.init_time).count() / 1000.0) << "ms";
			ss << "<b>]</b>   ";
		}
		ss << "\n";
	}
}

enum class FlsShutDown { FLS_STREAM, FLS_FORCE_CMD_SET, PATTERN };
static FlsShutDown fls_shutdown_method = FlsShutDown::FLS_STREAM;
ONEOF_VARIABLE(uv_shutdown_method, "fls_stream: fls_stream, fls_force_cmd_set, pattern", "FLS Shutdown Method") {
	if (uv_shutdown_method == "fls_stream") { fls_shutdown_method = FlsShutDown::FLS_STREAM; }
	else if (uv_shutdown_method == "fls_force_cmd_set") { fls_shutdown_method = FlsShutDown::FLS_FORCE_CMD_SET; }
	else if (uv_shutdown_method == "pattern") { fls_shutdown_method = FlsShutDown::PATTERN; }
}

void FlsTaskManager::shutdown()
{
	//Ask FLS to shut itself down
	//This should trigger the registered shutdown functions. 
	//fls_config_struct fc;
	//fls_config_set(&fc);

	output("Shutting down FLS. FLS Status before shutdown.");
	output_fls_status();

	if (fls_shutdown_method == FlsShutDown::FLS_FORCE_CMD_SET) {
		output("Using fls_force_cmd_set to ask FLS to shutdown.");
		// Confirm that there are no outstanding patterns running
		bool poll_for_fls_idle = true;
		while (poll_for_fls_idle) {
			poll_for_fls_idle = false;
			Sleep(10);
			for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++) {
				FLS_STATUS fs;
				fls_status_get(&fs, stream_id);
				if (fs.cmd_pending || (!fs.pat_done)) {
					poll_for_fls_idle = true;
					break;
				}
			}
			//TODO Add polling loop message or timeout
		}
		output("Injecting FLS_FLUSH command");
		fls_force_cmd_set(FLS_FLUSH, DWORD(shutdown_component.user.ureg));
	}
	else if (fls_shutdown_method == FlsShutDown::FLS_STREAM) {
		output("User fls_stream to ask FLS to shutdown");
		fls_stream(NULL, NULL, 0, 0, 0);
	}
	else {
		output("Using PAT__FlsShutdown to ask FLS to shutdown.");
		funtest(PAT__FlsShutdown, PatStopCond::finish);
	}

	//Wait for shutdown signals from both streams
	auto msgtime = std::chrono::steady_clock::now();
	while (any_fls_streams_active()) {
		auto now = std::chrono::steady_clock::now();
		if (now - msgtime > std::chrono::seconds(2)) {
			warning("fls shutdown: Waiting for ALL FLS streams to shutdown");
			msgtime = now;
		}
		Sleep(50);
		update_stream_status();
	}


#ifdef TASKMGR_DEBUG_LOGGING
	output("FLS is Shutdown. HW stream status");
	output_fls_status();

	//Output status
	output("FlsTaskManager Status");
	FlsTaskManager::output_status(true);
#endif

}

void FlsTaskManager::fcb_received_startup(enFlsTargetStreamNumberMag7 nstream_id)
{
	FlsTMgr[int(nstream_id)].state = FlsTaskManager::StreamInfo::StreamState::running;
}

void FlsTaskManager::fcb_recevied_shutdown(enFlsTargetStreamNumberMag7 nstream_id)
{
	FlsTMgr[int(nstream_id)].FlsTaskQueue.clear();
	FlsTMgr[int(nstream_id)].state = StreamInfo::StreamState::shutdown;
}

void FlsTaskManager::debug_output_callback(const int stream_id, std::string err_msg, DWORD curComp)
{
	std::stringstream ss;
	ss << "<b>Callback Contents:</b>";
	for (unsigned int i = 0; i < FTM->FlsTMgr[(int)stream_id].lastcb_num_comps; i++)
	{
		if (i == curComp) { ss << "<B>"; }
		ss << "FStrm" << std::dec << stream_id + 1 << ";" << i << "(" << err_msg << "): ";
		if (i == curComp) { ss << "</B>"; }
		fls_to_ss(ss, *((COMP128*)(FTM->FlsTMgr[(int)stream_id].lastcb_p_data) + i), true);
	}
	output("%s\n---------------------", ss.str().c_str());
	q_flush();
	return;
}


BOOL_VARIABLE(uv_fls_output, false, "FLS Low Level Stream Output") {}
BOOL_VARIABLE(uv_fls_output_summarize_fails, true, "FLS Low Level Summarize Fails") {}

inline void output_fls_stream(int stream_id, DWORD128* start_comp, int total_comps)
{
	bool summarize_fails = uv_fls_output_summarize_fails;
	int num_fails_before_summarize = 3;
	if (total_comps > 500) { summarize_fails = true; } //automatically enable summarized fails for large callbacks
	if (uv_fls_output) {
		std::stringstream out_ss;
		int fail_comp_cnt = 0;
		int begin_fail_offset = -1;
		auto add_summarized_fails = [&out_ss, &fail_comp_cnt, &num_fails_before_summarize, &stream_id, &begin_fail_offset](int cur_idx) {
			if (fail_comp_cnt > num_fails_before_summarize) {
				out_ss << "FStrm" << std::dec << stream_id + 1 << ":[" << cur_idx << ":" << begin_fail_offset << "]  "
					<< (fail_comp_cnt - num_fails_before_summarize) << " Fail CIDs Suppressed" << std::endl;
				begin_fail_offset = -1;
			}
		};
		COMP128* comp = (COMP128*)(start_comp);
		for (int i = 0; i < total_comps; i++)
		{
			if (uv_fls_output_summarize_fails) {
				if ((comp + i)->id.cid == CID_FAIL || (comp + i)->id.cid == CID_FFAIL)
				{
					fail_comp_cnt++;
					if (fail_comp_cnt > num_fails_before_summarize) {
						if (begin_fail_offset == -1) {
							begin_fail_offset = i;
						}
						continue; //skip outputting fails more than summary limit
					}
				}
				else {
					add_summarized_fails(i);
					fail_comp_cnt = 0;
				}
			}
			out_ss << "FStrm" << std::dec << stream_id + 1 << ";" << i << ": ";
			fls_to_ss(out_ss, *(comp + i), true);
		}
		add_summarized_fails(total_comps - 1);
		output("%s", out_ss.str().c_str());
	}
	//*****FOR HW Training ***************************
	if (uv_Hwtraining) {
		COMP128* comp = (COMP128*)(start_comp);
		int xadd = 0x0;
		int xaddflg = 0xf;
		

		DutNumArray all_duts;
		UINT64 active_dut_bit;
		int fail_dut_bit_flg = 0;
		int fail_dut_pin_bit_flg = 0;
		int active_dut_pin_bit_flg = 0;

		int dut_cnt = active_duts_get(&all_duts);
		active_duts_get(&active_dut_bit);
		//output("active_dut_bit=%x", active_dut_bit);
		for (int i_dut = 0; i_dut < dut_cnt; i_dut++)
		{
			DutNum dut = all_duts[i_dut];
			active_dut_pin_bit_flg |= (0xff << dut*8);
			//output("active_dut_pin_flg=%x", active_dut_pin_bit_flg);

		}
		for (int i = 0; i < total_comps; i++)
		{
			if (uv_fls_output_summarize_fails) {
				if ((comp + i)->id.cid == CID_FAIL || (comp + i)->id.cid == CID_FFAIL)
				{
					xadd = (((comp + i)->fail.addr >> 16) & 0xffff);

					if (xaddflg == xadd) {
						fail_dut_bit_flg = 0;
						fail_dut_pin_bit_flg = 0;
						continue;
					}

					//if (xaddflg == xadd && fail_dut_bit_flg == active_dut_bit) {
					//	//output("xaddflg=%d xadd=%d fail_dut_bit_flg=%x active_dut_bit=%x", xaddflg, xadd, fail_dut_bit_flg, active_dut_bit);
					//	fail_dut_bit_flg = 0;
					//	continue;
					//}
					//output("failaddress= %x", (((comp + i)->fail.addr >> 16) & 0xffff));

					for (int i_dut = 0; i_dut < dut_cnt; i_dut++)
					{
						DutNum dut = all_duts[i_dut];
						if (((comp + i)->fail.fails >> (8 * dut)) & 0xff) {
							fail_dut_pin_bit_flg |= (comp + i)->fail.fails;
							//output("fail_dut_pin_bit_flg=%x", fail_dut_pin_bit_flg);

							fail_dut_bit_flg |= (0x1<<dut);
							//output("fail_dut_bit=%x", fail_dut_bit_flg);
							//output("i_dut=%d dut=%d", i_dut, dut);
							//output("dut=%d faildata= %x",dut+1, (comp + i)->fail.fails);

							for (std::size_t j = 0; j < gv_vec_size; ++j) {
								gv_windows[dut].at(((comp + i)->fail.addr >> 16) & 0xffff) |= ((comp + i)->fail.fails >> (8 * dut)) & 0xff;
							}
						}
					}
					if(fail_dut_pin_bit_flg == active_dut_pin_bit_flg) xaddflg = xadd;
					//if(fail_dut_bit_flg == active_dut_bit) xaddflg = xadd;
				}
			}
		}
	}

}

std::vector<DutNum> fls_dutmap[8];  //2 is the max for HD7. Allocating 8 for future expansion
static thread_local std::vector<DutNum>* p_fls_dutmap;
#ifdef PRE_BETA4_STREAMID
static thread_local int g_stream_id;
#endif

// This callback is register as part of the flsTasksCallback
bool interrupt_fls = false;

//This function is registered as a callback during the FLS callback execution. If the system is
//unloading the job, it will call this function to ask us to return from the callback ASAP
void flsTasksCallback_interrupter() {
	for (int i = 0; i < fls_max_stream_count_get(); i++) {
		FTM->FlsTMgr[i].thread_being_interrupted = true;
	}
}

//FLS startup. We should initialize any stream local information here. 
BOOL flsTasksCallback_startup(NXTFlsTargetStreamNumber nstream_id)
{
	int stream_id = static_cast<int>(nstream_id);
#ifdef PRE_BETA4_STREAMID
	g_stream_id = stream_id;  //save this threads stream id into a thread local variable for usage later
#endif

	//Init for this stream
	FTM->FlsTMgr[stream_id].init();
	fls_dutmap[stream_id] = fls_stream_duts_get(nstream_id);
	p_fls_dutmap = &fls_dutmap[stream_id];  //Take THREAD LOCAL pointer to the proper dutmap for this stream. 

	//GWB DEBUG Print out dutmap for this stream
	std::stringstream ss;
	for (auto& i : *p_fls_dutmap) {
		ss << "t_DUT" << i + 1 << ", ";
	}
	output("FStrm%d DUTs assigned = %s", stream_id + 1, ss.str().c_str());
	qpop_history.clear();
	qpop_history.reserve(10000);
	qpop_cnt = 0;
	qnpop_cnt = 0;
	//GWB DEBUG

	//TODO Make a fcb_received_startup and move this to there

	FTM->fcb_received_startup(nstream_id);
	return TRUE;
}



void flsTasksCallback_shutdown(NXTFlsTargetStreamNumber stream_id, NXTFlsStreamShutdownReason nreason)
{
	enFlsStreamShutdownReasonMag7 reason = nreason;
	std::string shutdown_reason;
	if (reason == enFlsStreamShutdownReasonMag7::t_NORMAL) { shutdown_reason = "t_NORMAL (interpose requested)"; }
	else if (reason == enFlsStreamShutdownReasonMag7::t_USER_TERMINATED) { shutdown_reason = "t_USER_TERMINATED (normal)"; }
	else if (reason == enFlsStreamShutdownReasonMag7::t_USER_RECONFIGURED) { shutdown_reason = "t_USER_RECONFIGURED"; }
	else if (reason == enFlsStreamShutdownReasonMag7::t_FORCED_SHUTDOWN) { shutdown_reason = "t_FORCED_SHUTDOWN"; }
	else if (reason == enFlsStreamShutdownReasonMag7::t_ERROR) { shutdown_reason = "t_ERROR"; }
	else { shutdown_reason = "unknown enum"; }

	output("FLS Stream %d Shutdown due to: %s", (int)stream_id, shutdown_reason.c_str());
	q_flush();
	output_qpop_history((int)stream_id);
	FTM->fcb_recevied_shutdown(stream_id);

}

// The goal of this callback is to identify the beginning and end of each task and send the data in
// between to the FlsTask passed through the queue.


#ifdef PRE_BETA4_STREAMID
BOOL flsTasksCallback(DWORD128* data, DWORD total_components, void* cb_data)
{
	int stream_id = g_stream_id;
#else
BOOL flsTasksCallback(NXTFlsTargetStreamNumber nstream_id, DWORD128 * data, DWORD total_components, void* cb_data)
{
	int stream_id = static_cast<int>(nstream_id);
#endif
	thread_interrupter(flsTasksCallback_interrupter);

	output_fls_stream(stream_id, data, total_components);
	FlsTaskManager* FTM = reinterpret_cast<FlsTaskManager*>(cb_data);

	DWORD icomp = 0;
	COMP128* first_comp = reinterpret_cast<COMP128*>(data);

	auto& fTMgr = FTM->FlsTMgr[stream_id];

	//Check to make sure we didn't double receive the callback
	if (data == fTMgr.lastcb_p_data && total_components == fTMgr.lastcb_num_comps) {
		warning("We received the same pointer with the same size. This should not happen. Ignoring this callback and continuing.");
		return TRUE;
	}
	else {
		//Cache callback parameters in case a User Task needs access to the entire buffer (debug feature)
		fTMgr.lastcb_p_data = data;
		fTMgr.lastcb_num_comps = total_components;
		fTMgr.numcbs++;
		fTMgr.lastcb_time = std::chrono::high_resolution_clock::now();
	}

	//At this level, the code is mainly scanning for "framing" components. Once it sees an FLS User Frame
	//start, it passes the FLS data to the the Fls User Task for processing. Therefore, the heavy processing will be done
	//by the queued Fls User Task's fcb_add() method
	for (; icomp < total_components; icomp++)
	{
		// The should_stop() API returns a true when the user is trying to interrupt processing
		// This feels too expensive to do in the tight loop in the User Task. 
		// We will read should_stop() here and set thread_being_interrupted.
		// Note that the thread_interrupter may ALSO set thread_being_interrupted.
		// if (should_stop()) { fTMgr.thread_being_interrupted = true; }

		//Check to make sure that we should still be trying to process data
		if (fTMgr.thread_being_interrupted) {
			if (fTMgr.active_curFlsTask) { fTMgr.curFlsTask->fcb_release_ownership(stream_id, FlsTask::FlsTaskState::error); }
			fTMgr.curFlsTask = NULL;
			fTMgr.active_curFlsTask = false;
			fTMgr.curTaskID = fTMgr.INACTIVEID;
			return FALSE;  //This needs to be added to each while loop
		}

		//Get the next component to process
		COMP128* comp = first_comp + icomp;

		////First process any errors and startup or shutdown events
		if (comp->id.cid == CID_NOP) {
			continue;
		}

		//output("FStrm%d: Processing component at %d\n", int(fls_current_stream_id_get()), icomp); q_flush();
		if (comp->id.cid == CID_FLUSH) {
			if (FTM->is_shutdown_component(comp)) { return FALSE; }
			fTMgr.cb_num_flush++;
			continue;
		}

		if (comp->id.cid == CID_USER1) { fTMgr.cb_num_user1++; }

		if (comp->id.cid == CID_HALTED) {
			UINT64 error_code = comp->cmd.error;
			warning("FATAL FLS ERROR. CID_HALTED received on stream%d. Error code %d. FLS will need to be reconfigured to continue.", stream_id, error_code);
			if (fTMgr.curFlsTask != NULL) {
				fTMgr.curFlsTask->fcb_release_ownership(stream_id, FlsTask::FlsTaskState::error);
			}
			return FALSE;
		}


		//GWB Why do we need this?
		if (FTM->is_task_framing_component(comp, FTM->pattern_suffix_types)) {
			fTMgr.curPatID = fTMgr.INACTIVEID;
		}

		//// Find a FLS Task and send it data
		if (fTMgr.active_curFlsTask) {

			//If we are pointing to a framing suffix, process it
			if (FTM->is_task_framing_component(comp, FTM->task_suffix_types)) {
#ifdef TASKMGR_DEBUG_LOGGING
				auto& hist = fTMgr.framing_suffix_history;
				if (hist.size() < fTMgr.max_comp_history) {
					hist.emplace_back(*comp, std::chrono::high_resolution_clock::now(), 0);
				}
#endif
				fTMgr.curFlsTask->fcb_release_ownership(stream_id, FlsTask::FlsTaskState::filled);  //Send Task from this stream back
				fTMgr.curFlsTask = NULL;
				fTMgr.active_curFlsTask = false;
				fTMgr.curTaskID = fTMgr.INACTIVEID;
			}
			else {
				//We have an active task to send data to. We are going to send it our count. The
				//task will probably consume more than a single component. The component should
				//leave icomp at the LAST offset that was processed. It should NOT process any framing components.
				//A framing component will get processed on the next loop.  
				//TODO: How does this not fail if the framing component was the first in a callback?!?
				if (!fTMgr.curFlsTask->fcb_add(stream_id, first_comp, icomp, total_components, p_fls_dutmap)) {
					//this should only be called if the User Task was getting interrupted.
					fTMgr.curFlsTask->fcb_release_ownership(stream_id, FlsTask::FlsTaskState::error);
					fTMgr.curFlsTask = NULL;
					fTMgr.active_curFlsTask = false;
					fTMgr.curTaskID = fTMgr.INACTIVEID;
					return FALSE;
				}
			}
		}
		else {
			if (FTM->is_task_framing_component(comp, FTM->pattern_prefix_types)) {
				fTMgr.curPatID = FTM->user_component_payload(comp);
			}

			//Get a New FlsTask to work on
			if (FTM->is_task_framing_component(comp, FTM->task_prefix_types)) {
#ifdef TASKMGR_DEBUG_LOGGING
				auto& hist = fTMgr.framing_prefix_history;
				bool did_push = false;
				if (hist.size() < fTMgr.max_comp_history) {
					hist.emplace_back(*comp, std::chrono::high_resolution_clock::now(), 0);
					did_push = true;
				}
#endif
				fTMgr.curTaskID = FTM->user_component_payload(comp);
				//Grab the next Task Manager
				if (FTM->fcb_queue_pop(stream_id, fTMgr.curFlsTask, fTMgr.curPatID,
					fTMgr.curTaskID)) {
					fTMgr.active_curFlsTask = true;
#ifdef TASKMGR_DEBUG_LOGGING
					if (did_push) {
						hist.back().status = 1;
					}
#endif
				}
				else {
					return FALSE;  //Something went wrong so we could not get a new task. Ask FLS to shutdown
				}
			}

			//Throw away any components not within a Task that isn't a framing component.
			//We could print them here for debug

		} // End of if valid task

	} // End of component loop

	//output("FLS Callback Complete"); q_flush();
	return TRUE;
}

//Add a method that works like funtest. Queue this task, run funtest and wait for the results
PFState FlsTask::funtest(Pattern * pPattern)
{
	this->queue();
	PFState pfstate = ::funtest(pPattern, finish);
	this->wait_for_taskstop();
	return pfstate;
}

FlsTaskManager::StreamInfo::s_comp_history::s_comp_history(COMP128 _comp, std::chrono::time_point<std::chrono::high_resolution_clock> _ts, uint64_t _status)
{
	comp = _comp;
	ts = _ts;
	status = _status;
}


s_queue_history::s_queue_history(DWORD _stream_id, DWORD _tid, void* _ptr, std::chrono::time_point<std::chrono::high_resolution_clock> _ts)
{
	stream_id = _stream_id;
	tid = _tid;
	ptr = _ptr;
	ts = _ts;
}

