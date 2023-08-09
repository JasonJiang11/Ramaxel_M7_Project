#pragma once

namespace i2c
{
	// Connects the bulk bypass capacitors on the VCC and VCCQ pins
	void connect_all_vcc_vccq_caps();

	// Disconnects the bulk bypass capacitors on the VCC and VCCQ pins
	void disconnect_all_vcc_vccq_caps();

	// Either connects or disconnects all bypass caps upon construction, and then
	// disconnects or connects them during destruction.
	struct BypassCapController
	{
		BypassCapController( bool connect ) : _connect( connect )
		{
			if( _connect )
				i2c::connect_all_vcc_vccq_caps();
			else
				i2c::disconnect_all_vcc_vccq_caps();
		}

		// Undo whatever was done by the constructor
		~BypassCapController()
		{
			if( _connect )
				i2c::disconnect_all_vcc_vccq_caps();
			else
				i2c::connect_all_vcc_vccq_caps();
		}

	private:
		bool _connect;
	};
}

