#include "stdafx.h" 
#include "Datalogs.h"
#include "PinLists.h"
#include "Levels.h"
#include "timer.h"

const double contact_open_voltage_N = -0.9 V;
const double contact_short_voltage_N = -0.2 V;
const double contact_open_voltage_P = 1.0 V;
const double contact_short_voltage_P = 0.2 V;
enum class ForceIPolarity { forcePositiveI, forceNegativeI };

// Sort through the partest results to differentiate between OPEN and SHORT failures.
static void bin_open_short_pass( PinList* pinlist, ForceIPolarity polarity )
{
	// For each ADS
	SOFT_DUT_LOOP( dut )
	{
		CArray< double, double > data;

		// Retrieving DC Test Results
		if( all_dps( pinlist ) )
		{
			// returns an array of measurement results
			// from the DPS pins
			Dps_meas( data );
		}
		else if( all_hv( pinlist ) )
		{
			// returns an array of measurement results
			// from the HV pins
			Hv_meas( data );
		}
		else
		{
			// returns an array of measurement results
			// from the digital pins
			Pin_meas( data );
		}

		double open_voltage_limit = 0.0 V;
		double short_voltage_limit = 0.0 V;

		if( polarity == ForceIPolarity::forcePositiveI )
		{
			open_voltage_limit = contact_open_voltage_P;
			short_voltage_limit = contact_short_voltage_P;
		}
		else if( polarity == ForceIPolarity::forceNegativeI )
		{
			open_voltage_limit = contact_open_voltage_N;
			short_voltage_limit = contact_short_voltage_N;
		}
		else
		{
			warning( "DUT%d: contact limits are not set properly!", dut + 1 );
		}

		// Iterate each measurement of the pins in a |pinlist|
		for( int i = 0; i < data.GetSize(); ++i )
		{
			// Get next DutPin in the PinList
			DutPin* dpin;
			pin_info( pinlist, i, &dpin );

			// Skip DutPins that aren't assignment to a tester channel
			TesterPin tpin;
			pin_info( dpin, dut, &tpin );
			if( tpin == TesterPin::t_na )
				continue;

			// Get the DutPin's name and then look at its associated measurement value
			CString pin_name = resource_name( dpin );
			double meas = data[i];

			// Check if measurement indicates open, short or pass.
			if( std::abs( meas ) >= std::abs( open_voltage_limit ) )
			{
				result_set( dut, 0 ); // open fail
				print_dut_error_msg( dut, vFormat( "Open Fail -- %s ( %s )", pin_name, meas_result_to_string( meas ) ) );
				break;
			}
			else if( std::abs( meas ) <= std::abs( short_voltage_limit ) )
			{
				result_set( dut, 1 ); // short fail
				print_dut_error_msg( dut, vFormat( "Short Fail -- %s ( %s )", pin_name, meas_result_to_string( meas ) ) );
				break;
			}
			else
				result_set( dut, 2 ); // pass
		}
	}
}

// Contact test
static void test_contact( PinList* pPinList, double iForce )
{
	bool force_positive_current = (iForce >= 0) ? true : false;

	levels::power_down();

	// Avoid no-op
	DutPin* pDutPin;
	if( pin_info( pPinList, 0, &pDutPin ) == FALSE )
	{
		results_set( FAIL );
		warning( "test_contact() -- Specified PinList is empty! All DUTs will fail" );
		return;
	}

	// Enable retrieval of measurement values
	measure( TRUE );
	
	// Set voltage clamps
	vclamp( 1.4 V, -1 V, pPinList );

	// Set VCC and VCCQ
	dps( 0 V, dutpin::VCC() );
	dps( 0 V, dutpin::VCCQ() );

	dps_connect( dutpin::VCC() );
	dps_connect( dutpin::VCCQ() );

	// Define test parameters
	ipar_force( iForce, pPinList );

	if( force_positive_current )
	{
		vpar_high( contact_open_voltage_P, pPinList );
		vpar_low( contact_short_voltage_P, pPinList );
	}
	else
	{
		vpar_high( contact_short_voltage_N, pPinList );
		vpar_low( contact_open_voltage_N, pPinList );
	}

	partime( 1 MS );

	// OPEN and SHORT test
	//crashing program in simulation - remove for now - bryan 042821
	output("partest begin");
	partest( PassCond::pass_nivl, pPinList );
	output("partest done");

	// Datalog
	datalog_enable_set( TRUE );
	parametric_datalog_set( 0xFF );
	parametric_datalog_enable( TRUE );
	parametric_datalog( pPinList );
	//pdatalog(pPinList);

	// Sort through results to differentiate between OPEN and SHORT failures.
	//if( force_positive_current )
	//	bin_open_short_pass( pPinList, ForceIPolarity::forcePositiveI );
	//else
	//	bin_open_short_pass( pPinList, ForceIPolarity::forceNegativeI );
}

TEST_BLOCK( TB__ContactUpperDiode_PE )
{
	std::string tn = current_test_block();
	output("test = %s", tn.c_str());

	TIME_IN(tn);

	test_contact( pinlist::Continuity_Upper_Diode(), 100 UA );

	TIME_OUT(tn);

	return MULTI_DUT;
}

TEST_BLOCK( TB__ContactLowerDiode_PE )
{
	std::string tn = current_test_block();
	output("test = %s", tn.c_str());

	TIME_IN(tn);

	test_contact( pinlist::Continuity_Lower_Diode(), -100 UA );

	TIME_OUT(tn);
	
	return MULTI_DUT;
}

MULTI_DUT_CALL_BLOCK(PE_CompModeTriState)
{
	pe_comparator_mode_set(pinlist::AllPins(), enComparatorModeMag7::t_TRISTATE);
}

MULTI_DUT_CALL_BLOCK(PE_CompModeNormal)
{
	pe_comparator_mode_set(pinlist::AllPins(), enComparatorModeMag7::t_NORMAL);
}

