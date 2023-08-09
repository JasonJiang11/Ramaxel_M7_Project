#include "stdafx.h"
#include "Datalogs.h"

CString cnvt_result(double value, e_log_type log_type) {
	CString cstr;

	switch (log_type) {
	case e_log_type::LOG_V:	 // convert to voltage
		if (fabs(value) >= (1 V)) {
			cstr.Format("%8.3f v", value / (1 V));
		}
		else {
			cstr.Format("%8.0f mv", value / (1 MV));
		}
		break;
	case e_log_type::LOG_I:  // convert to current
		if (fabs(value) >= (1 MA)) {
			cstr.Format("%8.3f ma", value / (1 MA));
		}
		else if (fabs(value) >= (1 UA)) {
			cstr.Format("%8.3f ua", value / (1 UA));
		}
		else {
			cstr.Format("%8.0f na", value / (1 NA));
		}
		break;
	case e_log_type::LOG_T: break;
	default:	cstr.Format("#############");
	}

	return (cstr);
}  // end of cnvt_result


void pdatalog(PinList* pPinList)
{
	// determine the what type of test was just run
	e_log_type log_type;

	switch (parametric_mode()) {
	case 0:	log_type = e_log_type::LOG_I; break;
	case 2:	log_type = e_log_type::LOG_I; break;
	case 1:	log_type = e_log_type::LOG_V; break;
	default: output("Invalid parametric mode : %d", parametric_mode());
	}

	// Print header info
	output("TestBlock = %-15s   TestNumber = %d", current_test_block(), test_number());

	if (measure() == TRUE) {
			output("------------------------------------------");
			output("Tester      Pin           Pin     Measured");
			output(" Pin        Name          Result  Value   ");
			output("------------------------------------------");
		}
		else {
			output("----------------------------------");
			output("Tester      Pin           Pin     ");
			output(" Pin        Name          Result  ");
			output("----------------------------------");
		}

	// Loop over each active dut in turn
	SOFT_DUT_LOOP(dut_num)
	{

		CArray<double, double> meas_results;
		CArray<PFState, PFState> pf_results;

		// check the pinlist for either dps pins or signal pins
		if (all_dps(pPinList)) {
			Dps_meas(meas_results);	// get the measured results for this dut
			Dps_pf(pf_results);
		}
		else {
			Pin_meas(meas_results);	// get the measured results for this dut
			Pin_pf(pf_results);
		}
		output("Dut %d : %s", dut_num + 1, result_get(dut_num) ? "PASS" : "FAIL");

		int meas_index = 0;

		// Loop through all dut pins
		DutPin* dpin;
		for (int dpi = 0; pin_info(pPinList, dpi, &dpin); ++dpi) {

			// Get the HDTesterPin for this dut
			HDTesterPin tpin;
			pin_info(dpin, dut_num, &tpin);
			if (tpin == t_na)
				continue;

			CString pin_name = resource_name(dpin);
			CString tpin_name = testerpin_name(tpin);

			if (measure())
				output("%-10s   %-10s   %4s  %s", tpin_name, pin_name, 
					pf_results[meas_index] ? "<COLOR=GREEN>PASS</COLOR>" : "<COLOR=RED>FAIL</COLOR>",
					cnvt_result(meas_results[meas_index], log_type));

			else
				output("%-10s   %-10s   %4s", tpin_name, pin_name, 
					pf_results[meas_index] ? "<COLOR=GREEN>PASS" : "<COLOR=RED>FAIL</COLOR>");
			++meas_index;
		}

		ASSERT(meas_index == meas_results.GetSize());
	}
}

void fdatalog(PinList* pPinList)
{
	DutPin* dpin;

	SOFT_DUT_LOOP(dut)
	{
		output("Dut %d : %s", dut + 1, result_get(dut) ? "PASS" : "FAIL");

		// Loop through all pins and look for pass/fail
		for (int pin_index = 0; pin_info(pPinList, pin_index, &dpin); pin_index++)
		{
			PFState pf = test_pin(dpin);

			output("Pin = %6s: %s", resource_name(dpin),
				pf == PFState::FAIL ? "FAIL" : "PASS");
		}
	}
}

CString meas_result_to_string( double value, bool is_voltage )
{
	CString cstr;

	// Check the mode of the last executed parametric test
	if( is_voltage )
	{
		// Log voltage
		if( fabs( value ) >= (1 V) )
			cstr.Format( "%4.3f V", value / (1 V) );
		else
			cstr.Format( "%1.0f mV", value / (1 MV) );
	}
	else
	{
		// Log current
		if( fabs( value ) >= (1 MA) )
			cstr.Format( "%4.3f mA", value / (1 MA) );
		else if( fabs( value ) >= (1 UA) )
			cstr.Format( "%4.3f uA", value / (1 UA) );
		else
			cstr.Format( "%1.0f nA", value / (1 NA) );
	}
	return cstr;
}
CString meas_result_to_string( double value )
{
	bool is_voltage;
	switch( parametric_mode() )
	{
	case 1:
		is_voltage = true;
		break;
	case 0:
	case 2:
		is_voltage = false;
		break;
	default:
		print_error_msg( "Invalid parametric_mode()!" );
		return "";
	}
	return meas_result_to_string( value, is_voltage );
}
CString time_to_string( double val )
{
	CString ret;
	if( val == 0.0 )
		ret = "0 s";
	else if( val >= 1 S || val == 0.0 )
		ret = vFormat( "%4.3f s", val / (1 S) );
	else if( val >= 1 MS )
		ret = vFormat( "%4.3f ms", val / (1 MS) );
	else if( val >= 1 US )
		ret = vFormat( "%4.3f us", val / (1 US) );
	else if( val >= 1 NS )
		ret = vFormat( "%4.3f ns", val / (1 NS) );
	else if( val >= 1 PS )
		ret = vFormat( "%4.3f ps", val / (1 PS) );
	else
		ret = vFormat( "%3.0f fs", val / (1 FS) );
	return ret;
}
CString get_timestamp_string( UINT64 timestamp )
{
	const UINT64 ns_per_s = 1000000000; // 1e9
	const UINT64 ns_per_ms = 1000000; // 1e6
	const UINT64 ns_per_us = 1000; // 1e3

	UINT64 timestamp_ns = 10 * timestamp; // 10ns per clock cycle
	UINT64 time_s = timestamp_ns / ns_per_s;
	UINT64 time_ms = (timestamp_ns - (time_s * ns_per_s)) / ns_per_ms;
	UINT64 time_us = (timestamp_ns - (time_s * ns_per_s) - (time_ms * ns_per_ms)) / ns_per_us;
	UINT64 time_ns = timestamp_ns - (time_s * ns_per_s) - (time_ms * ns_per_ms) - (time_us * ns_per_us);

	CString s = vFormat( "%llu s", time_s );
	CString ms = vFormat( "%llu ms", time_ms );
	CString us = vFormat( "%llu us", time_us );
	CString ns = vFormat( "%llu ns", time_ns );

	// Return in units of seconds
	if( timestamp_ns > ns_per_s ) return vFormat( "%s : %s : %s : %s", s, ms, us, ns );

	// Return in units of miliseconds
	if( timestamp_ns > ns_per_ms ) return vFormat( "%s : %s : %s", ms, us, ns );

	// Return in units of microseconds
	if( timestamp_ns > ns_per_us ) return vFormat( "%s : %s", us, ns );

	// Return in units of nanoseconds
	return ns;
}