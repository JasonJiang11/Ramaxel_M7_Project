#include "stdafx.h"
#include "DeviceSpecs.h"
#include "Timings.h"
#include "file_mgmt.h"
#include "Levels.h"
#include "TestBlocks.h"
#include "dbm.h"
#include "i2c.h"
#include <math.h>
#include "timer.h"
#include "FlsTask.h"
#include "watchdog.h"

// Variable to keep track of the Sequence Table's state (running or not running).
static BOOL sequence_table_running = FALSE;
//choose to run tdr from TDR_BLOCK during test program load
static bool b_run_tdr = false;

//boc mode of default flow, used for dbm config
static enDbmUINumberMag7 dbm_boc_mode;

VOID_VARIABLE(uv_apg_pattern_dump, " ")
{
	CStringArray names;
	int size = resource_all_names(S_Pattern, &names);

	DWORD mar, var, mlen, vlen;
	for (int i = 0; i < size; ++i)
	{
		mar = var = mlen = vlen = -1;
		Pattern* pattern = Pattern_find(names[i]);
		addrs(pattern, &mar, &var, &mlen, &vlen);
		output("mar = %5d, var = %8d, mlen = %4d, vlen = %6d, name = %s", mar, var, mlen, vlen, names[i]);
	}
}

CSTRING_VARIABLE(__BinaryFile__, "..//binaries//crc32_bin_18kx4.bin", " ") {}

INT_VARIABLE(HRAM_TRIG_CYCLE_NUM, -1, " ") {
	output("HRAM_TRIG_CYCLE_NUM %d", HRAM_TRIG_CYCLE_NUM);
	if (HRAM_TRIG_CYCLE_NUM == -1)
		hram_trigger_set(t_wt_trigger_cycle_counter, t_wt_trigger_none, 0);
	else
		hram_trigger_set(t_wt_trigger_cycle_counter, t_wt_trigger_equal, HRAM_TRIG_CYCLE_NUM);
}
INT_VARIABLE(HRAM_READ_CYCLE_NUM, 10, " ") {}

BOOL_VARIABLE(uv_rb_vhiz, FALSE, " ") {}

void hram_datalog(int num_locs)
{
	int start_locs = 0;

	hram_data_container_hd7 my_hdp[512];

	hram_data_get(start_locs, num_locs, my_hdp);

	for (int i = 0; i < num_locs; i++)
	{
		output("idx %3d, mar 0x%04x, cycle 0x%04x, counter %4d\\",
			i,
			my_hdp[i].mar.value,
			my_hdp[i].cycle_counter.value,
			my_hdp[i].mar_counter.value
		);

		output(", apg1 subdg1: 0x%04x, subdg2: 0x%04x, subdg3: 0x%04x, subdg4: 0x%04x, apg2 subdg1: 0x%04x, subdg2: 0x%04x, subdg3: 0x%04x, subdg4: 0x%04x \\",
			my_hdp[i].apg1_subdgen1.value,
			my_hdp[i].apg1_subdgen2.value,
			my_hdp[i].apg1_subdgen3.value,
			my_hdp[i].apg1_subdgen4.value,
			my_hdp[i].apg2_subdgen1.value,
			my_hdp[i].apg2_subdgen2.value,
			my_hdp[i].apg2_subdgen3.value,
			my_hdp[i].apg2_subdgen4.value);

		output(", apg1 x: 0x%04x, y: 0x%04x, z: 0x%04x, apg2 x: 0x%04x, y: 0x%04x, z: 0x%04x",
			my_hdp[i].apg1_x.value,
			my_hdp[i].apg1_y.value,
			my_hdp[i].apg1_z.value,
			my_hdp[i].apg2_x.value,
			my_hdp[i].apg2_y.value,
			my_hdp[i].apg2_z.value
		);
	}
}

VOID_VARIABLE(HRAM_READ_HRAM, " ")
{
	hram_datalog(HRAM_READ_CYCLE_NUM);
}

UINT64_VARIABLE(__DutMask__, 0x4, "Specify the DUT mask for the next execution of the Sequence Table") {}

void log_act_summary();

// For changing SequenceTables interactively from the UserVariables tool
static CString filter_and_sort_seq_table_list(CString str)
{
	// Extract all Sequence Table names
	std::string input(str);
	std::istringstream ss(input);
	std::string name;

	std::vector< std::string > names;
	while (std::getline(ss, name, ','))
	{
		names.push_back(name);
	}

	// Sort alphabetically
	std::sort(names.begin(), names.end());  //std does not have sort - bryan 050521

	// Re-assemble comma-seperated string
	std::string ret;
	for (auto& it = names.begin(); it != names.end(); ++it)
	{
		ret += *it;
		if (it != names.end()) ret += ',';
	}

	// Convert to CString and return
	CString ret_cstr = ret.c_str();
	return ret_cstr;
}
ONEOF_VARIABLE(__SelectFlow__, filter_and_sort_seq_table_list(resource_all_names(S_SequenceTable)), "Select your Sequence Table!")
{
	// Check if sequence table is already runnning
	if (sequence_table_running)
	{
		warning("You can't swap out a Sequence Table while it is executing!");
		return;
	}
	// Swap out Sequence Table
	resource_deallocate(S_SequenceTable);
	SequenceTable_use(__SelectFlow__);
	resource_initialize(S_SequenceTable);

	filter_and_sort_seq_table_list(resource_all_names(S_SequenceTable));
}

DIMM_ASSIGNMENTS(da)
{
	DIMM_ALLOCATE(ECR, 1)
	DIMM_ALLOCATE(DBM, 1)		// use DIMM_ALLOCATE(DBM, 0) for onboard dram
}

PRE_INITIALIZATION_HOOK(pih)
{
	DWORD version = 0;

	//USE_CONFIGURATION(CONF_4site_Mag7_Training)
}

CONFIGURATION(CONF_4site_Mag7_Training)
{
	if (OnSite()) interface_signals_config_set(enMICModesMag7::t_two_I2C_mode);

	const int sitesPerDSA = 4;
	const int siteWithinDSA = (site_num() - 1) % sitesPerDSA + 1;

	bool b_non_shared = (siteWithinDSA == 1) || (siteWithinDSA == 2); //true if site is 1 or 2

	if (b_non_shared)
	{
		output("PA__NoShare_Mag7_Training");
		USE_PIN_ASSIGNMENTS(PA__NoShare_Mag7_Training);
		set_value(__DutMask__, 0xf);
	}
	else
	{
		output("PA__Share_Mag7_Training");
		USE_PIN_ASSIGNMENTS(PA__Share_Mag7_Training);
		set_value(__DutMask__, 0xff);
	}

	// Select desired SequenceTable
	{
		USE_SEQUENCE_TABLE(ONFI_seq);
		dbm_boc_mode = enDbmUINumberMag7::t_UI8;
	}
	USE_CURRENT_SHARE(ONFI_X_Share);
	USE_PIN_SCRAMBLE( ONFI_PINSCRAMBLE );
}

//CONFIGURATION(CONF_4site_Mag7_with_TDR)
//{
//	if (OnSite()) interface_signals_config_set(enMICModesMag7::t_two_I2C_mode);
//
//	b_run_tdr = true;	//enable run tdr from TDR_BLOCK
//
//	const int sitesPerDSA = 4;
//	const int siteWithinDSA = (site_num() - 1) % sitesPerDSA + 1;
//
//	bool b_non_shared = (siteWithinDSA == 1) || (siteWithinDSA == 2); //true if site is 1 or 2
//
//	if (b_non_shared)
//	{
//		output("PA__NoShare_Mag7_Training");
//		USE_PIN_ASSIGNMENTS(PA__NoShare_Mag7_Training);
//		set_value(__DutMask__, 0xf);
//	}
//	else
//	{
//		output("PA__Share_Mag7_Training");
//		USE_PIN_ASSIGNMENTS(PA__Share_Mag7_Training);
//		set_value(__DutMask__, 0xff);
//	}
//
//	// Select desired SequenceTable
//	USE_SEQUENCE_TABLE(ONFI_seq);
//	USE_CURRENT_SHARE(ONFI_X_Share);
//	USE_PIN_SCRAMBLE(ONFI_PINSCRAMBLE);
//}

CONFIGURATION(CONF_2site_Mag7_Training)
{
	if (OnSite()) interface_signals_config_set(enMICModesMag7::t_two_I2C_mode);

	const int sitesPerDSA = 2;
	const int siteWithinDSA = (site_num() - 1) % sitesPerDSA + 1;

	bool b_non_shared = (siteWithinDSA == 1); //true if site is 1 

	if (b_non_shared)
	{
		USE_PIN_ASSIGNMENTS(PA__NoShare_Mag7_Training);
		set_value(__DutMask__, 0xf);
	}
	else
	{
		USE_PIN_ASSIGNMENTS(PA__Share_Mag7_Training);
		set_value(__DutMask__, 0xff);
	}

	// Select desired SequenceTable
	{
		USE_SEQUENCE_TABLE(ONFI_seq);
		dbm_boc_mode = enDbmUINumberMag7::t_UI8;
	}
	//{
	//	USE_SEQUENCE_TABLE(ONFI_BOCx4_seq);
	//	dbm_boc_mode = enDbmUINumberMag7::t_UI4;
	//}
	USE_CURRENT_SHARE(ONFI_X_Share);
	USE_PIN_SCRAMBLE(ONFI_PINSCRAMBLE);
}

//CONFIGURATION(CONF_2site_Mag7_Training_with_TDR)
//{
//	if (OnSite()) interface_signals_config_set(enMICModesMag7::t_two_I2C_mode);
//
//	b_run_tdr = true;	//enable run tdr from TDR_BLOCK
//
//	const int sitesPerDSA = 2;
//	const int siteWithinDSA = (site_num() - 1) % sitesPerDSA + 1;
//
//	bool b_non_shared = (siteWithinDSA == 1); //true if site is 1 
//
//	if (b_non_shared)
//	{
//		USE_PIN_ASSIGNMENTS(PA__NoShare_Mag7_Training);
//		set_value(__DutMask__, 0xf);
//	}
//	else
//	{
//		USE_PIN_ASSIGNMENTS(PA__Share_Mag7_Training);
//		set_value(__DutMask__, 0xff);
//	}
//
//	// Select desired SequenceTable
//		// Select desired SequenceTable
//	{
//		USE_SEQUENCE_TABLE(ONFI_seq);
//		dbm_boc_mode = enDbmUINumberMag7::t_UI8;
//	}
//	//USE_SEQUENCE_TABLE(ONFI_seq);
//	USE_CURRENT_SHARE(ONFI_X_Share);
//	USE_PIN_SCRAMBLE(ONFI_PINSCRAMBLE);
//}

#if defined WD_DEBUG
CONFIGURATION(CONF_WatchDog)
{
	if (OnSite()) interface_signals_config_set(enMICModesMag7::t_two_I2C_mode);

	const int sitesPerDSA = 2;
	const int siteWithinDSA = (site_num() - 1) % sitesPerDSA + 1;

	bool b_non_shared = (siteWithinDSA == 1); //true if site is 1 

	if (b_non_shared)
	{
		USE_PIN_ASSIGNMENTS(PA__NoShare_Mag7_Training);
		set_value(__DutMask__, 0xf);
	}
	else
	{
		USE_PIN_ASSIGNMENTS(PA__Share_Mag7_Training);
		set_value(__DutMask__, 0xff);
	}

	// Select desired SequenceTable
	{
		USE_SEQUENCE_TABLE(WatchDog_seq);
		dbm_boc_mode = enDbmUINumberMag7::t_UI8;
	}
	//{
	//	USE_SEQUENCE_TABLE(ONFI_BOCx4_seq);
	//	dbm_boc_mode = enDbmUINumberMag7::t_UI4;
	//}
	USE_CURRENT_SHARE(ONFI_X_Share);
	USE_PIN_SCRAMBLE(ONFI_PINSCRAMBLE);
}
#endif

void TestBlock_Formatting(BOOL& after, int& result)
{
	if (!after) {
		output("<u>TEST_BLOCK <b>%s</b></u>", current_test_block());
	} else {
		//if we need a footer, put it here
	}
	return;
}

SITE_BEGIN_BLOCK(sb_NAND)
{
	numx(24);
	numy(15);

	output("numx %d", numx());
	output("numy %d", numy());
	output("numz %d", numz());

	//data_strobe(0xffff);

	x_fast_axis(TRUE);
	pin_connect(pinlist::AllPins());
	//pe_driver_rails_set(enDrvRailsNumberMag7::t_DRV_RANGE2);
	levels::power_down();

	if (!SimulationMode())
	{
		read_binary(std::string(__BinaryFile__), g_buffer1);

		i2c::disconnect_all_vcc_vccq_caps();

		dbm_load(dbm_boc_mode);
	}

	hram_stv_mode_set(STV_MODES::STV_OR_TRIGGER_ARM);
	install_debug_hook(TestBlock_Formatting, dhp_before|dhp_after, dhp_test_block);

	if (IsFLS())
	{
		output("fls fpga is loaded");
		FTM = new FlsTaskManager;
	}
}

TDR_BLOCK( Mag7_TDR )
{
	//CString TDR_folder = "ManualTDR";

	//output("Reading TDR data from folder %s...", (LPCSTR)TDR_folder);
	//if (db_read_tdr_files(TDR_folder) == 0)   // TDR data not located may trigger later special DxCal programming task
	//{
	//	output("<COLOR=RED>** Error: NO TDR DATA FOUND **</COLOR>");
	//}

	if (b_run_tdr)
	{
		output("Measuring TDR data");
		db_tdr(pinlist::AllPins(), 0 NS, 7 NS, t_tdr_mode_portable);								//to measure the PE pins you care about
		output("Saving TDR data to NVM");
		if (db_write_tdr() == FALSE)
		{
			warning("db_write_tdr: mismatch between data written to NVM and data readback");
		}

		//write tdr to file
		//db_write_tdr_files("device_dsa_tdr_files", TRUE);
	}	
	else
	{
		//read tdr from nvm
		output("Reading TDR data from NVM");
		if (db_read_tdr() == FALSE)
		{
			warning("db_read_tdr: data read from NVM is invalid");
		}
		//db_read_tdr_files("device_dsa_tdr_files");
	}

	output("TDR_BLOCK done");
}

BEFORE_TESTING_BLOCK( btb )
{
	// Set the Ignored DUT Mask
	ignored_duts_enable(~__DutMask__, FALSE);
	active_duts_enable(__DutMask__);

	levels::power_down();

	load_default_formats();	//load format ram
	
	//Bring up and configure FLS
	FTM->config(&NandFlsConfig_x8);

	//turn off background function
	bckfen(BckOperation::xor, enAddrTesterFuncMag7::t_x0, enAddrTesterFuncMag7::t_x0);

	dbm_segment_set(uv_dbm_segment);

	pe_driver_mode_set(pinlist::AllReadyBusyPins(), enPEDriverModeMag7::t_VTT);

	Eall_latched_ignore(TRUE);		//BOE - do not branch on latched errors

	//some duts cannot drive a 50 ohm termination on RB (t_VTT)
	//for those type of duts, we can set the channel to a high impedance input with enum t_VHIZ
	//t_VHIZ will work better with a pullup on RB, otherwise may have to reduce vol/voh 
	pe_driver_mode_set(pinlist::AllReadyBusyPins(), uv_rb_vhiz ? enPEDriverModeMag7::t_VHIZ : enPEDriverModeMag7::t_VTT);

	// Sequence Table exection has begun
	sequence_table_running = TRUE;
}

AFTER_TESTING_BLOCK(atb)
{
	levels::power_down();

	disable_src_sync();

	//FTM->shutdown();

	// Sequence Table exection is about to finish
	sequence_table_running = FALSE;

	TIME_SUMMARY();
}


///////////////////////////////////////