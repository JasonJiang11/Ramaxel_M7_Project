#pragma once

#include "bitmask.h"

// Helper macros for defining device configurations
#define CAST_SCOPED_ENUM( x ) static_cast< unsigned int >( x )
#define DEFINE_DEVICE_CONFIGURATION( package, die_count, ce_count )\
	CAST_SCOPED_ENUM( PackageType::package ) | CAST_SCOPED_ENUM( XDP::die_count ) | CAST_SCOPED_ENUM( ChipEnableCount::ce_count )


// TODO: Add customer name here
// <Customer> NAND Flash Memory Information
namespace device
{
	// # of Die
	enum class XDP : unsigned int
	{
		Shift = 0,
		SDP = (1 << 0) << Shift, // 1 die
		DDP = (1 << 1) << Shift, // 2 dies
		QDP = (1 << 2) << Shift, // 4 dies
		ODP = (1 << 3) << Shift, // 8 dies
		HDP = (1 << 4) << Shift, // 16 dies
		Mask = apps::bitmask::BitMask< unsigned int, 7, 0 >::value, // bitmask = 0xFF
	};

	enum class ChipEnableCount : unsigned int
	{
		Shift = 8,
		one   = 1 << Shift,
		two   = 2 << Shift,
		four  = 4 << Shift,
		eight = 8 << Shift,
		Mask = apps::bitmask::BitMask< unsigned int, 15, 8 >::value, // bitmask = 0xFF << 8
	};

	enum class PackageType : unsigned int
	{
		BGA132 = 1 << 16,
		BGA252 = 1 << 17,
	};

	// TODO: Customer specific device name
	enum class Configurations : unsigned int
	{
		//Each dut spans 2 sites,  132-BGA, DDP, 2 CE, which is 1 CE, 1 DIE per site
		BGA132_DDP_2CE_TRN  = DEFINE_DEVICE_CONFIGURATION( BGA132, DDP, two  )
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ******************************** Select the DUT's configuration here! ********************************

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO: Selects the DUT's configuration
	const Configurations dutConfig = Configurations::BGA132_DDP_2CE_TRN;

	enum class TMODE : BYTE		// Timing Mode
	{
		MODE0,  MODE1,  MODE2,  MODE3,  MODE4,
		MODE5,  MODE6,  MODE7,  MODE8,  MODE9,
		MODE10, MODE11, MODE12, MODE13,
		MODE14, MODE15
	};

	enum class DATA_INTERFACE : BYTE
	{
		SDR, NV_DDR, NV_DDR2, NV_DDR3
	};

	enum class WARMUP_CYCLES
	{
		WARMUP_CYCLE0,
		WARMUP_CYCLE1,
		WARMUP_CYCLE2,
		WARMUP_CYCLE3,
		WARMUP_CYCLE4
	};

	enum class ODT
	{
		Rtt_Disable = 0x00,
		Rtt_150     = 0x01,
		Rtt_75      = 0x03,
		Rtt_50      = 0x04
	};

	enum class WriteTrainingDataSize
	{
		BYTE8   = 0x00,
		BYTE16  = 0x01,
		BYTE24  = 0x02,
		BYTE32  = 0x03,
		BYTE40  = 0x04,
		BYTE48  = 0x05,
		BYTE56  = 0x06,
		BYTE64  = 0x07,
		BYTE72  = 0x08,
		BYTE80  = 0x09,
		BYTE88  = 0x0A,
		BYTE96  = 0x0B,
		BYTE104 = 0x0C,
		BYTE112 = 0x0D,
		BYTE120 = 0x0E,
		BYTE128 = 0x0F
	};

	// True the DUT's package type is 132-BGA. Else, it is 252-BGA.
	const bool is_132_BGA = (CAST_SCOPED_ENUM(dutConfig) & CAST_SCOPED_ENUM(PackageType::BGA132)) != 0 ? true : false;

	// Number of die (in the package)
	const unsigned int num_dies = (CAST_SCOPED_ENUM(dutConfig) & CAST_SCOPED_ENUM(XDP::Mask)) >> CAST_SCOPED_ENUM(XDP::Shift);

	// Number of CE# signals
	const unsigned int num_chip_enables = (CAST_SCOPED_ENUM(dutConfig) & CAST_SCOPED_ENUM(ChipEnableCount::Mask)) >> CAST_SCOPED_ENUM(ChipEnableCount::Shift);

	// Number of Targets is equivalent to the number of CE# signals
	const unsigned int num_targets = num_chip_enables;

	// Number of LUNs per Target (i.e. per CE# signal)
	const unsigned int num_luns_per_target = num_dies / num_chip_enables;

	// Bus width (number of bits)
	const unsigned int busWidth = 8;

	namespace memory
	{
		// Number of bytes per page (excluding the defect area)
		const unsigned int num_bytes_per_page = 16384;

		// Number of bytes per defect area (in a page)
		const unsigned int num_bytes_per_defect_area = 2208;

		// Number of pages per block
		const unsigned int num_pages_per_block = 3456;

		// Number of blocks per plane
		const unsigned int num_blocks_per_plane = 338;

		// Number of planes per die/LUN
		const unsigned int num_planes_per_lun = 4;

		// Number of bytes in param page 
		const unsigned int num_bytes_param_page = 768;
	}

	namespace dsa
	{
		// true if pins are shared
		const bool is_shared = true;

		// Number of individual data buses
		const unsigned int num_data_buses = is_132_BGA ? 1 : 2;

		// Number of VCC pins
		const unsigned int num_vcc_pins = is_132_BGA ? 4 : 12;

		// Bypass capacitance per VCC pin
		const unsigned int cap_per_vcc_pin_NanoFarads = 110;

		// Number of VCCQ pins
		const unsigned int num_vccq_pins = is_132_BGA ? 12 : 18;

		// Bypass capacitance per VCCQ pin
		const unsigned int cap_per_vccq_pin_NanoFarads = 110;

		// Number of VREFQ pins
		const unsigned int num_vrefq_pins = is_132_BGA ? 2 : 4;

		// Bypass capacitance per VREQ pin
		const unsigned int cap_per_vrefq_pin_NanoFarads = 110;

		// Switched bypass capacitance
		const unsigned int switched_cap_NanoFarads = 47000;
	}

	// Number of Targets per bus
	static const size_t numTargetsPerBus = device::num_targets / dsa::num_data_buses;

	namespace test
	{
		namespace read
		{
			// Number of bytes delimiting a sector in which the number of faiing bits will be counted.
			static const auto num_bytes_per_sector_in_page = static_cast<unsigned int>(1024);
			static const auto num_bytes_per_sector_in_defect_area = static_cast<unsigned int>(1104);

			// Number of sectors per page (rounding down)
			static const auto num_sectors_per_page = device::memory::num_bytes_per_page / num_bytes_per_sector_in_page;
			static const auto num_sectors_per_defect_area = device::memory::num_bytes_per_defect_area / num_bytes_per_sector_in_defect_area;

			// Number of failed bits considered as the test's fail limit
			static const auto num_failed_bits_limit_per_sector_in_page = static_cast<unsigned int>(30);
			static const auto num_failed_bits_limit_per_sector_in_defect_area = static_cast<unsigned int>(30);
		}
	}
}

// Don't pollute the macro space
#undef CAST_SCOPED_ENUM
#undef DEFINE_DEVICE_CONFIGURATION