#include "stdafx.h"
#include "PatternResources.h"
#include "Timings.h"
#include "DeviceStatus.h"
#include "DeviceSpecs.h"
#include "shmoo.h"
#include "FlsUserTasks.h"
//for debug
DOUBLE_VARIABLE(uv_cycle, 100, " ") {}
DOUBLE_VARIABLE(uv_stb,		50, " ") {}
DOUBLE_VARIABLE(uv_rate, 1.2, "rate Gbps") {}
DOUBLE_VARIABLE(uv_dqs_dq_offset, 400, "uv_dqs_dq_offset(PS)") {}

DOUBLE_VARIABLE(uv_eg_retime_offset, 2.0, " ") {}

ONEOF_VARIABLE(uv_source_sync, "OFF, EG_RETIME, DQS_RETIME", " ") {}

BOOL_VARIABLE(SHM_DATAEYE, FALSE, " ") {}
BOOL_VARIABLE(SHM_TORNADO, FALSE, " ") {}

BOOL_VARIABLE(uv_Hwtraining, FALSE, " ") {}

//const double IDLE_CYC = 1.515 NS; // 660 Mbps	//tester crash - remove for debug - bryan 050521
//const double IDLE_CYC = 3.030 NS; // 660 Mbps
const double IDLE_CYC = (1 / 330.0 * 1e3) NS; // 330 Mbps
//const double IDLE_CYC = (1 / 660.0 * 1e3) NS; // 625 Mbps

const double RBSY_CYC = IDLE_CYC; 
const double MIN_MAJOR_CYCLE = IDLE_CYC * 2;

const double get_IDLE_CYC()
{
	return IDLE_CYC;
}

//flags if dut training routine has been executed for current duts
//set to true by function setup_src_sync
static bool ssync_enabled = false;

//return true if source sync is enabled, otherwise return false
const bool is_ssync_enabled()
{
	return ssync_enabled;
}

std::unordered_map<DutNum, std::unordered_map<PinList *, double>> strobe_vals;
std::unordered_map<DutNum, double> tdqsre;

using namespace device;
using TimingParamValuesType = std::unordered_map<TMODE, double>;

TimingParamValuesType tper		  { {TMODE::MODE0,   30 NS }, {TMODE::MODE1,   25 NS}, {TMODE::MODE2,   15 NS}, {TMODE::MODE3,   12 NS}, {TMODE::MODE4,   10 NS}, {TMODE::MODE5,  7.5 NS}, {TMODE::MODE6,    6 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8, 3.75 NS}, {TMODE::MODE9,    3 NS}, {TMODE::MODE10,  2.5 NS}, {TMODE::MODE11, 1.875 NS}, {TMODE::MODE12, 1.667 NS}, {TMODE::MODE13,  1.5 NS}, {TMODE::MODE14, 1.364 NS}, {TMODE::MODE15,   1.25 NS} };
TimingParamValuesType tCS		  { {TMODE::MODE0,   20 NS }, {TMODE::MODE1,   20 NS}, {TMODE::MODE2,   20 NS}, {TMODE::MODE3,   20 NS}, {TMODE::MODE4,   20 NS}, {TMODE::MODE5,   20 NS}, {TMODE::MODE6,   20 NS}, {TMODE::MODE7,   20 NS}, {TMODE::MODE8,   20 NS}, {TMODE::MODE9,   20 NS}, {TMODE::MODE10,   20 NS}, {TMODE::MODE11,    20 NS}, {TMODE::MODE12,    20 NS}, {TMODE::MODE13,   20 NS}, {TMODE::MODE14,    20 NS}, {TMODE::MODE15,   20 NS}   };
TimingParamValuesType tCH		  { {TMODE::MODE0,    5 NS }, {TMODE::MODE1,    5 NS}, {TMODE::MODE2,    5 NS}, {TMODE::MODE3,    5 NS}, {TMODE::MODE4,    5 NS}, {TMODE::MODE5,    5 NS}, {TMODE::MODE6,    5 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8,    5 NS}, {TMODE::MODE9,    5 NS}, {TMODE::MODE10,    5 NS}, {TMODE::MODE11,     5 NS}, {TMODE::MODE12,     5 NS}, {TMODE::MODE13,    5 NS}, {TMODE::MODE14,     5 NS}, {TMODE::MODE15,    5 NS}   };
TimingParamValuesType tCALS		  { {TMODE::MODE0,   15 NS }, {TMODE::MODE1,   15 NS}, {TMODE::MODE2,   15 NS}, {TMODE::MODE3,   15 NS}, {TMODE::MODE4,   15 NS}, {TMODE::MODE5,   15 NS}, {TMODE::MODE6,   15 NS}, {TMODE::MODE7,   15 NS}, {TMODE::MODE8,   15 NS}, {TMODE::MODE9,   15 NS}, {TMODE::MODE10,   15 NS}, {TMODE::MODE11,    15 NS}, {TMODE::MODE12,    15 NS}, {TMODE::MODE13,   15 NS}, {TMODE::MODE14,    15 NS}, {TMODE::MODE15,   15 NS}   };
TimingParamValuesType tCALH		  { {TMODE::MODE0,    5 NS }, {TMODE::MODE1,    5 NS}, {TMODE::MODE2,    5 NS}, {TMODE::MODE3,    5 NS}, {TMODE::MODE4,    5 NS}, {TMODE::MODE5,    5 NS}, {TMODE::MODE6,    5 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8,    5 NS}, {TMODE::MODE9,    5 NS}, {TMODE::MODE10,    5 NS}, {TMODE::MODE11,     5 NS}, {TMODE::MODE12,     5 NS}, {TMODE::MODE13,    5 NS}, {TMODE::MODE14,     5 NS}, {TMODE::MODE15,    5 NS}   };
TimingParamValuesType tDQSRH	  { {TMODE::MODE0,    5 NS }, {TMODE::MODE1,    5 NS}, {TMODE::MODE2,    5 NS}, {TMODE::MODE3,    5 NS}, {TMODE::MODE4,    5 NS}, {TMODE::MODE5,    5 NS}, {TMODE::MODE6,    5 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8,    5 NS}, {TMODE::MODE9,    5 NS}, {TMODE::MODE10,    5 NS}, {TMODE::MODE11,     5 NS}, {TMODE::MODE12,     5 NS}, {TMODE::MODE13,    5 NS}, {TMODE::MODE14,     5 NS}, {TMODE::MODE15,    5 NS}   };
TimingParamValuesType tDQSD_max	  { {TMODE::MODE0,    18 NS }, {TMODE::MODE1,    18 NS}, {TMODE::MODE2,    18 NS}, {TMODE::MODE3,    18 NS}, {TMODE::MODE4,    18 NS}, {TMODE::MODE5,    18 NS}, {TMODE::MODE6,    18 NS}, {TMODE::MODE7,    18 NS}, {TMODE::MODE8,    18 NS}, {TMODE::MODE9,    18 NS}, {TMODE::MODE10,    18 NS}, {TMODE::MODE11,     18 NS}, {TMODE::MODE12,     18 NS}, {TMODE::MODE13,    18 NS}, {TMODE::MODE14,     18 NS}, {TMODE::MODE15,    18 NS}   };
TimingParamValuesType tDQSD_min	  { {TMODE::MODE0,     6 NS },{TMODE::MODE1,     6 NS},{TMODE::MODE2,     6 NS},{TMODE::MODE3,     6 NS},{TMODE::MODE4,     6 NS},{TMODE::MODE5,     6 NS},{TMODE::MODE6,     6 NS},{TMODE::MODE7,     6 NS},{TMODE::MODE8,     6 NS},{TMODE::MODE9,     6 NS},{TMODE::MODE10,     6 NS},{TMODE::MODE11,      6 NS},{TMODE::MODE12,      6 NS},{TMODE::MODE13,     6 NS},{TMODE::MODE14,      6 NS},{TMODE::MODE15,     6 NS}   };
TimingParamValuesType tCSD		  { {TMODE::MODE0,   10 NS }, {TMODE::MODE1,   10 NS}, {TMODE::MODE2,   10 NS}, {TMODE::MODE3,   10 NS}, {TMODE::MODE4,   10 NS}, {TMODE::MODE5,   10 NS}, {TMODE::MODE6,   10 NS}, {TMODE::MODE7,   10 NS}, {TMODE::MODE8,   10 NS}, {TMODE::MODE9,   10 NS}, {TMODE::MODE10,   10 NS}, {TMODE::MODE11,    10 NS}, {TMODE::MODE12,    10 NS}, {TMODE::MODE13,   10 NS}, {TMODE::MODE14,    10 NS}, {TMODE::MODE15,   10 NS}   };
TimingParamValuesType tWP		  { {TMODE::MODE0,   11 NS }, {TMODE::MODE1,   11 NS}, {TMODE::MODE2,   11 NS}, {TMODE::MODE3,   11 NS}, {TMODE::MODE4,   11 NS}, {TMODE::MODE5,   11 NS}, {TMODE::MODE6,   11 NS}, {TMODE::MODE7,   11 NS}, {TMODE::MODE8,   11 NS}, {TMODE::MODE9,   11 NS}, {TMODE::MODE10,   11 NS}, {TMODE::MODE11,    11 NS}, {TMODE::MODE12,    11 NS}, {TMODE::MODE13,   11 NS}, {TMODE::MODE14,    11 NS}, {TMODE::MODE15,   11 NS}   };
TimingParamValuesType tCAS		  { {TMODE::MODE0,    5 NS }, {TMODE::MODE1,    5 NS}, {TMODE::MODE2,    5 NS}, {TMODE::MODE3,    5 NS}, {TMODE::MODE4,    5 NS}, {TMODE::MODE5,    5 NS}, {TMODE::MODE6,    5 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8,    5 NS}, {TMODE::MODE9,    5 NS}, {TMODE::MODE10,    5 NS}, {TMODE::MODE11,     5 NS}, {TMODE::MODE12,     5 NS}, {TMODE::MODE13,    5 NS}, {TMODE::MODE14,     5 NS}, {TMODE::MODE15,    5 NS}   };
TimingParamValuesType tCAH		  { {TMODE::MODE0,    5 NS }, {TMODE::MODE1,    5 NS}, {TMODE::MODE2,    5 NS}, {TMODE::MODE3,    5 NS}, {TMODE::MODE4,    5 NS}, {TMODE::MODE5,    5 NS}, {TMODE::MODE6,    5 NS}, {TMODE::MODE7,    5 NS}, {TMODE::MODE8,    5 NS}, {TMODE::MODE9,    5 NS}, {TMODE::MODE10,    5 NS}, {TMODE::MODE11,     5 NS}, {TMODE::MODE12,     5 NS}, {TMODE::MODE13,    5 NS}, {TMODE::MODE14,     5 NS}, {TMODE::MODE15,    5 NS}   };
TimingParamValuesType tWH		  { {TMODE::MODE0,   11 NS }, {TMODE::MODE1,   11 NS}, {TMODE::MODE2,   11 NS}, {TMODE::MODE3,   11 NS}, {TMODE::MODE4,   11 NS}, {TMODE::MODE5,   11 NS}, {TMODE::MODE6,   11 NS}, {TMODE::MODE7,   11 NS}, {TMODE::MODE8,   11 NS}, {TMODE::MODE9,   11 NS}, {TMODE::MODE10,   11 NS}, {TMODE::MODE11,    11 NS}, {TMODE::MODE12,    11 NS}, {TMODE::MODE13,   11 NS}, {TMODE::MODE14,    11 NS}, {TMODE::MODE15,   11 NS}   };
TimingParamValuesType tWPRE		  { {TMODE::MODE0,   15 NS }, {TMODE::MODE1,   15 NS}, {TMODE::MODE2,   15 NS}, {TMODE::MODE3,   15 NS}, {TMODE::MODE4,   15 NS}, {TMODE::MODE5,   15 NS}, {TMODE::MODE6,   15 NS}, {TMODE::MODE7,   15 NS}, {TMODE::MODE8,   15 NS}, {TMODE::MODE9,   15 NS}, {TMODE::MODE10,   15 NS}, {TMODE::MODE11,    15 NS}, {TMODE::MODE12,    15 NS}, {TMODE::MODE13,   15 NS}, {TMODE::MODE14,    15 NS}, {TMODE::MODE15,   15 NS}   };
TimingParamValuesType tWPRE2	  { {TMODE::MODE0,   25 NS }, {TMODE::MODE1,   25 NS}, {TMODE::MODE2,   25 NS}, {TMODE::MODE3,   25 NS}, {TMODE::MODE4,   25 NS}, {TMODE::MODE5,   25 NS}, {TMODE::MODE6,   25 NS}, {TMODE::MODE7,   25 NS}, {TMODE::MODE8,   25 NS}, {TMODE::MODE9,   25 NS}, {TMODE::MODE10,   25 NS}, {TMODE::MODE11,    25 NS}, {TMODE::MODE12,    25 NS}, {TMODE::MODE13,   25 NS}, {TMODE::MODE14,    25 NS}, {TMODE::MODE15,   25 NS}   };
TimingParamValuesType tRPRE		  { {TMODE::MODE0,   15 NS }, {TMODE::MODE1,   15 NS}, {TMODE::MODE2,   15 NS}, {TMODE::MODE3,   15 NS}, {TMODE::MODE4,   15 NS}, {TMODE::MODE5,   15 NS}, {TMODE::MODE6,   15 NS}, {TMODE::MODE7,   15 NS}, {TMODE::MODE8,   15 NS}, {TMODE::MODE9,   15 NS}, {TMODE::MODE10,   15 NS}, {TMODE::MODE11,    15 NS}, {TMODE::MODE12,    15 NS}, {TMODE::MODE13,   15 NS}, {TMODE::MODE14,    15 NS}, {TMODE::MODE15,   15 NS}   };
TimingParamValuesType tRPRE2	  { {TMODE::MODE0,   25 NS }, {TMODE::MODE1,   25 NS}, {TMODE::MODE2,   25 NS}, {TMODE::MODE3,   25 NS}, {TMODE::MODE4,   25 NS}, {TMODE::MODE5,   25 NS}, {TMODE::MODE6,   25 NS}, {TMODE::MODE7,   25 NS}, {TMODE::MODE8,   25 NS}, {TMODE::MODE9,   25 NS}, {TMODE::MODE10,   25 NS}, {TMODE::MODE11,    25 NS}, {TMODE::MODE12,    25 NS}, {TMODE::MODE13,   25 NS}, {TMODE::MODE14,    25 NS}, {TMODE::MODE15,   25 NS}   };
TimingParamValuesType tDQSRE_min  { {TMODE::MODE0,    2 NS }, {TMODE::MODE1,    2 NS}, {TMODE::MODE2,    2 NS}, {TMODE::MODE3,    2 NS}, {TMODE::MODE4,    2 NS}, {TMODE::MODE5,    2 NS}, {TMODE::MODE6,    2 NS}, {TMODE::MODE7,    2 NS}, {TMODE::MODE8,    2 NS}, {TMODE::MODE9,    2 NS}, {TMODE::MODE10,    2 NS}, {TMODE::MODE11,     2 NS}, {TMODE::MODE12,     2 NS}, {TMODE::MODE13,    2 NS}, {TMODE::MODE14,     2 NS}, {TMODE::MODE15,    2 NS}   };
TimingParamValuesType tDQSRE	  { {TMODE::MODE0,    0 NS }, {TMODE::MODE1,    0 NS}, {TMODE::MODE2,    0 NS}, {TMODE::MODE3,    0 NS}, {TMODE::MODE4,    0 NS}, {TMODE::MODE5,    0 NS}, {TMODE::MODE6,    0 NS}, {TMODE::MODE7,    0 NS}, {TMODE::MODE8,    0 NS}, {TMODE::MODE9,    0 NS}, {TMODE::MODE10,    0 NS}, {TMODE::MODE11,     0 NS}, {TMODE::MODE12,     0 NS}, {TMODE::MODE13,    0 NS}, {TMODE::MODE14,     0 NS}, {TMODE::MODE15,    0 NS}   };
TimingParamValuesType tDQSRE_max  { {TMODE::MODE0,   25 NS }, {TMODE::MODE1,   25 NS}, {TMODE::MODE2,   25 NS}, {TMODE::MODE3,   25 NS}, {TMODE::MODE4,   25 NS}, {TMODE::MODE5,   25 NS}, {TMODE::MODE6,   25 NS}, {TMODE::MODE7,   25 NS}, {TMODE::MODE8,   25 NS}, {TMODE::MODE9,   25 NS}, {TMODE::MODE10,   25 NS}, {TMODE::MODE11,    25 NS}, {TMODE::MODE12,    25 NS}, {TMODE::MODE13,   25 NS}, {TMODE::MODE14,    25 NS}, {TMODE::MODE15,   25 NS}   };
TimingParamValuesType tWPST		  { {TMODE::MODE0,  6.5 NS }, {TMODE::MODE1,  6.5 NS}, {TMODE::MODE2,  6.5 NS}, {TMODE::MODE3,  6.5 NS}, {TMODE::MODE4,  6.5 NS}, {TMODE::MODE5,  6.5 NS}, {TMODE::MODE6,  6.5 NS}, {TMODE::MODE7,  6.5 NS}, {TMODE::MODE8,  6.5 NS}, {TMODE::MODE9,  6.5 NS}, {TMODE::MODE10,  6.5 NS}, {TMODE::MODE11,   6.5 NS}, {TMODE::MODE12,   6.5 NS}, {TMODE::MODE13,  6.5 NS}, {TMODE::MODE14,   6.5 NS}, {TMODE::MODE15,  6.5 NS}   };
TimingParamValuesType tWPSTH	  { {TMODE::MODE0,   25 NS }, {TMODE::MODE1,   25 NS}, {TMODE::MODE2,   25 NS}, {TMODE::MODE3,   25 NS}, {TMODE::MODE4,   25 NS}, {TMODE::MODE5,   25 NS}, {TMODE::MODE6,   25 NS}, {TMODE::MODE7,   25 NS}, {TMODE::MODE8,   25 NS}, {TMODE::MODE9,   25 NS}, {TMODE::MODE10,   25 NS}, {TMODE::MODE11,    25 NS}, {TMODE::MODE12,    25 NS}, {TMODE::MODE13,   25 NS}, {TMODE::MODE14,    25 NS}, {TMODE::MODE15,   25 NS}   };
TimingParamValuesType tRPSTH	  { {TMODE::MODE0,   15 NS }, {TMODE::MODE1,   15 NS}, {TMODE::MODE2,   15 NS}, {TMODE::MODE3,   15 NS}, {TMODE::MODE4,   15 NS}, {TMODE::MODE5,   15 NS}, {TMODE::MODE6,   15 NS}, {TMODE::MODE7,   15 NS}, {TMODE::MODE8,   15 NS}, {TMODE::MODE9,   15 NS}, {TMODE::MODE10,   15 NS}, {TMODE::MODE11,    15 NS}, {TMODE::MODE12,    15 NS}, {TMODE::MODE13,   15 NS}, {TMODE::MODE14,    15 NS}, {TMODE::MODE15,   15 NS}   };
TimingParamValuesType tRPST		  { {TMODE::MODE0,   tper[TMODE::MODE0] / 2 + tDQSRE_max[TMODE::MODE0] }, {TMODE::MODE1,   tper[TMODE::MODE1] / 2 + tDQSRE_max[TMODE::MODE1]}, {TMODE::MODE2,   tper[TMODE::MODE2] / 2 + tDQSRE_max[TMODE::MODE2]}, {TMODE::MODE3,   tper[TMODE::MODE3] / 2 + tDQSRE_max[TMODE::MODE3]}, {TMODE::MODE4,   tper[TMODE::MODE4] / 2 + tDQSRE_max[TMODE::MODE4]}, {TMODE::MODE5,   tper[TMODE::MODE5] / 2 + tDQSRE_max[TMODE::MODE5]}, {TMODE::MODE6,   tper[TMODE::MODE6] / 2 + tDQSRE_max[TMODE::MODE6]}, {TMODE::MODE7,   tper[TMODE::MODE7] / 2 + tDQSRE_max[TMODE::MODE7]}, {TMODE::MODE8,   tper[TMODE::MODE8] / 2 + tDQSRE_max[TMODE::MODE8]}, {TMODE::MODE9,   tper[TMODE::MODE9] / 2 + tDQSRE_max[TMODE::MODE9]}, {TMODE::MODE10,   tper[TMODE::MODE10] / 2 + tDQSRE_max[TMODE::MODE10]}, {TMODE::MODE11,   tper[TMODE::MODE11] / 2 + tDQSRE_max[TMODE::MODE11]}, {TMODE::MODE12,   tper[TMODE::MODE12] / 2 + tDQSRE_max[TMODE::MODE12]}, {TMODE::MODE13,   tper[TMODE::MODE13] / 2 + tDQSRE_max[TMODE::MODE13]}, {TMODE::MODE14,   tper[TMODE::MODE14] / 2 + tDQSRE_max[TMODE::MODE14]}, {TMODE::MODE15,   tper[TMODE::MODE15] / 2 + tDQSRE_max[TMODE::MODE15]} };
TimingParamValuesType tDS		  { {TMODE::MODE0,   4 NS }, {TMODE::MODE1,   3.3 NS}, {TMODE::MODE2,   2.0 NS}, {TMODE::MODE3,   1.1 NS}, {TMODE::MODE4,   0.7 NS}, {TMODE::MODE5,   0.6 NS}, {TMODE::MODE6,   0.55 NS}, {TMODE::MODE7,   0.4 NS}, {TMODE::MODE8,   0.3 NS}, {TMODE::MODE9,   0.26 NS}, {TMODE::MODE10,   0.24 NS}, {TMODE::MODE11,   0.22 NS}, {TMODE::MODE12,   0.2 NS}, {TMODE::MODE13,   0.18 NS}, {TMODE::MODE14,   0.16 NS}, {TMODE::MODE15,   0.14 NS} };
TimingParamValuesType tDH		  { {TMODE::MODE0,   4 NS }, {TMODE::MODE1,   3.3 NS}, {TMODE::MODE2,   2.0 NS}, {TMODE::MODE3,   1.1 NS}, {TMODE::MODE4,   0.7 NS}, {TMODE::MODE5,   0.6 NS}, {TMODE::MODE6,   0.55 NS}, {TMODE::MODE7,   0.4 NS}, {TMODE::MODE8,   0.3 NS}, {TMODE::MODE9,   0.26 NS}, {TMODE::MODE10,   0.24 NS}, {TMODE::MODE11,   0.22 NS}, {TMODE::MODE12,   0.2 NS}, {TMODE::MODE13,   0.2 NS}, {TMODE::MODE14,   0.18 NS}, {TMODE::MODE15,   0.18 NS} };

// Function to load default formats in format ram
void load_default_formats()
{
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT1,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT2,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_RTZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT3,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_RTO			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT4,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_RTC			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT5,		enTGFormatMag7::t_RTZ,			enTGFormatMag7::t_RTZ,			enTGFormatMag7::t_RTZ,			enTGFormatMag7::t_RTZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT6,		enTGFormatMag7::t_RTO,			enTGFormatMag7::t_RTO,			enTGFormatMag7::t_RTO,			enTGFormatMag7::t_RTO			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT7,		enTGFormatMag7::t_RTC,			enTGFormatMag7::t_RTC,			enTGFormatMag7::t_RTC,			enTGFormatMag7::t_RTC			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT8,		enTGFormatMag7::t_SBC,			enTGFormatMag7::t_SBC,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT9,		enTGFormatMag7::t_DCLKPOS,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT10,	enTGFormatMag7::t_DCLKNEG,		enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ,			enTGFormatMag7::t_NRZ			);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT11,	enTGFormatMag7::t_STROBE_E,		enTGFormatMag7::t_STROBE_E,		enTGFormatMag7::t_STROBE_E,		enTGFormatMag7::t_STROBE_E		);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT13,	enTGFormatMag7::t_STROBE_EM,	enTGFormatMag7::t_STROBE_EM,	enTGFormatMag7::t_STROBE_EM,	enTGFormatMag7::t_STROBE_EM	);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT14,	enTGFormatMag7::t_STROBE_EV,	enTGFormatMag7::t_STROBE_EV,	enTGFormatMag7::t_STROBE_EV,	enTGFormatMag7::t_STROBE_EV	);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT15,	enTGFormatMag7::t_NO_EDGES,		enTGFormatMag7::t_NO_EDGES,		enTGFormatMag7::t_NO_EDGES,		enTGFormatMag7::t_NO_EDGES		);
		format_set( pinlist::AllPins(), enFormatLocsNumberMag7::t_FMT16,	enTGFormatMag7::t_STROBE_EX,	enTGFormatMag7::t_STROBE_EX,	enTGFormatMag7::t_STROBE_EX,	enTGFormatMag7::t_STROBE_EX);
}

// NV-DDR2/3 Configuration Sub Feature P2 Warmup cycles for Data Output
 void update_timing_owc( device::TMODE tm, device::WARMUP_CYCLES warmup_cycles, bool is_odt_enabled, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui		= tper[tm] / 2;
	double stb_e1		= tDQSRE_max[tm] / 2;
	double read_pre1	= max((is_odt_enabled ? tRPRE2[tm] : tRPRE[tm]), tDQSD_max[tm]);

	// set default and overwrite RE in switch
	switch (warmup_cycles)
	{
		using device::WARMUP_CYCLES;
		double read_pre2;	// cycle length of tset for BOCB
		double warmup_ui;	// number of ui for warmup

	case device::WARMUP_CYCLES::WARMUP_CYCLE0:
		warmup_ui = 0;
		read_pre2 = IDLE_CYC;

		cycle( HSN_TSET(TS_READ_PREAMBLE_1), read_pre1, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_NONE, 0 );
		cycle( HSN_TSET(TS_READ_PREAMBLE_2), read_pre2, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_NONE, 0 );
		settime( HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllReadEnablePins(), FMT_NRZ,     0 NS, false ); // RE_t will go low
		settime( HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllReadEnablePins(), FMT_NRZ,	  -1,	false );
		settime( HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataStrobePins(), FMT_STB_E,	  HSN_NOEDGE8, false);
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE1:
		warmup_ui = 2 * dut_ui;
		read_pre2 = max(IDLE_CYC, warmup_ui);
		output("+++read_pre2 = %f NS, warmup = %f NS", read_pre2/(1 NS), warmup_ui /(1 NS));
		cycle( HSN_TSET(TS_READ_PREAMBLE_1), read_pre1, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_NONE, 0);
		cycle( HSN_TSET(TS_READ_PREAMBLE_2), read_pre2, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_12, 0x3);
		settime( HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllReadEnablePins(), FMT_NRZ,	0 NS,												false	); // RE_t will go low
		settime( HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllReadEnablePins(), FMT_NRZ,	HSN_EDGES2(read_pre2 - warmup_ui, dut_ui),			false	);
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE2:
		warmup_ui = 4 * dut_ui;
		read_pre2 = max(IDLE_CYC, warmup_ui);
		cycle( HSN_TSET(TS_READ_PREAMBLE_1), read_pre1, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_NONE, 0);
		cycle( HSN_TSET(TS_READ_PREAMBLE_2), read_pre2, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_1234, 0xf);
		settime( HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllReadEnablePins(), FMT_NRZ, 0 NS,													false	); // RE_t will go low
		settime( HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllReadEnablePins(), FMT_NRZ,	HSN_EDGES4(read_pre2 - warmup_ui, dut_ui),			false	);
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE3:
	case device::WARMUP_CYCLES::WARMUP_CYCLE4:
		warmup_ui = 8 * dut_ui;
		read_pre2 = max(IDLE_CYC, warmup_ui);
		output("+++read_pre2 = %f NS, warmup = %f NS", read_pre2 / (1 NS), warmup_ui / (1 NS));
		cycle( HSN_TSET(TS_READ_PREAMBLE_1), read_pre1, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_NONE, 0);
		cycle( HSN_TSET(TS_READ_PREAMBLE_2), read_pre2, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_12345678, 0xff);
		settime( HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllReadEnablePins(), FMT_NRZ, 0 NS,													false	); // RE_t will go low
		settime( HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllReadEnablePins(), FMT_NRZ,	HSN_EDGES8(read_pre2 - warmup_ui, dut_ui),			false	);
		break;
	}

	settime(HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllInputPins_minusRE(),		FMT_NRZ,	0 NS,				false);
	settime(HSN_TSET(TS_READ_PREAMBLE_1), pinlist::pl_ENo(),					FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllDataPins(),				FMT_STB_E,	HSN_NOEDGE8,		false);	
	settime(HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllDataStrobePins(),			FMT_NRZ,	tDQSRH[tm],			false);	//DQS should drive for tDQSRH, drive low at tDQSRH
	settime(HSN_TSET(TS_READ_PREAMBLE_1), pinlist::AllReadyBusyPins(),			FMT_STB_E,	HSN_NOEDGE8,		false);

	settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllInputPins_minusRE(),		FMT_NRZ,	0 NS,				false);
	settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::pl_ENo(),					FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataPins(),				FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataStrobePins(),			FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllReadyBusyPins(),			FMT_STB_E,	HSN_NOEDGE8,		false);
}

 // NV-DDR2/3 Configuration Sub Feature P2 Warmup cycles for Data Input
 void update_timing_iwc( device::TMODE tm, device::WARMUP_CYCLES warmup_cycles, bool is_odt_enabled, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = tper[tm] / 2;

	output("++++update_timing_iwc: dut_ui = %f NS", dut_ui / (1 NS));


	// set default and overwrite DQS and RE in switch
	settime( HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllPins(), FMT_NRZ, 0 NS, false );
	settime( HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllPins(), FMT_NRZ, -1, false );

	switch (warmup_cycles)
	{
		using device::WARMUP_CYCLES;
		double half_cyc;	// cycle length of tset for BOCA and BOCB
		double boca_cyc;
		double bocb_cyc;
		double warmup_ui;	// number of ui for warmup

	case device::WARMUP_CYCLES::WARMUP_CYCLE0:
		//half_cyc = is_odt_enabled ? tWPRE2[tm] / 2 : tWPRE[tm] / 2;
		boca_cyc = is_odt_enabled ? tWPRE2[tm] : tWPRE[tm];
		bocb_cyc = IDLE_CYC;
		//cycle( HSN_TSET(TS_WRITE_PREAMBLE_1), half_cyc, enPinModesMag7::t_PM_BOCx8, ehi );
		//cycle( HSN_TSET(TS_WRITE_PREAMBLE_2), half_cyc, enPinModesMag7::t_PM_BOCx8, ehi );
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_1), boca_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_2), bocb_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		settime( HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllDataStrobePins(), FMT_NRZ,		0 NS,										false ); // DQS_t will go low
		settime( HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataStrobePins(), FMT_NRZ,		-1,											false );
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE1:
		warmup_ui = 2 * dut_ui;
		half_cyc = is_odt_enabled ? (tWPRE2[tm] + warmup_ui) / 2 : (tWPRE[tm] + warmup_ui) / 2;
		boca_cyc = is_odt_enabled ? tWPRE2[tm] : tWPRE[tm];
		bocb_cyc = warmup_ui;
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_1), boca_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_2), bocb_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		settime( HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllDataStrobePins(), FMT_NRZ,		0 NS,										false ); // DQS_t will go low
		//settime( HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataStrobePins(), FMT_NRZ,		HSN_EDGES2(half_cyc - warmup_ui, dut_ui),	false );
		settime(HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataStrobePins(),	FMT_NRZ,		HSN_EDGES2(dut_ui / 2, dut_ui), false);
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE2:
		warmup_ui = 4 * dut_ui;
		//half_cyc = is_odt_enabled ? (tWPRE2[tm] + warmup_ui) / 2 : (tWPRE[tm] + warmup_ui) / 2;
		boca_cyc = is_odt_enabled ? tWPRE2[tm] : tWPRE[tm];
		bocb_cyc = warmup_ui;
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_1), boca_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_2), bocb_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		settime( HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllDataStrobePins(),	FMT_NRZ,		0 NS,										false ); // DQS_t will go low
		settime( HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataStrobePins(),	 FMT_NRZ,		HSN_EDGES4(dut_ui / 2, dut_ui),				false );
		break;

	case device::WARMUP_CYCLES::WARMUP_CYCLE3:
	case device::WARMUP_CYCLES::WARMUP_CYCLE4:
		warmup_ui = 8 * dut_ui;
		//half_cyc = is_odt_enabled ? (tWPRE2[tm] + warmup_ui) / 2 : (tWPRE[tm] + warmup_ui) / 2;
		boca_cyc = is_odt_enabled ? tWPRE2[tm] : tWPRE[tm];
		bocb_cyc = warmup_ui;
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_1), boca_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		cycle( HSN_TSET(TS_WRITE_PREAMBLE_2), bocb_cyc, enPinModesMag7::t_PM_BOCx8, vui, vm );
		settime( HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllDataStrobePins(), FMT_NRZ,			0 NS,										false ); // DQS_t will go low
		settime( HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataStrobePins(), FMT_NRZ,			HSN_EDGES8(dut_ui / 2, dut_ui),				false );
		break;
	}

	settime(HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllInputPins(),				FMT_NRZ,	0 NS,				false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::pl_ENo(),					FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllDataPins(),				FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_1), pinlist::AllReadyBusyPins(),			FMT_STB_E,	HSN_NOEDGE8,		false);

	settime(HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllInputPins(),				FMT_NRZ,	0 NS,				false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::pl_ENo(),					FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllDataPins(),				FMT_STB_E,	HSN_NOEDGE8,		false);
	settime(HSN_TSET(TS_WRITE_PREAMBLE_2), pinlist::AllReadyBusyPins(),			FMT_STB_E,	HSN_NOEDGE8,		false);
 }

void set_default_timings( device::TMODE tm, device::WARMUP_CYCLES input_wc, device::WARMUP_CYCLES output_wc, enUICaptureMag7 vui, BYTE vm )
{
	edge_strobe(pinlist::AllDataAndDataStrobePins(), TRUE);

	double dut_ui = tper[tm] / 2;
	//double dut_ui = uv_cycle NS;

	/////////////////////////////////////////////////////////////////////////

	// Timings that are consistent in all timing modes

	/////////////////////////////////////////////////////////////////////////

	// Idle Cycle - no edges generated (will be used for BOCB) 
	cycle( HSN_TSET(TS_IDLE), IDLE_CYC, enPinModesMag7::t_PM_BOCx1, vui, vm );

	settime( HSN_TSET(TS_IDLE), pinlist::AllInputPins(),				FMT_NRZ,					-1,		false );
	settime( HSN_TSET(TS_IDLE), pinlist::AllDataStrobePins(),			FMT_NRZ,					-1,		false );
	settime( HSN_TSET(TS_IDLE), pinlist::AllDataPins(),					FMT_STB_X,					-1,		false );		//turn driver off
	settime( HSN_TSET(TS_IDLE), pinlist::AllRB_ENo(),					FMT_STB_X,					-1,		false );

	//BOCx8
	//DQS @ HIZ
	cycle( HSN_TSET(TS_IDLE_DQZ), IDLE_CYC, enPinModesMag7::t_PM_BOCx8, vui, vm );

	settime( HSN_TSET(TS_IDLE_DQZ), pinlist::AllInputPins(),			FMT_NRZ,					-1,	false );
	settime( HSN_TSET(TS_IDLE_DQZ), pinlist::AllDataStrobePins(),		FMT_STB_E,					-1,	false );
	settime( HSN_TSET(TS_IDLE_DQZ), pinlist::AllDataPins(),				FMT_STB_E,					-1,	false );
	settime( HSN_TSET(TS_IDLE_DQZ), pinlist::AllRB_ENo(),				FMT_STB_E,					-1,	false );

	//settime(HSN_TSET(TS_IDLE),	pinlist::AllInputPins(),				enTGFormatMag7::t_IODRIVE,	0 NS);	
	//settime(HSN_TSET(TS_IDLE),	pinlist::AllDataStrobePins(),			enTGFormatMag7::t_IODRIVE,	0 NS);
	//settime(HSN_TSET(TS_IDLE),	pinlist::AllDataPins(),					enTGFormatMag7::t_IOSTROBE, 0 NS);
	//settime(HSN_TSET(TS_IDLE),	pinlist::AllRB_ENo(),					enTGFormatMag7::t_IOSTROBE, 0 NS);

	// Bus Idle Wait Cycle
	cycle( HSN_TSET(TS_BUS_IDLE_WAIT), IDLE_CYC, enPinModesMag7::t_PM_BOCx1, vui, vm );

	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllInputPins(),				FMT_NRZ,		0 NS,	false);
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::pl_ENo(),						FMT_STB_X,		-1,		false);
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllChipEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::ALE_CLE(),					FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllWriteEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllReadEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllDataPins(),				FMT_STB_X,		-1,		false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllDataStrobePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT), pinlist::AllReadyBusyPins(),			FMT_STB_X,		-1,		false );

	// Bus Idle Wait Cycle, will be used for DQS reset for source sync
	cycle( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), IDLE_CYC, enPinModesMag7::t_PM_BOCx1, vui, vm );

	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllInputPins(),				FMT_NRZ,		0 NS,	false);
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::pl_ENo(),						FMT_STB_X,		-1,		false);
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllChipEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::ALE_CLE(),					FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllWriteEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllReadEnablePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllDataPins(),				FMT_STB_X,		-1,		false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllDataStrobePins(),			FMT_NRZ,		0 NS,	false );
	settime( HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllReadyBusyPins(),			FMT_STB_X,		-1,		false );

	// Command Cycle
	//cycle( HSN_TSET(TS_CMD), tCS[tm] + tCALH[tm] - IDLE_CYC, enPinModesMag7::t_PM_BOCx1, ehi );
	cycle( HSN_TSET(TS_CMD), tCS[tm] + tCALH[tm], enPinModesMag7::t_PM_BOCx1, vui, vm );		//for debug only - bryan 082321

	settime( HSN_TSET(TS_CMD), pinlist::AllInputPins(),					FMT_NRZ,		0 NS,											false);
	settime( HSN_TSET(TS_CMD), pinlist::pl_ENo(),						FMT_STB_X,		-1,											false);
	settime( HSN_TSET(TS_CMD), pinlist::AllChipEnablePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_CMD), pinlist::ALE_CLE(),						FMT_NRZ,		tCS[tm] - tCALS[tm],							false );
	settime( HSN_TSET(TS_CMD), pinlist::AllWriteEnablePins(),			FMT_RTO,		tCS[tm] - tWP[tm],		tCS[tm],				false );
	settime( HSN_TSET(TS_CMD), pinlist::AllReadEnablePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_CMD), pinlist::AllDataStrobePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_CMD), pinlist::AllDataPins(),					FMT_NRZ,		tCS[tm] - tCAS[tm],								false );
	settime( HSN_TSET(TS_CMD), pinlist::AllReadyBusyPins(),				FMT_STB_X,		-1,											false );

	// Address Cycle
	//cycle( HSN_TSET(TS_ADDR), tCS[tm] + tCALH[tm] - IDLE_CYC, enPinModesMag7::t_PM_BOCx1, ehi );
	cycle( HSN_TSET(TS_ADDR), tCS[tm] + tCALH[tm], enPinModesMag7::t_PM_BOCx1, vui, vm );	//for debug only - bryan 082321

	settime( HSN_TSET(TS_ADDR), pinlist::AllInputPins(),				FMT_NRZ,		0 NS,											false);
	settime( HSN_TSET(TS_ADDR), pinlist::pl_ENo(),						FMT_STB_X,		-1,												false);
	settime( HSN_TSET(TS_ADDR), pinlist::AllChipEnablePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_ADDR), pinlist::ALE_CLE(),						FMT_NRZ,		tCS[tm] - tCALS[tm],							false );
	settime( HSN_TSET(TS_ADDR), pinlist::AllWriteEnablePins(),			FMT_RTO,		tCS[tm] - tWP[tm],		tCS[tm],				false );
	settime( HSN_TSET(TS_ADDR), pinlist::AllReadEnablePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_ADDR), pinlist::AllDataStrobePins(),			FMT_NRZ,		0 NS,											false );
	settime( HSN_TSET(TS_ADDR), pinlist::AllDataPins(),					FMT_NRZ,		tCS[tm] - tCAS[tm],								false );
	settime( HSN_TSET(TS_ADDR), pinlist::AllReadyBusyPins(),			FMT_STB_X,		-1,												false );

	// Write Postamble
	cycle( HSN_TSET(TS_WRITE_POSTAMBLE), tWPST[tm] + tWPSTH[tm]- IDLE_CYC, enPinModesMag7::t_PM_BOCx1, vui, vm);
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllInputPins(),			FMT_NRZ,	-1,		false);
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::pl_ENo(),				FMT_STB_X,	-1,		false);
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllChipEnablePins(),	FMT_NRZ,	0 NS,	false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::ALE_CLE(),				FMT_NRZ,	0 NS,	false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllWriteEnablePins(),	FMT_NRZ,	0 NS,	false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllReadEnablePins(),	FMT_NRZ,	0 NS,	false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllDataStrobePins(),	FMT_NRZ,	-1,		false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllDataPins(),			FMT_NRZ,	-1,		false );
	settime( HSN_TSET(TS_WRITE_POSTAMBLE), pinlist::AllReadyBusyPins(),		FMT_STB_X,	-1,		false );

	/////////////////////////////////////////////////////////////////////////

	// Timings that are different by timing modes

 	/////////////////////////////////////////////////////////////////////////

	// Read Postamble
	//cycle( HSN_TSET(TS_READ_POSTAMBLE), tRPST[tm] - IDLE_CYC, enPinModesMag7::t_PM_BOCx1, ehi );
	cycle( HSN_TSET(TS_READ_POSTAMBLE), tRPST[tm] + tRPSTH[tm], enPinModesMag7::t_PM_BOCx1, vui, vm );

	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllInputPins(),			FMT_NRZ,		0 NS,					false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::pl_ENo(),				FMT_STB_X,		-1,						false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllChipEnablePins(),		FMT_NRZ,		0 NS,					false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::ALE_CLE(),				FMT_NRZ,		0 NS,					false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllWriteEnablePins(),	FMT_NRZ,		0 NS,					false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllReadEnablePins(),		FMT_NRZ,		tRPST[tm],				false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllDataStrobePins(),		FMT_STB_E,		-1,						false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllDataPins(),			FMT_STB_E,		-1,						false );
	settime( HSN_TSET(TS_READ_POSTAMBLE), pinlist::AllReadyBusyPins(),		FMT_STB_E,		-1,						false );

	// Page Read
	// -for fast speeds and EG retime mode, the strobe edges will need training
	//   place 1st edge at tDQSRE found + 2ui
	// -for fast speeds and DQS retime mode, the 1st strobe edge is placed at tDQSRE/2
	double stb_e1 = tDQSRE_max[tm] / 2;

	cycle( HSN_TSET(TS_DATA_OUT), dut_ui *8, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_12345678, 0xff );

	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllInputPins(),				FMT_NRZ,	0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::pl_ENo(),						FMT_STB_E,	HSN_NOEDGE8,					false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllChipEnablePins(),			FMT_NRZ,	 0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::ALE_CLE(),						FMT_NRZ,	 0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllWriteEnablePins(),			FMT_NRZ,	 0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllReadEnablePins(),			FMT_NRZ,	 HSN_EDGES8( 0 NS,	  dut_ui),	false );
	////// RRRRR BOCx8 needs 8 times specified
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllDataStrobePins(),			FMT_STB_E,  HSN_EDGES8( stb_e1, dut_ui),	false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllDataPins(),					FMT_STB_E,	HSN_EDGES8( stb_e1, dut_ui),	false );
	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllReadyBusyPins(),			FMT_STB_E,	HSN_NOEDGE8,					false );

	//settime(HSN_TSET(TS_DATA_OUT),	pinlist::AllInputPins(),					enTGFormatMag7::t_IODRIVE,	0 NS);
	//settime(HSN_TSET(TS_DATA_OUT),	pinlist::AllDataAndDataStrobePins(),		enTGFormatMag7::t_IOSTROBE,	0 NS);
	//settime(HSN_TSET(TS_DATA_OUT),	pinlist::AllRB_ENo(),						enTGFormatMag7::t_IOSTROBE, 0 NS);

	output("****dut_ui = %f NS", dut_ui/(1 NS));

	cycle( HSN_TSET(TS_DATA_OUT_x4), dut_ui * 4, enPinModesMag7::t_PM_BOCx4, enUICaptureMag7::t_UI_1234, 0xf);

	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllInputPins(),			FMT_NRZ, 0 NS,									false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::pl_ENo(),				FMT_STB_X,		-1,								false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllChipEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::ALE_CLE(),				FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllWriteEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllReadEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataStrobePins(),	FMT_STB_E,		HSN_EDGES4(stb_e1, dut_ui),		false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataPins(),			FMT_STB_E,      HSN_EDGES4(stb_e1, dut_ui),		false );
	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllReadyBusyPins(),		FMT_STB_X,		-1,								false );

	double cycle_time = dut_ui * 2 >= MIN_MAJOR_CYCLE ? dut_ui * 2 : MIN_MAJOR_CYCLE;
	cycle( HSN_TSET(TS_DATA_OUT_x2), cycle_time, enPinModesMag7::t_PM_BOCx2, enUICaptureMag7::t_UI_12, 0x1);

	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllInputPins(),			FMT_NRZ, 0 NS,									false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::pl_ENo(),				FMT_STB_X,		-1,								false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllChipEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::ALE_CLE(),				FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllWriteEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllReadEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllDataStrobePins(),	FMT_STB_X,		-1,								false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllDataPins(),			FMT_STB_E,      HSN_EDGES2(stb_e1, dut_ui),		false );
	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllReadyBusyPins(),		FMT_STB_X,		-1,								false );

	//settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllInputPins(),             enTGFormatMag7::t_IODRIVE,  0 NS );
	//settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataAndDataStrobePins(), enTGFormatMag7::t_IOSTROBE, 0 NS );
	//settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllRB_ENo(),				enTGFormatMag7::t_IOSTROBE,	0 NS );

	cycle( HSN_TSET(TS_DATA_IN_x4), dut_ui * 4, enPinModesMag7::t_PM_BOCx4, vui, vm );

	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllInputPins(),			FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::pl_ENo(),				FMT_STB_X,		-1,								false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllChipEnablePins(),		FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::ALE_CLE(),				FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllWriteEnablePins(),	FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllReadEnablePins(),		FMT_NRZ,		0 NS,							false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataStrobePins(),		FMT_NRZ,		HSN_EDGES4(dut_ui / 2, dut_ui), false );	//debug only
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataPins(),			FMT_NRZ,		HSN_EDGES4(0 NS, dut_ui),		false );

	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllReadyBusyPins(),		FMT_STB_X,		-1,								false) ;

	//settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllInputPins(),				enTGFormatMag7::t_IODRIVE,  0 NS );
	//settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataAndDataStrobePins(),	enTGFormatMag7::t_IODRIVE, 0 NS );
	//settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllRB_ENo(),					enTGFormatMag7::t_IOSTROBE, 0 NS);

	// Page Write
	cycle( HSN_TSET(TS_DATA_IN), dut_ui *8, enPinModesMag7::t_PM_BOCx8, vui, vm );

	settime( HSN_TSET(TS_DATA_IN), pinlist::AllInputPins(),			FMT_NRZ,		0 NS,								false);
	settime( HSN_TSET(TS_DATA_IN), pinlist::pl_ENo(),				FMT_STB_E,		HSN_NOEDGE8,						false);
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllChipEnablePins(),	FMT_NRZ,		0 NS,								false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::ALE_CLE(),				FMT_NRZ,		0 NS,								false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllWriteEnablePins(),	FMT_NRZ,		0 NS,								false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllReadEnablePins(),	FMT_NRZ,		0 NS,								false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllDataStrobePins(),	FMT_NRZ,		HSN_EDGES8( dut_ui / 2, dut_ui),	false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllDataPins(),			FMT_NRZ,		HSN_EDGES8( 0 NS,		 dut_ui),	false );
	settime( HSN_TSET(TS_DATA_IN), pinlist::AllReadyBusyPins(),		FMT_STB_E,		HSN_NOEDGE8,						false );

	//settime(HSN_TSET(TS_DATA_IN),	pinlist::AllInputPins(),				enTGFormatMag7::t_IODRIVE,	0 NS);
	//settime(HSN_TSET(TS_DATA_IN),	pinlist::AllDataAndDataStrobePins(),	enTGFormatMag7::t_IODRIVE,	0 NS);
	//settime(HSN_TSET(TS_DATA_IN),	pinlist::AllRB_ENo(),					enTGFormatMag7::t_IOSTROBE,	0 NS);

	// Ready/Busy poll
	//cycle( HSN_TSET(TS_RBSY), RBSY_CYC, enPinModesMag7::t_PM_BOCx1, ehi );
	cycle( HSN_TSET(TS_RBSY), RBSY_CYC, enPinModesMag7::t_PM_BOCx1, enUICaptureMag7::t_UI_1, 1 );

	settime( HSN_TSET(TS_RBSY),		pinlist::AllInputPins(),				FMT_NRZ, 0 NS,								false );
	settime( HSN_TSET(TS_RBSY),		pinlist::pl_ENo(),						FMT_STB_X,		-1,							false );
	settime( HSN_TSET(TS_RBSY),		pinlist::AllDataStrobePins(),			FMT_NRZ,		-1,							false );
	settime( HSN_TSET(TS_RBSY),		pinlist::AllDataPins(),					FMT_STB_X,		-1,							false );
	settime( HSN_TSET(TS_RBSY),		pinlist::AllReadyBusyPins(),			FMT_STB_E,		0 NS,						false );
	
	//settime( HSN_TSET(TS_RBSY),		pinlist::AllInputPins(),			enTGFormatMag7::t_IODRIVE, 0 NS );
	//settime( HSN_TSET(TS_RBSY),		pinlist::AllDataStrobePins(),		enTGFormatMag7::t_IODRIVE, 0 NS);
	//settime( HSN_TSET(TS_RBSY),		pinlist::AllRB_ENo(),				enTGFormatMag7::t_IOSTROBE, 0 NS );

	//for checking levelsets
	//use 100 ns for Levelset cycle
	cycle(HSN_TSET(TS_LEVELSET), 1000 NS, enPinModesMag7::t_PM_BOCx1, vui, vm);
	settime(HSN_TSET(TS_LEVELSET),	pinlist::AllPins(),						FMT_NRZ, 0 NS, false);
	
	/////////////////////////////////////////////////////////////////////////

	// Timings that are different by warmup cycles

	/////////////////////////////////////////////////////////////////////////

	// Write and read preambles with warmups
	update_timing_owc( tm, input_wc, false, vui, vm ); // TODO: Get ODT status
	update_timing_iwc( tm, output_wc, false, vui, vm ); // TODO: Get ODT status

	update_timings_tm( tm, vui, vm );

	//enable strobe masking on DQ pins
	data_mask_mode_set( pinlist::AllDataAndDataStrobePins(), true);

	//enable use of APG cs1/cs2 strobing on DQS pins
	chip_select_read_mode_set( pinlist::AllDataStrobePins(), true);
}

///////////////////////////////////////////////////////////////////////////////////

// update_timings_tm() helper functions

///////////////////////////////////////////////////////////////////////////////////

static void set_TS_DATA_IN_x4( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = tper[tm] / 2;
	//double dut_ui = (uv_cycle / 4) NS;
	output( "++++set_TS_DATA_IN_x4: dut_ui = %f NS", dut_ui / (1 NS) );

	cycle( HSN_TSET(TS_DATA_IN_x4), dut_ui * 4, enPinModesMag7::t_PM_BOCx4, vui, vm );

	// TODO: Update DQS edge drive edge
	//settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataStrobePins(),  FMT_NRZ, HSN_EDGES4(tDS[tm], dut_ui), false );
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataStrobePins(), FMT_NRZ, HSN_EDGES4(dut_ui / 2, dut_ui), false );	//debug only - bryan 080421
	settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataPins(),      FMT_NRZ, HSN_EDGES4(0 NS, dut_ui), false );
}

static void set_TS_DATA_IN_x8( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = tper[tm] / 2;
	//double dut_ui = (uv_cycle / 4) NS;
	output("++++set_TS_DATA_IN: dut_ui = %f NS", dut_ui / (1 NS));

	cycle( HSN_TSET(TS_DATA_IN), dut_ui * 8, enPinModesMag7::t_PM_BOCx8, vui, vm );

	// TODO: Update DQS edge drive edge
	//settime( HSN_TSET(TS_DATA_IN_x4), pinlist::AllDataStrobePins(),  FMT_NRZ, HSN_EDGES4(tDS[tm], dut_ui), false );
	settime(HSN_TSET(TS_DATA_IN), pinlist::AllDataStrobePins(), FMT_NRZ, HSN_EDGES8(dut_ui / 2, dut_ui), false);	//debug only - bryan 080421
	settime(HSN_TSET(TS_DATA_IN), pinlist::AllDataPins(), FMT_NRZ, HSN_EDGES8(0 NS, dut_ui), false);
}

// TS_DATA_OUT_x2 is intended to be a tset for PINFUNC1
// IDLE_CYC must follow in PINFUNC2
static void set_TS_DATA_OUT_x2( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = tper[tm] / 2;
	//double dut_ui = uv_cycle NS;

	// TODO: Add tDQSQ trained value to stb_e1
	double stb_e1 = tDQSRE[tm]; // trained tDQSRE value per timing mode
	//double stb_e1 = 100 NS; // trained tDQSRE value per timing mode

	double cycle_time = dut_ui * 2 >= MIN_MAJOR_CYCLE ? dut_ui * 2 : MIN_MAJOR_CYCLE;
	cycle( HSN_TSET(TS_DATA_OUT_x2), cycle_time - IDLE_CYC, enPinModesMag7::t_PM_BOCx2, enUICaptureMag7::t_UI_12, 0x1 );

	settime( HSN_TSET(TS_DATA_OUT_x2), pinlist::AllReadEnablePins(),  FMT_NRZ,   HSN_EDGES2(0 NS, dut_ui),	false );
	settime(HSN_TSET(TS_DATA_OUT_x2), pinlist::AllDataPins(), FMT_STB_E, dut_ui / 2, false);

	//output("set_TS_DATA_OUT_x2: uv_stb = %f", uv_stb);
}

void set_TS_DATA_OUT_x4( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = SHM_TORNADO ? (1 / uv_rate) NS : tper[tm] / 2;
	output("++++set_TS_DATA_OUT_x4: dut_ui = %f NS", dut_ui / (1 NS));

	// TODO: Add tDQSQ trained value to stb_e1
	double stb_e1 = SHM_DATAEYE || SHM_TORNADO ? uv_stb NS : (tDQSRE_max[tm] - tDQSRE_min[tm]) / 2 + tDQSRE_min[tm];

	cycle( HSN_TSET(TS_DATA_OUT_x4), dut_ui * 4, enPinModesMag7::t_PM_BOCx4, enUICaptureMag7::t_UI_1234, 0xf );

	settime( HSN_TSET(TS_DATA_OUT_x4), pinlist::AllReadEnablePins(),		FMT_NRZ,   HSN_EDGES4(0 NS, dut_ui),		false );
	if (is_ssync_enabled() == false)
	{
		settime(HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataStrobePins(), FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
		settime(HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataPins(),		FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
	}
	else
	{
		DutNumArray duts;

		active_duts_get(&duts);
		for (int i = 0; i < duts.GetSize(); ++i)
		{
			DutNum dut = duts[i];

			stb_e1 = tdqsre[dut] + uv_eg_retime_offset * dut_ui;
			settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataPins(dut), FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
		}
	}
}

void set_TS_DATA_OUT_x8( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	double dut_ui = SHM_DATAEYE || SHM_TORNADO ? (1 / uv_rate) NS : tper[tm] / 2;
	output("++++set_TS_DATA_OUT: dut_ui = %f NS", dut_ui / (1 NS));

	// TODO: Add tDQSQ trained value to stb_e1
	double stb_e1 = SHM_DATAEYE || SHM_TORNADO ? uv_stb NS : (tDQSRE_max[tm] - tDQSRE_min[tm]) / 2 + tDQSRE_min[tm];

	cycle( HSN_TSET(TS_DATA_OUT), dut_ui * 8, enPinModesMag7::t_PM_BOCx8, enUICaptureMag7::t_UI_12345678, 0xff );

	settime( HSN_TSET(TS_DATA_OUT), pinlist::AllReadEnablePins(),	FMT_NRZ,	HSN_EDGES8(0 NS, dut_ui),	false );
	if (is_ssync_enabled() == false)
	{
		settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataStrobePins(), FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
		settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataPins(),		 FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
	}
	//SOURCE SYNC is enabled
	else 
	{
		DutNumArray duts;

		active_duts_get(&duts);
		for (int i = 0; i< duts.GetSize(); ++i)
		{
			DutNum dut = duts[i];

			stb_e1 = tdqsre[dut] + uv_eg_retime_offset * dut_ui;
			settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataPins(dut), FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////

// Call this function when timing mode is changed
void update_timings_tm( device::TMODE tm, enUICaptureMag7 vui, BYTE vm )
{
	output("update_timings_tm");

	set_TS_DATA_OUT_x2( tm, vui, vm );
	set_TS_DATA_OUT_x4( tm, vui, vm );
	set_TS_DATA_OUT_x8( tm, vui, vm );

	set_TS_DATA_IN_x4( tm, vui, vm );
	set_TS_DATA_IN_x8( tm, vui, vm );

	// Write and read preambles with warmups
	bool is_odt_enabled = (get_odt_setting() != device::ODT::Rtt_Disable);
	update_timing_owc( tm, get_output_warmup_cycles(), is_odt_enabled, vui, vm);
	update_timing_iwc( tm, get_input_warmup_cycles(),  is_odt_enabled, vui, vm);
}

//set dqs preamble for non-source sync
//set dq preamble for eg retime. Also if setting dq, set valid_ui and capture to 0
static void set_preamble(PinList *pinl, device::TMODE tm, double stb_e1)
{
	if (uv_source_sync != "OFF")
	{
		double period;	// cycle length of tset for BOCB
		enPinModesMag7 pm;
		enUICaptureMag7 valid_ui;
		BYTE ui_care_enable;
		cycle(HSN_TSET(TS_READ_PREAMBLE_2), &period, &pm, &valid_ui, &ui_care_enable);
		//set valid_ui and ui_care_enable to 0
		cycle(HSN_TSET(TS_READ_PREAMBLE_2), period, pm, enUICaptureMag7::t_UI_NONE, 0);
	}

	bool is_odt_enabled = (get_odt_setting() != device::ODT::Rtt_Disable);

	double dut_ui		= tper[tm] / 2;
	double read_pre1	= max((is_odt_enabled ? tRPRE2[tm] : tRPRE[tm]), tDQSD_max[tm]);

	switch (get_output_warmup_cycles())
	{
		double read_pre2;	// cycle length of tset for BOCB
		double warmup_ui;	// number of ui for warmup

	case device::WARMUP_CYCLES::WARMUP_CYCLE0:
		warmup_ui = 0;
		read_pre2 = IDLE_CYC;
		settime( HSN_TSET(TS_READ_PREAMBLE_2),	pinl, FMT_STB_E,	HSN_NOEDGE8, false);
		break;
	case device::WARMUP_CYCLES::WARMUP_CYCLE1:
		warmup_ui = 2 * dut_ui;
		read_pre2 = max( IDLE_CYC, warmup_ui);
		settime( HSN_TSET(TS_READ_PREAMBLE_2),	pinl, FMT_STB_E,	HSN_EDGES2(read_pre2 - warmup_ui + stb_e1, dut_ui),	false	);
		break;
	case device::WARMUP_CYCLES::WARMUP_CYCLE2:
		warmup_ui = 4 * dut_ui;
		read_pre2 = max(IDLE_CYC, warmup_ui);
		settime(HSN_TSET(TS_READ_PREAMBLE_2),	pinl, FMT_STB_E,	HSN_EDGES4(read_pre2 - warmup_ui + stb_e1, dut_ui), false);
		break;
	case device::WARMUP_CYCLES::WARMUP_CYCLE3:
	case device::WARMUP_CYCLES::WARMUP_CYCLE4:
		warmup_ui = 8 * dut_ui;
		read_pre2 = max(IDLE_CYC, warmup_ui);
		settime(HSN_TSET(TS_READ_PREAMBLE_2),	pinl, FMT_STB_E,	HSN_EDGES8(read_pre2 - warmup_ui + stb_e1, dut_ui), false);
		break;
	}
}

//for DQS retime
//set DQ bus strobe edges for preamble based on number of warmup cycles
static void set_dq_preamble(device::TMODE tm)
{
	if (uv_source_sync == "DQS_RETIME")
	{
		double period;	// cycle length of tset for BOCB
		enPinModesMag7 pm;
		enUICaptureMag7 valid_ui;
		BYTE ui_care_enable;
		cycle(HSN_TSET(TS_READ_PREAMBLE_2), &period, &pm, &valid_ui, &ui_care_enable);
		//set valid_ui and ui_care_enable to 0
		//cycle(HSN_TSET(TS_READ_PREAMBLE_2), period, pm, enUICaptureMag7::t_UI_NONE, 0);
		cycle(HSN_TSET(TS_READ_PREAMBLE_2), period, pm, valid_ui, 0);

		double re_edge0 = getedge(HSN_TSET(TS_READ_PREAMBLE_2), dutpin::RE_t(), 1);
		//output("re edge0 = %f NS", re_edge0/ (1 NS));

		double stb_e1 = (tDQSRE_max[tm] - tDQSRE_min[tm]) / 2 + tDQSRE_min[tm];
		stb_e1 += re_edge0;

		double dut_ui = tper[tm] / 2;
		bool is_odt_enabled = (get_odt_setting() != device::ODT::Rtt_Disable);

		switch (get_output_warmup_cycles())
		{
		case device::WARMUP_CYCLES::WARMUP_CYCLE0:
			settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataPins(), FMT_STB_E, HSN_NOEDGE8, false);
			break;
		case device::WARMUP_CYCLES::WARMUP_CYCLE1:
			settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataPins(), FMT_STB_E, HSN_EDGES2(stb_e1, dut_ui), false);
			break;
		case device::WARMUP_CYCLES::WARMUP_CYCLE2:
			settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataPins(), FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
			break;
		case device::WARMUP_CYCLES::WARMUP_CYCLE3:
		case device::WARMUP_CYCLES::WARMUP_CYCLE4:
			settime(HSN_TSET(TS_READ_PREAMBLE_2), pinlist::AllDataPins(), FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
			break;
		}
	}
}

//EG Retime
//find tDQSRE
//set strobes on DQ bus to tDQSRE + 2 ui
void search_and_set_tDQSRE_eg_retime(device::TMODE tm, double start, double stop, double step, Pattern* pat_training)
{
	if (uv_source_sync == "DQS_RETIME")
	{
		output("uv_source_sync is DQS_RETIME, bypass training");
		results_set(PASS);
		return;
	}

	shmoo::shmoo_setup_t setup;
	setup.keyword = "tDQSRE";

	std::string dqs_name = "tDQSRE_DQSt";
	std::string dq_bus_name = "tDQSRE_AllDataPins";

	SOFT_DUT_LOOP(dut)
	{
		setup.parameters[static_cast<int>(dut)] = {
			shmoo::shmoo_parameter_t(dqs_name,			pinlist::pl_DQS_t(dut)),
			shmoo::shmoo_parameter_t(dq_bus_name,	pinlist::AllDataPins(dut)),
		};
	}

	setup.start = start;
	setup.end = stop;
	setup.step = step;
	setup.pattern = (pat_training == nullptr) ? PAT__WriteReadCache_x4 : pat_training;
	setup.search_mode = shmoo::SEARCH_MODE::LINEAR;
	setup.stop_on_transition = false;
	setup.shmoo_type = shmoo::SHMOO_TYPE::WINDOW;
	setup.ignore_holes = true;

	DWORD active_dut_mask;
	active_duts_get(&active_dut_mask);

	shmoo::shmoo_result result(setup);
	result.set_active_dut_mask(active_dut_mask);

	double dut_ui = tper[tm] / 2;

	result.set_pre_pattern_hook(
		[&]()
		{
			SOFT_DUT_LOOP(dut)
			{
				// Reset JAM RAM address pointer
				// For APG2, 3, and 4, JAM RAM address should start from 4
				//reset_jamram_address(4);

				shmoo::shmoo_parameters_t& shmoo_parameters = result.shmoo_parameters(dut);
				for (auto it = std::begin(shmoo_parameters); it != std::end(shmoo_parameters); ++it)
				{
					double stb_e1 = result.current_shmoo_value(dut, it->name);

					//set strobe for TS_DATA_OUT_x4
					settime(HSN_TSET(TS_DATA_OUT_x4), it->pinlist, FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
					//set strobe for TS_DATA_OUT
					settime(HSN_TSET(TS_DATA_OUT), it->pinlist, FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
					//preamble strobe settings for DQS(non-source sync)
					std::string s_pinl(resource_name(it->pinlist));
					if (s_pinl.find("DQS") != -1)
						set_preamble(it->pinlist, tm, stb_e1);
				}
			}
		}
	);

	result.set_post_pattern_hook(
		[&]()
		{
			SOFT_DUT_LOOP(dut)
			{
				shmoo::shmoo_parameters_t& shmoo_parameters = result.shmoo_parameters(dut);

				for (auto it = std::begin(shmoo_parameters); it != std::end(shmoo_parameters); ++it)
				{
					bool pass_fail = test_pin(it->pinlist);
					result.set_result(dut, it->name, pass_fail);
				}
			}
		}
	);

	result.execute();
	result.print();

	{SOFT_DUT_LOOP(dut)
	{
		//find tDQSRE
		if (result.find_value(dut, dqs_name, shmoo::VALUE_TYPE::WINDOW_START) != -1)
		{
			double val = result.find_value(dut, dqs_name, shmoo::VALUE_TYPE::WINDOW_START);
			tdqsre[dut] = val;

			output("DUT%d tdqsre: %.2f PS", dut + 1, tdqsre[dut]);

			///////////////set DQ strobes//////////////
			strobe_vals[dut][pinlist::AllDataPins()] = tdqsre[dut] + uv_eg_retime_offset * dut_ui;
			//set dq preamble
			set_preamble(pinlist::AllDataPins(), tm, strobe_vals[dut][pinlist::AllDataPins()]);
			output("DUT%d DQ bus setstrobe: %.2f PS", dut + 1, strobe_vals[dut][pinlist::AllDataPins()]);

			//set strobe for TS_DATA_OUT_x4
			settime(HSN_TSET(TS_DATA_OUT_x4), pinlist::AllDataPins(), FMT_STB_E, HSN_EDGES4(strobe_vals[dut][pinlist::AllDataPins()], dut_ui), false);
			settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataPins(), FMT_STB_E, HSN_EDGES8(strobe_vals[dut][pinlist::AllDataPins()], dut_ui), false);

			///////////////////////////////////////////

			result_set(dut, PASS);
		}
		else
		{
			warning("DUT%d tDQSRE was not found.", dut + 1);
			result_set(dut, FAIL);
			continue;
		}

		//find DQ data eye center
		if (result.find_value(dut, dq_bus_name, shmoo::VALUE_TYPE::WINDOW_CENTER) != -1)
		{
			double val = result.find_value(dut, dqs_name, shmoo::VALUE_TYPE::WINDOW_CENTER);

			//set dqs_dq delay
			src_sync_dqs_dq_offset_set(pinlist::AllDataStrobePins(), val - tdqsre[dut]);

			result_set(dut, PASS);
		}
		else
		{
			warning("DUT%d DQ window was not found.", dut + 1);
			result_set(dut, FAIL);
			continue;
		}
	}}
}

//non source sync
void search_and_set_tDQSRE(device::TMODE tm, double start, double stop, double step, Pattern *pat_training)
{
	if (uv_source_sync == "DQS_RETIME")
	{
		output("uv_source_sync is DQS_RETIME, bypass training");
		results_set(PASS);
		return;
	}

	if (uv_source_sync == "EG_RETIME")
	{
		search_and_set_tDQSRE_eg_retime( tm, start, stop, step, pat_training);
		return;
	}

	shmoo::shmoo_setup_t setup;
	setup.keyword = "tDQSRE";

	SOFT_DUT_LOOP(dut)
	{
		setup.parameters[static_cast<int>(dut)] = {
		shmoo::shmoo_parameter_t("tDQSRE_DQSt",	pinlist::pl_DQS_t(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQSc",	pinlist::pl_DQS_c(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ0",	pinlist::pl_DQ0(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ1",	pinlist::pl_DQ1(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ2",	pinlist::pl_DQ2(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ3",	pinlist::pl_DQ3(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ4",	pinlist::pl_DQ4(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ5",	pinlist::pl_DQ5(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ6",	pinlist::pl_DQ6(dut)),
		shmoo::shmoo_parameter_t("tDQSRE_DQ7",	pinlist::pl_DQ7(dut))
		};
	}

	setup.start = start;
	setup.end = stop;
	setup.step = step;
	setup.pattern = (pat_training == nullptr) ? PAT__WriteReadCache_x4 : pat_training;
	setup.search_mode = shmoo::SEARCH_MODE::LINEAR;
	setup.stop_on_transition = false;
	setup.shmoo_type = shmoo::SHMOO_TYPE::WINDOW;
	setup.ignore_holes = true;

	DWORD active_dut_mask;
	active_duts_get(&active_dut_mask);

	shmoo::shmoo_result result(setup);
	result.set_active_dut_mask(active_dut_mask);

	double dut_ui = tper[tm] / 2;

	result.set_pre_pattern_hook(
		[&]()
		{
			SOFT_DUT_LOOP(dut)
			{
				// Reset JAM RAM address pointer
				// For APG2, 3, and 4, JAM RAM address should start from 4
				//reset_jamram_address(4);

				shmoo::shmoo_parameters_t& shmoo_parameters = result.shmoo_parameters(dut);
				for (auto it = std::begin(shmoo_parameters); it != std::end(shmoo_parameters); ++it)
				{
					double stb_e1 = result.current_shmoo_value(dut, it->name);

					//set strobe for TS_DATA_OUT_x4
					settime(HSN_TSET(TS_DATA_OUT_x4),	it->pinlist, FMT_STB_E, HSN_EDGES4(stb_e1, dut_ui), false);
					//set strobe for TS_DATA_OUT
					settime(HSN_TSET(TS_DATA_OUT),		it->pinlist, FMT_STB_E, HSN_EDGES8(stb_e1, dut_ui), false);
					//preamble strobe settings for DQS(non-source sync)
					std::string s_pinl(resource_name(it->pinlist));
					if (s_pinl.find("DQS") != -1)
						set_preamble(it->pinlist, tm, stb_e1);
				}
			}
		}
	);

	result.set_post_pattern_hook(
		[&]()
		{
			SOFT_DUT_LOOP(dut)
			{
				shmoo::shmoo_parameters_t& shmoo_parameters = result.shmoo_parameters(dut);

				for (auto it = std::begin(shmoo_parameters); it != std::end(shmoo_parameters); ++it)
				{
					bool pass_fail = test_pin(it->pinlist);
					result.set_result(dut, it->name, pass_fail);
				}
			}
		}
	);

	result.execute();
	result.print();

	output("Per Pin Data Eye");
	//use strobe values from training to program strobe edges
	{SOFT_DUT_LOOP(dut)
	{
		shmoo::shmoo_parameters_t& shmoo_parameters = result.shmoo_parameters(dut);
		for (auto it = shmoo_parameters.begin(); it != shmoo_parameters.end(); ++it)
		{
			if (result.find_value(dut, it->name, shmoo::VALUE_TYPE::WINDOW_CENTER) != -1)
			{
				std::string s_pinl(resource_name(it->pinlist));
				bool b_is_a_dqs = (s_pinl.find("DQS") != -1);

				double val = result.find_value(dut, it->name, shmoo::VALUE_TYPE::WINDOW_CENTER);
				strobe_vals[dut][it->pinlist] = val;

				if ((b_is_a_dqs == false) && (uv_source_sync == "EG_RETIME"))
				{
					strobe_vals[dut][it->pinlist] += uv_eg_retime_offset * dut_ui;
					//set dq preamble
					set_preamble(it->pinlist, tm, strobe_vals[dut][it->pinlist]);
				}
				output("DUT%d %-15s setstrobe: %.2f PS", dut + 1, resource_name(it->pinlist), strobe_vals[dut][it->pinlist]);

				//set strobe for TS_DATA_OUT_x4
				settime(HSN_TSET(TS_DATA_OUT_x4),	it->pinlist, FMT_STB_E, HSN_EDGES4(strobe_vals[dut][it->pinlist], dut_ui), false);
				settime(HSN_TSET(TS_DATA_OUT),		it->pinlist, FMT_STB_E, HSN_EDGES8(strobe_vals[dut][it->pinlist], dut_ui), false);
				//preamble strobe settings for DQS(non-source sync) and DQ(eg retime)
				if ((b_is_a_dqs == true) && (uv_source_sync == "OFF"))
				{
					set_preamble(it->pinlist, tm, strobe_vals[dut][it->pinlist]);
				}

				result_set(dut, PASS);
			}
			else
			{
				warning("DUT%d %s window was not found. No setstrobe edge was programmed.", dut + 1, resource_name(it->pinlist));
				result_set(dut, FAIL);
			}
		}
	}}
}


//This function is for hardware training and Shmoo
void Hwsearch_and_shmoo(device::TMODE tm, double start, double stop, double step, Pattern* pat_training)
{
	if (uv_Hwtraining==FALSE)
	{
		output("uv_Hwtraing is false, bypass training");
		results_set(PASS);
		return;
	}


	double dut_ui = tper[tm] / 2;

	//Specify TSET of sweeping edge for Hardware sweep
	CArray <NXTTSETNumber, NXTTSETNumber> tsets;
	tsets.Add(enTSETNumberMag7::t_TSET2);

	//Setting start position of strobe for sweep
	settime(HSN_TSET(TS_DATA_OUT), pinlist::AllDataAndDataStrobePins(), FMT_STB_E, HSN_EDGES8(start, dut_ui), false);

	//For sweep by only PIN_SWEEP_ADJ instruction 
	wt_capture_trigger_mode_set(enWaveToolCapTrigModesMag7::t_uRamBit_only);

	//Sweep count in pattern
	int sweepc = int(((stop - start) / step) - 1);
	count(C_PIN_SWEEP, sweepc);

	pin_sweep_setup_set(pinlist::AllDataAndDataStrobePins(), 0 NS, step, enPinSweepTypeMag7::t_STROBE, tsets);


	//Shmoo window size
	gv_vec_size = int((stop - start) / step);

	DutNumArray all_duts;
	int dut_cnt = active_duts_get(&all_duts);

	//Initialize global variable,gv_window
	for (int i_dut = 0; i_dut < dut_cnt; i_dut++)
	{
		DutNum dut = all_duts[i_dut];
		gv_windows[dut].resize(gv_vec_size);
		for (std::size_t j = 0; j < gv_vec_size; ++j) {
			gv_windows[dut].push_back(0);
		}
	}

	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(pat_training);

	//Global variable, gv_window are stored by fls callback 
	//Display shmoo from gv_window
	//"P" means all DQ pins PASS
	//"F" means all DQ pins FAIL
	//"0x--" means Fail pins. Ex. 0x3 means DQ0 and DA1 are fail.

	for (int i_dut = 0; i_dut < dut_cnt; i_dut++)
	{
		DutNum dut = all_duts[i_dut];

		std::stringstream os;
		// print all shmoo values
		os << std::left << std::setw(26) << "Shmoo Step :";
		for (unsigned int index = 0; index < gv_vec_size; ++index) {
			os << std::right << std::setw(6) << std::setfill(' ') << index;
		}
		os << std::endl;
		os << std::left << std::setw(26) << "Shmoo Value:";
		for (unsigned int index = 0; index < gv_vec_size; ++index) {
			os << std::right << std::setw(6) << start + step * index;
		}
		os << std::endl;
		os << std::left << "DUT" << std::dec << (dut + 1) << ":";
		os << std::setw(21) << " ";
		for (unsigned int index = 0; index < gv_vec_size; ++index) {
			os << std::right << std::setw(6);
			if (gv_windows[dut].at(index) == 0) os << "<COLOR=GREEN>     P</COLOR>";				
			else if (gv_windows[dut].at(index) == 0xff) os << "<COLOR=RED>     F</COLOR>";			
			else  os << "<COLOR=RED>  0x"<< std::left << std::setw(2) << std::hex<<(gv_windows[dut].at(index))<<"</COLOR>";
		}
		os << std::endl;
		output(os.str().c_str());
	}
	results_set(PASS);
	return;
}


void restore_timing( device::TMODE tm, enPinModesMag7 pm )
{
	//restore default cycles
	set_TS_DATA_OUT_x8( get_timing_mode(), enUICaptureMag7::t_UI_NONE, 0 );

	double dut_ui = tper[tm] / 2;

	//restore per dut strobe settings
	SOFT_DUT_LOOP(dut)
	{
		for (auto it = std::begin(strobe_vals[dut]); it != std::end(strobe_vals[dut]); ++it)
		{
			double val = strobe_vals[dut][it->first];

			std::string s_pinl(resource_name(it->first));
			bool b_is_a_dqs = (s_pinl.find("DQS") != -1);

			if(b_is_a_dqs == false)
			{
				output("restore pin %s %f PS", resource_name(it->first), val);

				if (pm == enPinModesMag7::t_PM_BOCx4)
					settime(HSN_TSET(TS_DATA_OUT_x4), it->first, FMT_STB_E, HSN_EDGES4(val, dut_ui), false);
				if (pm == enPinModesMag7::t_PM_BOCx8)
					settime(HSN_TSET(TS_DATA_OUT), it->first, FMT_STB_E, HSN_EDGES8(val, dut_ui), false);
			}
			else if(is_ssync_enabled() ==false)	//dqs pin, only program if ssync is off
			{
				output("restore pin %s %f PS", resource_name(it->first), val);

				if (pm == enPinModesMag7::t_PM_BOCx4)
					settime(HSN_TSET(TS_DATA_OUT_x4), it->first, FMT_STB_E, HSN_EDGES4(val, dut_ui), false);
				if (pm == enPinModesMag7::t_PM_BOCx8)
					settime(HSN_TSET(TS_DATA_OUT), it->first, FMT_STB_E, HSN_EDGES8(val, dut_ui), false);
			}
		}
	}
}

//place holder for the source sync functions
//this will change as I get more familiar with Mag7 source sync
//eg retime
//	find middle of dq data eye
//	dq strobe settime of dq + 2 ui
//	set dq_dqs offset to 0.5 ui???
//	call setup_src_sync in strobe training function
//dqs retime
//	dq strobe settime of 0.5 tdqsre
//	call setup_src_sync in set_default_timing
//how to do dq sweep???
void setup_src_sync()
{
	output("uv_source_sync %s", uv_source_sync);

	if (uv_source_sync != "OFF")
	{
		//set source sync mode per pin
		src_sync_mode_set(pinlist::AllPins(),			SRC_SYNC_MODE_OFF);			//disable allpins first
		src_sync_mode_set(pinlist::AllDataStrobePins(), SRC_SYNC_MODE_DQS);
		src_sync_mode_set(pinlist::AllDataPins(),		SRC_SYNC_MODE_DQ);

		//set up DQS channels
		//src_sync_dqs_cmp_mode_set(pinlist::AllDataStrobePins(), CMP_MODE_DIFF);
		src_sync_dqs_cmp_mode_set(pinlist::AllDataStrobePins(),		CMP_MODE_SE);
		src_sync_dqs_invert_ena_set(pinlist::AllDataStrobePins(),	false);
		src_sync_data_rate_mode_set(pinlist::AllDataStrobePins(),	enSrcSyncDataRateModeMag7::t_DDR);
		//for eg retime, it is set to middle of eye found by dut training
		//for dqs retime, it is a default for right now
		if(uv_source_sync == "DQS_RETIME") src_sync_dqs_dq_offset_set(pinlist::AllDataStrobePins(),	uv_dqs_dq_offset PS);	

		src_sync_dqs_bus_set(pinlist::AllDataPins(), 0);

		//DQS Retime Mode - enum member - SRC_SYNC_RETIME_MODE_DQS : If the mode is SRC_SYNC_RETIME_MODE_DQS, the DQS signal itself retimes the latched DQ data into the tester time domain, and is buffered via a FIFO.The TSET can trigger a reset of the DQS FIFO if the settime function that defines the TSET has the DQS_RESET parameter set to TRUE.See the dqs_reset parameter description of settime(enTSETNumberMag7, enFormatLocsNumberMag7, double, bool) for more information.
		//EG Retime Mode - enum member - SRC_SYNC_RETIME_MODE_EG : If the mode is SRC_SYNC_RETIME_MODE_EG, the compare Edge Generators are used to retime the latched DQ data into the tester time domain.
		src_sync_retime_mode_set(pinlist::AllDataStrobePins(), uv_source_sync == "EG_RETIME" ? enSrcSyncRetimeModeMag7::t_EG : enSrcSyncRetimeModeMag7::t_DQS);
		output("source sync mode %s", src_sync_retime_mode_get(pinlist::AllDataStrobePins())== enSrcSyncRetimeModeMag7::t_EG ? "EG RETIME" : "DQS RETIME");

		//turn on dqs reset
		settime(HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllDataPins(), FMT_STB_E, HSN_NOEDGE8, true);

		//turn off edges for DQS
		settime( HSN_TSET(TS_DATA_OUT_x4),		pinlist::AllDataStrobePins(), FMT_STB_E, HSN_NOEDGE8, false);
		settime( HSN_TSET(TS_DATA_OUT),			pinlist::AllDataStrobePins(), FMT_STB_E, HSN_NOEDGE8, false);
		settime( HSN_TSET(TS_READ_PREAMBLE_2),  pinlist::AllDataStrobePins(), FMT_STB_E, HSN_NOEDGE8, false);

		//take care of premable tset
		//turn off valid ui and ui care enable
		set_dq_preamble(get_timing_mode());

		ssync_enabled = true;
	}
}

void disable_src_sync()
{
	//set_value( uv_source_sync, "OFF");
	output("disabling source sync on allpins");
	src_sync_mode_set(pinlist::AllPins(), SRC_SYNC_MODE_OFF);

	//turn off dqs reset
	settime(HSN_TSET(TS_BUS_IDLE_WAIT_DQS_RST), pinlist::AllDataPins(), FMT_STB_E, HSN_NOEDGE8, false);

	ssync_enabled = false;
}

