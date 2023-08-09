#pragma once
#include <exception>
#include <vector>
#include <map>
#include "tester_enums.h" // DutNum
#include "../AppsDatalog/public.h"

namespace apps
{
	/// Utilities related to `UPBAssignments`s
	namespace upb_assignments
	{
		/// Alias for a map representing the contents of a `UPBAssignments` resource
		using upb_assignments_map_type = std::map< UPBModule, std::vector< DutNum > >;

		/// Returns the currently assigned `UPBAssignments`.
		///
		/// Throws an exception if no `UPBAssignments` table is currently assigned.
		inline UPBAssignments* get()
		{
			auto ret = UPBAssignments_find( NULL );
			if( ret == NULL )
				throw std::exception( "upb_assignments::get() -- No UPBAssignments is currently selected." );
			return ret;
		}

		/// Extracts all UPBs and their associated `DutNum`s from the current `UPBAssignments` table.
		///
		/// Stores the contents of the `UPBAssignments` in a map.
		///
		/// \param dutsPerUPB    map representing the contents of the `UPBAssignments`
		inline void capture( upb_assignments_map_type& dutsPerUPB )
		{
			// Clear UPB/DutNum map
			dutsPerUPB.clear();

			// Iterate over all UPBs and find the associated DutNums.
			for( UPBModule upb = UPBModule::t_upb1; upb != UPBModule::t_upb_na; ++upb )
			{
				CArray< DutNum, DutNum > duts;
				upb_assignments_get( upb, &duts );

				// Copy into a vector
				std::vector< DutNum > vec;
				for( int i = 0; i < duts.GetSize(); ++i )
					vec.emplace_back( duts[ i ] );

				// Move vector into map
				dutsPerUPB.emplace( upb, std::move( vec ) );
			}
		}

		/// De-allocates the specified `UPBAssignments`.
		///
		/// Optionally deletes the `UPBAssignments` from memory. If the `UPBAssignments`
		/// resource is not deleted from memory, it can still be found using `UPBAssignments_find()`.
		///
		/// Throws an exception if a problem occurred.
		///
		/// \param pUPBAssignments     `UPBAssignments` table to de-allocate.
		/// \param destroy             if true, destroys the resource.
		inline void deallocate( UPBAssignments* pUPBAssignments, bool destroy = false )
		{
			BOOL ok = resource_deallocate( pUPBAssignments, destroy ? TRUE : FALSE );
			if( !ok ) throw std::exception( "upb_assignments::deallocate() -- Unable to de-allocate the specified pUPBAssignments table" );
		}

		/// Initializes and selects the specified `UPBAssignments`.
		///
		/// \param name    name of the `UPBAssignments`
		inline void select( const std::string& name )
		{
			// Get pointer to the UpbAssignments
			UPBAssignments* pUpbAssignments = UPBAssignments_find( name.c_str() );
			if( pUpbAssignments == NULL )
				throw std::exception( apps::datalog::format( "upb_assignments::select() -- Couldn't find the UpbAssignments <%s>.", name ).c_str() );

			// Do nothing with currently used UPBAssignments if same as |name|
			if( UPBAssignments_find( 0 ) == pUpbAssignments )
				return;

			// De-allocate the currently used UpbAssignments
			deallocate( get() );

			// Re-initialize the user-provided UPBAssignments...
			BOOL ok = resource_initialize( pUpbAssignments, TRUE );
			if( !ok ) throw std::exception( apps::datalog::format( "upb_assignments::select() -- Unable to initialize UpbAssignments <%sv>", name ).c_str() );

			// ... and then re-use it.
			ok = UPBAssignments_use( name.c_str() );
			if( !ok ) throw std::exception( apps::datalog::format( "upb_assignments::select() -- Unable to use the UpbAssignments <%s>", name ).c_str() );
		}

		/// Prints the `UPBAssignments` table represented as a map.
		///
		/// \param name          name of the `UPBAssignments`
		/// \param paMap         map representation of the `UPBAssignments`
		inline void print( const std::string& name, upb_assignments_map_type& dutsPerUPB )
		{
			std::vector< std::string > headers;

			// Get number of UPBs
			size_t numUPBs = dutsPerUPB.size();

			// Define headers
			headers.emplace_back( "UPB" );
			for( size_t i = 0; i < 4; ++i ) // 4 DUTs per UPB
				headers.emplace_back( apps::datalog::format( "UPB-DUT %d", i + 1 ) );
	
			// Iterate over each DutPin in the table
			std::vector< std::vector< std::string > > cols( headers.size() );
			for( auto it : dutsPerUPB )
			{
				// Row header
				cols[ 0 ].emplace_back( apps::datalog::format( "UPB%d", it.first + 1 ) );

				// Table's main contents
				for( auto it_dut = begin( it.second ); it_dut != end( it.second ); ++it_dut )
				{
					auto ix = it_dut - begin( it.second );
					cols[ ix + 1 ].emplace_back( apps::datalog::format( "t_dut%d", *it_dut + 1 ) );
				}

				// Display remaining UPB-DUTs as "t_dut_na"
				size_t numDuts = it.second.size();
				for( size_t i = numDuts - 1; i < 3; ++i ) // 4 DUTs per UPB
					cols[ i + 1 ].emplace_back( "t_dut_na" );
			}
			output( "UPBAssignments Table <%s>:", name );
			output( apps::datalog::generate_table_from_columns( headers, cols ).c_str() );
		}

		/// Prints the currently selected `PinAssignments` table.
		inline void print()
		{
			std::map< UPBModule, std::vector< DutNum > > dutsPerUPB;
			capture( dutsPerUPB );
			print( resource_name( get() ), dutsPerUPB );
		}

		/// \cond skip
		namespace detail
		{
			// Construct a new UpbAssignments map based on the permutation vector, |permutation|,
			// that was presumably used to swap the columns of the PinAssignments table, |in|.
			inline void modify( const std::vector< DutNum >& permutation, const upb_assignments_map_type& in, upb_assignments_map_type& out )
			{
				using namespace std;
				out.clear();

				// Iterate over all UPBs that were previously assigned
				for( auto it = begin( in ); it != end( in ); ++it )
				{
					auto upb = it->first;

					// Create placeholder for |upb| in |out|
					out.emplace( upb, std::vector< DutNum >() );

					// For each DUT orginally assigned to |upb|, re-assign assign it to
					// the same UPB but under its new DutNum value.
					for( auto oldDutNum : it->second )
					{
						// Find the DUT's new DutNum value
						auto itFind = find( begin( permutation ), end( permutation ), oldDutNum );
						if( itFind == end( permutation ) )
							throw std::exception( apps::datalog::format( "modifyUpbAssignments() -- t_dut%d is missing from the permutation vector!", oldDutNum ).c_str() );
						auto newDutNum = static_cast< DutNum >( itFind - begin( permutation ) );

						// Assign to the same UPB as before
						out[ upb ].push_back( newDutNum );
					}
				}
			}

			// Initializer function for creating a UPBAssignments dynamically
			static const upb_assignments_map_type* _initDutsPerUpb;
			inline void initialize_upb_assignments( UPBAssignments* pUpbAssignments )
			{
				// Iterate over all UPBs
				for( auto it : *_initDutsPerUpb )
				{
					size_t numDuts = it.second.size();
					auto upb = it.first;
					auto& duts = it.second;

					// Assign specified DUTs to this UPB
					switch( numDuts )
					{
					case 1:
						_add( pUpbAssignments, upb, duts[ 0 ] );
						continue;
					case 2:
						_add( pUpbAssignments, upb, duts[ 0 ], duts[ 1 ] );
						continue;
					case 3:
						_add( pUpbAssignments, upb, duts[ 0 ], duts[ 1 ], duts[ 2 ] );
						continue;
					case 4:
						_add( pUpbAssignments, upb, duts[ 0 ], duts[ 1 ], duts[ 2 ], duts[ 3 ] );
						continue;
					default:
						throw std::exception( "Only 1 to 4 DUTs can be assigned to a single UPB in the UPBAssignments table!" );
					}
				}
			}

			// Create a new the UpbAssignments table called |name|
			inline void create( const upb_assignments_map_type& dutsPerUPB, const std::string& name )
			{
				// Check for invalid UpbAssignments table
				if( dutsPerUPB.empty() )
					throw std::exception( "updateModifiedUpbAssignments() -- The provided UpbAssignments table is empty!" );

				for( auto it : dutsPerUPB )
				{
					if( it.second.size() > 4 )
						throw std::exception( apps::datalog::format( "updateModifiedUpbAssignments() -- The provided "
						"UpbAssignments table has more than 4 DUTs assigned to t_upb%d", it ).c_str() );
				}

				// Reconstruct the UpbAssignments table.
				_initDutsPerUpb = &dutsPerUPB;
				_resource_create( initialize_upb_assignments, name.c_str(), __FILE__, __LINE__ );

				// Create new UPBAssignments. If a UPBAssignments called |name| already
				// exists, delete before creating a new one.
				auto pUPBAssignments = UPBAssignments_find( name.c_str() );
				if( pUPBAssignments )
				{
					apps::upb_assignments::deallocate( pUPBAssignments, true );
				}
				_initDutsPerUpb = &dutsPerUPB;
				_resource_create( initialize_upb_assignments, name.c_str(), __FILE__, __LINE__ );

				// Select the new UPBAssignments.
				apps::upb_assignments::select( name.c_str() );
	
				// After re-initialiaing UPBAssignments, we must also re-initialize a bunch of other resources...
				// so just to be safe, we re-initialize all resources.
				BOOL ok = resource_initialize();
				if( !ok ) throw std::exception( "upb_assignments::detail::create() -- Unable to re-initialize all Resources" );
			}
		}
		/// \endcond

		/// Swaps the columns of the currently used `UpbAssignments`.
		///
		/// Returns true if the `UpbAssignments` columns were swapped and the `UpbAssignments` table
		/// was re-initialized. Else returns false.
		///
		/// \param permutation     vector whose size must equal the number of columns (i.e. number of DUTs) in
		///                         the `PinAssignments` table. Element `i` specifies the column in the original
		///                         `PinAssignments` table which will be inserted as column `i` in the new `PinAssignments`
		///                         table.
		///
		/// \note The underlying assumption here is that this function would be invoked with the same input
		/// argument as `apps::pin_assignment::swap_columns()` since it wouldn't make any sense to swap
		/// the columns of the `UpbAssignments` without also swapping `PinAssignments` in the same fashion.
		inline void swap_columns( const std::vector< DutNum >& permutation, const std::string& name = "ModifiedUpbAssignments" )
		{
			std::map< UPBModule, std::vector< DutNum > > in, out;
			capture( in );
			detail::modify( permutation, in, out );
			detail::create( out, name );
			select( name );
		}

		/// RAII-style `UPBAssignments` restorer class.
		///
		/// Caches the currently used `UPBAssignments` table during construction, and
		/// restores it during destruction.
		///
		/// \param save     if true, caches the name of the currently selected `UPBAssignments`.
		///                  Else, restores the previously cached `UPBAssignments`.
		struct Restorer
		{
			Restorer()
			{
				ua = resource_name( get() );
			}
			~Restorer()
			{
				if( ua != resource_name( get() ) )
					select( ua );
			}
		private:
			std::string ua;
		};
	}
}