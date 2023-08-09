#include "stdafx.h"
#include "DefineCounters.h"
#include "PatternResources.h"
#include "PatternInitializations.h"
#include "Datalogs.h"
#include "file_mgmt.h"
#include "time.h"
#include "Timings.h"
#include "Levels.h"
#include "DeviceStatus.h"
#include "DeviceSpecs.h"
#include "TestBlocks.h"
#include "dbm.h"

const DWORD UNUSED		= 0xffff;
const int JAMRAM_SIZE	= 1024;
const int EBM_SIZE		= 4096;

std::vector<char> g_buffer1;

DWORD_VARIABLE( uv_bytes_in_page,	0x4000, " ") {}

DWORD_VARIABLE( uv_page,			0, " ") {}

//user variables for single plane ERP patterns
DWORD_VARIABLE( uv_block,	0, " ") {}
DWORD_VARIABLE( uv_plane,	0, " ") {}
DWORD_VARIABLE( uv_lun,		0, " ") {}
DWORD_VARIABLE(uv_page_payload_byte, 0x55aa55aa, " ") {}

//used to provide more flexible patterns with each ui unique
UINT64_VARIABLE(uv_page_payload_byte_bocx8, 0x5a5a5a5a5a5a5a5a, " ") {}

//user variables for multi plane ERP patterns
DWORD_VARIABLE(uv_block_pln0, 0, " ") {}
DWORD_VARIABLE(uv_block_pln1, 0, " ") {}
DWORD_VARIABLE(uv_block_pln2, 0, " ") {}
DWORD_VARIABLE(uv_block_pln3, 0, " ") {}
DWORD_VARIABLE(uv_page_payload_byte_pln0, 0x55aa55aa, " ") {}
DWORD_VARIABLE(uv_page_payload_byte_pln1, 0x55aa55aa, " ") {}
DWORD_VARIABLE(uv_page_payload_byte_pln2, 0x55aa55aa, " ") {}
DWORD_VARIABLE(uv_page_payload_byte_pln3, 0x55aa55aa, " ") {}

//dps ripple settings
DOUBLE_VARIABLE(uv_ripple_freq, 1000, " ") {}
DOUBLE_VARIABLE(uv_ripple_volts, 100, "uv_ripple_volts(MV) ") {}

//load jamram with random data from file
//can only load up to max size of jamram which is 1024
void load_jam_ram(std::vector<char> buf, enPinModesMag7 mode)
{
	int num_apg			= 2;	//number of apgs per cycle
	int units_per_apg	= 0;	//units used per apg depends on mode
	int dat_per_addr	= 0;	//bytes per unit
	int byte_per_addr	= 0;	//bytes per jamram address

	enUnitNumberMag7 end_unit;

	if(mode == enPinModesMag7::t_PM_BOCx1)
	{
		units_per_apg = 1;
		dat_per_addr = 1;
		end_unit = enUnitNumberMag7::t_UNIT1;
	}
	if(mode == enPinModesMag7::t_PM_BOCx2)
	{
		units_per_apg = 1;
		dat_per_addr = 2;
		end_unit = enUnitNumberMag7::t_UNIT1;
	}
	if (mode == enPinModesMag7::t_PM_BOCx4)
	{
		units_per_apg = 2;
		dat_per_addr = 2;
		end_unit = enUnitNumberMag7::t_UNIT2;
	}
	if( mode ==	enPinModesMag7::t_PM_BOCx8)
	{
		units_per_apg = 4;
		dat_per_addr = 2;
		end_unit = enUnitNumberMag7::t_UNIT4;
	}

	byte_per_addr = dat_per_addr * units_per_apg * num_apg;

	auto jamram_addr_cnt = buf.size() / byte_per_addr;
	if (jamram_addr_cnt > JAMRAM_SIZE)
	{
		output("load_jam_ram: too big for jamram, loading first %dK", byte_per_addr);
		jamram_addr_cnt = JAMRAM_SIZE;
	}

	//for (auto addr = 0; addr < jamram_addr_cnt; ++addr)
	//{
	//	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	//	{
	//		for (auto unit = enUnitNumberMag7::t_UNIT1; unit <= end_unit; ++unit)
	//		{
	//			int apg_idx		= static_cast<int>(apg) - 1;
	//			int unit_idx	= static_cast<int>(unit) - 1;

	//			auto idx = (addr * num_apg * units_per_apg + apg_idx * units_per_apg + unit_idx) * dat_per_addr;

	//			int64 lsb = buf[idx] & 0xff;
	//			int64 msb = buf[idx+1] & 0xff;

	//			apg_jam_ram_set(addr, apg, unit, (msb << 8) | lsb);
	//		}
	//	}
	//}

	Int64Array jamram_data;
	jamram_data.SetSize(jamram_addr_cnt);

	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		for (auto unit = enUnitNumberMag7::t_UNIT1; unit <= end_unit; ++unit)
		{
			for (auto addr = 0; addr < jamram_addr_cnt; ++addr)
			{
				int apg_idx = static_cast<int>(apg) - 1;
				int unit_idx = static_cast<int>(unit) - 1;

				auto idx = (addr * num_apg * units_per_apg + apg_idx * units_per_apg + unit_idx) * dat_per_addr;

				int64 lsb = buf[idx] & 0xff;
				int64 msb = buf[idx + 1] & 0xff;

				//jamram_data[addr] = (msb << 8) | lsb;
				jamram_data[addr] = (msb << 24) | (lsb << 16) | (msb << 8) | lsb;

				//apg_jam_ram_set(addr, apg, unit, (msb << 8) | lsb);
			}
			apg_jam_ram_set(0, apg, unit, jamram_data);
		}
	}

	//set jamram address
	output("apg_jam_ram_pointer_address_set");
	apg_jam_ram_pointer_address_set(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1, JAMRAM_SIZE-1);	//address will wraparound to 0
	apg_jam_ram_pointer_address_set(enApgNumberMag7::t_APG2, enJamRamPointerNumberMag7::t_JRP1, JAMRAM_SIZE-1);	//address will wraparound to 0

	//select address pointers for APG1/2
	output("apg_jam_ram_active_pointer_select_set");
	apg_jam_ram_active_pointer_select_set(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1);
	apg_jam_ram_active_pointer_select_set(enApgNumberMag7::t_APG2, enJamRamPointerNumberMag7::t_JRP1);
}

//this is a test and example of ebm functionality
// this functions supports ebm 1-bit modes such as data replacement, mask mode and 2-bit modes such as mask and replace mode.
//each element of the user array has the same number of bits as the input pinlist
//each bit number of the user array element maps to the corresponding same pin index in the pinlist. 
//	eg bit 0 maps to pin index 0 of the pinlist, bit 1 maps to pin index 1 of the pinlist, ..
//		bit 0 has data for pin index 0 of the pinlist
// for 1 bit modes and each element in the user array is for 1 ui
// in 2 bit modes suach as mask and replace, even elements of the user array are data replace bits and the odd elements are mask bits
void load_ebm(DutNum dut, PinList* ebm_pinl, std::vector<__int64>& user_data, enPinModesMag7 pin_mode, enEbmDataModeMag7 ebm_mode)
{
	CArray<ebm_pin_data, ebm_pin_data> ebm_data;	//each element of the array has data for one pin in the pinlist

	//how many pins in the pinlist?
	int pins_in_pinl;
	DutPin* dutpin;
	for (pins_in_pinl = 0; pin_info(ebm_pinl, pins_in_pinl, &dutpin); ++pins_in_pinl) {};
	//output("pins_in_pinl=%d", pins_in_pinl);

	ebm_data.SetSize(pins_in_pinl);

	int ui_per_boc;
	bool _1bit_ebm_mode = (ebm_mode == enEbmDataModeMag7::t_strobe_masking) || (ebm_mode == enEbmDataModeMag7::t_data_replacement) || (ebm_mode == enEbmDataModeMag7::t_data_masking) || (ebm_mode == enEbmDataModeMag7::t_register_write);
	int user_idx_per_ui = _1bit_ebm_mode ? 1 : 2; //in 1 bit modes each element of user array represents 1 ui. in 2 bit modes 2 elements of the user array are for 1 ui
	if (pin_mode == enPinModesMag7::t_PM_BOCx1)	ui_per_boc = 1;
	if (pin_mode == enPinModesMag7::t_PM_BOCx2)	ui_per_boc = 2;
	if (pin_mode == enPinModesMag7::t_PM_BOCx4)	ui_per_boc = 4;
	if (pin_mode == enPinModesMag7::t_PM_BOCx8)	ui_per_boc = 8;

	const int bits_in___int64 = 64;
	const int bits_in___int32 = 32;

	for (auto user_idx = 0, ebm_idx = 0; user_idx < user_data.size(); user_idx += bits_in___int64, ++ebm_idx)
	{
		for (auto pin_idx = 0; pin_info(ebm_pinl, pin_idx, &dutpin); ++pin_idx)
		{
			ebm_data[pin_idx].data[ebm_idx] = 0;

			for (auto bit = 0; bit < bits_in___int32; bit += ui_per_boc * user_idx_per_ui)
			{
				for (auto ui_in_boc = 0; ui_in_boc < ui_per_boc; ++ui_in_boc)
				{
					ebm_data[pin_idx].data[ebm_idx] |= ((user_data[user_idx + 2 * bit + user_idx_per_ui * ui_in_boc						] >> pin_idx) & 0x1) << (bit + ui_in_boc);					//BOCA
					if (_1bit_ebm_mode == false)
					{
					ebm_data[pin_idx].data[ebm_idx] |= ((user_data[user_idx + 2 * bit + user_idx_per_ui * ui_in_boc + 1					] >> pin_idx) & 0x1) << (bit + ui_in_boc + ui_per_boc);		//BOCA
					}
				}
				for (auto ui_in_boc = 0; ui_in_boc < ui_per_boc; ++ui_in_boc)
				{
					ebm_data[pin_idx].data[ebm_idx] |= ((user_data[user_idx + 2 * bit + user_idx_per_ui * ui_in_boc + ui_per_boc		] >> pin_idx) & 0x1) << (bit + 32 + ui_in_boc);				//BOCB
					if (_1bit_ebm_mode == false)
					{
					ebm_data[pin_idx].data[ebm_idx] |= ((user_data[user_idx + 2 * bit + user_idx_per_ui * ui_in_boc + ui_per_boc + 1	] >> pin_idx) & 0x1) << (bit + 32 + ui_in_boc + ui_per_boc);//BOCB
					}
				}
			}
		}
	}

	for (auto dpin = 0; dpin < pins_in_pinl; ++dpin)
	{
		ebm_data[dpin].count = static_cast<int>(user_data.size()) / 2;  //divide by 2 because 2 user array locations (one for boca and one for bocb) are loaded into 1 ebm address
		ebm_data[dpin].startAddress = 0;		//address starts at 0
	}

	//for (auto i = 0; i < ebm_data[0].count; ++i)
	//{
	//	output("0 0x%016I64x", ebm_data[0].data[i]);
	//}

	ebm_column_write(dut, ebm_pinl, ebm_data);
}

//this function loads the aux registers for use with the ERP patterns
//it supports:
//	single plane (arg multi_plane = false) 
//	multiplane (arg multi_plane = true) 
//for single plane, the aux registers are loaded as follows:
//AUX1 register Y_PLANE_0 has initial y address
//AUX2 register X_PLANE_0 has initial x address
//AUX3 register DATA_PLANE_0 has byte data to write/read to/from page
//
//for multiplane the aux registers are loaded as follow:
//AUX1 register Y_PLANE_0 has initial y address for plane 0
//AUX2 register X_PLANE_0 has initial x address for plane 0
//AUX3 register DATA_PLANE_0 has byte data to read from page in plane 0
//AUX4 register Y_PLANE_1 has initial y address for plane 1
//AUX5 register X_PLANE_1 has initial x address for plane 1
//AUX6 register DATA_PLANE_1 has byte data to read from page in plane 1
//AUX7 register Y_PLANE_2 has initial x address for plane 2
//AUX8 register X_PLANE_2 has initial y address for plane 2
//AUX9 register DATA_PLANE_2 has byte data to read from page in plane 2
//AUX10 register Y_PLANE_3 has initial x address for plane 3
//AUX11 register X_PLANE_3 has initial y address for plane 3
//AUX12 register DATA_PLANE_3 has byte data to read from page in plane 3
static void erp_apg_init(DWORD page, DWORD pln0_block, DWORD pln1_block, DWORD pln2_block, DWORD pln3_block, DWORD lun, DWORD pln0_byte_data, DWORD pln1_byte_data, DWORD pln2_byte_data, DWORD pln3_byte_data, bool slc_mode, bool multi_plane, enPinModesMag7 mode = enPinModesMag7::t_PM_BOCx8)
{
	//init apg count registers
	set_counters_init(mode);

	count(C_SLC_MODE, static_cast<int>(slc_mode));

	int boc_idx;	//used for y indexing in apg2

	if (mode == enPinModesMag7::t_PM_BOCx1) boc_idx = 1;
	if (mode == enPinModesMag7::t_PM_BOCx2) boc_idx = 2;
	if (mode == enPinModesMag7::t_PM_BOCx4) boc_idx = 4;
	if (mode == enPinModesMag7::t_PM_BOCx8) boc_idx = 8;

	if (pln0_block != UNUSED)
	{
		//init x/y, and data for plane 0
		//y address in aux1
		apg_auxreg_set(enApgNumberMag7::t_APG1, enAuxRegNumberMag7::t_AUXREG1, 0);			//Y_PLANE_0 in pattern, apg1
		apg_auxreg_set(enApgNumberMag7::t_APG2, enAuxRegNumberMag7::t_AUXREG1, boc_idx);	//Y_PLANE_0 in pattern, apg2

		//x address load in to aux2 apg1
		apg_auxreg_set(enApgNumberMag7::t_APG1, enAuxRegNumberMag7::t_AUXREG2, set_addr_xalu(lun, pln0_block, 0, page));	//X_PLANE_0 in pattern
		apg_auxreg_set(enApgNumberMag7::t_APG2, enAuxRegNumberMag7::t_AUXREG2, set_addr_xalu(lun, pln0_block, 0, page));	//X_PLANE_0 in pattern

		if (pln0_byte_data != UNUSED)
		{
			//byte data load in to aux3 all apgs
			for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
			{
				apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, pln0_byte_data);		//DATA_PLANE_0 in pattern
			}
		}
	}

	if (pln1_block != UNUSED)
	{
		//init x/y, and data for plane 1
		//y address in aux1
		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG4 : enAuxRegNumberMag7::t_AUXREG1, 0);		//Y_PLANE_1 in pattern
		apg_auxreg_set(enApgNumberMag7::t_APG2, enAuxRegNumberMag7::t_AUXREG1, boc_idx);	//Y_PLANE_0 in pattern, apg2

		//x address load in to aux2 apg1
		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG5 : enAuxRegNumberMag7::t_AUXREG2, set_addr_xalu(lun, pln1_block, 1, page));	//X_PLANE_1 in pattern
		if (pln1_byte_data != UNUSED)
		{
			//byte data load in to aux3 apg1/2
			for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
			{
				apg_auxreg_set(apg, multi_plane ? enAuxRegNumberMag7::t_AUXREG6 : enAuxRegNumberMag7::t_AUXREG3, pln1_byte_data);		//DATA_PLANE_1 in pattern
			}
		}
	}

	if (pln2_block != UNUSED)
	{
		//init x/y, and data for plane 2
		//y address in aux1
		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG7 : enAuxRegNumberMag7::t_AUXREG1, 0);		//Y_PLANE_2 in pattern
		//x address load in to aux2 apg1
		apg_auxreg_set(enApgNumberMag7::t_APG2, enAuxRegNumberMag7::t_AUXREG1, boc_idx);	//Y_PLANE_0 in pattern, apg2

		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG8 : enAuxRegNumberMag7::t_AUXREG2, set_addr_xalu(lun, pln2_block, 2, page));	//X_PLANE_2 in pattern
		if (pln2_byte_data != UNUSED)
		{
			//byte data load in to aux2 apg1
			for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
			{
				apg_auxreg_set(apg, multi_plane ? enAuxRegNumberMag7::t_AUXREG9 : enAuxRegNumberMag7::t_AUXREG3, pln2_byte_data);		//DATA_PLANE_2 in pattern
			}
		}
	}

	if (pln3_block != UNUSED)
	{
		//init x/y, and data for plane 3
		//y address in aux1
		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG10 : enAuxRegNumberMag7::t_AUXREG1, 0);		//Y_PLANE_3 in pattern
		//x address load in to aux2 apg1
		apg_auxreg_set(enApgNumberMag7::t_APG2, enAuxRegNumberMag7::t_AUXREG1, boc_idx);	//Y_PLANE_0 in pattern, apg2

		apg_auxreg_set(enApgNumberMag7::t_APG1, multi_plane ? enAuxRegNumberMag7::t_AUXREG11 : enAuxRegNumberMag7::t_AUXREG2, set_addr_xalu(lun, pln3_block, 3, page));	//X_PLANE_3 in pattern
		if (pln3_byte_data != UNUSED)
		{
			//byte data load in to aux2 apg1/2
			for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
			{
				apg_auxreg_set(apg, multi_plane ? enAuxRegNumberMag7::t_AUXREG12 : enAuxRegNumberMag7::t_AUXREG3, pln3_byte_data);		//DATA_PLANE_3 in pattern
			}
		}
	}
}

//single plane erase apg init 
static void sp_erase_apg_init( DWORD plane, DWORD block, DWORD lun, bool slc_mode = true)
{
	switch (plane)
	{
	case 0:
		erp_apg_init(0, block, UNUSED, UNUSED, UNUSED, lun, UNUSED, UNUSED, UNUSED, UNUSED, slc_mode, false);
		break;
	case 1:
		erp_apg_init(0, UNUSED, block, UNUSED, UNUSED, lun, UNUSED, UNUSED, UNUSED, UNUSED, slc_mode, false);
		break;
	case 2:
		erp_apg_init(0, UNUSED, UNUSED, block, UNUSED, lun, UNUSED, UNUSED, UNUSED, UNUSED, slc_mode, false);
		break;
	case 3:
		erp_apg_init(0, UNUSED, UNUSED, UNUSED, block, lun, UNUSED, UNUSED, UNUSED, UNUSED, slc_mode, false);
		break;
	}
}

//multi plane erase apg init 
static void mp_erase_apg_init( DWORD pln0_block, DWORD pln1_block, DWORD pln2_block, DWORD pln3_block, DWORD lun, bool slc_mode = true)
{
	erp_apg_init(0, pln0_block, pln1_block, pln2_block, pln3_block, lun, UNUSED, UNUSED, UNUSED, UNUSED, slc_mode, true);
}

//single plane prog/read apg init
//this function loads the aux registers for use with the single plane ERP patterns
//AUX1 register Y_PLANE_0 has initial y address for plane 0
//AUX2 register X_PLANE_0 has initial x address for plane 0
//AUX3 register DATA_PLANE_0 has byte data to read from page in plane 0
void sp_prog_read_apg_init(DWORD page, DWORD plane, DWORD block, DWORD lun, DWORD byte_data, bool slc_mode, enPinModesMag7 mode)
{
	switch (plane)
	{
	case 0:
		erp_apg_init(page, block, UNUSED, UNUSED, UNUSED, lun, byte_data, UNUSED, UNUSED, UNUSED, slc_mode, false, mode);
		break;
	case 1:
		erp_apg_init(page, UNUSED, block, UNUSED, UNUSED, lun, UNUSED, byte_data, UNUSED, UNUSED, slc_mode, false, mode);
		break;
	case 2:
		erp_apg_init(page, UNUSED, UNUSED, block, UNUSED, lun, UNUSED, UNUSED, byte_data, UNUSED, slc_mode, false, mode);
		break;
	case 3:
		erp_apg_init(page, UNUSED, UNUSED, UNUSED, block, lun, UNUSED, UNUSED, UNUSED, byte_data, slc_mode, false, mode);
		break;
	}
}

//multi plane prog/read apg init
static void mp_prog_read_apg_init(DWORD page, DWORD pln0_block, DWORD pln1_block, DWORD pln2_block, DWORD pln3_block, DWORD lun, DWORD pln0_byte_data, DWORD pln1_byte_data, DWORD pln2_byte_data, DWORD pln3_byte_data, bool slc_mode = true)
{
	erp_apg_init(page, pln0_block, pln1_block, pln2_block, pln3_block, lun, pln0_byte_data, pln1_byte_data, pln2_byte_data, pln3_byte_data, slc_mode, true);
}

PinList *shm_pinl = pinlist::AllDataPins();

ONEOF_VARIABLE(uv_shm_pinl, "AllDataPins, pl_DQ0, pl_DQ1, pl_DQ2, pl_DQ3, pl_DQ4, pl_DQ5, pl_DQ6, pl_DQ7", " ")
{
	if (uv_shm_pinl == "AllDataPins")	shm_pinl = pinlist::AllDataPins();
	if (uv_shm_pinl == "pl_DQ0")		shm_pinl = pinlist::pl_DQ0();
	if (uv_shm_pinl == "pl_DQ1")		shm_pinl = pinlist::pl_DQ1();
	if (uv_shm_pinl == "pl_DQ2")		shm_pinl = pinlist::pl_DQ2();
	if (uv_shm_pinl == "pl_DQ3")		shm_pinl = pinlist::pl_DQ3();
	if (uv_shm_pinl == "pl_DQ4")		shm_pinl = pinlist::pl_DQ4();
	if (uv_shm_pinl == "pl_DQ5")		shm_pinl = pinlist::pl_DQ5();
	if (uv_shm_pinl == "pl_DQ6")		shm_pinl = pinlist::pl_DQ6();
	if (uv_shm_pinl == "pl_DQ7")		shm_pinl = pinlist::pl_DQ7();
}

void shmoo_setup(enPinModesMag7 mode)
{
	//tornado shmoo is for non-source sync only
	if (SHM_DATAEYE || SHM_TORNADO)
	{
		if (is_ssync_enabled() == false)
		{
			if (mode == enPinModesMag7::t_PM_BOCx8) set_TS_DATA_OUT_x8(get_timing_mode(), enUICaptureMag7::t_UI_NONE, 0);
			if (mode == enPinModesMag7::t_PM_BOCx4) set_TS_DATA_OUT_x4(get_timing_mode(), enUICaptureMag7::t_UI_NONE, 0);
		}
		else
		{
			if (mode == enPinModesMag7::t_PM_BOCx8) set_TS_DATA_OUT_x8(get_timing_mode(), enUICaptureMag7::t_UI_NONE, 0);
			src_sync_dqs_dq_offset_set(pinlist::AllDataStrobePins(), uv_dqs_dq_offset PS);	
		}
	}
	if(SHM_DATAEYE)
	{
		voh(-1 V, pinlist::AllDataPins());
		vol( 3 V, pinlist::AllDataPins());
		output("shm_pinl = %s", resource_name(shm_pinl));
		voh(uv_voh V, shm_pinl);
		vol(uv_vol V, shm_pinl);
	}
}

void shmoo_restore(enPinModesMag7 mode)
{
	if (SHM_DATAEYE || SHM_TORNADO)
	{
		if (is_ssync_enabled() == false)
		{
			restore_timing(get_timing_mode(), mode);
		}
		else
		{
			uv_dqs_dq_offset = 0;
			src_sync_dqs_dq_offset_set(pinlist::AllDataStrobePins(), uv_dqs_dq_offset PS);
		}
		levels::restore_volvoh();
	}
}

void dps_ripple_setup(double frequency_in_hz, double volts)
{
	dps_vpulse_mode_set(enVPulseModeMag7::t_Ripple);
	dps_ripple_set(frequency_in_hz /*Hz*/, volts);
}

//configure dbm for bocx4
//when considering test time, it is more efficient to have this code called one time in site_begin.
MULTI_DUT_CALL_BLOCK(TB__SetupDBM__BOCx4)
{
	dbm_load(enDbmUINumberMag7::t_UI4);
}

TEST_BLOCK(TB__EraseOneBlock_SinglePlane_SLC)
{
	output("%s", current_test_block());

	//const DWORD block = 0;
	//const DWORD plane = 0;
	//const DWORD lun = 0;

	//init apg registers
	sp_erase_apg_init(uv_block, uv_plane, uv_lun, true);

	int before_cnt = count(C_tBERS);
	funtest(PAT__EraseOneBlock_SinglePlane, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tBERS);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tBERS = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));
	output("count2 = %d", count(2));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ProgramSinglePage_SinglePlane_SLC_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	int before_cnt = count(C_tPROG);
	funtest(PAT__ProgramOnePage_SinglePlane_SLC_x4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tPROG);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("tPROG = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadSinglePage_SinglePlane_SLC_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	int before_cnt = count(C_tR);
	funtest(PAT__ReadOnePage_SinglePlane_x4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ProgramSinglePage_SinglePlane_SLC)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	int before_cnt = count(C_tPROG);
	funtest(PAT__ProgramOnePage_SinglePlane_SLC, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tPROG);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("tPROG = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadSinglePage_SinglePlane_SLC)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	int before_cnt = count(C_tR);
	funtest(PAT__ReadOnePage_SinglePlane, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

//program 1 block using dbm data
TEST_BLOCK(TB__ProgramBlock_SP_SLC)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	bckfen(BckOperation::and , enAddrTesterFuncMag7::t_x0, enAddrTesterFuncMag7::t_x0);

	int before_cnt = count(C_tPROG);
	funtest(PAT__Prog_Blk_SP_SLC, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tPROG);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tPROG = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadBlock_SP_SLC)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	bckfen(BckOperation::and, enAddrTesterFuncMag7::t_x0, enAddrTesterFuncMag7::t_x0);

	int before_cnt = count(C_tR);
	funtest(PAT__Read_Blk_SP, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

//program 1 block using dbm data
TEST_BLOCK(TB__ProgramBlock_SP_SLC_DBM)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	int before_cnt = count(C_tPROG);
	funtest(PAT__Prog_RND_Blk_SP_SLC, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tPROG);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("tPROG = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadBlock_SP_SLC_DBM)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	int before_cnt = count(C_tR);
	funtest(PAT__Read_RND_Blk_SP, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));
	output("COUNT50 %d", count(50));

	return MULTI_DUT;
}

//
//program 1 block using dbm data
TEST_BLOCK(TB__ProgramBlock_SP_SLC_BOCx4)
{
	output("%s", current_test_block());

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	bckfen(BckOperation::and, enAddrTesterFuncMag7::t_x0, enAddrTesterFuncMag7::t_x0);

	int before_cnt = count(C_tPROG);
	funtest(PAT__Prog_Blk_SP_SLC_BOCx4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadBlock_SP_SLC_BOCx4)
{
	output("%s", current_test_block());

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	bckfen(BckOperation::and, enAddrTesterFuncMag7::t_x0, enAddrTesterFuncMag7::t_x0);

	int before_cnt = count(C_tR);
	funtest(PAT__Read_Blk_SP_BOCx4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

//program 1 block using dbm data
TEST_BLOCK(TB__ProgramBlock_SP_SLC_DBM_BOCx4)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	int before_cnt = count(C_tPROG);
	funtest(PAT__Prog_RND_Blk_SP_SLC_BOCx4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tPROG);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("tPROG = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadBlock_SP_SLC_DBM_BOCx4)
{
	output("%s", current_test_block());

	dbm_segment_set(uv_dbm_segment);

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	int before_cnt = count(C_tR);
	funtest(PAT__Read_RND_Blk_SP_BOCx4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	int after_cnt = count(C_tR);
	int c_pipe = count(C_PIPE);

	double period;
	enPinModesMag7 pm;
	enUICaptureMag7 vui;
	BYTE vm;
	cycle(HSN_TSET(TS_RBSY), &period, &pm, &vui, &vm);

	output("C_tR = %3.0f US", (c_pipe * (before_cnt - after_cnt) * 2 * period) / (1 US));
	output("COUNT50 %d", count(50));

	return MULTI_DUT;
}
//

TEST_BLOCK(TB__EraseOneBlock_SinglePlane_TLC)
{
	//const DWORD block = 0;
	//const DWORD plane = 0;
	//const DWORD lun = 0;

	//init apg registers
	sp_erase_apg_init(uv_block, uv_plane, uv_lun, false);
	funtest(PAT__EraseOneBlock_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ProgramSinglePage_SinglePlane_TLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, false);
	funtest(PAT__ProgramOnePage_SinglePlane_SLC, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadSinglePage_SinglePlane_TLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, false);
	funtest(PAT__ReadOnePage_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__EraseOneBlock_MultiPlane_SLC)
{
	//const DWORD block = 0;
	//const DWORD plane = 0;
	//const DWORD lun = 0;

	//init apg registers
	mp_erase_apg_init(uv_block, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, true);
	funtest(PAT__EraseOneBlock_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ProgramSinglePage_MultiPlane_SLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	mp_prog_read_apg_init(uv_page, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, uv_lun, uv_page_payload_byte_pln0, uv_page_payload_byte_pln1, uv_page_payload_byte_pln2, uv_page_payload_byte_pln3, true);
	funtest(PAT__ProgramOnePage_SinglePlane_SLC, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadSinglePage_MultiPlane_SLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	mp_prog_read_apg_init(uv_page, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, uv_lun, uv_page_payload_byte_pln0, uv_page_payload_byte_pln1, uv_page_payload_byte_pln2, uv_page_payload_byte_pln3, true);
	funtest(PAT__ReadOnePage_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__EraseOneBlock_MultiPlane_TLC)
{
	//const DWORD block = 0;
	//const DWORD plane = 0;
	//const DWORD lun = 0;

	//init apg registers
	mp_erase_apg_init(uv_block, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, false);
	funtest(PAT__EraseOneBlock_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ProgramSinglePage_MultiPlane_TLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	mp_prog_read_apg_init(uv_page, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, uv_lun, uv_page_payload_byte_pln0, uv_page_payload_byte_pln1, uv_page_payload_byte_pln2, uv_page_payload_byte_pln3, false);
	funtest(PAT__ProgramOnePage_SinglePlane_SLC, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadSinglePage_MultiPlane_TLC)
{
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	mp_prog_read_apg_init(uv_page, uv_block_pln0, uv_block_pln1, uv_block_pln2, uv_block_pln3, uv_lun, uv_page_payload_byte_pln0, uv_page_payload_byte_pln1, uv_page_payload_byte_pln2, uv_page_payload_byte_pln3, false);
	funtest(PAT__ReadOnePage_SinglePlane, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

	output("PAT__WriteReadCache_x4");
	funtest(PAT__WriteReadCache_x4, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	output("xmain = 0x%08x", apg_xmain_get( enApgNumberMag7::t_APG1));

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("WriteReadCache pat");

	//int byte = 0;
	//for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	//{
	//	for (auto unit = enUnitNumberMag7::t_UNIT1; unit <= enUnitNumberMag7::t_UNIT4; ++unit, ++byte)
	//	{
	//		auto data_d0d7 = ((uv_page_payload_byte_bocx8 >> (8 * byte)) & 0xf) >> 0;
	//		auto data_d8df = ((uv_page_payload_byte_bocx8 >> (8 * byte)) & 0xf0) >> 4;
	//		auto data = (((data_d8df << 4) | data_d8df) << 8) | (data_d0d7 << 4) | data_d0d7;
	//		output("data 0x%04x", data);
	//		apg_dmain_set(apg, unit, data);
	//	}
	//}

	funtest(PAT__WriteReadCache, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

DOUBLE_VARIABLE( uv_FV_odt, 0.6, " ") {}

TEST_BLOCK(TB__ODT_MEASURE)
{
	output("%s", current_test_block());

	measure(TRUE);

	vclamp(1.4 V, -1 V, pinlist::AllDataAndDataStrobePins());

	vpar_force(uv_FV_odt V, pinlist::AllDataAndDataStrobePins(), 50, 0 V);
	ipar_high(40 MA, pinlist::AllDataAndDataStrobePins());
	ipar_low(-40 MA, pinlist::AllDataAndDataStrobePins());

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__ODT_SETUP pat");
	funtest(PAT__ODT_SETUP, finish);

	partest(pass_nicl, pinlist::AllDataAndDataStrobePins());

	pdatalog(pinlist::AllDataAndDataStrobePins());

	measure(TRUE);


	return MULTI_DUT;
}


TEST_BLOCK(TB__WriteReadCache_INV)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteReadCache_INV pat");
	funtest(PAT__WriteReadCache_INV, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//test data replacement mode of ebm
TEST_BLOCK(TB__WriteReadCache_EBM_REPL)
{
	output("%s", current_test_block());

	static std::vector<__int64> ebm_data_inc, ebm_data_dec;

	ebm_data_inc.resize(uv_bytes_in_page);
	ebm_data_dec.resize(uv_bytes_in_page);

	DutNumArray ads_dn;
	active_duts_get(&ads_dn);

	//load user arrays
	//odd duts (dut1, 3, ..) incrementing pattern
	output("ebm_data_inc size %d", ebm_data_inc.size());
	for (auto i = 0; i < ebm_data_inc.size(); ++i)
	{
		ebm_data_inc[i] = i & 0xff;
	}

	//even duts (dut2, 4, ..) decrementing pattern
	for (auto i = 0; i < ebm_data_dec.size(); ++i)
	{
		ebm_data_dec[i] = (ebm_data_dec.size() - 1 - i) & 0xff;
	}

	enEbmModeRateMag7	mode_rate		= enEbmModeRateMag7::t_MR_8;
	enPinModesMag7		pin_mode = enPinModesMag7::t_PM_BOCx8;
	enEbmDataModeMag7	ebm_data_mode	= enEbmDataModeMag7::t_data_replacement;

	for (auto i_dut = 0; i_dut < ads_dn.GetSize(); ++i_dut)
	{
		if ((ads_dn[i_dut] % 2) == 0)
		{
			load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data_inc, pin_mode, ebm_data_mode);
		}
		else
		{
			load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data_dec, pin_mode, ebm_data_mode);
		}
	}

	ebm_address_reg_set(		enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_address_reload_reg_set(	enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_active_ptr_set(			enEbmRamPointerNumberMag7::t_ERP1);
	ebm_pinset_set(enEbmPSNumberMag7::t_EBM_PS1, pinlist::AllDataPins());

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, pin_mode);

	output("C_DATA = %d", count(C_DATA));

	output("PAT__WriteReadCache_EBM pat");
	funtest(PAT__WriteReadCache_EBM, finish);

	int addr;

	ebm_address_reg_get(enEbmRamPointerNumberMag7::t_ERP1, &addr, &mode_rate, &ebm_data_mode);
	output("ebm addr = %d", addr);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteCache)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, 0x55aa);

	output("WriteReadCache pat");
	funtest(PAT__WriteCache, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//test data mask mode of ebm
//TB__WriteCache_BOCx4 writes 0x55aa pattern to dut
//TB__ReadCache_EBM_MASK_BOCx4 read the dut with 0xffff as expect data
//strobe masking is used to mask 0s at even addresses and mask 1s at even addresses
TEST_BLOCK(TB__WriteCache_BOCx4)
{
	output("%s", current_test_block());

	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, 0x55aa, true, enPinModesMag7::t_PM_BOCx4);

	output("WriteReadCache pat");
	funtest(PAT__WriteCache_x4, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//test data mask mode of ebm
//TB__WriteCache_BOCx4 writes 0x55aa pattern to dut
//TB__ReadCache_EBM_MASK_BOCx4 read the dut with 0xffff as expect data
//strobe masking is used to mask 0s at even addresses and mask 1s at even addresses
TEST_BLOCK(TB__ReadCache_EBM_MASK_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

/////ebm setup//////
	std::vector<__int64> ebm_data;

	ebm_data.resize(uv_bytes_in_page);

	DutNumArray ads_dn;
	active_duts_get(&ads_dn);

	//load user arrays
	for (auto i = 0; i < ebm_data.size(); i+=2)
	{
		ebm_data[i]		= 0x55;			//1 is mask(strobe disable), 0 is no mask(strobe enable)
		ebm_data[i + 1] = 0xaa;
	}

	enEbmModeRateMag7	mode_rate = enEbmModeRateMag7::t_MR_4;
	enPinModesMag7		pin_mode		= enPinModesMag7::t_PM_BOCx4;
	enEbmDataModeMag7	ebm_data_mode	= enEbmDataModeMag7::t_strobe_masking;

	for (auto i_dut = 0; i_dut < ads_dn.GetSize(); ++i_dut)
	{
		load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data, pin_mode, ebm_data_mode);
	}

	ebm_address_reg_set(		enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_address_reload_reg_set(	enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_active_ptr_set(			enEbmRamPointerNumberMag7::t_ERP1);
	ebm_pinset_set(enEbmPSNumberMag7::t_EBM_PS1, pinlist::AllDataPins());
////////////////////

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, 0x0, true, pin_mode);

	//aux3 is used to initalize the dmain register which is used for expect data
	//initialize aux3 to all high
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, 0xffff);		//DATA_PLANE_0 in pattern, 0xffff expect data
	}

	output("PAT__ReadCache_EBM_MASK_x4 pat");
	funtest(PAT__ReadCache_EBM_MASK_x4, finish);

	int addr;
	ebm_address_reg_get(enEbmRamPointerNumberMag7::t_ERP1, &addr, &mode_rate, &ebm_data_mode);

	output("ebm cnt = %d", addr);
	output("C_DATA = %d", count(C_DATA));

	//output("apg1 unit1 0x%x", apg_dmain_get(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1));
	//output("apg1 unit2 0x%x", apg_dmain_get(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2));
	//output("apg2 unit1 0x%x", apg_dmain_get(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1));
	//output("apg2 unit2 0x%x", apg_dmain_get(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2));

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//test data replacement and mask mode of ebm
//TB__WriteCache_BOCx4 writes 0x55aa pattern to dut
//TB__ReadCache_EBM_MASK_BOCx4 read the dut with 0xffff as expect data
//strobe masking is used to mask 0s at even addresses and mask 1s at even addresses
TEST_BLOCK(TB__ReadCache_EBM_REPL_MASK_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

/////ebm setup//////
	std::vector<__int64> ebm_data;

	//each ebm address is 2 locations - data followed by mask
	ebm_data.resize(2 * uv_bytes_in_page);  

	DutNumArray ads_dn;
	active_duts_get(&ads_dn);

	//load user arrays
	for (auto i = 0; i < ebm_data.size(); i += 4)
	{
		ebm_data[i]		= 0x00;			//data replace, expect data
		ebm_data[i + 1] = 0xaa;			//strobe, 1 is mask(strobe disable), 0 is no mask(strobe enable)
		ebm_data[i + 2] = 0x00;			//data replace, expect data
		ebm_data[i + 3] = 0x55;			//strobe, 1 is mask(strobe disable), 0 is no mask(strobe enable)
	}

	enEbmModeRateMag7	mode_rate		= enEbmModeRateMag7::t_MR_8;
	enPinModesMag7		pin_mode		= enPinModesMag7::t_PM_BOCx4;
	enEbmDataModeMag7	ebm_data_mode	= enEbmDataModeMag7::t_mask_and_replace;

	for (auto i_dut = 0; i_dut < ads_dn.GetSize(); ++i_dut)
	{
		load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data, pin_mode, ebm_data_mode);
	}

	ebm_address_reg_set(		enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_address_reload_reg_set(	enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_active_ptr_set(			enEbmRamPointerNumberMag7::t_ERP1);
	ebm_pinset_set(enEbmPSNumberMag7::t_EBM_PS1, pinlist::AllDataPins());
////////////////////

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, 0x0, true, pin_mode);

	//aux3 is used to initalize the dmain register which is used for expect data
	//initialize aux3 to all high
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, 0xffff);		//DATA_PLANE_0 in pattern
	}

	output("PAT__ReadCache_EBM_MASK_x4 pat");
	funtest(PAT__ReadCache_EBM_MASK_x4, finish);

	int addr;
	ebm_address_reg_get(enEbmRamPointerNumberMag7::t_ERP1, &addr, &mode_rate, &ebm_data_mode);

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_RND_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx4);

	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	output("PAT__WriteReadCache_RND_x4 pat");
	funtest(PAT__WriteReadCache_RND_x4, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_EBM_REPL_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	std::vector<__int64> ebm_data_inc, ebm_data_dec;

	ebm_data_inc.resize(uv_bytes_in_page);
	ebm_data_dec.resize(uv_bytes_in_page);

	DutNumArray ads_dn;
	active_duts_get(&ads_dn);

	//load user arrays
	//odd duts (dut1, 3, ..) incrementing pattern
	for (auto i = 0; i < ebm_data_inc.size(); ++i)
	{
		ebm_data_inc[i] = i & 0xff;
	}

	//even duts (dut2, 4, ..) decrementing pattern
	for (auto i = 0; i < ebm_data_dec.size(); ++i) ebm_data_dec[i] = (ebm_data_dec.size() - 1 - i) & 0xff;

	enEbmModeRateMag7	mode_rate		= enEbmModeRateMag7::t_MR_4;
	enPinModesMag7		pin_mode		= enPinModesMag7::t_PM_BOCx4;
	enEbmDataModeMag7	ebm_data_mode	= enEbmDataModeMag7::t_data_replacement;

	for (auto i_dut = 0; i_dut < ads_dn.GetSize(); ++i_dut)
	{
		if ((ads_dn[i_dut] % 2) == 0)
		{
			load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data_inc, pin_mode, ebm_data_mode);
		}
		else
		{
			load_ebm(ads_dn[i_dut], pinlist::AllDataPins(), ebm_data_dec, pin_mode, ebm_data_mode);
		}
	}
	ebm_address_reg_set(		enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_address_reload_reg_set(	enEbmRamPointerNumberMag7::t_ERP1, 0, mode_rate, ebm_data_mode);
	ebm_active_ptr_set(			enEbmRamPointerNumberMag7::t_ERP1);
	ebm_pinset_set(				enEbmPSNumberMag7::t_EBM_PS1, pinlist::AllDataPins());

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	output("C_DATA = %d", count(C_DATA));

	output("PAT__WriteReadCache_EBM_x4 pat");
	funtest(PAT__WriteReadCache_EBM_x4, finish);

	int addr;

	ebm_address_reg_get(enEbmRamPointerNumberMag7::t_ERP1, &addr, &mode_rate, &ebm_data_mode);
	output("ebm addr = %d", addr);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}


TEST_BLOCK(TB__WriteReadCache_RND)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx8);

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
		output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1));

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteReadCache_RND pat");
	funtest(PAT__WriteReadCache_RND, finish);

	//for(auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
	//	output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1 ));

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadCache_RND)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
		output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1));

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__ReadCache_RND pat");
	funtest(PAT__ReadCache_RND, finish);

	//for(auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
	//	output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1 ));

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadCache_RND_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	//load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx4);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	////change random bit in jamram to check strobe functionality
	//srand(static_cast<unsigned int>(time(NULL)));
	//auto addr				= rand() % JAMRAM_SIZE;
	//auto bit				= rand() % 64;
	//auto buf_idx			= addr * 8 + (bit / 8);
	//auto bit_within_byte	= bit % 8;
	//auto old_byte			= g_buffer1[buf_idx] & 0xff;
	//auto old_word			= buf_idx % 2 ? (old_byte << 8) | (g_buffer1[buf_idx-1] & 0xff) : ((g_buffer1[buf_idx + 1] & 0xff) << 8) | old_byte;
	//auto new_byte			= old_byte ^ (0x1 << bit_within_byte);
	//auto msb				= buf_idx % 2 ? new_byte : (g_buffer1[buf_idx + 1] & 0xff);
	//auto lsb				= buf_idx % 2 ? (g_buffer1[buf_idx - 1] & 0xff) : new_byte;
	//auto new_word			= (msb << 8) | lsb;
	//enApgNumberMag7 apg		= bit < 32 ? enApgNumberMag7::t_APG1 : enApgNumberMag7::t_APG2;
	//enUnitNumberMag7 unit = (bit % 32) < 16 ? enUnitNumberMag7::t_UNIT1 : enUnitNumberMag7::t_UNIT2;

	//output("addr = %d, bit = %d, APG = %s, Unit = %s old_word = 0x%x, new_word = 0x%x", addr, bit, 
	//	apg == enApgNumberMag7::t_APG1 ? "APG1": "APG2", unit == enUnitNumberMag7::t_UNIT1 ? "UNIT1" : "UNIT2",
	//	old_word, new_word);
	//apg_jam_ram_set(addr, apg, unit, new_word);
	////

	output("PAT__ReadCache_RND_x4 pat");
	funtest(PAT__ReadCache_RND_x4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteCache_RND_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx4);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	output("PAT__WriteCache_RND_x4 pat");
	funtest(PAT__WriteCache_RND_x4, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteCache_RND)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteCache_RND pat");
	funtest(PAT__WriteCache_RND, finish);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_LFSR)
{
	output("%s", current_test_block());

	//////////////setup LFSR//////////////
	lfsr_equation_set(enLFSRRegisterNumberMag7::t_LFSR_1, enLFSRPolynomialSelectionMag7::t_PRBS_23);
	lfsr_seed_set(enLFSRRegisterNumberMag7::t_LFSR_1, 0xaa55);
	lfsr_rate_set(enLFSRRateSelectionMag7::t_RATE_16);
	/////////////////////////////////////

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteReadCache_LFSR pat");
	funtest(PAT__WriteReadCache_LFSR, finish);

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_DBM)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	dbm_segment_set(uv_dbm_segment);
	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteReadCache_DBM pat");
	funtest(PAT__WriteReadCache_DBM, finish);

	//for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	//{
	//	output("apg%d xmain = 0x%08x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_xmain_get(apg));
	//	output("apg%d ymain = 0x%08x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_ymain_get(apg));
	//}

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_DBM_BOCx4)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	dbm_segment_set(uv_dbm_segment);
	shmoo_setup(enPinModesMag7::t_PM_BOCx4);

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	output("PAT__WriteReadCache_DBM_x4 pat");
	funtest(PAT__WriteReadCache_DBM_x4, finish);

	//for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	//{
	//	output("apg%d xmain = 0x%08x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_xmain_get(apg));
	//	output("apg%d ymain = 0x%08x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_ymain_get(apg));
	//}

	shmoo_restore(enPinModesMag7::t_PM_BOCx4);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__WriteReadCache_DBI)
{
	output("%s", current_test_block());
	//const size_t block = 0;
	//const size_t plane = 0;
	//const size_t lun = 0;

	//setup DBI//
	dm_dbi_mode_set(enDmDbiModeMag7::t_DM_DBI_NONE);
	dbi_count_polarity_set(true);	//count 0s
	dbi_polarity_set(false);		//dbi is active high	
	dm_dbi_control_set( enDmDbiFuncCtrlMag7::t_USE_DBI_MODE, enDmDbiFuncCtrlMag7::t_USE_DBI_MODE, enDmDbiFuncCtrlMag7::t_USE_DBI_MODE, enDmDbiFuncCtrlMag7::t_USE_DBI_MODE); //each arg represents 1 byte of the 4 byte DG dword
	//dmi_strobe_enable_set();
	/////////////

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx8);

	shmoo_setup(enPinModesMag7::t_PM_BOCx8);

	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
		output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1));

	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte);

	output("PAT__WriteReadCache_DBI pat");
	funtest(PAT__WriteReadCache_DBI, finish);

	//for(auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG1; ++apg)
	//	output("APG %d, 0x%x", apg == enApgNumberMag7::t_APG1 ? 1 : 2, apg_jam_ram_pointer_address_get(enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1 ));

	shmoo_restore(enPinModesMag7::t_PM_BOCx8);

	dm_dbi_mode_set(enDmDbiModeMag7::t_DM_DBI_NONE);

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//////////////////////////////////////////////////////////////////////////////////////
//testblocks to checkout ripple function.
//These are run without a dut and use a scope to observe the waveforms
//test the vpulse/ripple feature
TEST_BLOCK(TB__vpulse_VCC)
{
	output("%s", current_test_block());

	dps_vpulse_mode_set(enVPulseModeMag7::t_VPulse);
	dps_vpulse_enable(pinlist::pl_VCC(), TRUE);

	// Execute
	funtest(PAT__vpulse, finish);
	dps_vpulse_enable(pinlist::pl_VCC(), FALSE);

	return MULTI_DUT;
}

TEST_BLOCK(TB__ripple_VCC)
{
	output("%s", current_test_block());

	dps_ripple_setup(uv_ripple_freq, uv_ripple_volts MV);

	dps_vpulse_mode_set( enVPulseModeMag7::t_Ripple);
	dps_vpulse_enable(pinlist::pl_VCC(), TRUE);

	// Execute
	funtest(PAT__vpulse, finish);
	dps_vpulse_enable(pinlist::pl_VCC(), FALSE);
	dps_vpulse_mode_set(enVPulseModeMag7::t_VPulse);   //do not leave in ripple mode because of the potential for dps ramp timeouts

	return MULTI_DUT;
}
//////////////////////////////////////////////////////////////////////////////////////
//testblocks to checkout ripple function.
//These are run without a dut and use a scope to observe the waveforms
//test the vpulse/ripple feature

INT_VARIABLE( uv_delay_between_lvlsets, 5, "uv_delay_between_lvlsets(MS)") {}
INT_VARIABLE( uv_delay_wait_change,		5, "uv_delay_wait_change(MS)") {}
INT_VARIABLE( uv_lvlsets_loop,			10, " ") {}

TEST_BLOCK(TB__PE_LevelSets)
{
	output("%s", current_test_block());

	count(1, int((uv_delay_between_lvlsets MS) / (1000 NS)) - 1);	//time between levelsets
	count(2, int((uv_delay_wait_change MS) / (1000 NS)) - 1);		//time for change to begin
	count(3, uv_lvlsets_loop-1);									//number of levelset changes

	pin_dc_state_set(builtin_UsedPins, t_vih, FALSE);
	pin_dc_state_set(builtin_UsedPins, t_vil, FALSE);
	pin_dc_state_set(builtin_UsedPins, t_vih, FALSE);

	// Execute
	funtest( PAT__PE_LevelSets, finish);

	output("count1 = %d", count(1));
	output("count2 = %d", count(2));
	output("count3 = %d", count(3));

	return MULTI_DUT;
}

TEST_BLOCK(TB_DPS_LevelSets_set)
{
	output("%s", current_test_block());

	double per;
	enPinModesMag7 pm;
	enUICaptureMag7 cap;
	BYTE vm;

	cycle(HSN_TSET(TS_LEVELSET), &per, &pm, &cap, &vm);
	output("per = %f NS", per / (1 NS));

	count(1, int((uv_delay_between_lvlsets MS) / per) - 1);	//time between levelsets
	count(2, int((uv_delay_wait_change MS) / per) - 1);		//time for change to begin
	count(3, uv_lvlsets_loop - 1);							//number of levelset changes

	output("count1 = %d", count(1));
	output("count2 = %d", count(2));

	// Execute
	funtest(PAT__DPS_LevelSets_set, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB_DPS_LevelSets_twk)
{
	output("%s", current_test_block());

	double per;
	enPinModesMag7 pm;
	enUICaptureMag7 cap;
	BYTE vm;

	cycle(HSN_TSET(TS_LEVELSET), &per, &pm, &cap, &vm);
	output("per = %f NS", per / (1 NS));

	count(1, int((uv_delay_between_lvlsets MS) / per) - 1);	//time between levelsets
	count(2, int((uv_delay_wait_change MS) / per) - 1);		//time for change to begin
	count(3, uv_lvlsets_loop - 1);							//number of levelset changes

	output("count1 = %d", count(1));
	output("count2 = %d", count(2));

	// Execute
	funtest(PAT__DPS_LevelSets_twk, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB_HV_LevelSets_set)
{
	output("%s", current_test_block());

	double per;
	enPinModesMag7 pm;
	enUICaptureMag7 cap;
	BYTE vm;

	cycle(HSN_TSET(TS_LEVELSET), &per, &pm, &cap, &vm);
	output("per = %f NS", per / (1 NS));

	count(1, int((uv_delay_between_lvlsets MS) / per) - 1);	//time between levelsets
	count(2, int((uv_delay_wait_change MS) / per) - 1);		//time for change to begin
	count(3, uv_lvlsets_loop - 1);							//number of levelset changes

	output("count1 = %d", count(1));
	output("count2 = %d", count(2));

	hv_voltage_set(1 V, dutpin::VREFQ());
	hv_voltage_set(2 V, dutpin::VREFQ());
	hv_voltage_set(0 V, dutpin::VREFQ());


	// Execute
	funtest(PAT__HV_LevelSets_set, finish);

	return MULTI_DUT;
}

TEST_BLOCK(TB_HV_LevelSets_twk)
{
	output("%s", current_test_block());

	double per;
	enPinModesMag7 pm;
	enUICaptureMag7 cap;
	BYTE vm;

	cycle(HSN_TSET(TS_LEVELSET), &per, &pm, &cap, &vm);
	output("per = %f NS", per / (1 NS));

	count(1, int((uv_delay_between_lvlsets MS) / per) - 1);	//time between levelsets
	count(2, int((uv_delay_wait_change MS) / per) - 1);		//time for change to begin
	count(3, uv_lvlsets_loop - 1);							//number of levelset changes

	output("count1 = %d", count(1));
	output("count2 = %d", count(2));

	hv_voltage_set(1 V, dutpin::VREFQ());
	hv_voltage_set(2 V, dutpin::VREFQ());
	hv_voltage_set(0 V, dutpin::VREFQ());

	// Execute
	funtest(PAT__HV_LevelSets_twk, finish);

	return MULTI_DUT;
}


//////////////////////////////////////////////////////////////////////////////////////







