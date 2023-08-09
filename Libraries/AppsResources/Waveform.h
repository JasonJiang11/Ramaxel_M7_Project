#pragma once
#include <map>
#include "../AppsDatalog/public.h"

namespace apps
{
	namespace waveform
	{
		using waveform_map_type = std::map< DutPin*, std::map< DutNum, Waveform* > >;

		// Implementation details
		namespace detail
		{
			// Creates and formats Waveform objects (using the ADC samples stored in the PMU for the specified DutPin/PinList)
			// for each DUT in the Active DUT Set, and stores a pointer to the resultant Waveform in the DUT map |wfmap|.
			static void _store_waveforms( PassCond pass_cond, DutPin *pin, const std::string& tl_name,
				std::map< DutNum, Waveform* >& wfmap, bool xAxisIsTime )
			{
				// Get ADC measurement interval
				double meas_interval = xAxisIsTime ? adc_sample_interval_get( pin ) : 1.0;
				auto xUnits = xAxisIsTime ? MKS_OR_LEGACY( SCALE_SECONDS, SCALE_PICOSECONDS ) : "Sample";

				// Choose the 'yunits' for the Waveform objects
				LPCTSTR yunits = "";
				CString measType = "";
				switch(pass_cond)
				{
				case pass_ncl:
				case pass_nicl:
				case pass_pcl:
					yunits = MKS_OR_LEGACY( SCALE_AMPERES,SCALE_NANOAMPERES );
					measType = "CURRENT";
					break;

				default:
				case pass_vg:
				case pass_vl:
				case pass_nivl:
					yunits = MKS_OR_LEGACY( SCALE_VOLTS, SCALE_MICROVOLTS );
					measType = "VOLTAGE";
					break;
				}

				// Get DutPin name and use for the Waveform name
				auto pin_name = resource_name( pin );

				// Empty the DutNum-Waveform map before proceeding
				wfmap.clear();

				SOFT_DUT_LOOP( dut )
				{
					// Get ADC measurements.
					DoubleArray data;
					adc_measurements_get( pin, dut, data );

					// Create a new Waveform object using all the samples stored in |data|.
					auto wf_name = apps::datalog::format( "%s__%s__%s__DUT%02d", tl_name, pin_name, measType, dut+1 );
					Waveform* wf = Waveform_find( wf_name.c_str() );
					if ( wf == NULL ) wf = waveform_create( wf_name.c_str() );

					// Store measurments in |wf|
					waveform_set_rrect( wf, data, yunits, 0, meas_interval, xUnits );

					// Store |wf| in a DUT map
					wfmap[ dut ] = wf;
				}
			}
			static void _store_waveforms( PassCond pass_cond, PinList *pl, const std::string& tl_name,
				std::map< DutPin*, std::map< DutNum, Waveform* > >& wfmap, bool xAxisIsTime )
			{
				DutPin *pin;
				wfmap.clear();
				for( int i = 0; pin_info( pl, i, &pin ); ++i )
				{
					std::map< DutNum, Waveform* > pin_wfmap;
					_store_waveforms( pass_cond, pin, tl_name, pin_wfmap, xAxisIsTime );
					wfmap[ pin ] = pin_wfmap;
				}
			}
		}

		/// Immediately triggers the ADC to measure either current or voltage on the specified PinList
		inline void trigger_capture( PinList* pPinList, BOOL measureCurrent, int sampleCount, double sampleInterval )
		{
			adc_average_count_set( 1, pPinList );
			adc_sample_count_set( sampleCount, pPinList );
			adc_sample_interval_set( sampleInterval, pPinList );
			adc_vcomp_delay_set( 0, pPinList );

			adc_dps_samples_trigger( pPinList, measureCurrent ? DCForceMeasType::t_measI : DCForceMeasType::t_measV );
		}

		/// Reads the current or voltage Waveform from the ADC RAM
		inline void read( PinList* pPinList, bool measureCurrent, const std::string& waveformName,
			waveform_map_type& waveforms = waveform_map_type{}, bool xAxisIsTime = true )
		{
			auto passCond = measureCurrent ? PassCond::pass_nicl : PassCond::pass_vg;
			detail::_store_waveforms( passCond, pPinList, waveformName, waveforms, xAxisIsTime );
		}

	#if 0 // Disable until we fix this dependency on logfile_utils.h

		// Saves the specified Waveform to a file
		inline void save_to_file( const Waveform* wf, const char* subDir )
		{
			using namespace waveform;

			// Avoid invalid Waveforms
			if( wf == NULL )
			{
				warning( "save_waveform_to_file() -- Invalid Waveform specified." );
				return;
			}

			// Get path to logging directory
			auto dirPath = logfile::LogFileFolderPath();
			dirPath += "/";
			dirPath += "Waveforms";
			dirPath += "/";
			dirPath += subDir;
			dirPath += "/";

			// Create a directory to store this Waveform (and all other Waveforms)
			logfile::CreateDirectories( dirPath );

			// Use the Waveform's name as the file name. But also append the slot id.
			int chassis, slot, slice;
			get_slot_id( site_num(), &chassis, &slot, &slice );
			auto wfName = waveform_get_name( const_cast< Waveform* >( wf ) );
			auto fileName = format_filename( "%s%s_%d_%d_%d<AUTOINC:_%04d>.nwav", dirPath, wfName, chassis, slot, slice );

			waveform_write_file( const_cast< Waveform* >( wf ), fileName );
		}
		void waveform::save_to_file( DutPin* pDutPin, DutNum dut, const Waveform* wf )
		{
			auto dutPinName = resource_name( pDutPin );
			auto subDir = vFormat( "%s/DUT_%d", dutPinName, dut + 1 );
			save_to_file( wf, subDir );
		}
		inline waveform::save_to_file( const waveform_map_type& wfMap )
		{
			for( auto& it : wfMap )
			{
				auto pDutPin = it.first;

				for( auto& jt : it.second )
				{
					auto dut = jt.first;
					save_to_file( pDutPin, dut, jt.second );
				}
			}
		}
	#endif
	}
}