#pragma once
#include <exception>
#include <vector>
#include <map>
#include "../AppsDatalog/public.h"

namespace apps
{
	/// Utilities related to `PinAssignments`s
	namespace pin_assignments
	{
		/// Alias for map type representing the contents of a `PinAssignments` table
		using pin_assignments_map_type = std::map< DutPin*, std::map< DutNum, HDTesterPin > >;

		/// Returns the current `PinAssignments`.
		///
		/// Throws an exception if no `PinAssignments` is currently assigned.
		inline PinAssignments* get()
		{
			auto ret = PinAssignments_find( NULL );
			if( ret == NULL )
				throw std::exception( "pin_assignments::get() -- No PinAssignments table is currently selected." );
			return ret;
		}

		/// Extract all `DutPin`s and associated `HDTesterPin`s from the currently assigned `PinAssignments` table.
		///
		/// \param paMap    map containing the `DutPin`s and associated `HDTesterPin`s of all DUTs.
		inline void capture( pin_assignments_map_type& paMap )
		{
			// Clear DutPin/HDTesterPin map
			paMap.clear();

			// Iterate over all DutPins, for each DUT, and find the associate HDTesterPin.
			std::vector< PinList* > allPinLists = { builtin_UsedPins, builtin_UsedHVPins, builtin_UsedDPS };
			for( auto it : allPinLists )
			{
				DutPin* pDutPin;
				for( int i = 0; pin_info( it, i, &pDutPin ); ++i )
				{
					HDTesterPin testerPin;
					for( DutNum dut = t_dut1; pin_info( pDutPin, dut, &testerPin ); ++dut )
					{
						paMap[ pDutPin ].emplace( dut, testerPin );
					}
				}
			}
		}

		/// De-allocates the sepcified `PinAssignments` table.
		///
		/// De-allocates the sepcified `PinAssignments` table and optionally deletes it
		/// from memory. By default, the `PinAssignments` table is not destroyed such that
		/// we can still find it using `PinAssignments_find()`. Throws an exception if a
		/// problem occurred.
		///
		/// \param pPinAssignments    PinAssignments table
		/// \param destroy            if true, destroys the resource.
		inline void deallocate( PinAssignments* pPinAssignments, bool destroy = false )
		{
			BOOL ok = resource_deallocate( pPinAssignments, destroy ? TRUE : FALSE );
			if( !ok ) throw std::exception( "pin_assignments::deallocate() -- Unable to de-allocate the specified PinAssignments table" );
		}

		/// Initializes and selects the specified `PinAssignments`.
		///
		/// \param name    name of the `PinAssignments`
		inline void select( const std::string& name )
		{
			using namespace apps;

			// Get pointer to PinAssignments
			PinAssignments* pPA = PinAssignments_find( name.c_str() );
			if( pPA == NULL )
				throw std::exception( apps::datalog::format( "pin_assignments::select() -- Couldn't find the PinAssignments <%s> table.", name.c_str() ).c_str() );
			
			// Do nothing with currently used PinAssignments if same as |name|
			if( PinAssignments_find( 0 ) == pPA )
				return;

			// De-allocate the currently used PinAssignments
			pin_assignments::deallocate( pin_assignments::get() );

			// Re-initialize the user-provided PinAssignments...
			BOOL ok = resource_initialize( pPA, TRUE );
			if( !ok ) throw std::exception( apps::datalog::format( "apps::pin_assignments::select() -- Unable to initialize PinAssignments <%sv> table", name.c_str() ).c_str() );

			// ... and then re-use it.
			ok = PinAssignments_use( name.c_str() );
			if( !ok ) throw std::exception( apps::datalog::format( "apps::pin_assignments::select() -- Unable to use the PinAssignments <%s> table", name.c_str() ).c_str() );
		}

		/// Returns a string of the `PinAssignments` table represented as a map.
		///
		/// \param paMap         map representation of the `PinAssignments`
		inline std::string print( pin_assignments_map_type& paMap )
		{
			std::vector< std::string > headers;

			// Get number of DUTs
			size_t numDuts = paMap.begin()->second.size();

			// Define headers
			headers.emplace_back( "DutPin" );
			for( size_t i = 0; i < numDuts; ++i )
				headers.emplace_back( apps::datalog::format( "t_dut%d", i + 1 ) );
	
			// Iterate over each DutPin in the table
			std::vector< std::vector< std::string > > cols( headers.size() );
			for( auto it : paMap )
			{
				cols[ 0 ].emplace_back( resource_name( it.first ) );

				for( size_t i = 0; i < numDuts; ++i )
					cols[ i + 1 ].emplace_back( testerpin_name( it.second[ DutNum( i ) ] ) );
			}

			return apps::datalog::generate_table_from_columns( headers, cols );
		}

		/// Returns a string of the `PinAssignments` table represented as a map.
		///
		inline std::string print()
		{
			std::map< DutPin*, std::map< DutNum, HDTesterPin > > pinsPerDut;
			capture( pinsPerDut );
			return print( pinsPerDut );
		}

		/// \cond skip
		namespace detail
		{
			// This is the maximum number of DUTs that PinAssignmentsCreator
			// supports when adding a new row to a given PinAssignments table.
			// This number doesn't need to be larger than 64 since the largest
			// ASSIGN_xxDUTS macro is for 64 DUTs.
			static const unsigned int maxNumDuts = 64;

			template< unsigned int N >
			struct PinAssignmentsCreatorImpl
			{
				static const unsigned int dutNum = maxNumDuts - N;

				template< typename... _HDTesterPins >
				void operator()( unsigned int numDuts, const std::vector< DutNum >& permutation, const std::map< DutNum, HDTesterPin >& hdTesterPinMap,
					PinAssignments* pPinAssignments, DutPin* pDutPin, _HDTesterPins... hdPins )
				{
					// Get the HDTesterPin associated with DutNum |dutNum|
					auto hdPinNext = hdTesterPinMap.at( permutation[ static_cast< DutNum >( dutNum ) ] );

					if( numDuts == dutNum + 1 )
					{
						add( pPinAssignments, pDutPin, "", numDuts, hdPins..., hdPinNext );
					}
					else
					{
						PinAssignmentsCreatorImpl< N - 1 >{}( numDuts, permutation, hdTesterPinMap, pPinAssignments, pDutPin, hdPins..., hdPinNext );
					}
				}
			};

			// End condition
			template<>
			struct PinAssignmentsCreatorImpl< 1 >
			{
				static const unsigned int dutNum = maxNumDuts - 1;

				template< typename... _HDTesterPins >
				void operator()( unsigned int numDuts, const std::vector< DutNum >& permutation, const std::map< DutNum, HDTesterPin >& hdTesterPinMap,
					PinAssignments* pPinAssignments, DutPin* pDutPin, _HDTesterPins... hdPins )
				{
					// Get the HDTesterPin associated with t_dut1
					auto hdPinNext = hdTesterPinMap.at( permutation[ static_cast< DutNum >( dutNum ) ] );

					add( pPinAssignments, pDutPin, "", numDuts, hdPins..., hdPinNext );
				}
			};

			// Helper for adding a DutPin to a PinAssignments table, using a given permutation vector.
			struct PinAssignmentsCreator
			{
				void operator()( unsigned int numDuts, const std::vector< DutNum >& permutation, const std::map< DutNum, HDTesterPin >& hdTesterPinMap, 
					PinAssignments* pPinAssignments, DutPin* pDutPin )
				{
					// Check if valid number of columns
					if( numDuts <= detail::maxNumDuts )
						throw std::exception( "A maximum of 128 DUTs is supported for this PinAssignments table!" );

					PinAssignmentsCreatorImpl< detail::maxNumDuts >{}( numDuts, permutation, hdTesterPinMap, pPinAssignments, pDutPin );
				}
			};

			// Initializer function for creating a PinAssignments dynamically
			static const apps::pin_assignments::pin_assignments_map_type* _initPinsPerDut;
			static size_t _initNumDuts;
			static const std::vector< DutNum >* _initPermutation;
			static void pin_assignments_initializer( PinAssignments* pPinAssignments )
			{
				// Iterate over all DutPins
				for( auto it : *_initPinsPerDut )
				{
					DutPin* pDutPin = it.first;
					const auto& hdTesterPinMap = it.second;

					// Get the HDTesterPin associated with each column (DutNum)
					PinAssignmentsCreator{}( static_cast< unsigned int >( _initNumDuts ), *_initPermutation, hdTesterPinMap, pPinAssignments, pDutPin );
				}
			};

			// Swap the columns of the provided PinAssignments table.
			//
			// Returns true if the PinAssignments columns were swapped and the PinAssignments table
			// was re-initialized. Else returns false.
			static bool swapPinAssignmentsColumns(
				const std::vector< DutNum > permutation,
				const apps::pin_assignments::pin_assignments_map_type& pinsPerDut,
				const std::string& name )
			{
				// Avoid no-op (where each column stays at their current location inside the table).
				bool no_op = true;
				for( auto it = std::begin( permutation ); it != std::end( permutation ); ++it )
				{
					if( *it != ( it - std::begin( permutation ) ) )
					{
						no_op = false;
						break;
					}
				}
				if( no_op ) return false;

				// Check for invalid PinAssignments table
				if( pinsPerDut.empty() )
					throw std::exception( "swapPinAssignmentsColumns() -- The provided PinAssignments table is empty!" );

				// Check the number of DUTs in the table and make sure it correlates
				// to the |permutation| vector provided by the user
				size_t numDuts = pinsPerDut.begin()->second.size();
				if( numDuts != permutation.size() )
					throw std::exception( "swapPinAssignmentsColumns() -- The number of DUTs in the provided PinAssignments table "
						"does not correspond to the number of entries in the provided permutation vector!" );

				// Reconstruct the PinAssignments table with its columns swapped
				// according to the value of |permutationNumber|.
				_initPinsPerDut = &pinsPerDut;
				_initNumDuts = numDuts;
				_initPermutation = &permutation;

				// Create new PinAssignments. If a PinAssignments called |name| already
				// exists, delete before creating a new one.
				auto pPinAssignments = PinAssignments_find( name.c_str() );
				if( pPinAssignments )
				{
					apps::pin_assignments::deallocate( pPinAssignments, true );
				}
				_resource_create( pin_assignments_initializer, name.c_str(), __FILE__, __LINE__ );

				// Initialize the modified PinAssignments.
				apps::pin_assignments::select( name.c_str() );
	
				// After re-initialiaing PinAssignments, we must also re-initialize a bunch of other resources...
				// so just to be safe, we re-initialize all resources.
				BOOL ok = resource_initialize();
				if( !ok ) throw std::exception( "swapPinAssignmentsColumns() -- Unable to re-initialize all Resources" );

				return true;
			}
		}
		/// \endcond

		/// Swaps the columns of the currently used `PinAssignments` table.
		///
		/// Returns true if the `PinAssignments` columns were swapped and the `PinAssignments` table
		/// was re-initialized. Else returns false.
		///
		/// \param permutation     vector whose size must equal the number of columns (i.e. number of DUTs) in
		///                         the `PinAssignments` table. Element `i` specifies the column in the original
		///                         `PinAssignments` table which will be inserted as column `i` in the new `PinAssignments`
		///                         table.
		/// \param name            name of the new `PinAssignments` table
		///
		/// For example, if our `PinAssignments` contained 3 DUTs, swapping the columns corresponding to
		/// `t_dut1` and `t_dut3` would be accomplished with the following:
		///
		/// \code
		/// std::vector< DutNum > permutation = { t_dut3, t_dut2, t_dut1 };
		/// swap_columns( permutation );
		/// \endcode
		///
		inline bool swap_columns( const std::vector< DutNum >& permutation, const std::string& name = "ModifiedPinAssignments" )
		{
			if( sites_per_controller() != 1 )
			{
				warning( "pin_assignments::swap_columns(): Only supports sites-per-controller = 1" );
				return false;
			}

			// Extract currently selected PinAssignments
			std::map< DutPin*, std::map< DutNum, HDTesterPin > > pinsPerDut;
			capture( pinsPerDut );

			// Swap the columns
			return detail::swapPinAssignmentsColumns( permutation, pinsPerDut, name );
		}

		/// RAII-style `PinAssignments` restorer class.
		///
		/// Caches the currently used `PinAssignments` table during construction, and
		/// restores it during destruction.
		///
		/// \param save     if true, caches the name of the currently selected `PinAssignments`.
		///                  Else, restores the previously cached `PinAssignments`.
		struct Restorer
		{
			Restorer()
			{
				pa = resource_name( get() );
			}
			~Restorer()
			{
				if( pa != resource_name( get() ) )
					select( pa );
			}
		private:
			std::string pa;
		};
	}
}