#pragma once
#include "TestProgApp/public.h"
#include "DeviceSpecs.h"

// Update |_current_odt_setting| static global variable
// when Set Features to change ODT is requested.
// Return true if ODT is enabled, otherwise false
bool set_odt_setting(device::ODT odt);

// Return current ODT setting
// Default is ODT setting is disabled
device::ODT get_odt_setting();

// Update |_current_timing_mode| static global variable
// when Set Features to change timing mode is requested.
void set_timing_mode(device::TMODE timing_mode);

// Update |_current_data_Interface| static global variable
// when Set Features to change timing mode is requested.
void set_data_interface(device::DATA_INTERFACE data_interface);

// Return current timing mode
// Default is timing mode 0
device::TMODE get_timing_mode();

// Update |_current_input_warmup_cycles| static global variable
// when Set Features to change input warmup cycles is requested.
void set_input_warmup_cycles(device::WARMUP_CYCLES wc);

// Return input warmup cycles
device::WARMUP_CYCLES get_input_warmup_cycles();

// Update |_current_output_warmup_cycles| static global variable
// when Set Features to change output warmup cycles is requested.
void set_output_warmup_cycles(device::WARMUP_CYCLES wc);

// Return current timing mode
// Default is timing mode 0
device::WARMUP_CYCLES get_output_warmup_cycles();
