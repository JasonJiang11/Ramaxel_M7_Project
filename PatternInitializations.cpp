#include "stdafx.h"
#include "PatternInitializations.h"
#include "PinLists.h"
#include "DefineCounters.h"
#include "Timings.h"
#include "TestBlocks.h"
#include <cmath>

BOOL_VARIABLE(POLL_EN, TRUE, " ") {}	//if 1 it will use APG BOE to reduce RB wait time

INT_VARIABLE(uv_pages_per_block_slc, 0x140, " ") {}

// Initialize pattern counters
//optionally take pattern simulator config as argument
void set_counters_init(enPinModesMag7 mode, PatSimConfigStructMagnum7 *sim_config)
{
	const double tsetCycle = get_IDLE_CYC();

	const double tPOR = 4 MS;
	//const double tPOR = 100 MS;		//for debug only - bryan 080221
	const double tWHR = 80 NS; // Command cycle to data output
	const double tWB = 100 NS; // WB# high to RB# low
	const double tFEAT = 1 US; // Busy time for SET FEATURES and GET FEATURES operations - support tTemp as well

	const double tADL = 150 NS; // ALE to data loading time
	const double tRR = 20 NS; // Ready to data output
	const double tWPST = 6.5 NS; // DQS write postamble
	const double tWPSTH = 25 NS; // DQS write postabmle hold time
	const double tPROG = 4000 US; // Program page operation effective time (per page)

	const double tCCS = 400 NS; // Change column setup time to data in/out or next command
	const double tWHRT = 400 NS; // Address cycle to data output for training
	const double tWTRN = 200 US; // Ready busy time during write RX training
	const double tR = 140 US; // Read page operation time without/with VPP
	const double tBERS = 20 MS; // Erase block operation time
	const double tWC = 25 NS; // WE# cycle time
	const double tDBSY = 1 US; // Dummy busy time
	const double tSLC = 500 NS; // Enter SLC mode wait

	//boe pipeline w/o dbm or ecr is 235 at 6.4 ns and 348 at 3.2 ns
	//+550 with dbm
	int c_pipe = 1000;	//apg pipeline length
	int c_tPOR = static_cast<int>(std::ceil(tPOR / tsetCycle / 2));
	int c_tWHR = static_cast<int>(std::ceil(tWHR / tsetCycle / 2));
	int c_tWB = static_cast<int>(std::ceil(tWB / tsetCycle / 2));
	int c_tFEAT = static_cast<int>(std::ceil(tFEAT / tsetCycle / 2));
	int c_tADL = static_cast<int>(std::ceil(tADL / tsetCycle / 2));
	int c_tRR = static_cast<int>(std::ceil(tRR / tsetCycle / 2));
	int c_tWPST = static_cast<int>(std::ceil(tWPST / tsetCycle / 2));
	int c_tWPSTH = static_cast<int>(std::ceil(tWPSTH / tsetCycle / 2));
	int c_tPROG = static_cast<int>(std::ceil(tPROG / tsetCycle / 2));
	int c_tCCS = static_cast<int>(std::ceil(tCCS / tsetCycle / 2));
	int c_tWHRT = static_cast<int>(std::ceil(tWHRT / tsetCycle / 2));
	int c_tWTRN = static_cast<int>(std::ceil(tWTRN / tsetCycle / 2));
	int c_tR = static_cast<int>(std::ceil(tR / tsetCycle / 2));
	int c_tBERS = static_cast<int>(std::ceil(tBERS / tsetCycle / 2));
	int c_tWC = static_cast<int>(std::ceil(tWC / tsetCycle / 2));
	int c_tDBSY = static_cast<int>(std::ceil(tDBSY / tsetCycle / 2));
	int c_tSLC = static_cast<int>(std::ceil(tSLC / tsetCycle / 2));

	count(C_PIPE,				c_pipe - 1					);
	count(C_POLL_EN,			POLL_EN						);
	count(C_tPOR,				c_tPOR - 1					);
	count(C_tWHR,				c_tWHR - 1					);
	count(C_tWB,				c_tWB - 1					);
	count(C_tFEAT,				c_tFEAT - 1					);
	count(C_tADL,				c_tADL - 1					);
	count(C_tRR,				c_tRR - 1					);
	count(C_tWPST,				c_tWPST - 1					);
	count(C_tWPSTH,				c_tWPSTH - 1				);
	count(C_tPROG,				c_tPROG - 1					);
	count(C_tCCS,				c_tCCS - 1					);
	count(C_tWHRT,				c_tWHRT - 1					);
	count(C_tWTRN,				c_tWTRN - 1					);
	count(C_tR,					c_tR - 1					);
	count(C_tBERS,				c_tBERS - 1					);
	count(C_tWC,				c_tWC - 1					);
	count(C_tDBSY,				c_tDBSY - 1					);
	count(C_tSLC,				c_tSLC - 1					);
	count(C_SLC_MODE,			0							);
	count(C_SLC_PAGES_PER_BLK, uv_pages_per_block_slc - 1	);

	if (mode == enPinModesMag7::t_PM_BOCx4)
	{
		count(C_PARAMETERS,		63);							//BOCx4
		count(C_DATA,			uv_bytes_in_page / 8 - 1);		//BOCx4
	}
	else
	{
		count(C_DATA,			uv_bytes_in_page / 16 - 1);		//BOCx8
		count(C_PARAMETERS,		31);							//BOCx8
	}

	//if pattern simulator is being used, initialize the config argumnet
	if (sim_config != nullptr)
	{
		sim_config->count[C_tPOR	 -1] = c_tPOR - 1	 ;
		sim_config->count[C_tWHR	 -1] = c_tWHR - 1	 ;
		sim_config->count[C_tWB		 -1] = c_tWB - 1	 ;
		sim_config->count[C_tFEAT	 -1] = c_tFEAT - 1	 ;
		sim_config->count[C_tADL	 -1] = c_tADL - 1	 ;
		sim_config->count[C_tRR		 -1] = c_tRR - 1	 ;
		sim_config->count[C_tWPST	 -1] = c_tWPST - 1	 ;
		sim_config->count[C_tWPSTH	 -1] = c_tWPSTH - 1	 ;
		sim_config->count[C_tPROG	 -1] = c_tPROG - 1	 ;
		sim_config->count[C_tCCS	 -1] = c_tCCS - 1	 ;
		sim_config->count[C_tWHRT	 -1] = c_tWHRT - 1	 ;
		sim_config->count[C_tWTRN	 -1] = c_tWTRN - 1	 ;
		sim_config->count[C_tR		 -1] = c_tR - 1		 ;
		sim_config->count[C_tBERS	 -1] = c_tBERS - 1	 ;
		sim_config->count[C_tWC		 -1] = c_tWC - 1	 ;
		sim_config->count[C_tDBSY	 -1] = c_tDBSY - 1	 ;
		sim_config->count[C_tSLC	 -1] = c_tSLC - 1	 ;
		sim_config->count[C_SLC_MODE -1] = 0			 ;
	}
}

//use DWORD to set 4 subdgs of apg
//1st subdg gets 1st byte(LSB), 2nd subdg gets 2nd byte, ..,  

static void set_apg_dbase(enApgNumberMag7 apg, DWORD dw_val)
{
	//load 4 subdgs with shifted bytes
	apg_dbase_set(apg, enUnitNumberMag7::t_UNIT1, (dw_val >> 0)  & 0xffff);
	apg_dbase_set(apg, enUnitNumberMag7::t_UNIT2, (dw_val >> 16) & 0xffff);
	apg_dbase_set(apg, enUnitNumberMag7::t_UNIT3, (dw_val >> 0)  & 0xffff);
	apg_dbase_set(apg, enUnitNumberMag7::t_UNIT4, (dw_val >> 16) & 0xffff);
}

//for all apgs use 4 bytes in DWORD to program all subdgs
void set_dbase_4byte_pattern(DWORD val)
{
	for( auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
		set_apg_dbase(apg, val);
}

// Return ADD1 (COL1) lower 8 bit column address
// bit |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
//     |  C0 |  C1 |  C2 |  C3 |  C4 |  C5 |  C6 |  C7 |
BYTE get_address1(size_t col)
{
	if (col & 0x1)
	{
		warning("For NV-DDR3 interface, Column bit 0 must be 0. Setting Column bit 0 to 0.");
		return col & 0xFE;
	}
	return col & 0xFF;
}

// Return ADD2 (COL2) upper 8 bit column address
// bit |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
//     | C8  | C9  | C10 | C11 | C12 | C13 | C14 | C15 |
BYTE get_address2(size_t col)
{
	static const auto num_bytes = device::memory::num_bytes_per_page + device::memory::num_bytes_per_defect_area;

	if (col > num_bytes)
	{
		warning("CA[14:0] addresses 0 through %d. Setting it to %d", num_bytes - 1, num_bytes - 2);
		col = num_bytes - 2;
	}

	return (col >> 8) & 0xFF;
}

// Return ADD3 (ROW1) lower 8 bit page address
// bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//     |  P7 |  P6 |  P5 |  P4 |  P3 |  P2 |  P1 |  P0 |
BYTE get_address3(size_t page)
{
	return page & 0xFF;
}

// Return ADD4 (ROW2) upper 1 bit page address,
// 1 bit plane address,
// and  6 bit block address
// bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//     | B1  | B0  | PL1 | PL0 | P11 | P10 | P9  | P8  |

BYTE get_address4(size_t block, size_t plane, size_t page)
{
	return ((block & 0x03) << 6) | ((plane & 0x3) << 4) | ((page >> 8) & 0xf);
}

// Return ADD5 (ROW3) upper 5 bit block address
// bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//     |  L0 |  B8 |  B7 |  B6 |  B5 |  B4 |  B3 |  B2  |

BYTE get_address5(size_t lun, size_t block)
{
	return ((lun & 0x1) << 7) | ((block >> 2) & 0x7F);
}

//this function is used to set the XALU based on lun, block, plane, page
DWORD set_addr_xalu(size_t lun, size_t block, size_t plane, size_t page)
{
	return (get_address5(lun, block) << 16) | (get_address4(block, plane, page) << 8) | get_address3(page);
}

//this function is used to set the YALU based on column
DWORD set_addr_yalu(size_t col)
{
	return ((get_address2(col) << 8) | get_address1(col));
}