#pragma once

namespace apps
{
	/// RAII-style timer for measuring elapsed time
	struct Timer 
	{
		/// Starts the timer
		Timer()
		{
			// Take timestamp representing the start of the flow.
			QueryPerformanceFrequency( &_freq );
			QueryPerformanceCounter( &_start );
		}

		/// Returns the elapsed time (in seconds) since the instatiation of the object.
		double query()
		{
			// Get timestamp representing the end of the flow
			QueryPerformanceCounter( &_end );

			// Compute elapsed time. First convert the elapsed number
			// of ticks to miliseconds to avoid loss of precision.
			double elapsed = (_end.QuadPart - _start.QuadPart) * 1e3;
			elapsed /= _freq.QuadPart;

			// Return value in seconds
			return elapsed / 1e3;
		}

	private:
		LARGE_INTEGER _start, _end, _freq;
	};
}