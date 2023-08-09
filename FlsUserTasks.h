#pragma once
#include "tester.h"
#include "FlsTask.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <math.h>
#include <iomanip>

class FlsSimple : public FlsTask {
private:

public:
	//data structure. This is a 
	struct FailAddressSummary {
		std::array<DWORD, 16> perCIDcnts;  //Array that holds the number of each type of component
	};

	//per dut 
	std::vector<FailAddressSummary> perDutAddrs;

	//methods
	FlsSimple::FlsSimple();
	inline bool fcb_add(int stream_id, COMP128* compbegin, DWORD& curComp, const DWORD TotalComp, const std::vector<DutNum>* fls_dutmap);
	std::string dump_counts();
};
//FlsStoreRegCnt::: General FLS Task used to store:
//  * Multiple banks of register reads. Banks for each register read type can be switched via pattern command.
//    The different register reads are:
//    * Status Register Reads: per bank, per X address 
//    * Parameter Page Reads: per bank, per Y address
//    * Get Features: per bank, per Y address
//  * TEC counts: stored per bank per TEC region. 
//      TEC regions are selected via the ADDRESS at time the fail is captured. See your FLS Config to determine which address
//  * Fails: This class will store up to a defined number of fails. These fails are AFTER the FLS hardware has applied it's filters
const int max_tec_regions = 256;  //Assuming max region size. Should read this from fls_config_set in future
class FlsStoreRegCnt : public FlsTask
{

private:

public:

	struct CountRegionStore {
		CountRegionStore() {
			region.resize(max_tec_regions, 0);
		}
		std::vector<DWORD> region;
	};

	struct perCountBank {
		std::unordered_map<DWORD, CountRegionStore> subbank;
		bool subbank_get(const DWORD subbankID, CountRegionStore* p_TimeRegionStore) {
			auto sbf = subbank.find(subbankID);
			if (sbf == subbank.end()) {
				return false;
			}
			else {
				p_TimeRegionStore = &(sbf->second);
				return true;
			}
		}
		void subbank_create(const DWORD subbankID) { subbank.emplace(subbankID, CountRegionStore()); }
	};

	DWORD CountBankIDMax = 3; //FIXME make this part of the constructor
	struct Counts {
		Counts() {
			//bank.resize(numBanks);
			bank.resize(4);
		}
		std::vector<perCountBank> bank;
		int get_numBanks() { return int(bank.size()); }
	};

	struct perRegBank {
		perRegBank() {
			features.resize(0x100, 0);		//Max Feature from ONFI4
			param.resize(768, 0);			//Max Parameter Page Bytes
		}
		std::vector<uint64_t> features;		//Get Features storage
		std::vector<BYTE> param;		//Paramter Page storage

		//Status Register Storage will by per page. Address will be decoded to decide page.
		std::unordered_map<DWORD, BYTE> sr;

	};

	const DWORD RegisterBankIDMax = 3; //FIXME make this part of the constructor
	struct Registers {
		Registers() {
			bank.resize(4);
		}
		std::vector<perRegBank> bank;
	};

	struct TimeRegionStore {
		TimeRegionStore() {
			region.resize(256, nan("init")); //Assuming max region size. Should read this from fls_config_set in future
		}
		std::vector<double> region;
	};

	struct perTimeMeasure {
		std::unordered_map<DWORD, TimeRegionStore> subbank;
		bool subbank_get(const DWORD subbankID, TimeRegionStore* p_TimeRegionStore) {
			auto sbf = subbank.find(subbankID);
			if (sbf == subbank.end()) {
				return false;
			}
			else {
				p_TimeRegionStore = &(sbf->second);
				return true;
			}
		}
		void subbank_create(const DWORD subbankID) { subbank.emplace(subbankID, TimeRegionStore()); }
	};

	const DWORD TimeBankIDMax = 3; //FIXME make this part of the constructor
	struct Times {
		Times() {
			bank.resize(4);  //FIXME make this part of the constructor
		}
		std::vector<perTimeMeasure> bank;
	};

	//This will make a summary of the fails seen by CID_FAIL. This code will ONLY count CID_FAIL (assumed to be array reads)
	//not CID_FFAIL (assumed to be register reads). FLS has many options to filter fails to reduce processing. This
	//code will NOT count any of the fails filtered by the FLS hardware
	struct ArrayFailStore {
		uint64_t accumulated_pin_fail_mask = 0;
		uint64_t fail_cnt = 0;
	};

	struct ReadId {
		ReadId() {
			id.resize(12, 0);  //6 bytes at 2 captures per byte
		}
		std::vector<BYTE> id;
	};

	struct perDutStore {
		Counts counts;
		Times times;
		Registers registers;
		ArrayFailStore array;
		ReadId readid;
	};
	std::vector<perDutStore> DutStore;

	enum class TEC_type { count, time };
	enum class REG_type { param, sr, features, readid };
	struct perStreamStore {
		DWORD RegisterBankID = 0;
		DWORD CountBankID = 0;
		DWORD CountSubBankID = 0;
		DWORD TimeBankID = 0;
		DWORD TimeSubBankID = 0;
		DWORD UnrecognizedComponents = 0;
		DWORD DEBUGONLY_hold_queue_ms = 0;
		REG_type RegType;
		TEC_type TecType;
		std::vector<uint64_t> TimeStamps;
	};

	std::unordered_map<int, perStreamStore> Stream;

	FlsStoreRegCnt::FlsStoreRegCnt(int NumCountBanks = 4, int NumRegisterBanks = 4);
	void fcb_take_ownership(int stream_id, DWORD p_PatternID, DWORD p_flsTaskID);
	inline bool fcb_add(int stream_id, COMP128* compbegin, DWORD& curComp, const DWORD TotalComp, const std::vector<DutNum>* fls_dutmap);
	void fcb_release_ownership(int stream_id, FlsTaskState release_state);
	inline bool fcb_next_failbit(uint64_t& fails, DutNum& dut, DWORD& pinnum, const std::vector<DutNum>* fls_dutmap);
	inline void fcb_process_fail(COMP128* comp, const std::vector<DutNum>* fls_dutmap);
	inline void fcb_process_ffail(COMP128* comp, const std::vector<DutNum>* fls_dutmap);
	inline void fcb_process_tec(COMP128* comp, const std::vector<DutNum>* fls_dutmap);
	inline void fcb_process_user1(COMP128* comp);
	inline void fcb_process_user23(COMP128* comp, const std::vector<DutNum>* fls_dutmap);
	inline void fcb_process_user4(COMP128* comp);
	inline void fcb_process_user5(COMP128* comp);
	inline void fcb_process_flush(COMP128* comp);
	inline void fcb_process_error(COMP128* comp);
	inline void fcb_process_debug(COMP128* comp);

	//These API's should be called AFTER waiting for the FLS callbacks to get done with this Task
	//After queue'ing and doing a funtest, use wait_for_taskstop() 
	std::string sr_dump();
	std::string tec_dump();
	std::string param_dump();
	std::string features_dump();
	std::string readid_dump();
	std::string array_read_summary_dump();
	std::string timestamps_dump();
};

