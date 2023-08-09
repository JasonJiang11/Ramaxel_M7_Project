#pragma once
#include "TestProgApp/public.h"
#include "MacroUtils.h"
#include <vector>

// Dut looper
enum class DutLoopType { Hard, Soft, SWOnly };

// UINT64 variable name used in the DUT_LOOP macro
// This name is used to store active DUTs
#define DL_ADS_NAME CAT(dl_ads, __LINE__)

const auto lam =
[]( const UINT64& ads, DutNum& dut, const DutLoopType& LoopType )
{
	static BOOL write_hw;
	static bool is_first_iteration = true;
	if( is_first_iteration == true && LoopType == DutLoopType::Soft )
	{
		write_hw = dutset_change_notify_write_hw( FALSE );
		is_first_iteration = false;
	}

	if( ads >> dut & 1 )
	{
		if( LoopType != DutLoopType::SWOnly )
		{
			active_duts_enable( dut );
		}
		return true;
	}
	else
	{
		++dut;
		while( ads >> dut != 0 )
		{
			if( ads >> dut & 1 )
			{
				if( LoopType != DutLoopType::SWOnly )
				{
					active_duts_enable( dut );
				}
				return true;
			}
			++dut;
		}
		if( LoopType != DutLoopType::SWOnly )
		{
			if( LoopType == DutLoopType::Soft )
			{
				dutset_change_notify_write_hw( write_hw );
			}
			active_duts_enable( ads );
		}
		return false;
	}
};


#define DUT_LOOP( LoopType, dut )   \
UINT64 DL_ADS_NAME;                 \
active_duts_get( &DL_ADS_NAME );    \
for( DutNum dut = t_dut1; (DL_ADS_NAME != 0) && lam( DL_ADS_NAME, dut, LoopType ); ++dut )


// Dut group looper
enum class DutGroupLoopType { Hard, Soft };

inline bool DutGroupLoopRestore( const UINT64& ads, DutGroupLoopType LoopType )
{
	if( LoopType == DutGroupLoopType::Soft )
		dutset_change_notify_write_hw( TRUE );
	active_duts_enable( ads ); // Restore back
	return false;
}

using DutmaskIteratorType = std::vector<UINT64>::iterator;
const auto lam_dg =
[]( const UINT64& ads,
	DutmaskIteratorType& it,
	const DutmaskIteratorType& end,
	DutGroupLoopType LoopType )->bool
{
	static bool is_first_iteration = true;

	// end of dutmask
	if( it == end )
		return DutGroupLoopRestore( ads, LoopType );

	if( is_first_iteration && LoopType == DutGroupLoopType::Soft )
	{
		dutset_change_notify_write_hw( FALSE );
		is_first_iteration = false;
	}

	if( (ads & *it) != 0 )
	{
		active_duts_enable( (ads & *it) );
		return true;
	}
	else // if all duts specified in dutmask are disabled
	{
		++it;
		if( it != end )
		{
			while( (ads & *it) != 0 )
			{
				if( ads & *it )
				{
					active_duts_enable( ads & *it );
					return true;
				}
				++it;
				if( it == end ) break;
			}
		}

		is_first_iteration = true;
		return DutGroupLoopRestore( ads, LoopType );
	}
};

// UINT64 variable name used in the DUT_GROUP_LOOP macro
// This name is used to store active DUTs
#define DGL_ADS_NAME CONCAT(dgl_ads, __LINE__)

// Iterate each dutmask in |vec| and 
// enable a group of DUTs specified in the dutmask
// For example, 
// std::vector< UINT64 > vec{ 0x55, 0xAA };
// DUT_GROUP_LOOP( DutGroupLoopType::Hard, vec )
// {
//   // First Iteration:
//   // Enable t_dut1, t_dut3, t_dut5, and t_dut7 (0x55)
//   // Second Iteration:
//   // Enable t_dut2, t_dut4, t_dut6, and t_dut8 (0xAA)
// }
#define DUT_GROUP_LOOP( LoopType, vec ) \
UINT64 DGL_ADS_NAME;                    \
active_duts_get( &DGL_ADS_NAME );       \
for( auto it = std::begin(vec); lam_dg( DGL_ADS_NAME, it, std::end(vec), LoopType ); ++it )