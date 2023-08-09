#include "stdafx.h"
#include "i2c.h"

// For each bypass-cap-relay controller, configures all 8 IO pins as output (using the CONFIGURATION register),
// and then writes |write_data| to the OUTPUT register.
static void bypass_cap_relay_controller_output_level_set( int i2c_addr, BYTE output_reg_data )
{
	I2C_control i2cc;
	unsigned __int8 writeData[1];
	unsigned __int8 readData[1];

	// Send command to relay controller
	i2cc.target = i2c_addr;
	i2cc.ten_bit_target = FALSE;
	i2cc.address = 0x3; // CONFIGURATION register
	i2cc.address_length = 0x1;
	// Configure all 8 IO pins as OUTPUT by writing to the CONFIGURATION register (0x3).
	writeData[0] = 0x00; // configure switch's pins as OUTPUT pins
	i2cc.write_data = writeData;
	i2cc.write_data_length = 0x1;
	i2cc.target_buffer_size = 0x0; // device does not have an input buffer
	i2cc.read_data = readData;
	i2cc.read_data_length = 0;
	i2cc.actual_read_length = 0;

	BOOL ok = I2C_operation( i2cc );
	if( !ok )
		warning( "bypass_cap_relay_controller_output_level_set() -- I2C operation( CONFIGURATION REGISTER ) returned FALSE" );

	// Set the output level of all 8 IO pins by writing to the OUTPUT register (0x1).
	writeData[0] = output_reg_data;
	i2cc.address = 0x1;

	ok = I2C_operation( i2cc );
	if( !ok )
		warning( "bypass_cap_relay_controller_output_level_set() -- I2C operation( OUTPUT REGISTER ) returned FALSE" );
}

using PCA9538_Address = int;
static void bypass_cap_relay_controller_output_level_set( const std::vector< PCA9538_Address >& config, BYTE output_reg_data )
{
	for( auto& it : config )
	{
		bypass_cap_relay_controller_output_level_set( it, output_reg_data );
	}
}

static const std::vector< PCA9538_Address >* get_all_configs()
{
	static std::vector< PCA9538_Address > ret;
	if( ret.empty() )
	{
		ret.emplace_back( 0x70 | 0x0 ); // U1 PCA9538PW/Q900
		//ret.emplace_back( 0x70 | 0x1 ); // U2 PCA9538PW/Q900
		//ret.emplace_back( 0x70 | 0x2 ); // U3 PCA9538PW/Q900
	}
	return &ret;
}

// Set all 8 IO pins (of each bypass-cap-relay controller) to drive HIGH. 
// This connects the bulk capacitance to ground.
void i2c::connect_all_vcc_vccq_caps()
{
	bypass_cap_relay_controller_output_level_set( *get_all_configs(), 0xFF );
}

// Set all 8 IO pins (of each bypass-cap-relay controller) to drive LOW. 
// This disconnects the bulk capacitance from ground.
void i2c::disconnect_all_vcc_vccq_caps()
{
	bypass_cap_relay_controller_output_level_set( *get_all_configs(), 0x00 );
}

