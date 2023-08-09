#pragma once
#include <map>
#include <array>
#include <functional>

namespace apps
{
	/// Utilities related to `PinList`s
	namespace pinlist
	{
		/// Returns a vector of all `HDTesterPin`s associated with the specified `PinList`.
		///
		/// \param pPinList    `PinList`
		/// \param hdPins      vector containing the associated `HDTesterPin`s
		inline void get_hd_tester_pins( PinList* pPinList, std::vector< HDTesterPin >& hdPins )
		{
			hdPins.clear();
			HDTesterPin hdPin;
			for( int i = 0; pin_info( pPinList, i, &hdPin ); ++i )
			{
				hdPins.emplace_back( hdPin );
			}
		}

		/// Returns a vector of all the `DutPin`s contained in the specified `PinList`
		///
		/// \param pPinList    `PinList`
		/// \param dutPins     vector containing the associated `DutPin`s
		inline void get_pinlist_as_vector( PinList* pPinList, std::vector< DutPin* >& dutPins )
		{
			dutPins.clear();

			// Get all DutPins from the PinList
			DutPin* pDutPinTemp;
			for( int i = 0; pin_info( pPinList, i, &pDutPinTemp ); ++i )
			{
				dutPins.emplace_back( pDutPinTemp );
			}
		}
		
		/// \cond skip
		namespace detail
		{
			static std::vector< HDTesterPin > myPinListCreatorHdPins ;
			inline void my_pinlist_creator( PinList* pPinList )
			{
				for( auto& it : myPinListCreatorHdPins )
				{
					_add( pPinList, testerpin_name( it ) );
				}
			}
		}
		/// \endcond

		/// Creates a new PinList resource from the specified HDTesterPins
		///
		/// This provides an alternative to using `pinlist_create()`. By using this
		/// function instead of `pinlist_create()`, we are creating a true (static)
		/// PinList instead of a dynamic one.
		///
		/// \param pins    list of HDTesterPins
		/// \param name    name of the PinList
		///
		inline PinList* create( const std::vector< HDTesterPin >& pins, const std::string& name )
		{
			return _resource_create( detail::my_pinlist_creator, name.c_str(), __FILE__, __LINE__ );
		}

		/// \cond skip
		namespace detail
		{
			// Returns true if the list of TesterPins is equivalent to the
			// contents of the PinList, else returns false.
			inline bool equal( const std::vector< TesterPin >& testerPins, PinList* pPinList )
			{
				std::vector< TesterPin > otherTesterPins;
				TesterPin tPin;
				for( auto i = 0; pin_info( pPinList, i, &tPin ); ++i )
				{
					otherTesterPins.emplace_back( tPin );
				}
				return std::equal( std::begin( testerPins ), std::end( testerPins ), std::begin( otherTesterPins ) );
			}

			// Returns a dynamically created PINS_OF_1DUT PinList for each DUT in the PinAssignments table.
			inline std::map< DutNum, PinList* > get_per_dut_pinlists( PinList* pPinList )
			{
				std::map< DutNum, PinList* > ret;
				auto plName = resource_name( pPinList );

				// Get all DutPins from the PinList
				std::vector< DutPin* > dutPins;
				apps::pinlist::get_pinlist_as_vector( pPinList, dutPins );

				// Create a PINS_OF_1DUT PinList for each DUT
				for( auto dut = t_dut1; dut <= max_dut(); ++dut )
				{
					std::vector< TesterPin > pins;
					TesterPin pinTemp;
					for( auto& it : dutPins )
					{
						pin_info( it, dut, &pinTemp );
						pins.push_back( pinTemp );
					}

					// Create the actual PinList object. If a PinList with the
					// same name already exists, it is **probably** because get_per_dut_pinlits()
					// in a different translation unit was called for the same PinList. So
					// in that case, verify that both PinLists contain the same TesterPins
					// and then store the existing PinList.
					for( auto i = 0; true; ++i )
					{
						// In case a PinList with the same name already exists
						// and is different from what the PinList we want to create,
						// we simply change the PinList's name until we find a unique name.
						auto name = vFormat( "%s - Pins of t_dut%d%s",
							plName,
							dut + 1,
							i == 0 ? "" : vFormat( " (%d)", i ) );

						auto pPinList = PinList_find( name );
						if( pPinList )
						{
							// Verify that |pins| and |pPinList| are equivalent
							if( equal( pins, pPinList ) )
								ret[ dut ] = pPinList;
							else
								continue; // Try a different PinList name
						}
						else
							ret[ dut ] = pinlist_create( pins.data(), static_cast< int >( pins.size() ), name );

						// Either a new PinList was created or a PinList meeting our
						// requirements already existed. Exit this loop.
						break;
					}
				}
				return ret;
			}

			// Returns both the SuperMUX 'early' and 'late' subset of a specified PinList.
			// The element at index 0 is the 'early' PinList.
			// The element at index 1 is the 'late' PinList.
			inline std::array< PinList*, 2 > get_supermux_pinlists( PinList* pPinList, bool ignoreNonPePinLists )
			{
				std::array< PinList*, 2 > ret;
				ret.fill( nullptr );

				// Verify that all contained pins are PE channels
				if( !all_pe( pPinList ) )
				{
					if( !ignoreNonPePinLists )
					{
						warning( "get_supermux_pinlists() -- PinList <%s> isn't all PE channels.",
							resource_name( pPinList ) );
					}
					return ret;
				}

				// Get all HDTesterPins from the PinList
				std::vector< HDTesterPin > hdPins;
				apps::pinlist::get_hd_tester_pins( pPinList, hdPins );

				// First, find all early pins, and then all late pins.
				std::vector< TesterPin > earlyPins, latePins; 
				for( auto& hdPin : hdPins )
				{
					auto hdPinBase = hdPin;
					DWORD offset;
					auto ok = testerpin_offset( &hdPinBase, &offset );

					// Check that pin is valid
					if( !ok )
					{
						warning( "get_supermux_pinlists() -- Invalid HDTesterPin in PinList <%s>",
							resource_name( pPinList ) );
						continue;
					}

					// Determine whether pin is early or late
					if( offset % 2 == 0 )
					{
						// early (odd) PE channel
						earlyPins.emplace_back( hdPin );
					}
					else
					{
						// late (even) PE channel
						latePins.emplace_back( hdPin );
					}
				}

				// Create both the early and late PinLists
				std::array< std::pair< std::vector< TesterPin >*, std::string >, 2 > argArray =
				{
					std::make_pair( &earlyPins, "Early" ),
					std::make_pair( &latePins, "Late" ),
				};
				for( auto it = std::begin( argArray ); it != std::end( argArray ); ++it )
				{
					auto ix = it - std::begin( argArray );
					auto pPinVec = it->first;
					auto plName = vFormat( "%s - %s",
						resource_name( pPinList ),
						it->second.c_str() );
					ret[ ix ] = pinlist_create( pPinVec->data(), static_cast< int >( pPinVec->size() ), plName );
				}

				return ret;
			}

			// Processes each existing PinList using func() and returns a map contain the results.
			// Namely, each element in the returned map is equivalent to the return value of
			// std::make_pair( pPinList, func( pPinList ) ). The name of the overarching function is
			// passed as a second argument in order to generate a meaninful warning message if some
			// existing PinList cannot be found via PinList_find() for some odd reason.
			//
			template< typename MappedType >
			inline std::map< PinList*, MappedType > process_all_pinlists( typename std::function< typename MappedType ( PinList* ) > func,
				const std::string funcName )
			{
				std::map< PinList*, MappedType > ret;
	
				// For each existing PinList
				CStringArray plNames;
				resource_all_names( S_PinList, &plNames );
				for( int i = 0; i < plNames.GetSize(); ++i )
				{
					PinList* pPinList = PinList_find( plNames[ i ] );

					if( pPinList )
						ret[ pPinList ] = func( pPinList );
					else
						warning( "%s() -- PinList <%s> doesn't exist.",
						funcName.c_str(),
						plNames[ i ] );
				}

				return ret;
			}

			// Returns per-DUT PinLists for all existing PinLists
			inline std::map< PinList*, std::map< DutNum, PinList* > > get_all_per_dut_pinlists()
			{
				std::function< std::map< DutNum, PinList* >( PinList* ) > func =
					[]( PinList* pPinList ){ return get_per_dut_pinlists( pPinList ); };
				return process_all_pinlists( func, "get_all_per_dut_pinlists" );
			}

			// Returns SuperMux PinLists for all existing PinLists
			inline std::map< PinList*, std::array< PinList*, 2 > > get_all_supermux_pinlists()
			{
				std::function< std::array< PinList*, 2 >( PinList* ) > func =
					[]( PinList* pPinList ){ return get_supermux_pinlists( pPinList, true ); };
				return process_all_pinlists( func, "get_all_supermux_pinlists" );
			}
		}
		/// \endcond

		/// Returns a `PinList` containing only the `TesterPin`s of the specified DUT.
		///
		/// To increase performance, this function caches all its return values. In addition,
		/// the first time this function is invoked, it computes and caches all the per-DUT
		/// PinLists of all the existing PinLists in the test program.
		///
		/// \warning Since this function is `inline`, each translation unit that defines it
		///          will hold a cache of this function's return values. For applications where
		///          memory is scarce, you should consider implementing this function in a
		///          single translation unit and allow external linkage to it from other
		///          translation units.
		///
		/// \param pPinList    `PinList`
		/// \param dut         DUT for which the `TesterPin`s will be extracted and returned.
		inline PinList* get_pins_of_dut( PinList* pPinList, DutNum dut )
		{
			static auto pinlistsPerDut = detail::get_all_per_dut_pinlists();
	
			// Check for no-op
			if( pPinList == nullptr )
				return pPinList;

			// Get the per-DUT PinLists of |pPinList|
			auto it = pinlistsPerDut.find( pPinList );
			if( it != std::end( pinlistsPerDut ) )
			{
				return it->second.at( dut );
			}
			else
			{
				// Per-DUT PinLists have not yet been created for |pPinList|
				// so we need to create them now.
				return ( pinlistsPerDut[ pPinList ] = detail::get_per_dut_pinlists( pPinList ) ).at( dut );
			}
		}

		/// Returns a `PinList` containing only the 'early' (odd) or 'late' (even) PE channels
		/// of the specified `PinList`.
		///
		/// The concept of 'early' and 'late' PE channels is important when dealing with Super-MUX
		/// because all 'early' PE channels must have their timing programmed before their 'late'
		/// counterparts. For example, PE channel a_1 (early) must always have its timing programmed
		/// before a_2 (late counterpart).
		///
		/// To increase performance, this function caches all its return values. In addition,
		/// the first time this function is invoked, it computes and caches both the 'early' and
		/// 'late' PinLists of all the existing PinLists in the test program.
		///
		/// \warning Since this function is `inline`, each translation unit that defines it
		///          will hold a cache of this function's return values. For applications where
		///          memory is scarce, you should consider implementing this function in a
		///          single translation unit and allow external linkage to it from other
		///          translation units.
		///
		/// \param pPinList   `PinList`
		/// \param late       if true, extracts the 'late' pins, else extracts the 'early' pins.
		inline PinList* get_supermux_pins( PinList* pPinList, bool late )
		{
			static auto superMuxPinLists = detail::get_all_supermux_pinlists();
	
			// Check for no-op
			if( pPinList == nullptr )
				return pPinList;

			// Get the SuperMUX PinLists of |pPinList|
			auto it = superMuxPinLists.find( pPinList );
			if( it != std::end( superMuxPinLists ) )
			{
				return it->second.at( late ? 1 : 0 );
			}
			else
			{
				// Per-DUT PinLists have not yet been created for |pPinList|
				// so we need to create them now.
				return ( superMuxPinLists[ pPinList ] = detail::get_supermux_pinlists( pPinList, false ) ).at( late ? 1 : 0 );
			}
		}

	}
}