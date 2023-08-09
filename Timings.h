#pragma once
#include "stdafx.h"
#include "DeviceSpecs.h"

//format ram defines for BOCx1
#define FMT_NRZ		enFormatLocsNumberMag7::t_FMT1
#define FMT_RTZ		enFormatLocsNumberMag7::t_FMT5
#define FMT_RTO		enFormatLocsNumberMag7::t_FMT6
#define FMT_RTC		enFormatLocsNumberMag7::t_FMT7
#define FMT_SBC		enFormatLocsNumberMag7::t_FMT8
#define FMT_DCLKP	enFormatLocsNumberMag7::t_FMT9
#define FMT_DCLKN	enFormatLocsNumberMag7::t_FMT10
#define FMT_STB_E	enFormatLocsNumberMag7::t_FMT11
#define FMT_STB_W	enFormatLocsNumberMag7::t_FMT12
#define FMT_STB_EM	enFormatLocsNumberMag7::t_FMT13
#define FMT_STB_EV	enFormatLocsNumberMag7::t_FMT14
#define FMT_NO_EDGE	enFormatLocsNumberMag7::t_FMT15
#define FMT_STB_X	enFormatLocsNumberMag7::t_FMT16

//will generate Opus tset numbers from pattern alias
#define HSN_TSET(tset) enTSETNumberMag7::CONCAT( t_, tset)

#define HSN_EDGES2( edge1, ui ) \
edge1, edge1 + ui

#define HSN_EDGES4( edge1, ui ) \
edge1, edge1 + ui, edge1 + ui * 2, edge1 + ui * 3

#define HSN_EDGES6( edge1, ui ) \
edge1, edge1 + ui, edge1 + ui * 2, edge1 + ui * 3, edge1 + ui * 4, edge1 + ui * 5

#define HSN_EDGES8( edge1, ui ) \
edge1, edge1 + ui, edge1 + ui * 2, edge1 + ui * 3, edge1 + ui * 4, edge1 + ui * 5, edge1 + ui * 6, edge1 + ui * 7

#define HSN_NOEDGE1 \
- 1

#define HSN_NOEDGE8 \
- 1, -1, -1, -1, -1, -1, -1, -1

EXTERN_BOOL_VARIABLE(SHM_DATAEYE)
EXTERN_BOOL_VARIABLE(SHM_TORNADO)

EXTERN_ONEOF_VARIABLE(uv_source_sync)

EXTERN_DOUBLE_VARIABLE(uv_dqs_dq_offset) 

// Return 1515 PS
const double get_IDLE_CYC();

// Function to load default formats in format ram
void load_default_formats();

// Set default timings
// Must be called before using the update_timings_* functions
void set_default_timings( device::TMODE tm = device::TMODE::MODE0, device::WARMUP_CYCLES input_wc = device::WARMUP_CYCLES::WARMUP_CYCLE0, device::WARMUP_CYCLES output_wc = device::WARMUP_CYCLES::WARMUP_CYCLE0, enUICaptureMag7 vui = enUICaptureMag7::t_UI_NONE, BYTE vm = 0 );

// Update timings based on the timing mode
void update_timings_tm( device::TMODE tm, enUICaptureMag7 vui, BYTE vm = 0 );

// Update timings based on the input warmup cycles
void update_timing_iwc( device::TMODE tm, device::WARMUP_CYCLES input_warmup_cycles, bool is_odt_enabled, enUICaptureMag7 vui, BYTE vm = 0 );

// Update timings based on the output warmup cycles
void update_timing_owc( device::TMODE tm, device::WARMUP_CYCLES output_warmup_cycles, bool is_odt_enabled, enUICaptureMag7 vui, BYTE vm = 0 );

//update BOCx4 read timing
void set_TS_DATA_OUT_x4( device::TMODE tm, enUICaptureMag7 vui, BYTE vm = 0 );

//update BOCx8 read timing
void set_TS_DATA_OUT_x8( device::TMODE tm, enUICaptureMag7 vui, BYTE vm = 0 );

void search_and_set_tDQSRE( device::TMODE tm, double start, double stop, double step, Pattern* pat_training = nullptr );

//Hwsearch_and_shmoo
void Hwsearch_and_shmoo(device::TMODE tm, double start, double stop, double step, Pattern* pat_training = nullptr);

//restore timing after shmoo
void restore_timing( device::TMODE tm, enPinModesMag7 pm );

//enable source sync
void setup_src_sync(void);

//disable source sync
void disable_src_sync(void);

//return true if source sync is enabled, otherwise return false
const bool is_ssync_enabled();

EXTERN_BOOL_VARIABLE(uv_Hwtraining)
//Global varible to store FLS results
__declspec(selectany) std::map<DutNum, std::vector<int>> gv_windows;
__declspec(selectany) std::size_t gv_vec_size;