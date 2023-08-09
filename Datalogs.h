#pragma once
#include "TestProgApp/public.h"

template< typename... _Args >
static void print_dut_msg( DutNum dut, _Args... args )
{
	output( "(DUT_%d) : %s", dut + 1, vFormat( args... ) );
}
template< typename... _Args >
static void print_msg( _Args... args )
{
	output( args... );
}
template< typename... _Args >
static void print_dut_error_msg( DutNum dut, _Args... args )
{
	output( "<color=red>(DUT_%d) ERROR : %s</color>", dut + 1, vFormat( args... ) );
}
template< typename... _Args >
static void print_error_msg( _Args... args )
{
	output( "<color=red>ERROR : %s</color>", vFormat( args... ) );
}

enum class e_log_type { LOG_I, LOG_V, LOG_T };

void pdatalog(PinList* pPinList);
void fdatalog(PinList* pPinList);

CString meas_result_to_string( double value, bool is_voltage );
CString meas_result_to_string( double value );
CString time_to_string( double val );
CString get_timestamp_string( UINT64 timestamp );