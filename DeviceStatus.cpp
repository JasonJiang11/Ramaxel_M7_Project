#include "stdafx.h"
#include "DeviceStatus.h"

// Default ODT is disabled
static device::ODT _current_odt_setting = device::ODT::Rtt_Disable;

bool set_odt_setting(device::ODT odt)
{
	_current_odt_setting = odt;

	if( odt == device::ODT::Rtt_Disable )
		return false;
	else
		return true;
}

device::ODT get_odt_setting()
{
	return _current_odt_setting;
}

// Default warmup cycles for data input (See feature address 0x02) is 0
static device::WARMUP_CYCLES _current_input_warmup_cycles = device::WARMUP_CYCLES::WARMUP_CYCLE0;
// Default warmup cycles for data output (See feature address 0x02) is 0
static device::WARMUP_CYCLES _current_output_warmup_cycles = device::WARMUP_CYCLES::WARMUP_CYCLE0;

// Default timing mode is 0
// Some pin scrambles shall be changed for different timing modes
static device::TMODE _current_timing_mode = device::TMODE::MODE0;

//default data interface is SDR
static device::DATA_INTERFACE _current_data_interface = device::DATA_INTERFACE::SDR;

void set_data_interface(device::DATA_INTERFACE data_interface)
{
	_current_data_interface = data_interface;
}

void set_timing_mode(device::TMODE timing_mode)
{
	_current_timing_mode = timing_mode;
}

void set_input_warmup_cycles(device::WARMUP_CYCLES wc)
{
	_current_input_warmup_cycles = wc;
}

void set_output_warmup_cycles(device::WARMUP_CYCLES wc)
{
	_current_output_warmup_cycles = wc;
}

device::TMODE get_timing_mode()
{
	return _current_timing_mode;
}

device::WARMUP_CYCLES get_input_warmup_cycles()
{
	return _current_input_warmup_cycles;
}

device::WARMUP_CYCLES get_output_warmup_cycles()
{
	return _current_output_warmup_cycles;
}