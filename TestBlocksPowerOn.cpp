#include "stdafx.h" 
#include "PinLists.h"
#include "Levels.h"
#include "Timings.h"
#include "PatternInitializations.h" // set_counters_init()
#include "i2c.h"
#include "WaveformUtils.h"
#include "Datalogs.h"
#include "DeviceStatus.h"

DOUBLE_VARIABLE(ISB_VCC_MAX_UA, 70, " ") {}
DOUBLE_VARIABLE(ISB_VCC_MIN_UA, 10, " ") {}

BOOL_VARIABLE(Waveform_MeasureCurrent, FALSE, " ") {}
BOOL_VARIABLE(Waveform_VCC, TRUE, " ") {} 

TEST_BLOCK( TB__PowerOn )
{
	output("%s", current_test_block());

	// Capture power-on ramp
	DWORD ads;
	auto pPinList = Waveform_VCC ? pinlist::pl_VCC() : pinlist::pl_VCCQ();
	if(active_duts_get(&ads) == 1)
	{
		waveform::trigger_capture(pPinList, Waveform_MeasureCurrent, 2048, 10 US);		
	}

	// Apply power
	// TODO: Check typical Vcc, Vccq, Vpp levels
	levels::power_up( 2.5 V, 1.2 V, 0 V );

	// Read captured waveform
	if(active_duts_get(&ads) == 1)
	{
		waveform::read(pPinList, Waveform_MeasureCurrent, vFormat("Power Up"));		
	}

	// Default modes:
	// -Timing Mode 0
	// -Input Warmup Cycles 0
	// -Output Warmup Cycles 0
	set_timing_mode(device::TMODE::MODE0);
	set_input_warmup_cycles(device::WARMUP_CYCLES::WARMUP_CYCLE0);
	set_output_warmup_cycles(device::WARMUP_CYCLES::WARMUP_CYCLE0);
	set_default_timings();

	// Set counter values
	set_counters_init();

	// Force all DUTs to pass
	results_set(PASS);

	return MULTI_DUT;
}

TEST_BLOCK(TB__isb_vcc)
{
	output("%s", current_test_block());

	measure(TRUE);

	pin_dc_state_set(pinlist::AllChipEnablePins(), PEDriverState::t_vih, TRUE);

	adc_average_count_set(100, pinlist::pl_VCC());

	dps_current_high(ISB_VCC_MAX_UA UA, pinlist::pl_VCC());            //range 2
	dps_current_low(ISB_VCC_MIN_UA UA, pinlist::pl_VCC());
	test_supply(pass_nicl, pinlist::pl_VCC(), PartestOpt::iacc);
	pdatalog(pinlist::pl_VCC());

	pin_dc_state_set(pinlist::AllChipEnablePins(), PEDriverState::t_vil, FALSE);

	return MULTI_DUT;
}

TEST_BLOCK( TB__Reset_0xFF )
{
	output("%s", current_test_block());

	// Make sure all CE# signals are asserted and can be controlled by the Pattern
	pin_dc_state_set( pinlist::AllChipEnablePins(), PEDriverState::t_vil, FALSE );

	// Execute
	funtest( PAT__Reset_0xFF, finish );

	// This pattern shouldn't cause any DUT to fail but check anyways
	// in case there is a problem w/ the pattern definition.
	if( any_results_match(FAIL) ) 
		warning("reset_targets() -- One or more DUTs failed unexpectedly during the pattern execution");

	// Force all DUTs to pass
	results_set(PASS);

	return MULTI_DUT;
}

TEST_BLOCK(TB__voltage_force)
{
	output("%s", current_test_block());

	// Make sure all CE# signals are asserted and can be controlled by the Pattern
	pin_dc_state_set(pinlist::AllPins(), PEDriverState::t_vih, TRUE);

	return MULTI_DUT;
}


