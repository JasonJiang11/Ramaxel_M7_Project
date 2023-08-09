#pragma once

#include "tester.h"
#include "FlsPatternMacros.h"
#include "FlsConfig.h"
#include <mutex>
#include <condition_variable>
//#include <filesystem>
#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>
#include <fstream>
#include <unordered_map>
#include <set>

//#define TASKMGR_DEBUG_LOGGING
//#define PRE_BETA4_STREAMID

extern void output_fls_status();
extern void get_fls_status(std::stringstream& ss);

//Base class for all FLS Tasks. The base class handles the locking of the data structure
//Only use methods prefixed with fcb_ in the FLS CallBack routine.
class FlsTask {
protected:

public:
	// This enum helps the object track itself through it's lifetime
	enum class FlsTaskState {
		init,      //Object has been constructed 
		queued,    //User is done configuring object and has queued it
		callback,  //Callback has taken ownership
		filled,    //Callback has filled and released ownership
		error
	};   //Callback has released ownership, but it had a fatal error
	std::mutex mtx;
	std::condition_variable cv;

	//This struct should only be set by the callbacks
	struct sFlsTaskState {
		FlsTaskState state;
		DWORD PatternID, FlsTaskID;
	};

	DWORD seqbin_FlsTaskID;

	std::unordered_map<int, sFlsTaskState> TaskState;

	//Classes dervided from this base class are expected to call the following methods
	//in the current order. Methods prefixed by fcb_ are expected to be called from 
	//the FLS callback function (running in an FLS callback worker thread). 

	//   1:  construct the object
	FlsTask() {
		//Make an entry in FlsTaskState for each FLS stream that this system supports (MagV=1, Mag7=2)
		//FlsTaskState.resize(fls_max_stream_count_get());
		for (auto stream_id = enFlsTargetStreamNumberMag7::t_STREAM1; stream_id < enFlsTargetStreamNumberMag7::t_end; stream_id++)
		{
			TaskState[int(stream_id)].state = FlsTaskState::init;
			TaskState[int(stream_id)].PatternID = 0xDEAD;
			TaskState[int(stream_id)].FlsTaskID = 0xDEAD;
		}
	}

	//   2:  Derived Object Configuration Data

	//   3:  Queue the object. This will coordinate with the FLS Task Manager to send 
	//       a pointer to this object through thread safe queues to the FLS Callback function
	//       running on the FLS worker thread
	void queue();

	//   4:  Patgen is started. FLS data will begin flowing to the callback

	//   5:  FLS Callback receives a FLS Framing task start component. The FLS Callback will 
	//       pop pointer from FLS Task Manager queue and take ownership of the object via calling
	//       fcb_take_ownership. On Mag7, there will be MULTIPLE threads. 
	virtual void fcb_take_ownership(int stream_id, DWORD p_PatternID, DWORD p_flsTaskID);

	//   6:  FLS Callback adds components to the object. Derived Object processes and captures the data
	//       The derived method must use the FLS Task Manager to recognize framing components and return
	//       control to the callback without processing them.
	virtual inline bool fcb_add(int stream_id, COMP128* comp, DWORD& CurComp, const DWORD TotalComp, const std::vector<DutNum>* fls_dutmap) = 0;

	//   8:  FLS Callback receives a FLS Framing task stop component. The FLS Callback will release ownership of the object
	virtual void fcb_release_ownership(int stream_id, FlsTaskState release_state);

	//   9:  After starting the pattern but before trying to access any of the captured data, the seqbin thread
	//       calls wait_for_taskstop. This waits for all FLS streams to call fcb_release_ownership
	void wait_for_taskstop();
	bool any_streams_running();

	FlsTaskState stream_state(enFlsTargetStreamNumberMag7 stream_id);
	std::string FlsTask::taskstate_to_string(FlsTaskState state);
	void get_task_state(std::stringstream& ss);
	void output_task_state();
	bool all_streams_ok();
	bool any_streams_error();
	void set_taskid(DWORD id);
	PFState FlsTask::funtest(Pattern* pPattern);
};

struct s_queue_history {
	DWORD stream_id;
	DWORD tid;
	void* ptr;
	std::chrono::time_point<std::chrono::high_resolution_clock> ts;
	s_queue_history(DWORD stream_id, DWORD _tid, void* _ptr, std::chrono::time_point<std::chrono::high_resolution_clock> _ts);
};

//FlsTaskManager... There should only be ONE copy of this object.
class FlsTaskManager {
public:
	//Per Stream Persistant Data Structures for the callback
	struct StreamInfo {
		const DWORD INACTIVEID = 0xffffffff;
		enum class StreamState { unstarted, running, shutdown, error };
		StreamState state = StreamState::unstarted;
		std::vector<DutNum> dutmap;
		concurrency::concurrent_queue<FlsTask*> FlsTaskQueue;
		bool active_curFlsTask = false;
		bool thread_being_interrupted = false;
		FlsTask* curFlsTask = NULL;
		DWORD curTaskID = INACTIVEID;
		DWORD curPatID = INACTIVEID;
		uint64_t numcbs;
		DWORD128* lastcb_p_data = NULL;
		DWORD lastcb_num_comps;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastcb_time, init_time;
		uint64_t cb_num_user1;
		uint64_t cb_num_flush;

		struct s_comp_history {
			COMP128 comp;
			std::chrono::time_point<std::chrono::high_resolution_clock> ts;
			uint64_t status;
			s_comp_history(COMP128 _comp, std::chrono::time_point<std::chrono::high_resolution_clock> _ts, uint64_t _status);
		};

		uint64_t qpush_cnt;

		const int max_comp_history = 10000;
		std::vector<s_comp_history> framing_prefix_history;
		std::vector<s_comp_history> framing_suffix_history;
		std::vector<s_comp_history> flush_history;
		std::vector<s_queue_history> qpush_history;

		//StreamInfo(const StreamInfo&) = delete;
		StreamInfo& operator=(const StreamInfo&) = delete;
		void init() {
			state = FlsTaskManager::StreamInfo::StreamState::unstarted;
			active_curFlsTask = false;
			curFlsTask = NULL;
			curPatID = INACTIVEID;
			curTaskID = INACTIVEID;
			thread_being_interrupted = false;
			FlsTaskQueue.clear();  //this is not thread safe, but only one thread should ever be calling init() at a time on this object
			numcbs = 0;
			cb_num_flush = 0;
			cb_num_user1 = 0;
			qpush_cnt = 0;
			init_time = std::chrono::high_resolution_clock::now();
#ifdef TASKMGR_DEBUG_LOGGING
			framing_prefix_history.reserve(max_comp_history);
			framing_suffix_history.reserve(max_comp_history);
			qpush_history.clear();
			qpush_history.reserve(max_comp_history);
#endif
		}
		StreamInfo::StreamInfo() { init(); }
	};

	FlsConfig* ActiveFlsConfig;
	std::vector<StreamInfo> FlsTMgr;

	COMP128 init_component;
	COMP128 shutdown_component;
	const uint64_t shutdown_ureg_id = 0x1234CAFE;

	void config(FlsConfig* fls_config);			//Use to configure FLS
	void shutdown();							//Used from flow to ask FLS to shutdown
	void fcb_received_startup(enFlsTargetStreamNumberMag7 stream_id);
	void fcb_recevied_shutdown(enFlsTargetStreamNumberMag7 stream_id);	//Callback notifies Manager that it has seen a shutdown compontn
	void queue_push(FlsTask* FlsTask);			//Send an FlsTask from seqbin to FLS callbacks
	bool fcb_queue_pop(int stream_id, FlsTask*& FlsTask, DWORD& PatID, DWORD& TaskID);		//Get next task from the seqbin thread
	void FlsTaskManager::update_stream_status();
	bool FlsTaskManager::any_fls_streams_active();
	void debug_output_callback(const int stream_id, std::string err_msg, DWORD curComp);
	void get_ftm_status(std::stringstream& ss, bool detailed_history = false);
	void output_ftm_status(bool detailed_history = false);

	// Interfaces FlsTasks and Callback use to determine Task Framing
	const std::set<DWORD> pattern_prefix_types{ FLSF_PAT_START, FLSF_PAT_TASK_START };
	const std::set<DWORD> pattern_suffix_types{ FLSF_PAT_STOP, FLSF_PAT_TASK_STOP };
	const std::set<DWORD> task_prefix_types{ FLSF_TASK_START, FLSF_PAT_TASK_START };
	const std::set<DWORD> task_suffix_types{ FLSF_TASK_STOP, FLSF_PAT_TASK_STOP };
	inline bool is_task_framing_component(COMP128* comp, const std::set<DWORD>& types);	//Check if the User type
	bool is_framing_component(COMP128* comp);									//Check all the framing component types
	inline bool FlsTaskManager::is_init_component(COMP128* comp);
	inline bool FlsTaskManager::is_shutdown_component(COMP128* comp);

	inline DWORD user_component_type(COMP128* comp) {
		switch (comp->id.cid) {
		case CID_USER1: //same as CID_USER2
		case CID_USER2: return comp->user.yaddr;
		case CID_USER3: return comp->user.xaddr;
		default: return 0xFFFFFFFF;
		}
	}
	inline DWORD user_component_payload(COMP128* comp) {
		switch (comp->id.cid) {
		case CID_USER1: //same as CID_USER2
		case CID_USER2: return comp->user.xaddr;
		case CID_USER3: return comp->user.yaddr;
		default: return 0xFFFFFFFF;
		}
	}
};

//Handle to the Global FLS Task Manager
extern FlsTaskManager* FTM;
