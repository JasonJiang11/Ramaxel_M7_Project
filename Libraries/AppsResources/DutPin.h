#pragma once

namespace apps
{
	/// Utilities related to `DutPin`s
	namespace dutpin
	{
		/// Returns the `HDTesterPin` associated with the specified `DutPin` and `DutNum`.
		///
		/// If the specified `DutNum` does not appear in the PinAssignments table (i.e. if
		/// `dut > max_dut()`, `t_na` is returned.
		///
		/// \param pDutPin    DutPin
		/// \param dut        DutNum
		inline HDTesterPin get_hd_tester_pin( DutPin* pDutPin, DutNum dut )
		{
			// Check for valid input
			if( dut > max_dut() )
				return TesterPin::t_na;

			HDTesterPin ret;
			pin_info( pDutPin, dut, &ret );
			return ret;
		}
	}
}