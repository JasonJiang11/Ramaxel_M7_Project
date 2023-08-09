#pragma once
#include "TestProgApp/public.h"

EXTERN_INT_VARIABLE(uv_pages_per_block_slc)

// Initialize pattern counters
//optionally take pattern simulator config as argument
void set_counters_init(enPinModesMag7 mode = enPinModesMag7::t_PM_BOCx8, PatSimConfigStructMagnum7* sim_config = nullptr);

//initialize dbase with 4 byte pattern
void set_dbase_4byte_pattern(DWORD val);

//following functions used to load xalu/yalu registers for nand addressing
DWORD set_addr_yalu(size_t col);
DWORD set_addr_xalu(size_t lun, size_t block, size_t plane, size_t page);

