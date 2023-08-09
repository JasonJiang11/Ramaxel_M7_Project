#include "stdafx.h"
#include "OnfiFeatures.h"
#include "PinLists.h"
#include "Levels.h"
#include "Timings.h"
#include "DeviceStatus.h"
#include "PatternInitializations.h"
#include "FlsUserTasks.h"

// Returns a pointer-to-const mapping from a Timing Mode to its corresponding RE_t clock speed (in PS)
static const std::map< device::TMODE, double >* get_timing_mode_map()
{
	static std::map< device::TMODE, double > ret =
	{
		{ device::TMODE::MODE0,  15000 },
		{ device::TMODE::MODE1,  12500 },
		{ device::TMODE::MODE2,  7500  },
		{ device::TMODE::MODE3,  6000  },
		{ device::TMODE::MODE4,  5000  },
		{ device::TMODE::MODE5,  3750  },
		{ device::TMODE::MODE6,  3000  },
		{ device::TMODE::MODE7,  2500  },
		{ device::TMODE::MODE8,  1875  },
		{ device::TMODE::MODE9,  1500  },
		{ device::TMODE::MODE10, 1250  },
		{ device::TMODE::MODE11, 937.5 },
		{ device::TMODE::MODE12, 833.5 },
		{ device::TMODE::MODE13, 750 },
		{ device::TMODE::MODE14, 682 },
		{ device::TMODE::MODE15, 625 }
	};
	return &ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////

// Configure ONFI Interface

///////////////////////////////////////////////////////////////////////////////////////////////

// Returns a pointer-to-const mapping from a Feature Address to its name
static const std::map< BYTE, std::string >* get_feature_address_map()
{
	static std::map< BYTE, std::string > ret =
	{
		{ 0x01, "Timing Mode" },
		{ 0x02, "NV-DDR3 Configuration" },
		{ 0x10, "I/O Drive Strength" },
		{ 0x20, "DCC, Read, Write Tx Training" },
		{ 0x21, "Write Training Rx" },
		{ 0x30, "External Vpp Configuration" },
		{ 0x58, "Volume Configuration" },
		{ 0x80, "Vendor Specific" },
		{ 0x81, "RB Pull Down Strength" },
		{ 0x89, "Read Retry" },
		{ 0x90, "Array Operation Mode" },
		{ 0x91, "SLC Mode Enable" },
		{ 0x96, "Auto Read Calibration" },
		{ 0x97, "Soft Information" },
		{ 0xDF, "Word Line Status Bypass" },
		{ 0xE6, "Sleep VCCQ" },
		{ 0xE7, "Temperature Sensor Readout" },
		{ 0xF5, "Snap Read" },
		{ 0xF6, "Sleep Lite" },
		{ 0xF8, "Internal External ZQ Calibration Switching" }
	};
	return &ret;
}

// Write data to JAM RAM for set (EFh) /get (EEh) features
static void _set_jam_ram( BYTE feature_address, const std::array<BYTE, 4> & p )
{
	for( enApgNumberMag7 apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg )
	{
		for( enUnitNumberMag7 unit = enUnitNumberMag7::t_UNIT1; unit <= enUnitNumberMag7::t_UNIT4; ++unit )
		{
			apg_jam_ram_set( 0, apg, unit, feature_address );
		}
	}

	apg_jam_ram_set( 1, enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, (p[0] << 8) | p[0] );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, (p[1] << 8) | p[1] );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT3, 0 );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT4, 0 );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1, (p[2] << 8) | p[2] );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2, (p[3] << 8) | p[3] );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT3, 0 );
	apg_jam_ram_set( 1, enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT4, 0 );
}

static Pattern* _get_pattern( features::FeatureSource method, bool is_set_feature )
{
	switch( method )
	{
	case features::FeatureSource::JamRam:
		return is_set_feature ? PAT__SetFeaturesJamRam : PAT__GetFeaturesJamRam;
	case features::FeatureSource::Dmain:
		return  is_set_feature ? PAT__SetFeaturesDmain : PAT__GetFeaturesDmain;
	default:
		warning( "_get_pattern() - Not valid FeatureSource option" );
		return nullptr;
	}
}

static void _save_feature_data( BYTE feature_address, const std::array<BYTE, 4> & p, features::FeatureSource method )
{
	switch( method )
	{
	case features::FeatureSource::JamRam:
		apg_jam_ram_pointer_address_set( enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1, mag7::JAMRAM_MAX_ROW - 1 );
		apg_jam_ram_pointer_address_set( enApgNumberMag7::t_APG2, enJamRamPointerNumberMag7::t_JRP1, mag7::JAMRAM_MAX_ROW - 1 );
		apg_jam_ram_active_pointer_select_set( enApgNumberMag7::t_APG1, enJamRamPointerNumberMag7::t_JRP1 );
		apg_jam_ram_active_pointer_select_set( enApgNumberMag7::t_APG2, enJamRamPointerNumberMag7::t_JRP1 );
		_set_jam_ram( feature_address, p );
		break;

	case features::FeatureSource::Dmain:
		apg_xmain_set( enApgNumberMag7::t_APG1, feature_address );

		apg_dmain_set( enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, (p[0] << 8) | p[0] );
		apg_dmain_set( enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, (p[1] << 8) | p[1] );
		apg_dmain_set( enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1, (p[2] << 8) | p[2] );
		apg_dmain_set( enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2, (p[3] << 8) | p[3] );
		break;

	default:
		warning( "_save_feature_data() - Not valid FeatureSource option" );
		break;
	}
}

static void _set_features( BYTE feature_address, const std::array<BYTE, 4>& p, features::FeatureSource method )
{ 
	output( "feature address 0x%02x", feature_address );
	output( "feature bytes 0x%02x 0x%02x 0x%02x 0x%02x", p[0], p[1], p[2], p[3] );

	 _save_feature_data( feature_address, p, method );
	 Pattern* pPattern = _get_pattern( method, true );

	// Generate Feature name to address map
	auto pFeatureAddressMap = get_feature_address_map();
	std::string featureName;
	auto fm_it = pFeatureAddressMap->find( feature_address );
	if( fm_it == pFeatureAddressMap->end() )
		featureName = "INVALID FEATURE ADDRESS";
	else
		featureName = fm_it->second;

	output( "<COLOR=PURPLE>***** Target 0, Feature Address 0x%2X (%s) *****</COLOR>", feature_address, featureName.c_str() );
	output( "Current Timing Mode: %d", get_timing_mode() );

	funtest( pPattern, finish );
}

static void _get_features( BYTE feature_address, const std::array<BYTE, 4> & p, features::FeatureSource method )
{
	_save_feature_data( feature_address, p, method );
	Pattern* pPattern = _get_pattern( method, false );

	//funtest(pPattern, finish );
	FlsStoreRegCnt fls_data;
	fls_data.funtest(pPattern);
	output("%s", fls_data.features_dump().c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////

// Timing Mode

///////////////////////////////////////////////////////////////////////////////////////////

static BYTE get_timing_mode_p1( device::TMODE timing_mode, device::DATA_INTERFACE data_interface, bool clear_selected_cache_registers )
{
	return static_cast<BYTE>( timing_mode )                    |
	     ( static_cast<BYTE>( data_interface) << 4 )           |
	     ( clear_selected_cache_registers ? 1 << 6 : 0 << 6 );
}

void features::TimingMode::set_features()
{
	clear();

	output( "data_interface=0x%02x, clear_selected_cache_registers=0x%02x", data_interface, clear_selected_cache_registers );

	BYTE p1 = get_timing_mode_p1( timing_mode, data_interface, clear_selected_cache_registers );
	std::array<BYTE, 4> p{ p1, 0x0, 0x0, 0x0 };

	_set_features( feature_address, p, method );

	// Update the current timing mode
	set_timing_mode( timing_mode );

	// Update timings
	update_timings_tm( timing_mode, enUICaptureMag7::t_UI_NONE, 0 );
}

void features::TimingMode::get_features( bool print )
{
	clear();

	BYTE p1 = get_timing_mode_p1( timing_mode, data_interface, clear_selected_cache_registers );

	// Expected subfeature parameters
	const std::array<BYTE, 4> p{ p1, 0x00, 0x00, 0x00 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// NV-DDR3 Configurations

///////////////////////////////////////////////////////////////////////////////////////////

static BYTE get_nvddr3config_p1( bool use_external_vrefq, bool use_dqs_c, bool use_re_c, device::ODT odt )
{
	return ( use_external_vrefq ? 1 << 0 : 0 << 0 ) | // Voltage Reference
		   ( use_dqs_c ? 1 << 1 : 0 << 1 )          | // Complementary DQS
		   ( use_re_c ? 1 << 2 : 0 << 2 )           | // Complementary RE#
		   ( static_cast<BYTE>(odt) << 4 );           // ODT enable
}

static BYTE get_nvddr3config_p2( device::WARMUP_CYCLES input_warmup_cycles, device::WARMUP_CYCLES output_warmup_cycles )
{
	return ( static_cast<BYTE>(output_warmup_cycles) << 0 ) | // Output warmup cycles
		   ( static_cast<BYTE>(input_warmup_cycles) << 4 );   // Input warmup cycles
}

void features::NvDdr3Configuration::set_features()
{
	clear();

	BYTE p1 = get_nvddr3config_p1( use_external_vrefq, use_dqs_c, use_re_c, odt );
	BYTE p2 = get_nvddr3config_p2( input_warmup_cycles, output_warmup_cycles );

	std::array<BYTE, 4> p{ p1, p2, 0x00, 0x00 };

	output( "NvDdr3Configuration 0x%02x 0x%02x 0x%02x 0x%02x", p1, p2, 0, 0 );

	// Use JamRam as a source (method) of Feature Address 
	// and Subfeature parameters otherwise specified
	_set_features( feature_address, p, method );

	// Update R-term
	double rterm = 0.0;

	switch( odt )
	{
	case device::ODT::Rtt_Disable:
		rterm = 5000000;
		break;
	case device::ODT::Rtt_150:
		rterm = 150.0;
		break;
	case device::ODT::Rtt_75:
		rterm = 75.0;
		break;
	case device::ODT::Rtt_50:
		rterm = 50.0;
		break;
	default:
		break;
	}

	// Set levels, including rterm
	levels::update_rterm(rterm);

	// Update input/output warmup cycles
	set_input_warmup_cycles( input_warmup_cycles );
	set_output_warmup_cycles( output_warmup_cycles );

	// Update the current ODT setting
	bool is_odt_enabled = set_odt_setting( odt );

	// Update timings
	update_timing_owc( get_timing_mode(), get_output_warmup_cycles(), is_odt_enabled, enUICaptureMag7::t_UI_NONE, 0 );
	update_timing_iwc( get_timing_mode(), get_input_warmup_cycles(), is_odt_enabled, enUICaptureMag7::t_UI_NONE, 0 );
}

void features::NvDdr3Configuration::get_features( bool print )
{
	clear();

	BYTE p1 = get_nvddr3config_p1( use_external_vrefq, use_dqs_c, use_re_c, odt );
	BYTE p2 = get_nvddr3config_p2( input_warmup_cycles, output_warmup_cycles );

	// Expected subfeature parameters
	const std::array<BYTE, 4> p{ p1, p2, 0x00, 0x00 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// ZQ Calibration

///////////////////////////////////////////////////////////////////////////////////////////

static BYTE get_zqcal_p1( bool use_internal_zq_calibration )
{
	return use_internal_zq_calibration ? 1 : 0; // Internal/External ZQ Calibration switching
}

void features::ZqCalibration::set_features()
{
	clear();

	BYTE p1 = get_zqcal_p1( use_internal_zq_calibration );

	std::array<BYTE, 4> p{ p1, 0x00, 0x00, 0x00 };

	_set_features( feature_address, p, method );
}

void features::ZqCalibration::get_features( bool print )
{
	clear();

	BYTE p1 = get_zqcal_p1( use_internal_zq_calibration );

	const std::array<BYTE, 4> p{ p1, 0x00, 0x00, 0x00 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// Write DQ Training (Tx Side)

///////////////////////////////////////////////////////////////////////////////////////////

static BYTE get_write_dq_training_tx_p3( device::WriteTrainingDataSize size )
{
	return (static_cast<BYTE>(size) << 0)| // Write Training Data Size
		                         (1 << 4); // reserved
}

void features::WriteDqTrainingTx::set_features()
{
	clear();

	BYTE p3 = get_write_dq_training_tx_p3( size );

	std::array<BYTE, 4> p{ 0x00, 0x00, p3, 0x00 };

	_set_features( feature_address, p, method );
}

void features::WriteDqTrainingTx::get_features( bool print )
{
	clear();

	BYTE p3 = get_write_dq_training_tx_p3( size );

	const std::array<BYTE, 4> p{ 0x00, 0x00, p3, 0x00 };

	_get_features( feature_address, p, method );
}
///////////////////////////////////////////////////////////////////////////////////////////

// Array Operation Mode

///////////////////////////////////////////////////////////////////////////////////////////

void features::ArrayOperationMode::set_features()
{
	clear();

	BYTE p1 = array_operation_mode; // array operation mode
	BYTE p2 = 0;                    // reserved
	BYTE p3 = 0;                    // reserved
	BYTE p4 = 0;                    // reserved

	std::array<BYTE, 4> p{ p1, p2, p3, p4 };

	_set_features( feature_address, p, method );
}

void features::ArrayOperationMode::get_features( bool print )
{
	clear();

	BYTE p1 = array_operation_mode; // array operation mode
	BYTE p2 = 0;                    // reserved
	BYTE p3 = 0;                    // reserved
	BYTE p4 = 0;                    // reserved

	const std::array<BYTE, 4> p{ p1, p2, p3, p4 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// SLC Mode Enable

///////////////////////////////////////////////////////////////////////////////////////////
void features::SlcModeEnable::set_features()
{
	clear();

	BYTE p1 = bits_per_cell; // Bits per cell
	BYTE p2 = 1;             // required
	BYTE p3 = 0;             // reserved
	BYTE p4 = 0;             // reserved

	std::array<BYTE, 4> p{ p1, p2, p3, p4 };

	_set_features( feature_address, p, method );
}

void features::SlcModeEnable::get_features( bool print )
{
	clear();

	BYTE p1 = bits_per_cell; // Bits per cell
	BYTE p2 = 1;             // required
	BYTE p3 = 0;             // reserved
	BYTE p4 = 0;             // reserved

	const std::array<BYTE, 4> p{ p1, p2, p3, p4 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// Read Offset SLC 

///////////////////////////////////////////////////////////////////////////////////////////
void features::ReadOffsetSlc::set_features()
{
	clear();

	BYTE p1 = offset_level;
	std::array<BYTE, 4> p{ p1, 0x00, 0x00, 0x00 };

	_set_features( feature_address, p, method );
}

void features::ReadOffsetSlc::get_features( bool print )
{
	clear();

	BYTE p1 = offset_level;
	const std::array<BYTE, 4> p{ p1, 0x00, 0x00, 0x00 };

	_get_features( feature_address, p, method );
}

///////////////////////////////////////////////////////////////////////////////////////////

// Temperature Sensor Readout

///////////////////////////////////////////////////////////////////////////////////////////

void features::TemperatureSensorReadout::set_features()
{
	clear();
	warning( "Temperature Sensor Readout - Set Features: Not supported" );
}

void features::TemperatureSensorReadout::get_features( bool print )
{
	clear();

	// Expected subfeature parameters
	const std::array<BYTE, 4> p{ 0x00, 0x00, 0x00, 0x00 };

	_get_features( feature_address, p, method );
}

void features::VendorSpecific::set_features()
{
	clear();

	_set_features( feature_address, bytes, method );
}

void features::VendorSpecific::get_features( bool print )
{
	clear();

	_get_features( feature_address, bytes, method );
}