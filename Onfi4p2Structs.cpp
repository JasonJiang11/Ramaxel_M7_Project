#include "stdafx.h"
#include  "Onfi4p2Structs.h"
#include "bitmask.h"

// For somewhat-safe copying of buffers
template< size_t _Start, size_t _End, typename _DestType >
static inline void copy( const BYTE* pSrc, _DestType& data )
{
	static_assert((_End - _Start) == sizeof( _DestType ), "Source and Destination buffer size mismatch!");
	memmove( reinterpret_cast<BYTE*>(&data), pSrc + _Start, _End - _Start );
}

// Define ParameterPageData's implementation class
struct ParameterPageData::Impl
{
	Impl()
	{
	}
	Impl( const std::vector< BYTE >& _data )
	{
		unpack( _data );
	}
	void unpack( const std::vector< BYTE >& _data )
	{
		using namespace std;

		// Make copy of the parameter page
		static vector< BYTE > data;
		data.reserve( 256 * 10 ); // in case there are 9 redundant pages
		data.clear();
		copy( begin( _data ), end( _data ), back_inserter( data ) );

		// Parameter Page must have minimum size
		bool parse_additional_redundant_parameter_pages = true;
		if( _data.size() < 768 )
		{
			warning( "Parameter Page size is %d but is expected to be at least "
				"768 bytes. Missing bytes will be interpreted as 00h" );
			parse_additional_redundant_parameter_pages = false;
			data.resize( 768, 0 );
		}

		// Get pointer to underlying vector's data
		auto pData = data.data();

		// Parse contents
		copy<   0, 4 >( pData, this->parameter_page_signature );
		copy<   4, 6 >( pData, this->revision_number );
		copy<   6, 8 >( pData, this->features_supported );
		copy<   8, 10 >( pData, this->optional_commands_supported );
		copy<  10, 11 >( pData, this->jtg_primary_advanced_command_support );
		copy<  11, 12 >( pData, this->training_commands_supported );

		copy<  12, 14 >( pData, this->extended_parameter_page_length );
		copy<  14, 15 >( pData, this->number_of_parameter_pages );
		copy<  32, 44 >( pData, this->device_manufacturer );
		copy<  44, 64 >( pData, this->device_model );
		copy<  64, 65 >( pData, this->jedec_manufacturer_id );
		copy<  65, 67 >( pData, this->date_code );
		copy<  80, 84 >( pData, this->number_of_data_bytes_per_page );
		copy<  84, 86 >( pData, this->number_of_spare_bytes_per_page );
		copy<  92, 96 >( pData, this->number_of_pages_per_block );
		copy<  96, 100 >( pData, this->number_of_block_per_lun );
		copy< 100, 101 >( pData, this->number_of_luns );
		copy< 101, 102 >( pData, this->number_of_address_cycles );
		copy< 102, 103 >( pData, this->number_of_bits_per_cell );
		copy< 103, 105 >( pData, this->bad_blocks_maximum_per_lun );
		copy< 105, 107 >( pData, this->block_endurance );
		copy< 107, 108 >( pData, this->guaranteed_valid_block_at_beginning_of_target );
		copy< 108, 110 >( pData, this->block_endurance_for_guaranteed_valid_blocks );
		copy< 110, 111 >( pData, this->number_of_programs_per_page );
		copy< 112, 113 >( pData, this->number_of_bits_ecc_correctability );
		copy< 113, 114 >( pData, this->number_of_plane_address_bits );
		copy< 114, 115 >( pData, this->multi_plane_operation_attributes );
		copy< 115, 116 >( pData, this->ez_nand_support );

		// Eeters block
		copy< 128, 129 >( pData, this->io_pin_capacitance_maximum );
		copy< 129, 131 >( pData, this->sdr_timing_mode_support );
		copy< 133, 135 >( pData, this->tprog_maximum_page_program_time_us );
		copy< 135, 137 >( pData, this->tbers_maximum_block_erase_time_us );
		copy< 137, 139 >( pData, this->tr_maximum_page_read_time_us );
		copy< 139, 141 >( pData, this->tccs_minimum_change_column_setup_time_ns );
		copy< 141, 142 >( pData, this->nv_ddr_timing_mode_support );

		// Special case: field is split in two sections
		copy< 142, 143 >( pData, *(reinterpret_cast<BYTE*>(&this->nv_ddr2_timing_mode_support) + 0) );
		copy< 162, 163 >( pData, *(reinterpret_cast<BYTE*>(&this->nv_ddr2_timing_mode_support) + 1) );

		copy< 143, 144 >( pData, this->nv_ddr_nv_ddr2_features );
		copy< 144, 146 >( pData, this->clk_input_pin_capacitance_typical );
		copy< 146, 148 >( pData, this->io_pin_capacitance_typical );
		copy< 148, 150 >( pData, this->input_pin_capacitance_typical );
		copy< 150, 151 >( pData, this->input_pin_capacitance_maximum );
		copy< 151, 152 >( pData, this->driver_strength_support );
		copy< 152, 154 >( pData, this->tr_maximum_multi_plane_page_read_time_us );
		copy< 154, 156 >( pData, this->tadl_program_page_resiter_clear_enhancement_tadl_value_ns );
		copy< 156, 158 >( pData, this->tr_typical_page_read_time_for_ez_nand_us );
		copy< 158, 159 >( pData, this->nv_ddr2_ddr3_features );
		copy< 159, 160 >( pData, this->nv_ddr2_ddr3_warmup_cycles );
		copy< 160, 162 >( pData, this->nv_ddr3_timing_mode_support );
		copy< 162, 163 >( pData, this->nv_ddr2_timing_mode_support_2 );

		copy< 164, 166 >( pData, this->vendor_specific_revision_number );
		copy< 166, 254 >( pData, this->vendor_specific );
		copy< 254, 256 >( pData, this->integrity_crc );

		copy< 256, 512 >( pData, this->redundant_parameter_page_1 );
		copy< 512, 768 >( pData, this->redundant_parameter_page_2 );

		if( parse_additional_redundant_parameter_pages )
			std::copy( data.begin() + 768, data.end(), back_inserter( additional_redundant_parameter_pages ) );
	}
	void dump()
	{
		CString str;

		// Revision information and features block
		str += "***** Revision information and features block *****\n";
		str += vFormat( "Parameter page signature : 0x%02X, 0x%02X, 0x%02X, 0x%02X\n", parameter_page_signature[0], parameter_page_signature[1], parameter_page_signature[2], parameter_page_signature[3] );
		str += vFormat( "Revision number : 0x%04X\n", revision_number );
		str += vFormat( "Features supported : 0x%04X\n", features_supported );
		str += vFormat( "Optional commands supported : 0x%04X\n", optional_commands_supported );
		str += vFormat( "ONFI-JEDEC JTG primary advanced command support : 0x%02X\n", jtg_primary_advanced_command_support );
		str += vFormat( "Training commands supported : 0x%02X\n", training_commands_supported );
		str += vFormat( "Extended parameter page length : %d\n", extended_parameter_page_length );
		str += vFormat( "Number of parameter pages : %d\n", number_of_parameter_pages );
		str += "\n";

		// Manufacturer information block
		str += "***** Manufacturer information block *****\n";
		str += vFormat( "Device Manufacturer : %s\n", std::string( reinterpret_cast<char*>(device_manufacturer.data()), reinterpret_cast<char*>(device_manufacturer.data() + device_manufacturer.size()) ).c_str() );
		str += vFormat( "Device model : %s\n", std::string( reinterpret_cast<char*>(device_model.data()), reinterpret_cast<char*>(device_model.data() + device_model.size()) ).c_str() );
		str += vFormat( "JEDEC manufacturer ID : 0x%02X\n", jedec_manufacturer_id );
		str += vFormat( "Date code : 0x%04X\n", date_code );
		str += "\n";

		// Memory organization block
		str += "***** Memory organization block *****\n";
		str += vFormat( "Number of data bytes per page : %d\n", number_of_data_bytes_per_page );
		str += vFormat( "Number of spare bytes per page : \n", number_of_spare_bytes_per_page );
		str += vFormat( "Number of pages per block : %d\n", number_of_pages_per_block );
		str += vFormat( "Number of blocks per LUN : %d\n", number_of_block_per_lun );
		str += vFormat( "Number of LUNs : %d\n", number_of_luns );
		str += vFormat( "Number of address cycles : %d\n", number_of_address_cycles );
		str += vFormat( "Number of bits per cell : %d\n", number_of_bits_per_cell );
		str += vFormat( "Bad blocks maximum per LUN : %d\n", bad_blocks_maximum_per_lun );
		str += vFormat( "Block endurance : %d\n", block_endurance );
		str += vFormat( "Guaranteed valid block at beginning of target : 0x%02X\n", guaranteed_valid_block_at_beginning_of_target );
		str += vFormat( "Block endurance for guaranteed valid blocks : %d\n", block_endurance_for_guaranteed_valid_blocks );
		str += vFormat( "Number of programs per page : %d\n", number_of_programs_per_page );
		str += vFormat( "Number of bits ECC correctability : %d\n", number_of_bits_ecc_correctability );
		str += vFormat( "Number of plane address bits : %d\n", number_of_plane_address_bits );
		str += vFormat( "Multi-Plane operation attributes : 0x%02X\n", multi_plane_operation_attributes );
		str += vFormat( "EZ NAND support : 0x%02X\n", ez_nand_support );
		str += "\n";

		// Electrical parameters block
		str += "***** Electrical parameters block *****\n";
		str += vFormat( "IO pin capacitance maximum : %d\n", io_pin_capacitance_maximum );
		str += vFormat( "SDR timing mode support : 0x%04X\n", sdr_timing_mode_support );
		str += vFormat( "tPROG maximum page program time (us) : %d\n", tprog_maximum_page_program_time_us );
		str += vFormat( "tBERS maximum block erase time (us) : %d\n", tbers_maximum_block_erase_time_us );
		str += vFormat( "tR maximum page read time (us) : %d\n", tr_maximum_page_read_time_us );
		str += vFormat( "tCCS minimum change column setup time (ns) : %d\n", tccs_minimum_change_column_setup_time_ns );
		str += vFormat( "NV-DDR timing mode support : 0x%02X\n", nv_ddr_timing_mode_support );
		str += vFormat( "NV-DDR2 timing mode support : 0x%02X\n", nv_ddr2_timing_mode_support );
		str += vFormat( "NV-DDR/NV-DDR2 features : 0x%02X\n", nv_ddr_nv_ddr2_features );
		str += vFormat( "CLK input pin capacitance, typical : %d\n", clk_input_pin_capacitance_typical );
		str += vFormat( "IO pin capacitance, typical : %d\n", io_pin_capacitance_typical );
		str += vFormat( "Input pin capacitance, typical : %d\n", input_pin_capacitance_typical );
		str += vFormat( "Input pin capacitance, maximum : %d\n", input_pin_capacitance_maximum );
		str += vFormat( "Driver strength support : 0x%02X\n", driver_strength_support );
		str += vFormat( "tR maximum multi-plane page read time (us) : %d\n", tr_maximum_multi_plane_page_read_time_us );
		str += vFormat( "tADL program page register clear enhancement tADL value (ns ) : %d\n", tadl_program_page_resiter_clear_enhancement_tadl_value_ns );
		str += vFormat( "tR typical page read time for EZ NAND (us) : %d\n", tr_typical_page_read_time_for_ez_nand_us );
		str += vFormat( "NV-DDR2/3 features : 0x%02X\n", nv_ddr2_ddr3_features );
		str += vFormat( "NV-DDR2/3 warmup cycles : 0x%02X\n", nv_ddr2_ddr3_warmup_cycles );
		str += vFormat( "NV-DDR3 timing mode support : 0x%04X\n", nv_ddr3_timing_mode_support );
		str += vFormat( "NV-DDR2 timing mode support : 0x%02X\n", nv_ddr2_timing_mode_support_2 );
		str += "\n";

		// Vendor block
		str += "***** Vendor block *****\n";
		str += vFormat( "Vendor specific revision number : 0x%04X\n", vendor_specific_revision_number );
		str += vFormat( "Vendor specific : ...\n" );
		str += vFormat( "Integrity CRC : 0x%04X\n", integrity_crc );
		str += "\n";

		// Print to UI console
		output( str );
	}

	// Revision information and features block
	std::array< BYTE, 4 > parameter_page_signature;
	WORD revision_number;
	WORD features_supported;
	WORD optional_commands_supported;
	BYTE jtg_primary_advanced_command_support;
	BYTE training_commands_supported;
	WORD extended_parameter_page_length;
	BYTE number_of_parameter_pages;

	// Manufacturer information block
	std::array< BYTE, 12 > device_manufacturer;
	std::array< BYTE, 20 > device_model;
	BYTE jedec_manufacturer_id;
	WORD date_code;

	// Memory organization block
	DWORD number_of_data_bytes_per_page;
	WORD  number_of_spare_bytes_per_page;
	DWORD number_of_pages_per_block;
	DWORD number_of_block_per_lun;
	BYTE  number_of_luns;
	BYTE  number_of_address_cycles;
	BYTE  number_of_bits_per_cell;
	WORD  bad_blocks_maximum_per_lun;
	WORD  block_endurance;
	BYTE  guaranteed_valid_block_at_beginning_of_target;
	WORD  block_endurance_for_guaranteed_valid_blocks;
	BYTE  number_of_programs_per_page;
	BYTE  number_of_bits_ecc_correctability;
	BYTE  number_of_plane_address_bits;
	BYTE  multi_plane_operation_attributes;
	BYTE  ez_nand_support;

	// Electrical parameters block
	BYTE io_pin_capacitance_maximum;
	WORD sdr_timing_mode_support;
	WORD tprog_maximum_page_program_time_us;
	WORD tbers_maximum_block_erase_time_us;
	WORD tr_maximum_page_read_time_us;
	WORD tccs_minimum_change_column_setup_time_ns;
	BYTE nv_ddr_timing_mode_support;
	WORD nv_ddr2_timing_mode_support;
	BYTE nv_ddr_nv_ddr2_features;
	WORD clk_input_pin_capacitance_typical;
	WORD io_pin_capacitance_typical;
	WORD input_pin_capacitance_typical;
	BYTE input_pin_capacitance_maximum;
	BYTE driver_strength_support;
	WORD tr_maximum_multi_plane_page_read_time_us;
	WORD tadl_program_page_resiter_clear_enhancement_tadl_value_ns;
	WORD tr_typical_page_read_time_for_ez_nand_us;
	BYTE nv_ddr2_ddr3_features;
	BYTE nv_ddr2_ddr3_warmup_cycles;
	WORD nv_ddr3_timing_mode_support;
	BYTE nv_ddr2_timing_mode_support_2;

	// Vendor block
	WORD vendor_specific_revision_number;
	std::array< BYTE, 88 > vendor_specific;
	WORD integrity_crc;

	// Redundant parameter pages
	std::array< BYTE, 256 > redundant_parameter_page_1, redundant_parameter_page_2;
	std::vector< BYTE > additional_redundant_parameter_pages;
};

ParameterPageData::ParameterPageData()
{
	_pImpl = std::make_unique< Impl >();
}
ParameterPageData::ParameterPageData( const ParameterPageData& rhs )
{
	_pImpl = std::make_unique< Impl >( *rhs._pImpl );
}
ParameterPageData::ParameterPageData( const std::vector< BYTE >& data )
{
	// Create new implementation instance
	_pImpl = std::make_unique< Impl >( data );
}
void ParameterPageData::unpack( const std::vector< BYTE >& data )
{
	_pImpl->unpack( data );
}
void ParameterPageData::dump() const
{
	_pImpl->dump();
}


// Define compiler-generated members now that ParameterPageData::Impl is
// defined. If defined prior to ParameterPageData::Impl, would get compile
// error since ParameterPageData::Impl was an incomplete type until now and
// we were using std::unique_ptr.
ParameterPageData::~ParameterPageData() = default;

// Revision information and features block
void ParameterPageData::get_parameter_page_signature( std::array< BYTE, 4 > & data ) const
{
	using namespace std;
	copy( begin( _pImpl->parameter_page_signature ), begin( _pImpl->parameter_page_signature ), begin( data ) );
}
WORD ParameterPageData::get_revision_number() const
{
	return _pImpl->revision_number;
}
WORD ParameterPageData::get_features_supported() const
{
	return _pImpl->features_supported;
}
WORD ParameterPageData::get_optional_commands_supported() const
{
	return _pImpl->optional_commands_supported;
}
BYTE ParameterPageData::get_jtg_primary_advanced_command_support() const
{
	return _pImpl->jtg_primary_advanced_command_support;
}
BYTE ParameterPageData::get_training_commands_supported() const
{
	return _pImpl->training_commands_supported;
}
WORD ParameterPageData::get_extended_parameter_page_length() const
{
	return _pImpl->extended_parameter_page_length;
}
BYTE ParameterPageData::get_number_of_parameter_pages() const
{
	return _pImpl->number_of_parameter_pages;
}

// Manufacturer information block
void ParameterPageData::get_device_manufacturer( std::array< BYTE, 12 > & data ) const
{
	using namespace std;
	copy( begin( _pImpl->device_manufacturer ), begin( _pImpl->device_manufacturer ), begin( data ) );
}
void ParameterPageData::get_device_model( std::array< BYTE, 20 > & data ) const
{
	using namespace std;
	copy( begin( _pImpl->device_model ), begin( _pImpl->device_model ), begin( data ) );
}
BYTE ParameterPageData::get_jedec_manufacturer_id() const
{
	return _pImpl->jedec_manufacturer_id;
}
WORD ParameterPageData::get_date_code() const
{
	return _pImpl->date_code;
}

// Memory organization block
DWORD ParameterPageData::get_number_of_data_bytes_per_page() const
{
	return _pImpl->number_of_data_bytes_per_page;
}
DWORD ParameterPageData::get_number_of_spare_bytes_per_page() const
{
	return _pImpl->number_of_spare_bytes_per_page;
}
DWORD ParameterPageData::get_number_of_pages_per_block() const
{
	return _pImpl->number_of_pages_per_block;
}
DWORD ParameterPageData::get_number_of_block_per_lun() const
{
	return _pImpl->number_of_block_per_lun;
}
BYTE  ParameterPageData::get_number_of_luns() const
{
	return _pImpl->number_of_luns;
}
BYTE  ParameterPageData::get_number_of_address_cycles() const
{
	return _pImpl->number_of_address_cycles;
}
BYTE  ParameterPageData::get_number_of_bits_per_cell() const
{
	return _pImpl->number_of_bits_per_cell;
}
WORD  ParameterPageData::get_bad_blocks_maximum_per_lun() const
{
	return _pImpl->bad_blocks_maximum_per_lun;
}
WORD  ParameterPageData::get_block_endurance() const
{
	return _pImpl->block_endurance;
}
BYTE  ParameterPageData::get_guaranteed_valid_block_at_beginning_of_target() const
{
	return _pImpl->guaranteed_valid_block_at_beginning_of_target;
}
WORD  ParameterPageData::get_block_endurance_for_guaranteed_valid_blocks() const
{
	return _pImpl->block_endurance_for_guaranteed_valid_blocks;
}
BYTE  ParameterPageData::get_number_of_programs_per_page() const
{
	return _pImpl->number_of_programs_per_page;
}
BYTE  ParameterPageData::get_number_of_bits_ecc_correctability() const
{
	return _pImpl->number_of_bits_ecc_correctability;
}
BYTE  ParameterPageData::get_number_of_plane_address_bits() const
{
	return _pImpl->number_of_plane_address_bits;
}
BYTE  ParameterPageData::get_multi_plane_operation_attributes() const
{
	return _pImpl->multi_plane_operation_attributes;
}
BYTE  ParameterPageData::get_ez_nand_support() const
{
	return _pImpl->ez_nand_support;
}

// Electrical parameters block
BYTE ParameterPageData::get_io_pin_capacitance_maximum() const
{
	return _pImpl->io_pin_capacitance_maximum;
}
WORD ParameterPageData::get_sdr_timing_mode_support() const
{
	return _pImpl->sdr_timing_mode_support;
}
WORD ParameterPageData::get_tprog_maximum_page_program_time_us() const
{
	return _pImpl->tprog_maximum_page_program_time_us;
}
WORD ParameterPageData::get_tbers_maximum_block_erase_time_us() const
{
	return _pImpl->tbers_maximum_block_erase_time_us;
}
WORD ParameterPageData::get_tr_maximum_page_read_time_us() const
{
	return _pImpl->tr_maximum_multi_plane_page_read_time_us;
}
WORD ParameterPageData::get_tccs_minimum_change_column_setup_time_ns() const
{
	return _pImpl->tccs_minimum_change_column_setup_time_ns;
}
BYTE ParameterPageData::get_nv_ddr_timing_mode_support() const
{
	return _pImpl->nv_ddr_timing_mode_support;
}
WORD ParameterPageData::get_nv_ddr2_timing_mode_support() const
{
	return _pImpl->nv_ddr2_timing_mode_support;
}
BYTE ParameterPageData::get_nv_ddr_nv_ddr2_features() const
{
	return _pImpl->nv_ddr_nv_ddr2_features;
}
WORD ParameterPageData::get_clk_input_pin_capacitance_typical() const
{
	return _pImpl->clk_input_pin_capacitance_typical;
}
WORD ParameterPageData::get_io_pin_capacitance_typical() const
{
	return _pImpl->io_pin_capacitance_typical;
}
WORD ParameterPageData::get_input_pin_capacitance_typical() const
{
	return _pImpl->input_pin_capacitance_typical;
}
BYTE ParameterPageData::get_input_pin_capacitance_maximum() const
{
	return _pImpl->input_pin_capacitance_maximum;
}
BYTE ParameterPageData::get_driver_strength_support() const
{
	return _pImpl->driver_strength_support;
}
WORD ParameterPageData::get_tr_maximum_multi_plane_page_read_time_us() const
{
	return _pImpl->tr_maximum_multi_plane_page_read_time_us;
}
WORD ParameterPageData::get_tadl_program_page_resiter_clear_enhancement_tadl_value_ns() const
{
	return _pImpl->tadl_program_page_resiter_clear_enhancement_tadl_value_ns;
}
WORD ParameterPageData::get_tr_typical_page_read_time_for_ez_nand_us() const
{
	return _pImpl->tr_typical_page_read_time_for_ez_nand_us;
}
BYTE ParameterPageData::get_nv_ddr2_ddr3_features() const
{
	return _pImpl->nv_ddr2_ddr3_features;
}
BYTE ParameterPageData::get_nv_ddr2_ddr3_warmup_cycles() const
{
	return _pImpl->nv_ddr2_ddr3_warmup_cycles;
}
WORD ParameterPageData::get_nv_ddr3_timing_mode_support() const
{
	return _pImpl->nv_ddr3_timing_mode_support;
}

// Vendor block
WORD ParameterPageData::get_vendor_specific_revision_number() const
{
	return _pImpl->vendor_specific_revision_number;
}
void ParameterPageData::get_vendor_specific( std::array< BYTE, 88 > & data ) const
{
	using namespace std;
	copy( begin( _pImpl->vendor_specific ), end( _pImpl->vendor_specific ), begin( data ) );
}
WORD ParameterPageData::get_integrity_crc() const
{
	return _pImpl->integrity_crc;
}

// Redundant parameter pages
void ParameterPageData::get_redundant_parameter_page_1( std::array< BYTE, 256 > & p )
{
	using namespace std;
	copy( begin( _pImpl->redundant_parameter_page_1 ), end( _pImpl->redundant_parameter_page_1 ), begin( p ) );
}
void ParameterPageData::get_redundant_parameter_page_2( std::array< BYTE, 256 > & p )
{
	using namespace std;
	copy( begin( _pImpl->redundant_parameter_page_2 ), end( _pImpl->redundant_parameter_page_2 ), begin( p ) );
}
void ParameterPageData::get_additional_redundant_parameter_pages( std::vector< BYTE >& p )
{
	using namespace std;
	p.clear();
	p.reserve( _pImpl->additional_redundant_parameter_pages.size() );
	copy( begin( _pImpl->additional_redundant_parameter_pages ), end( _pImpl->additional_redundant_parameter_pages ), back_inserter( p ) );
}

////////////////////////////////////////////////////////////////////////////////

// Helper functions

////////////////////////////////////////////////////////////////////////////////

bool is_parameter_page_signature_valid( const ParameterPageData& ppd )
{
	std::array< BYTE, 4 > val;
	ppd.get_parameter_page_signature( val );

	if( val[0] == 0x4F &&
		val[1] == 0x4E &&
		val[2] == 0x46 &&
		val[3] == 0x49 )
		return true;
	else
		return false;
}

bool are_onfi_revisions_supported( const ParameterPageData& ppd, WORD revisions )
{
	WORD revs = ppd.get_revision_number();

	// Sanity checks
	if( (apps::bitmask::BitMask< WORD, 0, 0 >::value & revs) != 0 )
		warning( "are_onfi_revisions_supported() -- Bit 0 of the revision number is not zero!" );
	if( (apps::bitmask::BitMask< WORD, 15, 11 >::value & revs) != 0 )
		warning( "are_onfi_revisions_supported() -- Bits 11-15 of the revision number are not zero!" );

	// Check if all user-specified revisions are supported
	if( ((revs & revisions) ^ revisions) == 0 )
		return true;
	else
		return false;
}

bool is_feature_supported_16_bit_data_bus_width( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 0, 0 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_multiple_lun_operations( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 1, 1 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_non_sequential_page_programming( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 2, 2 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_multi_plane_program_and_erase_operations( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 3, 3 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_odd_to_even_page_copyback( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 4, 4 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_nv_ddr( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 5, 5 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_multi_plane_read_operations( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 6, 6 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_extended_parameter_page( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 7, 7 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_program_page_register_clear_enhancement( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 8, 8 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_ez_nand( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 9, 9 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_nv_ddr2( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 10, 10 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_volume_addressing( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 11, 11 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_external_vpp( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 12, 12 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_nv_ddr3( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 13, 13 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_zq_calibration( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 14, 14 >::value & ppd.get_features_supported() ? true : false;
}
bool is_feature_supported_package_electrical_specification( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 15, 15 >::value & ppd.get_features_supported() ? true : false;
}

// Optional commands supported
bool is_command_supported_page_cache_program( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 0, 0 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_read_cache( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 1, 1 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_get_set_features( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 2, 2 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_read_status_enhanced( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 3, 3 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_copyback( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 4, 4 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_read_unique_id( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 5, 5 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_change_read_column_enhanced( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 6, 6 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_change_row_address( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 7, 7 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_small_data_move( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 8, 8 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_reset_lun( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 9, 9 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_volume_select( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 10, 10 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_odt_configure( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 11, 11 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_lun_get_set_features( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 12, 12 >::value & ppd.get_optional_commands_supported() ? true : false;
}
bool is_command_supported_zq_calibration( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< WORD, 13, 13 >::value & ppd.get_optional_commands_supported() ? true : false;
}

bool is_advanced_command_supported_random_data_out( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 0, 0 >::value & ppd.get_jtg_primary_advanced_command_support() ? true : false;
}
bool is_advanced_command_supported_multi_plane_page_program( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 1, 1 >::value & ppd.get_jtg_primary_advanced_command_support() ? true : false;
}
bool is_advanced_command_supported_multi_plane_copyback_program( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 2, 2 >::value & ppd.get_jtg_primary_advanced_command_support() ? true : false;
}
bool is_advanced_command_supported_multi_plane_block_erase( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 3, 3 >::value & ppd.get_jtg_primary_advanced_command_support() ? true : false;
}

bool is_training_command_supported_explicit_dcc_training( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 0, 0 >::value & ppd.get_training_commands_supported() ? true : false;
}
bool is_training_command_supported_implicit_dcc_training( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 1, 1 >::value & ppd.get_training_commands_supported() ? true : false;
}
bool is_training_command_supported_read_dq_training( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 2, 2 >::value & ppd.get_training_commands_supported() ? true : false;
}
bool is_training_command_supported_write_tx_dq_training( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 3, 3 >::value & ppd.get_training_commands_supported() ? true : false;
}
bool is_training_command_supported_write_rx_dq_training( const ParameterPageData& ppd )
{
	return apps::bitmask::BitMask< BYTE, 3, 3 >::value & ppd.get_training_commands_supported() ? true : false;
}

// pair.first  = row address cycles
// pair.second = column address cycles
std::pair< BYTE, BYTE > get_num_address_cycles( const ParameterPageData& ppd )
{
	auto val = ppd.get_number_of_address_cycles();
	std::pair< BYTE, BYTE > ret;
	ret.first = 0xF & val;
	ret.second = val >> 4;
	return ret;
}


//////////////////////////////////////////////////////////////////////////////

// For parsing the Status Register

//////////////////////////////////////////////////////////////////////////////

struct StatusRegister::Impl
{
	Impl( BYTE reg )
	{
		unpack( reg );
	}
	void unpack( BYTE reg )
	{
		fail = apps::bitmask::BitMask< BYTE, 0, 0 >::value & reg ? true : false;
		failc = apps::bitmask::BitMask< BYTE, 1, 1 >::value & reg ? true : false;
		csp = apps::bitmask::BitMask< BYTE, 3, 3 >::value & reg ? true : false;
		vsp = apps::bitmask::BitMask< BYTE, 4, 4 >::value & reg ? true : false;
		ardy = apps::bitmask::BitMask< BYTE, 5, 5 >::value & reg ? true : false;
		rdy = apps::bitmask::BitMask< BYTE, 6, 6 >::value & reg ? true : false;
		wp_n = apps::bitmask::BitMask< BYTE, 7, 7 >::value & reg ? true : false;
	}
	bool fail, failc, csp, ardy, rdy, wp_n, vsp;
};

// Define compiler-generated copy ctor and dtor now that
// StatusRegister::Impl is defined.
StatusRegister::~StatusRegister() = default;

StatusRegister::StatusRegister()
{
	_pImpl = std::make_unique< Impl >( 0 );
}
StatusRegister::StatusRegister( const StatusRegister& rhs )
{
	_pImpl = std::make_unique< Impl >( *rhs._pImpl );
}
StatusRegister::StatusRegister( BYTE reg )
{
	_pImpl = std::make_unique< Impl >( reg );
}
void StatusRegister::unpack( BYTE reg )
{
	_pImpl->unpack( reg );
}