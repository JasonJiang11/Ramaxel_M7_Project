#pragma once
#include <set>
#include <map>

namespace apps
{
	/// Alias template for defining DUT maps
	///
	/// \param T    mapped type
	template< typename T >
	using DutMap = typename std::map< DutNum, T >;

	/// Captures the current Active DUT Set upon construction,
	/// and restores it during destruction.
	struct AdsRestorer
	{
		/// Captures the current active DUT set.
		AdsRestorer()
		{
			active_duts_get( &ads ); 
		}

		/// Restores the capture active DUT set.
		~AdsRestorer()
		{
			active_duts_enable( ads );
		}

	private:
		DutNumArray ads;
	};

	/// Populates a set with all active DUTs
	///
	/// \param ads    active DUT set
	inline void active_duts_get( std::set< DutNum >& ads )
	{
		DutNumArray duts;
		::active_duts_get( &duts );

		ads.clear();
		for( auto i = 0; i < duts.GetCount(); ++i )
			ads.insert( duts[ i ] );
	}

	/// Removes the specified DUTs from the Active DUT Set.
	///
	/// \param ads            DUTs to remove from the active DUT set.
	/// \param incremental    if true, removes the DUTs in |ads| from the active DUT set,
	///                        else sets the active DUT set to |ads|.
	inline void active_duts_disable( const std::set< DutNum >& ads, bool incremental = false )
	{
		DutNumArray adsTemp;
		for( auto&& dut : ads )
			adsTemp.Add( dut );

		::active_duts_disable( adsTemp, incremental );
	}

	// Adds the specified DUTs from the Active DUT Set.
	///
	/// \param ads            DUTs to add to the active DUT set.
	/// \param incremental    if true, adds the DUTs in |ads| to the active DUT set,
	///                        else sets the active DUT set to |ads|.
	inline void active_duts_enable( const std::set< DutNum >& ads, bool incremental = false )
	{
		DutNumArray adsTemp;
		for( auto&& dut : ads )
			adsTemp.Add( dut );

		::active_duts_enable( adsTemp, incremental );
	}

	/// Wrapper for an std::set< DutNum > that allows to iterator over
	/// all active DUTs using stdlib-style iteration.
	struct Ads
	{
		using container = std::set< DutNum >; ///< underlying DutNum container
		using iterator = container::const_iterator; ///< forward iterator for the underlying container
		using reverse_iterator = container::const_reverse_iterator; ///< reverse iterator for the underlying container
		
		/// Initializes *this with the current active DUT set.
		Ads() : update( true )
		{
			active_duts_get( duts );
		}

		/// Initializes *this with the specified DUT set.
		Ads( const std::set< DutNum >& duts ) : update( true )
		{
			this->duts = duts;
			apps::active_duts_enable( duts, false );
		}

		/// Returns std::begin() for the underlying std::set< DutNum >
		iterator begin() const
		{
			return std::begin( duts );
		}
		
		/// Returns std::rbegin() for the underlying std::set< DutNum >
		reverse_iterator rbegin() const
		{
			return std::rbegin( duts );
		}
		
		/// Returns std::end() for the underlying std::set< DutNum >
		iterator end() const
		{
			return std::end( duts );
		}
		
		/// Returns std::rend() for the underlying std::set< DutNum >
		reverse_iterator rend() const
		{
			return std::rend( duts );
		}

		/// Removes the specified DUT from the ADS.
		///
		/// \param dut    DUT
		void remove( DutNum dut )
		{
			duts.erase( dut );
			active_duts_disable( dut, TRUE );
			update = true;
		}

		/// Adds the specified DUT to the ADS
		///
		/// \param dut    DUT
		void add( DutNum dut )
		{
			duts.insert( dut );
			active_duts_enable( dut, TRUE );
			update = true;
		}

		/// Returns true if the ADS is empty
		bool empty() const
		{
			return duts.empty();
		}

		/// Returns a copy of the set of DutNum. This allows
		/// for compatibility with apps::active_duts_{enable,disable}().
		operator const std::set< DutNum >&() const
		{
			return duts;
		}

		/// Returns the Active DUT Set in a bitmask format.
		operator DWORD() const
		{
			_update_mask();
			return static_cast< DWORD >( mask );
		}

		/// Returns the Active DUT Set in a bitmask format.
		operator UINT64() const
		{
			_update_mask();
			return mask;
		}

	private:
		mutable bool update;
		mutable UINT64 mask;
		std::set< DutNum > duts;

		// Converts the ADS into a bitmask.
		UINT64 _compute_mask() const
		{
			UINT64 ret = 0;
			for( auto& it : duts )
				ret |= static_cast< UINT64 >( 1 ) << it;
			return ret;
		}

		// Update mask
		inline void _update_mask() const
		{
			if( update )
			{
				mask = _compute_mask();
				update = false;
			}
		}
	};

	/// Returns the set of all DUTs with a specified result.
	///
	/// \param duts     DUTs whose result is |val|
	/// \param val      result of interest
	inline void results_get( std::set< DutNum >& duts, int val )
	{
		Ads ads;
		for( auto dut : ads )
		{
			if( result_get( dut ) == val )
				duts.insert( dut );
		}
	}

	/// Adds DUT set |b| to DUT set |a|.
	///
	/// \param a    set of DUTs that is modified
	/// \param b    set of DUTs that will be added
	inline void add_duts( std::set< DutNum >& a, const std::set< DutNum >& b )
	{
		for( auto& it : b )
			a.insert( it );
	}

	/// Parks all DUTs with a specific result value.
	///
	/// \param val    result value for which DUTs will be parked, i.e. removed
	///                from the active DUT set.
	inline void park_dut_result( int val )
	{
		DutNumArray duts;
		results_get( &duts, val );

		// Avoid no-op in case of bug/overhead in ::active_duts_disable()
		if( !duts.IsEmpty() )
			active_duts_disable( duts, TRUE );
	}

	/// Parks all DUTs with a specific result value that are in |ads|.
	///
	/// \param ads    active DUT set that will be modified.
	/// \param val    result value for which DUTs will be parked, i.e. removed
	///                from the active DUT set.
	inline void park_dut_result( Ads& ads, int val )
	{
		DutNumArray duts;
		results_get( &duts, val );
		for( auto i = 0; i < duts.GetCount(); ++i )
		{
			ads.remove( duts[ i ] );
		}
	}

	/// For all DUTs with results |oldVal|, changes the result to |newVal|.
	///
	/// \param oldVal     result value that will be changed
	/// \param newVal     result value that will replace |oldVal|
	inline void change_dut_result( int oldVal, int newVal )
	{
		DutNumArray duts;
		results_get( &duts, oldVal );
		results_set( duts, newVal );
	}

	/// Returns whether the Active DUT Set is empty
	inline bool is_ads_empty()
	{
		return Ads{}.empty();
	}

	/// Reverse a DUT map. General case.
	///
	/// \param in     DUT map that will be reversed.
	template< typename T >
	std::map< T, std::set< DutNum > > reverse_dut_map( const std::map< DutNum, T >& in )
	{
		// Iterate through all mapped values and add associated DutNum to |out| (for that mapped value).
		std::map< T, std::set< DutNum > > out;
		for( auto& it : in )
		{
			auto dut = it.first;
			auto& val = it.second;

			// If a DutNum set already exists in |out| for |val|, add it to the set,
			// otherwise create a new set containing |dut|.
			auto jt = out.find( val );
			if( jt == std::end( out ) )
				out.emplace( val, std::set< DutNum >{ dut } );
			else
				jt->second.insert( dut );
		}

		return out;
	}

	/// Reverses a DUT map whose mapped type is a std::vector<> type.
	///
	/// \param in     DUT map that will be reversed.
	template< typename T >
	std::map< T, std::set< DutNum > > reverse_dut_map( const std::map< DutNum, std::vector< T > >& in )
	{
		// Iterate through all mapped values and add associated DutNum to |out| (for that mapped value).
		std::map< T, std::set< DutNum > > out;
		for( auto& it : in )
		{
			auto dut = it.first;
			auto& cont = it.second;

			// Iterate through all the values in |cont|
			for( auto& val : cont )
			{
				// If a DutNum set already exists in |out| for |val|, add it to the set,
				// otherwise create a new set containing |dut|.
				auto kt = out.find( val );
				if( kt == std::end( out ) )
				{
					std::set< DutNum > temp = { dut };
					out.emplace( val, std::move( temp ) );
				}
				else
					kt->second.insert( dut );
			}
		}

		return out;
	}
}