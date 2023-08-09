#pragma once
#include "TestProgApp/public.h"
#include "PatternInitializations.h"
#include "DeviceSpecs.h"

// TODO: Support per target (CE) 
// TODO: Support per LUN (D4h/D5h) commands
namespace features
{
	enum class FeatureSource
	{
		JamRam,
		Dmain
	};

	struct FeatureAddress
	{
		FeatureAddress():method(FeatureSource::JamRam)
		{
			clear();
		}

		virtual void set_features() = 0;
		virtual void get_features( bool print = false ) = 0;

		FeatureSource method; // Specifies which pattern alternative to use

	protected:
		void clear()
		{
			// Reset JAMRAM to known states (0x00)
			// TODO: Reset JAMRAM 
		}
	};

	// Timing Mode feature address
	struct TimingMode : public FeatureAddress
	{
		// Subfeature parameters
		device::TMODE timing_mode;
		device::DATA_INTERFACE data_interface;
		bool clear_selected_cache_registers;

		// Constructor - initialize subfeature parameters
		TimingMode() : timing_mode( device::TMODE::MODE0 ),
			data_interface( device::DATA_INTERFACE::SDR ),
			clear_selected_cache_registers( false ),
		    feature_address( 0x01 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// NV-DDR3 Configuration feature address
	struct NvDdr3Configuration : public FeatureAddress
	{
		// Subfeature parameters
		bool use_external_vrefq;
		bool use_dqs_c;
		bool use_re_c;
		device::ODT odt;
		device::WARMUP_CYCLES input_warmup_cycles;
		device::WARMUP_CYCLES output_warmup_cycles;

		// Constructor - initialize subfeature parameters
		NvDdr3Configuration() : use_external_vrefq( false ),
			use_dqs_c( false ),
			use_re_c( false ),
			odt( device::ODT::Rtt_Disable ),
			input_warmup_cycles( device::WARMUP_CYCLES::WARMUP_CYCLE0 ),
			output_warmup_cycles( device::WARMUP_CYCLES::WARMUP_CYCLE0 ),
			feature_address( 0x02 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// ZQ Calibration feature address
	struct ZqCalibration : public FeatureAddress
	{
		// Subfeature parameters
		bool use_internal_zq_calibration;

		// Constructor - initialize subfeature parameters
		ZqCalibration() : use_internal_zq_calibration( false ),
			feature_address( 0xF8 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// Write DQ Training (Tx side) feature address
	struct WriteDqTrainingTx : public FeatureAddress
	{
		// Subfeature parameters
		device::WriteTrainingDataSize size;

		// Constructor - initialize subfeature parameters
		WriteDqTrainingTx() : size( device::WriteTrainingDataSize::BYTE8 ),
			feature_address( 0x20 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// Array Operation Mode
	struct ArrayOperationMode : public FeatureAddress
	{
		// Subfeature parameters
		BYTE array_operation_mode;

		// Constructor - initialize subfeature parameters
		ArrayOperationMode() : array_operation_mode( 0 ),
			feature_address( 0x90 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// SLC Mode Enable feature address
	struct SlcModeEnable : public FeatureAddress
	{
		// Subfeature parameters
		BYTE bits_per_cell;

		// Constructor - initialize subfeature parameters
		SlcModeEnable() : bits_per_cell( 4 ),
			feature_address( 0x91 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// Read offset SLC
	struct ReadOffsetSlc : public FeatureAddress
	{
		BYTE offset_level;

		// Constructor - initialize subfeature parameters
		ReadOffsetSlc() : offset_level ( 0 ),
			feature_address( 0xA4 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// Temperature Sensor Readout feature address
	struct TemperatureSensorReadout : public FeatureAddress
	{
		// Constructor - initialize subfeature parameters
		TemperatureSensorReadout() : feature_address( 0xE7 ) {}

		// Overriding virtual functions
		void set_features() override;
		void get_features( bool print = false ) override;

	private:
		BYTE feature_address;
	};

	// Set Vendor specific feature address
	struct VendorSpecific : public FeatureAddress
	{
		std::array< BYTE, 4 > bytes;

		// Constructor - initialize subfeature parameters
		VendorSpecific(BYTE addr, BYTE byte0, BYTE byte1, BYTE byte2, BYTE byte3) : feature_address(addr)
			{
				feature_address = addr;
				bytes[0] = byte0;
				bytes[1] = byte1;
				bytes[2] = byte2;
				bytes[3] = byte3;
		}

		// Overriding virtual functions
		void set_features() override;
		void get_features(bool print = false) override;

	private:
		BYTE feature_address;
	};
}



