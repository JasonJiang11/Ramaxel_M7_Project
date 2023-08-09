#include "stdafx.h" 
#include "Datalogs.h"
#include "PinLists.h"
#include "Levels.h"
#include "DeviceSpecs.h"
#include "i2c.h"

///////////////////////////////////////////////////////////////////////////////////////////////

// LEAKAGE

///////////////////////////////////////////////////////////////////////////////////////////////

// Parks all DUTs w/ the specified result
static bool park_dut_result( int result )
{
	DutNumArray duts, ads;
	results_get( &duts, result, FALSE );
	active_duts_disable( duts, TRUE );

	// return true if ADS is now empty
	active_duts_get( &ads );
	return ads.GetSize() == 0;
}

static DutPin* first_pin_in_pinlist(PinList* pPinList)
{
	DutPin* pDutPin;

	pin_info(pPinList , 0, &pDutPin);

	return pDutPin;
}

static void test_leakage( double vTest, double vInvert, double iHigh, double iLow, PinList *pPinList )
{
	// Avoid no-op
	DutPin *pDutPin;
	if( pin_info( pPinList, 0, &pDutPin ) == 0 )
	{
		results_set( FAIL );
		warning( "test_leakage() -- Specified PinList is empty! All DUTs will fail" );
		return;
	}

	// Enable retrieval of measurement values
	measure( TRUE );

	vclamp(1.4 V, -1 V, pPinList);

	// Disconnect ENi
	disconnect( pinlist::pl_ENi() );

	// Apply power
	levels::power_up(2.5 V, 1.2 V, 12.2 V);

	// Configure PMU. Apply |v_test| on target pin, and |v_high| on others.
	vpar_force( vTest, pPinList);

	//hv_voltage_set( vInvert, builtin_UsedHVPins );

	//set vil to 0
	const auto min_vil_vih = levels::get_min_vil_vih();
	//pe_driver_set(pinlist::AllPins(), vInvert + min_vil_vih, 0, 0, levels::HIGH_IMPEDANCE, enPEDrvPeakingMag7::t_PEAK_NONE);
	pin_dc_state_set(pinlist::AllPins(), vTest < vInvert ? PEDriverState::t_vih : PEDriverState::t_vil, FALSE);
	//pe_driver_set(pinlist::AllPins(), vInvert + min_vil_vih, vInvert, 0, levels::HIGH_IMPEDANCE, enPEDrvPeakingMag7::t_PEAK_NONE);

	// Configure PMU
	iacc_count_set( 200 );
	
	ipar_high( iHigh, pPinList );
	ipar_low( iLow, pPinList );

	partime(1 MS);
	partest(PassCond::pass_nicl, pPinList);

	// Datalog
	output( "vpar_force = %s\n", meas_result_to_string( vpar_force(first_pin_in_pinlist(pPinList)), true ) );
	//datalog_enable_set( TRUE );
	//parametric_datalog_set( 0xFF );
	//parametric_datalog_enable( TRUE );
	//parametric_datalog( pPinList );
	pdatalog(pPinList);

	// Disable back voltage
	//back_voltage_enable( false );

	// Remove power
	levels::power_down();
}

static void test_leakage_input_high( double iHigh, double iLow, PinList *pPinList )
{
	output( ">> LEAKAGE HIGH" );
	test_leakage( 1.8 V, 0 V, iHigh, iLow,pPinList );
}

static void test_leakage_input_low( double iHigh, double iLow, PinList *pPinList )
{
	output( ">> LEAKAGE LOW" );
	test_leakage( 0 V, 1.8 V, iHigh, iLow,pPinList );
}

static void test_leakage_input( unsigned int numSharedPins, PinList *pPinList )
{
	//double iHigh = 2 UA * numSharedPins;
	//double iLow = -2 UA * numSharedPins;
	double iHigh = 10 UA * numSharedPins;
	double iLow = -10 UA * numSharedPins;

	output(">> LEAKAGE HIGH");
	test_leakage(1.2 V, 0 V, iHigh, iLow, pPinList);

	park_dut_result( FAIL );

	output(">> LEAKAGE LOW");
	test_leakage(0 V, 1.2 V, iHigh, iLow, pPinList);
}

TEST_BLOCK(TB__Input_Leakage)
{
	test_leakage_input(1, pinlist::AllInputPins_Leakage());

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_ALE )
{
	test_leakage_input( device::is_132_BGA ? 2 : 4, pinlist::AllAddressLatchEnablePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_CEn )
{
	test_leakage_input( device::is_132_BGA ? 2 : 4, pinlist::AllChipEnablePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_CLE )
{
	test_leakage_input( device::is_132_BGA? 2 : 4, pinlist::AllCommandLatchEnablePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_REn )
{
	//double iHigh = 2 UA * 2;
	//double iLow = -2 UA * 2;
	double iHigh = 10 UA * 2;
	double iLow = -10 UA * 2;

	test_leakage_input( 2, pinlist::AllReadEnablePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_WEn )
{
	test_leakage_input( 2, pinlist::AllWriteEnablePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_RB )
{
	test_leakage_input( 4, pinlist::AllReadyBusyPins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_DQ_DQS_H )
{
	test_leakage( 1.2 V, 0 V, 2 UA, -2 UA, pinlist::AllDataAndDataStrobePins() );

	return MULTI_DUT;
}

TEST_BLOCK( TB__Leakage_PE_DQ_DQS_L )
{
	test_leakage( 0 V, 1.2 V, 2 UA, -2 UA, pinlist::AllDataAndDataStrobePins() );

	return MULTI_DUT;
}
