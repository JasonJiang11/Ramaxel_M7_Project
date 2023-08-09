#pragma once

#include "TestProgApp/public.h"
#include <array>
#include <vector>
#include <memory> // std::unique_ptr

//////////////////////////////////////////////////////////////////////////////

// For parsing a Parameter Page Data

//////////////////////////////////////////////////////////////////////////////

struct ParameterPageData
{
	// Ctors
	ParameterPageData();
	ParameterPageData( const ParameterPageData& );

	// Unpack parameter page data's binary representation
	ParameterPageData( const std::vector< BYTE >& );
	void unpack( const std::vector< BYTE >& );

	// Dump contents to UI console
	void dump() const;

	// Need to declare dtor for std::unique_ptr
	~ParameterPageData();

	// Revision information and features block
	void get_parameter_page_signature( std::array< BYTE, 4 >& ) const;
	WORD get_revision_number() const;
	WORD get_features_supported() const;
	WORD get_optional_commands_supported() const;
	BYTE get_jtg_primary_advanced_command_support() const;
	BYTE get_training_commands_supported() const;
	WORD get_extended_parameter_page_length() const;
	BYTE get_number_of_parameter_pages() const;

	// Manufacturer information block
	void get_device_manufacturer( std::array< BYTE, 12 >& ) const;
	void get_device_model( std::array< BYTE, 20 >& ) const;
	BYTE get_jedec_manufacturer_id() const;
	WORD get_date_code() const;

	// Memory organization block
	DWORD get_number_of_data_bytes_per_page() const;
	DWORD get_number_of_spare_bytes_per_page() const;
	DWORD get_number_of_pages_per_block() const;
	DWORD get_number_of_block_per_lun() const;
	BYTE get_number_of_luns() const;
	BYTE get_number_of_address_cycles() const;
	BYTE get_number_of_bits_per_cell() const;
	WORD get_bad_blocks_maximum_per_lun() const;
	WORD get_block_endurance() const;
	BYTE get_guaranteed_valid_block_at_beginning_of_target() const;
	WORD get_block_endurance_for_guaranteed_valid_blocks() const;
	BYTE get_number_of_programs_per_page() const;
	BYTE get_number_of_bits_ecc_correctability() const;
	BYTE get_number_of_plane_address_bits() const;
	BYTE get_multi_plane_operation_attributes() const;
	BYTE get_ez_nand_support() const;

	// Electrical parameters block
	BYTE get_io_pin_capacitance_maximum() const;
	WORD get_sdr_timing_mode_support() const;
	WORD get_tprog_maximum_page_program_time_us() const;
	WORD get_tbers_maximum_block_erase_time_us() const;
	WORD get_tr_maximum_page_read_time_us() const;
	WORD get_tccs_minimum_change_column_setup_time_ns() const;
	BYTE get_nv_ddr_timing_mode_support() const;
	WORD get_nv_ddr2_timing_mode_support() const;
	BYTE get_nv_ddr_nv_ddr2_features() const;
	WORD get_clk_input_pin_capacitance_typical() const;
	WORD get_io_pin_capacitance_typical() const;
	WORD get_input_pin_capacitance_typical() const;
	BYTE get_input_pin_capacitance_maximum() const;
	BYTE get_driver_strength_support() const;
	WORD get_tr_maximum_multi_plane_page_read_time_us() const;
	WORD get_tadl_program_page_resiter_clear_enhancement_tadl_value_ns() const;
	WORD get_tr_typical_page_read_time_for_ez_nand_us() const;
	BYTE get_nv_ddr2_ddr3_features() const;
	BYTE get_nv_ddr2_ddr3_warmup_cycles() const;
	WORD get_nv_ddr3_timing_mode_support() const;

	// Vendor block
	WORD get_vendor_specific_revision_number() const;
	void get_vendor_specific( std::array< BYTE, 88 >& ) const;
	WORD get_integrity_crc() const;

	// Redundant Parameter Pages
	void get_redundant_parameter_page_1( std::array< BYTE, 256 > & p );
	void get_redundant_parameter_page_2( std::array< BYTE, 256 > & p );
	void get_additional_redundant_parameter_pages( std::vector< BYTE >& p );

private:
	struct Impl;
	std::unique_ptr< Impl > _pImpl;
};


///////////////////////////////////////////////////////////////////////////////////

// Non-member functions to interpret the contents of a Parameter Page

///////////////////////////////////////////////////////////////////////////////////

bool is_parameter_page_signature_valid( const ParameterPageData& ppd );

enum class OnfiRevisions : WORD
{
	rev1p0 = 1 << 1,
	rev2p0 = 1 << 2,
	rev2p1 = 1 << 3,
	rev2p2 = 1 << 4,
	rev2p3 = 1 << 5,
	rev3p0 = 1 << 6,
	rev3p1 = 1 << 7,
	rev3p2 = 1 << 8,
	rev4p0 = 1 << 9,
	rev4p1 = 1 << 10,
	rev4p2 = 1 << 11
};
DEFINE_ENUM_FLAG_OPERATORS( OnfiRevisions );

/// \param revision     bitmask of OnfiRevisions
bool are_onfi_revisions_supported( const ParameterPageData& ppd, WORD revisions );

bool is_feature_supported_16_bit_data_bus_width( const ParameterPageData& ppd );
bool is_feature_supported_multiple_lun_operations( const ParameterPageData& ppd );
bool is_feature_supported_non_sequential_page_programming( const ParameterPageData& ppd );
bool is_feature_supported_multi_plane_program_and_erase_operations( const ParameterPageData& ppd );
bool is_feature_supported_odd_to_even_page_copyback( const ParameterPageData& ppd );
bool is_feature_supported_nv_ddr( const ParameterPageData& ppd );
bool is_feature_supported_multi_plane_read_operations( const ParameterPageData& ppd );
bool is_feature_supported_extended_parameter_page( const ParameterPageData& ppd );
bool is_feature_supported_program_page_register_clear_enhancement( const ParameterPageData& ppd );
bool is_feature_supported_ez_nand( const ParameterPageData& ppd );
bool is_feature_supported_nv_ddr2( const ParameterPageData& ppd );
bool is_feature_supported_volume_addressing( const ParameterPageData& ppd );
bool is_feature_supported_external_vpp( const ParameterPageData& ppd );
bool is_feature_supported_nv_ddr3( const ParameterPageData& ppd );
bool is_feature_supported_zq_calibration( const ParameterPageData& ppd );
bool is_feature_supported_package_electrical_specification( const ParameterPageData& ppd );

// Optional commands supported
bool is_command_supported_page_cache_program( const ParameterPageData& ppd );
bool is_command_supported_read_cache( const ParameterPageData& ppd );
bool is_command_supported_get_set_features( const ParameterPageData& ppd );
bool is_command_supported_read_status_enhanced( const ParameterPageData& ppd );
bool is_command_supported_copyback( const ParameterPageData& ppd );
bool is_command_supported_read_unique_id( const ParameterPageData& ppd );
bool is_command_supported_change_read_column_enhanced( const ParameterPageData& ppd );
bool is_command_supported_change_row_address( const ParameterPageData& ppd );
bool is_command_supported_small_data_move( const ParameterPageData& ppd );
bool is_command_supported_reset_lun( const ParameterPageData& ppd );
bool is_command_supported_volume_select( const ParameterPageData& ppd );
bool is_command_supported_odt_configure( const ParameterPageData& ppd );
bool is_command_supported_lun_get_set_features( const ParameterPageData& ppd );
bool is_command_supported_zq_calibration( const ParameterPageData& ppd );

bool is_advanced_command_supported_random_data_out( const ParameterPageData& ppd );
bool is_advanced_command_supported_multi_plane_page_program( const ParameterPageData& ppd );
bool is_advanced_command_supported_multi_plane_copyback_program( const ParameterPageData& ppd );
bool is_advanced_command_supported_multi_plane_block_erase( const ParameterPageData& ppd );

bool is_training_command_supported_explicit_dcc_training( const ParameterPageData& ppd );
bool is_training_command_supported_implicit_dcc_training( const ParameterPageData& ppd );
bool is_training_command_supported_read_dq_training( const ParameterPageData& ppd );
bool is_training_command_supported_write_tx_dq_training( const ParameterPageData& ppd );
bool is_training_command_supported_write_rx_dq_training( const ParameterPageData& ppd );

// pair.first  = row address cycles
// pair.second = column address cycles
std::pair< BYTE, BYTE > get_num_address_cycles( const ParameterPageData& ppd );


//////////////////////////////////////////////////////////////////////////////

// For parsing the Status Register

//////////////////////////////////////////////////////////////////////////////

struct StatusRegister
{
	// Ctors
	StatusRegister();
	StatusRegister( const StatusRegister& );

	// Parses the binary representation of the Status Register
	StatusRegister( BYTE reg );
	void unpack( BYTE reg );

	// Need to declare dtor for std::unique_ptr
	~StatusRegister();

	// Getters
	bool fail();
	bool failc();
	bool csp();
	bool ardy();
	bool rdy();
	bool wp_n();
	bool vsp();

private:
	struct Impl;
	std::unique_ptr< Impl > _pImpl;
};