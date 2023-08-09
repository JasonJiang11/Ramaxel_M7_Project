#pragma once
#include "FlsTask.h"

// Class to scan for and store any time stamps sent from a CID_DEBUG and take a corresponding 
// C++ timestamp at time of processing. Can be used to plot how far behind the CPU is falling
// behind the pattern.
class cFlsTimeStampLogger : public FlsTask
{
private:
	struct timerEvent
	{
		DWORD op, id;
		__int64 cpu_ts;
		uint64_t delta_comps;
		COMP128 fls_ts;
		timerEvent(DWORD p_op, DWORD p_id, COMP128 p_fls_ts, __int64 p_cpu_ts, uint64_t p_delta_comps)
			: op(std::move(p_op)), id(std::move(p_id)), fls_ts(std::move(p_fls_ts)), cpu_ts(std::move(p_cpu_ts)), delta_comps(std::move(p_delta_comps)) {}
		timerEvent(timerEvent&& other) noexcept
			: op(std::move(other.op)), id(std::move(other.id)), fls_ts(std::move(other.fls_ts)), cpu_ts(std::move(other.cpu_ts)), delta_comps(std::move(other.delta_comps)) {}
	};
	uint64_t delta_comps;
	std::vector<timerEvent> tEvents;
	COMP128 last_meta_ts;
	inline void add_comp(COMP128& comp);

public:
	cFlsTimeStampLogger(int presize);
	inline void fcb_add(COMP128& comp);
	inline void fcb_add_block(DWORD128* data, int length);
	void(*wrkr)(DWORD128* data, int length) = NULL;
	void save_file(std::string fileprefix);
};


inline void cFlsTimeStampLogger::add_comp(COMP128& comp)
{
	int streamnum = static_cast<int>(fls_current_stream_id_get());
	sFlsTaskState* pFTS = &FlsTaskState[streamnum];
	pFTS->state = FlsTaskState::callback;

	delta_comps++;	//keep a count of how many components
	//only watch for timer components
	if (comp.id.cid == CID_USER4)
	{
		last_meta_ts = comp;
	}
	else if (comp.id.cid == CID_DEBUG && comp.timer.type == 0 && last_meta_ts.id.cid != CID_NOP)
	{
		DWORD flstype, payload;
		decode_user(&last_meta_ts, flstype, payload);
		static int numts = 0;
		LARGE_INTEGER ts;
		QueryPerformanceCounter(&ts);
		switch (flstype)
		{
		case CIDU4_TYPE_PAT_START:
		case CIDU4_TYPE_PAT_STOP:
			tEvents.emplace_back(flstype, payload, comp, ts.QuadPart, delta_comps);
			delta_comps = 0;  //reset count
			numts = 0;
			break;
		case CIDU4_TYPE_TIMESTAMP:
			//Add to the end of the vector. Using emplace_back to construct the struct in place
			//Ze ro is used for the cpu timestamp
			tEvents.emplace_back(flstype, numts++, comp, ts.QuadPart, delta_comps);
			delta_comps = 0;  //reset count
			break;
		}
		//Change CID of the cache to CID_NOP to represent no active ts metadata
		last_meta_ts.id.cid = CID_NOP;
	}
	return;
}

cFlsTimeStampLogger::cFlsTimeStampLogger(int presize) {
	delta_comps = 0;
	tEvents.reserve(presize);
	last_meta_ts.id.cid = CID_NOP;
	state = fls_task_state::ready;
}

inline void cFlsTimeStampLogger::fcb_add(COMP128& comp)
{
	if (state == callback) {
		add_comp(comp);
	}
	else {
		state = error;
	}
	return;
}

inline void cFlsTimeStampLogger::fcb_add_block(DWORD128* data, int length)
{
	if (state == callback) {
		if (wrkr != NULL) { wrkr(data, length); }
		for (int i = 0; i < length; i++)
		{
			COMP128* comp = (COMP128*)data + i;
			add_comp(*comp);
		}
	}
	else {
		state = error;
	}
	return;
}

void cFlsTimeStampLogger::save_file(std::string fileprefix)
{
	//Make timer directory if it doesn't exist
	//Currently on VS2013 so I can't use std::filesystem
	std::string fls_timer_dir = "./fls_timers";
	if (CreateDirectory(fls_timer_dir.c_str(), NULL)) {}
	else if (ERROR_ALREADY_EXISTS == GetLastError()) {}
	else { fatal("Was not able to create a directory for the fls timers."); }

	//Make the filename
	std::string filename;
	int chassis, slot, site;
	slot_id(&chassis, &slot, &site);
	std::string slotID = std::to_string(chassis) + "_" + std::to_string(slot) + "_" + std::to_string(site);
	filename = fls_timer_dir + "/" + fileprefix + "-" + slotID + ".csv";

	if (tEvents.size() < 2) {
		warning("save_file called when not enough timestamps have been saved. Nothing being dumped.");
		return;
	}

	//Open the file
	std::ofstream osTimer(filename.c_str(), std::ofstream::trunc);

	//Get the Frequencies for the timers
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	double cpu_freq = (double)freq.QuadPart;
	output("CPU Frequency = %f", cpu_freq);

	const double sys_clock_freq = 428e6;

	//Output Header
	osTimer << "SlotID" << "," "PatID" << "," << "OP" << "," << "ID" << "," << "FLSTime,CPUTime," << "FLSDelta" << "," << "CPUDelta" << "," << "Components,\n";

	DWORD patid = tEvents[0].id;
	//Loop through the vector starting at the 2nd element
	for (auto it = std::next(tEvents.begin()); it != tEvents.end(); ++it)
	{
		double cpudelta = (it->cpu_ts - std::prev(it)->cpu_ts) / cpu_freq;
		double cputotal = (it->cpu_ts - tEvents[0].cpu_ts) / cpu_freq;
		double flsdelta = (it->fls_ts.timer.timer - std::prev(it)->fls_ts.timer.timer) / sys_clock_freq;
		double flstotal = (it->fls_ts.timer.timer - tEvents[0].fls_ts.timer.timer) / sys_clock_freq;

		osTimer << slotID << "," << patid << "," << it->op << "," << it->id << "," <<
			flstotal << "," << cputotal << "," << flsdelta << "," << cpudelta << "," << it->delta_comps << ",\n";
	}
	osTimer.close();

	//for (auto it : tEvents)
	//{
	//	q_output("%s: patid=%d, op=%d, id=%d, flsts=%f, cputs=%f, deltacomps=%d",
	//		slotID.c_str(), patid, it.op, it.id, (it.fls_ts.timer.timer)/sys_clock_freq, it.cpu_ts/cpu_freq, it.delta_comps);
	//}
	//q_flush();
}
