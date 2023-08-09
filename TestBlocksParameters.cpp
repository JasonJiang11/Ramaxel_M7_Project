#include "stdafx.h"
#include "DefineCounters.h"
#include "PatternInitializations.h"
#include "Onfi4p2Structs.h"
#include "file_io.h"
#include "FlsUserTasks.h"

static ParameterPageData param_data;
static Int64Array param_data_jamram;

//Read Parameter data
CSTRING_VARIABLE(__ReadParametersFile__, "..//binaries//read_paramter.bin", " ") {}

//get Read Parameter data from binary file and store in static global param_data
void read_param_data_from_file()
{
	std::vector<BYTE> v_binary_params;

	read_binary(std::string(__ReadParametersFile__), v_binary_params);

	//copy to Int64Array
	param_data_jamram.SetSize(v_binary_params.size());
	std::copy(v_binary_params.begin(), v_binary_params.end(), param_data_jamram.GetData());

	//store to ParameterPageData variabble
	param_data.unpack(v_binary_params);
}

//load jamram with Read Parameter data
static void load_jamram_read_param()
{
	// Write the address byte and parameter bytes to JAM RAM
	apg_jam_mode_set(ApgJamMode::t_jam_mode_ram); // configure JAM as RAM (instead of reg)
	apg_jam_ram_set(0x00, param_data_jamram); // Write data to JAM RAM
	apg_jam_ram_address_set(0x3FFF); // reset address pointer

	// Load the num bytes MAR counter
	count(C_DATA_OUT_NUM_BYTES, static_cast<int>(param_data_jamram.GetCount() - 1));
}

//execute Read Parameters command and compare with 0s
TEST_BLOCK(TB__ReadParameters)
{
	//init apg registers
	set_counters_init();

	//load jamram with read param data
	//load_jamram_read_param();

	//set dmain to zero on all apgs and subdgens
	//set xmain/ymain to 0 on all apgs
	//init apg count registers
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_xmain_set(apg, 0);
		for (auto dg = enUnitNumberMag7::t_UNIT1; dg <= enUnitNumberMag7::t_UNIT4; ++dg)
		{
			apg_dmain_set(apg, dg, 0);
		}
	}

	FlsStoreRegCnt fls_capture_data;
	fls_capture_data.funtest(PAT__ReadParameters);

	std::vector< ParameterPageData> perDutParameters;
	perDutParameters.resize(max_dut() + 1);
	SOFT_DUT_LOOP(dut) {
		output("<b>PARAMETER PAGE DATA for DUT%d</b>", dut + 1);
		ParameterPageData pd(fls_capture_data.DutStore[dut].registers.bank[0].param);
		pd.dump();
	}
	//output("%s", fls_capture_data.param_dump().c_str());

	return MULTI_DUT;
}

//execute Read Parameters command and compare with 0s
TEST_BLOCK(TB__ReadParameters_BOCx4)
{
	output("%s", current_test_block());

	//init apg registers
	set_counters_init();

	//load jamram with read param data
	//load_jamram_read_param();

	//set dmain to zero on all apgs and subdgens
	//set xmain/ymain to 0 on all apgs
	//init apg count registers
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_xmain_set(apg, 0);
		for (auto dg = enUnitNumberMag7::t_UNIT1; dg <= enUnitNumberMag7::t_UNIT4; ++dg)
		{
			apg_dmain_set(apg, dg, 0);
		}
	}

	FlsStoreRegCnt fls_capture_data;
	fls_capture_data.funtest(PAT__ReadParameters_x4);
	output("%s", fls_capture_data.param_dump().c_str());

	return MULTI_DUT;
}