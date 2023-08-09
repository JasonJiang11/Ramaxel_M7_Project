#include "stdafx.h"
#include "OnfiFeatures.h"
#include "DeviceSpecs.h"
#include "Datalogs.h"
#include "Timings.h"

// Set Features Timing Mode
static device::TMODE _timingMode = device::TMODE::MODE12;
static device::ODT	 _rterm = device::ODT::Rtt_Disable;

ONEOF_VARIABLE( TimingMode, "Mode12, Mode15, Mode14, Mode13, Mode11, Mode10, Mode9, Mode8, Mode7, Mode6, Mode5, Mode4, Mode3, Mode2, Mode1, Mode0", " " )	
{
	if( TimingMode == "Mode0" )       _timingMode = device::TMODE::MODE0;
	else if( TimingMode == "Mode1" )  _timingMode = device::TMODE::MODE1;
	else if( TimingMode == "Mode2" )  _timingMode = device::TMODE::MODE2;
	else if( TimingMode == "Mode3" )  _timingMode = device::TMODE::MODE3;
	else if( TimingMode == "Mode4" )  _timingMode = device::TMODE::MODE4;
	else if( TimingMode == "Mode5" )  _timingMode = device::TMODE::MODE5;
	else if( TimingMode == "Mode6" )  _timingMode = device::TMODE::MODE6;
	else if( TimingMode == "Mode7" )  _timingMode = device::TMODE::MODE7;
	else if( TimingMode == "Mode8" )  _timingMode = device::TMODE::MODE8;
	else if( TimingMode == "Mode9" )  _timingMode = device::TMODE::MODE9;
	else if( TimingMode == "Mode10" ) _timingMode = device::TMODE::MODE10;
	else if( TimingMode == "Mode11" ) _timingMode = device::TMODE::MODE11;
	else if( TimingMode == "Mode12" ) _timingMode = device::TMODE::MODE12;
	else if( TimingMode == "Mode13" ) _timingMode = device::TMODE::MODE13;
	else if( TimingMode == "Mode14" ) _timingMode = device::TMODE::MODE14;
	else if( TimingMode == "Mode15" ) _timingMode = device::TMODE::MODE15;
}
ONEOF_VARIABLE( DUT_RTERM,	"Disable, 50, 75, 150", " ")
{
	if (DUT_RTERM == "Disable")	_rterm = device::ODT::Rtt_Disable;
	if (DUT_RTERM == "50")		_rterm = device::ODT::Rtt_50;
	if (DUT_RTERM == "75")		_rterm = device::ODT::Rtt_75;
	if (DUT_RTERM == "150")		_rterm = device::ODT::Rtt_150;
}

// Set Features Input Warmup Cycles
static device::WARMUP_CYCLES _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE0;

ONEOF_VARIABLE(InputWarmupCycles, "InputWarmupCycle0, InputWarmupCycle1, InputWarmupCycle2, InputWarmupCycle3, InputWarmupCycle4", " ")
{
	if (InputWarmupCycles == "InputWarmupCycle4") _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE4;
	else if (InputWarmupCycles == "InputWarmupCycle3") _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE3;
	else if (InputWarmupCycles == "InputWarmupCycle2") _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE2;
	else if (InputWarmupCycles == "InputWarmupCycle1") _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE1;
	else if (InputWarmupCycles == "InputWarmupCycle0") _inputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE0;
}

// Set Features Output Warmup Cycles
static device::WARMUP_CYCLES _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE0;

ONEOF_VARIABLE(OutputWarmupCycles, "OutputWarmupCycle0, OutputWarmupCycle1, OutputWarmupCycle2, OutputWarmupCycle3, OutputWarmupCycle4", " ")
{
	if (		OutputWarmupCycles == "OutputWarmupCycle4") _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE4;
	else if (	OutputWarmupCycles == "OutputWarmupCycle3") _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE3;
	else if (	OutputWarmupCycles == "OutputWarmupCycle2") _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE2;
	else if (	OutputWarmupCycles == "OutputWarmupCycle1") _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE1;
	else if (	OutputWarmupCycles == "OutputWarmupCycle0") _outputWarmupCycle = device::WARMUP_CYCLES::WARMUP_CYCLE0;
}

TEST_BLOCK( TB__SetFeaturesTimingMode )
{
	output( "%s", current_test_block() );

	device::TMODE timing_mode = _timingMode;

	features::TimingMode tm;
	tm.timing_mode = timing_mode;
	tm.data_interface = device::DATA_INTERFACE::NV_DDR3;
	tm.clear_selected_cache_registers = true;

	tm.set_features();

	return MULTI_DUT;
}

TEST_BLOCK( TB__GetFeaturesTimingMode )
{
	output("%s", current_test_block());

	device::TMODE timing_mode = _timingMode;

	features::TimingMode tm;
	tm.timing_mode = timing_mode;
	tm.data_interface = device::DATA_INTERFACE::NV_DDR3;
	tm.clear_selected_cache_registers = true;

	Delay( 100 MS );
	tm.get_features();

	fdatalog( pinlist::AllDataDataStrobeReadyBusyPins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__SetFeaturesNvddr3Config )
{
	output( "%s", current_test_block() );

	// P1
	// External VREFQ is enabled
	// DQS_c signal is enabled
	// RE_c signal is enabled
	// ODT 50 ohm is enabled
	// P2
	// 0 (default) warmup cycles (output)
	// 0 (default) warmup cycles (input)
	features::NvDdr3Configuration nvddr3_config;
	nvddr3_config.use_external_vrefq = false;
	nvddr3_config.use_dqs_c = true;
	nvddr3_config.use_re_c = true;
	nvddr3_config.odt = _rterm;
	nvddr3_config.input_warmup_cycles = _inputWarmupCycle;
	nvddr3_config.output_warmup_cycles = _outputWarmupCycle;

	nvddr3_config.set_features();

	return MULTI_DUT;
}

TEST_BLOCK( TB__GetFeaturesNvddr3Config )
{
	output( "%s", current_test_block() );

	features::NvDdr3Configuration nvddr3_config;
	nvddr3_config.use_external_vrefq = false;
	nvddr3_config.use_dqs_c = true;
	nvddr3_config.use_re_c = true;
	nvddr3_config.odt = _rterm;
	nvddr3_config.input_warmup_cycles = _inputWarmupCycle;
	nvddr3_config.output_warmup_cycles = _outputWarmupCycle;

	nvddr3_config.get_features();

	fdatalog( pinlist::AllDataDataStrobeReadyBusyPins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__SetFeaturesZqCalSwitching )
{
	output( "%s", current_test_block() );

	features::ZqCalibration zq;
	zq.use_internal_zq_calibration = false;

	zq.set_features();

	return MULTI_DUT;
}

TEST_BLOCK( TB__GetFeaturesZqCalSwitching )
{
	output( "%s", current_test_block() );

	features::ZqCalibration zq;
	zq.use_internal_zq_calibration = false;

	zq.get_features();

	fdatalog( pinlist::AllDataDataStrobeReadyBusyPins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__SetFeaturesWriteDqTrainingTx )
{
	output( "%s", current_test_block() );

	features::WriteDqTrainingTx wdt;
	wdt.size = device::WriteTrainingDataSize::BYTE32;

	wdt.set_features();

	return MULTI_DUT;
}

TEST_BLOCK( TB__GetFeaturesWriteDqTrainingTx )
{
	output( "%s", current_test_block() );

	features::WriteDqTrainingTx wdt;

	wdt.get_features();

	return MULTI_DUT;
}

TEST_BLOCK(TB__SetFeaturesVSP)
{
	output( "%s", current_test_block() );

	features::VendorSpecific vsp( 0x80, 0, 0, 0, 0);
	vsp.set_features();

	return MULTI_DUT;
}

TEST_BLOCK(TB__GetFeaturesVSP)
{
	output( "%s", current_test_block() );

	features::VendorSpecific vsp(0x80, 0, 0, 0, 0);
	vsp.get_features();

	fdatalog( pinlist::AllDataDataStrobeReadyBusyPins() );

	return MULTI_DUT;
}