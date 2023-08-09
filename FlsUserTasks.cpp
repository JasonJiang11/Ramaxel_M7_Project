#include "FlsUserTasks.h"
#include <iomanip>
#include <fstream>
#include <iostream>
//#include <format>

static thread_local FlsStoreRegCnt::perStreamStore* p_local_RegCnt_Stream;

std::array<std::string, 16> cid_name_lookup = {
	"CID_NOP",
	"CID_FAIL",
	"CID_RSVD2",
	"CID_FFAIL",
	"CID_TEC",
	"CID_ERROR",
	"CID_CMD",
	"CID_RSVD7",
	"CID_FLUSH",
	"CID_USER1",
	"CID_USER2",
	"CID_USER3",
	"CID_USER4",
	"CID_USER5",
	"CID_DEBUG",
	"CID_HALTED"
};

FlsSimple::FlsSimple()
{
	perDutAddrs.resize(fls_max_stream_count_get());
	for (auto& i : perDutAddrs)
	{
		//Initialize each stream's counts and ID's to 0
		i.perCIDcnts.fill(0);
	}
}

inline bool FlsSimple::fcb_add(int stream_id, COMP128* compbegin, DWORD& curComp, const DWORD TotalComp, const std::vector<DutNum>* fls_dutmap)
{
	// Iterate through all the FLS components in this group
	for (; curComp < TotalComp; curComp++)
	{
		if (FTM->FlsTMgr[(int)stream_id].thread_being_interrupted) return false;  //This needs to be added to each while loop

		COMP128* comp = compbegin + curComp;
		//Look to see if this component is the end of this FlsTask
		if (FTM->is_framing_component(comp)) {
			//Do not process framing components, leave curComp pointing to last processed component
			curComp--;
			return true;
		}

		//Processing Code Goes Here
		BYTE thisCID = comp->id.cid;
		perDutAddrs[int(stream_id)].perCIDcnts[thisCID] ++; //Increment count per CID
	}
	return true;
}

std::string FlsSimple::dump_counts()
{
	std::string outstr;
	int stream_id = 0;
	for (auto& i : perDutAddrs)
	{
		stream_id++;
		outstr += "Stream" + std::to_string(stream_id) + " Counts: ";
		int cid = -1;
		bool first = true;
		for (auto& cnt : i.perCIDcnts)
		{
			cid++;
			//Add a proceeding comma if not the first in list
			if (first) { first = false; }
			else { outstr += ", "; }
			if (cnt == 0) { outstr += "<COLOR=GRAY>"; }
			outstr += cid_name_lookup[cid] + "=" + std::to_string(cnt);
			if (cnt == 0) { outstr += "</COLOR>"; }
		}
		outstr += "\n";
	}
	return outstr;
}

FlsStoreRegCnt::FlsStoreRegCnt(int NumCountBanks, int NumRegisterBanks) {
	//Since the FlsTask Base Class has no arguments, it will be called automatically

	//Presize per DUT data store
	DutStore.resize(size_t(max_dut()) + 1);

	//Make a new perStreamStore and associate it with each stream
	for (auto i = enFlsTargetStreamNumberMag7::t_STREAM1; i < enFlsTargetStreamNumberMag7::t_end; i++)
	{
		Stream.emplace((int)i, perStreamStore());
	}

	CountBankIDMax = NumCountBanks - 1;
	for (auto& DS : DutStore)
	{

	}

}

void FlsStoreRegCnt::fcb_take_ownership(int stream_id, DWORD p_PatternID, DWORD p_flsTaskID)
{
	FlsTask::fcb_take_ownership(stream_id, p_PatternID, p_flsTaskID);  //Call base class

	//Set thread local pointer to the correct stream data
	auto i = Stream.find(stream_id);
	if (i != Stream.end()) {
		p_local_RegCnt_Stream = &(i->second);
	}
	else {
		fatal("FLS Task could not find it's own stream data.");
	}
	auto& sleeptime = p_local_RegCnt_Stream->DEBUGONLY_hold_queue_ms;
	//output("FlsStoreRegCnt User Task dequeued into callback."); q_flush();
	if (sleeptime > 0) {
		output("DEBUG: FStrm%d Sleeping for %d ms", stream_id, sleeptime);
		Sleep(sleeptime);
	}
}

inline bool FlsStoreRegCnt::fcb_add(int stream_id, COMP128* compbegin, DWORD& curComp, const DWORD TotalComp, const std::vector<DutNum>* fls_dutmap)
{
	for (; curComp < TotalComp; curComp++)
	{
		if (FTM->FlsTMgr[stream_id].thread_being_interrupted) return false;  //This needs to be added to each while loop
		COMP128* comp = compbegin + curComp;
		if (FTM->is_framing_component(comp)) {
			//Do not process framing components, leave curComp pointing to last processed component
			curComp--;
			return true;
		}

		switch (comp->id.cid) {
		case CID_NOP: continue;
		case CID_FAIL: fcb_process_fail(comp, fls_dutmap); break;
		case CID_FFAIL: fcb_process_ffail(comp, fls_dutmap); break;
		case CID_TEC: fcb_process_tec(comp, fls_dutmap); break;
		case CID_USER1: fcb_process_user1(comp); break;
		case CID_USER2: fcb_process_user23(comp, fls_dutmap); break;
		case CID_USER3: fcb_process_user23(comp, fls_dutmap); break;
		case CID_USER4: fcb_process_user4(comp); break;
		case CID_USER5: fcb_process_user5(comp); break;
		case CID_FLUSH: fcb_process_flush(comp); break;
		case CID_ERROR: fcb_process_error(comp); break;
		case CID_DEBUG: fcb_process_debug(comp); break;
		default:
			//Log the component as unrecognized
			//warning("Received unhandled component type.");
			p_local_RegCnt_Stream->UnrecognizedComponents++;
		};
	}
	return true;
}

void FlsStoreRegCnt::fcb_release_ownership(int stream_id, FlsTaskState release_state)
{
	FlsTask::fcb_release_ownership(stream_id, release_state);  //Call base class


}

inline bool FlsStoreRegCnt::fcb_next_failbit(uint64_t& fails, DutNum& dut, DWORD& pinnum, const std::vector<DutNum>* fls_dutmap)
{
	DWORD lsb_set_bit = 0;
	if (_BitScanForward64(&lsb_set_bit, fails)) {  //Find first set LSB of fails 
		fails = fails & ~(1 << lsb_set_bit);       //mask off the bit we are processing
		DWORD flsdutnum = 0;
		FTM->ActiveFlsConfig->flsdata_to_dut_pin(lsb_set_bit, flsdutnum, pinnum);	//Use the Active FLS Config to Convert fls fail bit to FLS Dut and Pin Number
		dut = (*fls_dutmap)[flsdutnum];									//Convert from FLS DUT to job's DUT
		return true;
	}
	else {
		return false;
	}
}

inline void FlsStoreRegCnt::fcb_process_fail(COMP128* comp, const std::vector<DutNum>* fls_dutmap) {
	//Take a copy of the fail word as fcb_next_failbit() modifies it
	uint64_t fails = comp->fail.fails;
	DWORD lsb_set_bit = 0;
	DWORD pinnum = 0;
	DutNum dut;
	//Iterate through every fail bit
	while (fcb_next_failbit(fails, dut, pinnum, fls_dutmap)) {
		DutStore[(int)dut].array.accumulated_pin_fail_mask |= (1ULL << pinnum);	//OR fail into pin fail mask
		DutStore[(int)dut].array.fail_cnt++;										//Count the Fail
	}
	return;
}

inline void FlsStoreRegCnt::fcb_process_ffail(COMP128* comp, const std::vector<DutNum>* fls_dutmap) {
	uint64_t fails = comp->fail.fails;
	DWORD lsb_set_bit = 0;
	DWORD pinnum = 0;
	DutNum dut = t_dut1;
	DWORD Xaddr = FTM->ActiveFlsConfig->flsaddr_to_X(comp->fail.addr);
	DWORD Yaddr = FTM->ActiveFlsConfig->flsaddr_to_Y(comp->fail.addr);

	while (fcb_next_failbit(fails, dut, pinnum, fls_dutmap)) {
		perRegBank* p_RegBank = &(DutStore[(int)dut].registers.bank[p_local_RegCnt_Stream->RegisterBankID]);
		ASSERT(pinnum < 8);
		switch (p_local_RegCnt_Stream->RegType) {
		case REG_type::param:
			p_RegBank->param[Yaddr] |= (1 << pinnum);
			break;
		case REG_type::sr:
			p_RegBank->sr[Xaddr] |= (1 << pinnum);
			break;
		case REG_type::features:
			//Pack the subfeatures into a wide fail word. If needed, we could make this another vector.
			p_RegBank->features[Xaddr] |= (1ULL << (pinnum + (Yaddr & 0x3) * 8));
			break;
		case REG_type::readid:
			DutStore[(int)dut].readid.id[Yaddr] |= (1 << pinnum);
			break;
		default:
			warning("Unrecognized REGREAD Message Type. Register Read Type will NOT be set.");
		};
	}

#if 0
	//Debug Code
	enFlsTargetStreamNumberMag7 stream_id = fls_current_stream_id_get();
	std::stringstream dbgmsg_ss;
	dbgmsg_ss << "FStrm" << std::dec << int(stream_id) << ":";
	switch (p_local_RegCnt_Stream->RegType) {
	case REG_type::param:
		dbgmsg_ss << " Update param[" << Yaddr << "]";
		for (auto dut : *fls_dutmap) {
			int idut = (int)dut;
			dbgmsg_ss << ", DUT" << std::dec << idut + 1 << "=" << std::hex << std::setfill('0') << std::setw(2) <<
				static_cast<unsigned>(DutStore[idut].registers.bank[p_local_RegCnt_Stream->RegisterBankID].param[Yaddr]);
		}
		output("%s", dbgmsg_ss.str().c_str());
		break;
	case REG_type::sr:
	case REG_type::features:
	case REG_type::readid:
	default:
		break;
	};
#endif

	return;
}

inline void FlsStoreRegCnt::fcb_process_tec(COMP128* comp, const std::vector<DutNum>* fls_dutmap) {
	for (int i = 0; i <= 1; i++)
	{
		if (comp->tec.sub[i].valid) {
			int dut = (*fls_dutmap)[comp->tec.sub[i].dut];
			int region = comp->tec.sub[i].region;
			int cnt = comp->tec.sub[i].count;
			switch (p_local_RegCnt_Stream->TecType) {

			case TEC_type::count:
				DutStore[dut].counts.bank[p_local_RegCnt_Stream->CountBankID].subbank[p_local_RegCnt_Stream->CountSubBankID].region[region] = cnt;
				break;

			case TEC_type::time:
				DutStore[dut].times.bank[p_local_RegCnt_Stream->TimeBankID].subbank[p_local_RegCnt_Stream->TimeSubBankID].region[region] = fls_tickcount_to_time(cnt);
				break;

			default:
				warning("Unknown Count type");
			}
		}
	}
	return;
}

inline void FlsStoreRegCnt::fcb_process_user1(COMP128* comp)
{
	// Not used by this FLS Task
}
inline void FlsStoreRegCnt::fcb_process_user23(COMP128* comp, const std::vector<DutNum>* fls_dutmap)
{
	DWORD fls_user_payload = FTM->user_component_payload(comp);
	DWORD fls_user_type = FTM->user_component_type(comp);
	switch (fls_user_type) {
	case FLSU_COUNT_BANK_SET:
		if (fls_user_payload > CountBankIDMax) {
			warning("FLSU_COUNT_BANK_SET used with a payload greater than Max. NOT apply ID change. Pattern=%d, Max=%d", fls_user_payload, CountBankIDMax);
		}
		else {
			p_local_RegCnt_Stream->CountBankID = fls_user_payload;
		}
		break;
	case FLSU_COUNT_SUBBANK_SET:
		p_local_RegCnt_Stream->CountSubBankID = fls_user_payload;
		//For each local DUT, make sure that subbank is allocated
		for (auto& i : *fls_dutmap) { DutStore[(int)i].counts.bank[p_local_RegCnt_Stream->CountBankID].subbank_create(fls_user_payload); }
		break;
	case FLSU_REG_BANK_SET:
		if (fls_user_payload > RegisterBankIDMax) { warning("FLSU_REG_BANK_SET used with a payload greater than Max. NOT apply ID change. Pattern=%d, Max=%d", fls_user_payload, RegisterBankIDMax); }
		else { p_local_RegCnt_Stream->RegisterBankID = fls_user_payload; }
		break;
	case FLSU_REG_TYPE:
		switch (fls_user_payload) {
		case FLSP_REG_PARAM: p_local_RegCnt_Stream->RegType = REG_type::param; break;
		case FLSP_REG_SR: p_local_RegCnt_Stream->RegType = REG_type::sr; break;
		case FLSP_REG_FEATURES: p_local_RegCnt_Stream->RegType = REG_type::features; break;
		case FLSP_REG_READID: p_local_RegCnt_Stream->RegType = REG_type::readid; break;
		default: warning("FLSU_REG_TYPE used an unknown register type. Type will NOT be changed. ID=%d", fls_user_payload);
		}
		break;
	case FLSU_TIME_BANK_SET:
		if (fls_user_payload > TimeBankIDMax) { warning("FLSU_TIME_BANK_SET used with a payload greater than Max. NOT apply ID change. Pattern=%d, Max=%d", fls_user_payload, TimeBankIDMax); }
		else { p_local_RegCnt_Stream->TimeBankID = fls_user_payload; }
		break;
	case FLSU_TIME_SUBBANK_SET:
		p_local_RegCnt_Stream->TimeSubBankID = fls_user_payload;
		//For each local DUT, make sure that subbank is allocated
		for (auto& i : *fls_dutmap) { DutStore[(int)i].times.bank[p_local_RegCnt_Stream->TimeBankID].subbank_create(fls_user_payload); }
		break;
	case FLSU_TEC_TYPE:
		switch (fls_user_payload) {
		case FLSP_TEC_COUNTS: p_local_RegCnt_Stream->TecType = TEC_type::count; break;
		case FLSP_TEC_TIME: p_local_RegCnt_Stream->TecType = TEC_type::time; break;
		default: warning("FLSU_TEC_TYPE used an unknown register type. Type will NOT be changed. ID=%d", fls_user_payload);
		}
		break;
	default:
		warning("CID_USER2 or CID_USER3 component received with unknown type. Type=%d, Payload=%d", fls_user_type, fls_user_payload);
	}
}
inline void FlsStoreRegCnt::fcb_process_user4(COMP128* comp)
{
	// Not used by this FLS Task
}
inline void FlsStoreRegCnt::fcb_process_user5(COMP128* comp)
{
	// Not used by this FLS Task
}
inline void FlsStoreRegCnt::fcb_process_flush(COMP128* comp)
{
	// Not used by this FLS Task
}
inline void FlsStoreRegCnt::fcb_process_error(COMP128* comp)
{
	warning("Pattern Generated an error component. Please review pattern. error#=%d, cmd#=%d", comp->cmd.error, comp->cmd.cmd);
}
inline void FlsStoreRegCnt::fcb_process_debug(COMP128* comp)
{
	p_local_RegCnt_Stream->TimeStamps.push_back(comp->timer.timer);
}

std::string FlsStoreRegCnt::array_read_summary_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		//output("DUT%d, BANK%d: Address=%d, FailMask=%xh", (int)dut + 1, banknum, sr.first, sr.second);
		ss << "<COLOR=BLUE>Array-Fail</COLOR>:: DUT" << std::dec << int(dut) + 1;
		ss << ", FailCount" << std::dec << DutStore[(int)dut].array.fail_cnt;
		ss << ", FailMask=" << std::hex << std::setw(3) << std::setfill('0') << DutStore[(int)dut].array.accumulated_pin_fail_mask << "h";
		ss << "\n";
	}
	return ss.str();
}

std::string FlsStoreRegCnt::sr_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		int banknum = 0;
		for (auto& bank : this->DutStore[(int)dut].registers.bank)
		{
			for (auto& sr : bank.sr)
			{
				//output("DUT%d, BANK%d: Address=%d, FailMask=%xh", (int)dut + 1, banknum, sr.first, sr.second);
				ss << "<COLOR=BLUE>SR-Fail</COLOR>:: DUT" << std::dec << int(dut) + 1;
				ss << ", BANK" << std::dec << banknum;
				ss << ": Address=0x" << std::hex << sr.first;
				ss << ", FailMask=" << std::hex << std::setw(2) << std::setfill('0') << int(sr.second) << "h";
				ss << "\n";
			}
			banknum++;
		}
	}
	return ss.str();
}

std::string FlsStoreRegCnt::tec_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		int banknum = 0;
		for (auto& bank : this->DutStore[(int)dut].counts.bank)
		{
			int subbanknum = 0;
			for (auto& subbank : bank.subbank)
			{
				int tecregnum = 0;
				for (auto& failcnt : subbank.second.region) {
					//output("DUT%d, BANK%d: Address=%d, FailMask=%xh", (int)dut + 1, banknum, sr.first, sr.second);
					if (failcnt > 0) {
						ss << "<COLOR=BLUE>TEC-Count</COLOR>: DUT" << std::dec << int(dut) + 1;
						ss << ", BANK" << std::dec << banknum << ":" << subbanknum;
						ss << ": TECRegion=" << std::dec << tecregnum;
						ss << ", Fails=" << std::dec << failcnt;
						ss << "\n";
					}
					tecregnum++;
				}
				subbanknum++;
			}
			banknum++;
		}
	}
	return ss.str();
}

std::string FlsStoreRegCnt::features_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		int banknum = 0;
		for (auto& bank : this->DutStore[(int)dut].registers.bank)
		{
			int featureaddr = 0;
			for (auto& feature_failmask : bank.features)
			{
				if (feature_failmask != 0) {
					ss << "<COLOR=BLUE>Features-Fail</COLOR>:: DUT" << std::dec << int(dut) + 1;
					ss << ", BANK" << std::dec << banknum;
					ss << ": FeatureAddr=" << std::dec << featureaddr;
					for (int i = 0; i < 4; i++) {
						ss << ", Sub" << std::dec << i << "=";
						ss << std::hex << std::setw(2) << std::setfill('0') << int((feature_failmask >> (i * 16)) & 0xff) << "h";  // shift right by i*16 to consider bytes from RE_t rising edge only
					}
					ss << "\n";
				}
				featureaddr++;
			}
			banknum++;
		}
	}
	return ss.str();
}

std::string FlsStoreRegCnt::param_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		int banknum = 0;
		for (auto& bank : this->DutStore[(int)dut].registers.bank)
		{
			int paramaddr = 0;
			for (auto& param_failmask : bank.param)
			{
				if (param_failmask > 0) {
					ss << "<COLOR=BLUE>Param-Fail</COLOR>:: DUT" << std::dec << int(dut) + 1;
					ss << ", BANK" << std::dec << banknum;
					ss << ": ParamAddr=" << std::dec << paramaddr;
					ss << ", FailMask=" << std::hex << std::setw(2) << std::setfill('0') << (int)param_failmask << "h";
					ss << "\n";
				}
				paramaddr++;
			}
			banknum++;
		}
	}
	return ss.str();
}

std::string FlsStoreRegCnt::readid_dump()
{
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		std::stringstream ss_fail;
		ss_fail << "<COLOR=BLUE>ReadID-Fail</COLOR>:: DUT" << std::dec << int(dut) + 1 << ": ";
		ss_fail << std::hex;
		std::string readid_str;
		bool bytefail = false;
		for (auto& byte : this->DutStore[(int)dut].readid.id) {
			ss_fail << std::setw(2) << std::setfill('0') << (int)byte << " ";
			if (byte != 0) { bytefail = true; }
			readid_str.push_back(byte);
		}
		if (bytefail) {
			ss << ss_fail.rdbuf();
			//ss << "string=" << readid_str.c_str();
			ss << "\n";
		}
	}
	return ss.str();
}

std::string FlsStoreRegCnt::timestamps_dump()
{
	std::stringstream ss;
	ss << "Pattern Timestamps\n";
	ss << "TStmp# PG#0 [   ticks       ms       last-curr]   PG#1 [   ticks        ms      last-curr]  ticks between PG\n";
	//     TStmp# PG#0 [ticks       ms       last - curr]   PG#1[ticks        ms      last - curr]  ticks between PG
	//         0 FStrm1[124706097 = 377.897ms ? ? ? . ? ? ? us]  FStrm2[124706099 = 377.897ms ? ? ? . ? ? ? us]  spread = 2
	//         1 FStrm1[124714197 = 377.922ms   24.545us]  FStrm2[124714199 = 377.922ms   24.545us]  spread = 2


	size_t ts_size = 0;
	int num_streams = fls_max_stream_count_get();
	for (int stream_id = 1; stream_id < num_streams; stream_id++)
	{
		if (Stream[stream_id].TimeStamps.size() > ts_size)
		{
			ts_size = Stream[stream_id].TimeStamps.size();
		}
	}
	for (int i = 0; i < ts_size; i++) {
		uint64_t mints = 0xffffffffffffffff;
		uint64_t maxts = 0;
		ss << std::dec << std::setw(5) << i << " ";
		int valid_spread = true;
		for (int stream_id = 0; stream_id < num_streams; stream_id++)
		{
			bool valid_ts = false;
			uint64_t ts;
			if (i < Stream[stream_id].TimeStamps.size()) {
				valid_ts = true;
				ts = Stream[stream_id].TimeStamps[i];
			}
			else {
				ts = -1;
				valid_spread = false;
			}
			if (ts > maxts) { maxts = ts; }
			if (ts < mints) { mints = ts; }
			ss << "FStrm" << stream_id + 1 << "[ ";
			if (valid_ts) {
				ss << ts;
				ss << " = ";
				double time = fls_tickcount_to_time(ts);
				ss << std::setw(7) << std::fixed << std::setprecision(3);
				ss << time / (1 MS) << "ms  ";
				if (i > 0) {
					uint64_t last_ts = Stream[stream_id].TimeStamps[i - 1];
					ss << std::setw(7) << std::fixed << std::setprecision(3);
					ss << fls_tickcount_to_time(ts - last_ts) / (1 US) << "us";
				}
				else {
					ss << " ??.???us";
				}
				ss << " ]  ";
			}
			else {
				ss << "???????? ???.???ms ???.???us ]  ";
			}
		}
		int spread = int(maxts - mints);
		std::string spreadstr = (valid_spread) ? std::to_string(spread) : "invalid";
		ss << "spread=" << spreadstr;
		ss << "\n";
	}
	return ss.str();
}



