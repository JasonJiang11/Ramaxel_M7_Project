#include "stdafx.h" 
#include "Datalogs.h"
#include "Levels.h"
#include "PinLists.h"
#include "DutPins.h"
#include "Timings.h"
#include "DefineCounters.h"
#include "FlsUserTasks.h"

// Type used to extract per-DUT from the ECR
using EcrDataType = std::map< DutNum, std::vector< BYTE >>;

// Helper for ecr_read()
static void _ecr_read( size_t numBytes, size_t rowStart, size_t colStart, EcrDataType& data )
{
	// TODO:
	// Read ECR, Store to data per DUT
}

static void ecr_read( size_t numBytes, size_t xAddr, size_t yAddr, EcrDataType& data )
{
	// Read data from the ECR
	_ecr_read( numBytes, xAddr, yAddr, data );
}

// Compares ECR data with specified expect-data
static void compare_ecr_data( const std::vector< unsigned __int64 >& expectData, size_t x, size_t y )
{
	// Extract data from the ECR
	EcrDataType ecrData;
	ecr_read( expectData.size(), x, y, ecrData );

	// Compare each byte of data on each DUT
	results_set( PASS );

	for( auto it : ecrData )
	{
		auto dut = it.first;
		auto& vec = it.second;
		bool isFail = false;

		for( auto jt = std::begin( vec ); jt != std::end( vec ); ++jt )
		{
			auto ix = jt - begin( vec );
			auto expected = expectData[ix];
			auto actual = expectData[ix] ^ *jt;
			auto str = vFormat( "BYTE %04d -- Expect = 0x%02X, Actual = 0x%02X", ix, expected, actual );

			if( expected != actual )
			{
				isFail = true;
				str = "<COLOR=RED>" + str + "</COLOR>";
			}

			output( str );
		}
		output( "" );
		if( isFail ) result_set( dut, FAIL );
	}
}

static void compare_ecr_data( const Int64Array& expectData, size_t x, size_t y )
{
	// Place data from CArray into an std::vector, then call other overload
	std::vector< unsigned __int64 > temp;
	temp.reserve( expectData.GetCount() );
	for( auto i = 0; i < expectData.GetCount(); ++i )
		temp.push_back( expectData[i] );

	compare_ecr_data( temp, x, y );
}

// Issue a READ ID with Address 20h or 40h.
static void read_id_using_jam_ram( Pattern* pPattern, const Int64Array& valsJamRam, const Int64Array& valsEcr, bool compareDataOnFail )
{
	std::set< DutNum > failedDuts;

	// TODO: Disable even and enable odd DUTs
	//       Disable odd and enable even DUTs

	// Write the address byte and parameter bytes to JAM RAM
	apg_jam_mode_set( ApgJamMode::t_jam_mode_ram ); // configure JAM as RAM (instead of reg)
	apg_jam_ram_set( 0x00, const_cast<Int64Array&>(valsJamRam) ); // Write data to JAM RAM
	apg_jam_ram_address_set( 0x3FFF ); // reset address pointer

	// Load the num bytes MAR counter
	count( C_DATA_OUT_NUM_BYTES, static_cast<int>(valsJamRam.GetCount() - 1) );

	// Execute pattern with capturing to ECR
	funtest( pPattern, PatStopCond::fullec );

	// Get failed DUT results
	DutNumArray temp;
	results_get( &temp, FAIL );
	for( auto i = 0; i < temp.GetCount(); ++i )
		failedDuts.insert( temp[i] );

	// Compare expected vs actual data
	if( compareDataOnFail && any_results_match( FAIL ) )
	{
		compare_ecr_data( valsEcr, 0, 0 );
	}

	// Set results
	results_set( PASS );
	for( auto it : failedDuts )
		result_set( it, FAIL );
}

// Generate JAM RAM and ECR expect data
template< typename T >
static void generate_jam_ram_and_ecr_expect_data( const T& expectData, Int64Array& valsJamRam, Int64Array& valsEcr )
{
	valsJamRam.RemoveAll();
	valsEcr.RemoveAll();

	for( auto& it : expectData )
	{
		valsJamRam.Add( it );
		valsEcr.Add( it );
	}
}

static void read_id_with_address_20h()
{
	// Define the address byte and parameter bytes for the JAM RAM
	std::array< unsigned long, 5 > payloadData = { 0x4f, 0x4e, 0x46, 0x49, 0x1 };

	Int64Array valsJamRam, valsEcr;
	generate_jam_ram_and_ecr_expect_data( payloadData, valsJamRam, valsEcr );
}

TEST_BLOCK( TB__ReadId90hWithAddress20h )
{
	read_id_with_address_20h();

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadId90hWithAddress20h_dmain_BOCx4)
{
	output("%s", current_test_block());

	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, 0x4F4F);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, 0x4E4E);

	//funtest(PAT__ReadId90hWithAddress20h_dmain_x4, finish);
	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(PAT__ReadId90hWithAddress20h_dmain_x4);
	output("%s", fls_capture_data.readid_dump().c_str());

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//readid is 6 bytes
TEST_BLOCK(TB__ReadId90hWithAddress20h_dmain)
{
	output("%s", current_test_block());

	//BOCx8 tset to read first 4 bytes
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, 0x4f4f);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, 0x4e4e);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT3, 0x4646);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT4, 0x4949);

	//BOCx4 tset to read last 2 bytes 
	apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1, 0x0101);
	apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2, 0x0000);

	//funtest(PAT__ReadId90hWithAddress20h_dmain, finish);
	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(PAT__ReadId90hWithAddress20h_dmain);
	output("%s", fls_capture_data.readid_dump().c_str());

	////Execute again with FlsSimple Object
	//FlsSimple fls_simple;
	//fls_simple.funtest(PAT__ReadId90hWithAddress20h_dmain);
	//output("%s", fls_simple.dump_counts().c_str());

	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

//readid is 6 bytes
TEST_BLOCK(TB__LogReadId90hWithAddress20h_dmain)
{
	//Read all 0's so fails represent 1's in the FLS stream
	//BOCx8 tset to read first 4 bytes
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, 0);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, 0);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT3, 0);
	apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT4, 0);

	//BOCx4 tset to read last 2 bytes 
	apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1, 0);
	apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2, 0);

	//funtest(PAT__ReadId90hWithAddress20h_dmain, finish);
	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(PAT__ReadId90hWithAddress20h_dmain);

	//Process data

	//Pattern is capturing two pass/fail per clock cycle, so bytes are doubled
	std::array<byte, 10> readid_compare_bytes = { 0x4f, 0x4f, 0x4e, 0x4e, 0x46, 0x46, 0x49, 0x49, 0x01, 0x01 };
	std::stringstream ss;
	SOFT_DUT_LOOP(dut) {
		bool pass = equal(readid_compare_bytes.begin(), readid_compare_bytes.end(), fls_capture_data.DutStore[(int)dut].readid.id.begin());
		result_set(dut, pass);

		ss << "<COLOR=" << (pass ? "GREEN" : "RED") << ">ReadID-Log</COLOR>:: DUT" << std::dec << int(dut) + 1 << ": ";

		ss << "str=";
		for (int i = 0; i < 4; i++) {
			ss << char(fls_capture_data.DutStore[(int)dut].readid.id[i*2]);
		}

		switch (fls_capture_data.DutStore[(int)dut].readid.id[8]) {
		case 0: ss << ", SDR"; break;
		case 1: ss << ", NV-DDR3"; break;
		default: ss << ", UnknownIID";
		}

		ss << "\n";
	}
	output("%s", ss.str().c_str());
	return MULTI_DUT;
}