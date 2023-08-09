#include "stdafx.h"
#include <exception>
#include <vector>
#include <map>
#include <functional>

#define DLL_EXPORT

#include "PinAssignments.h"

// Modifiable function object for initializing a new PinAssignments table.
std::function< void ( PinAssignments* ) > _initModifiedPinAssignments;
std::function< void ( PinAssignments* ) >& getPinAssignmentsFunctor()
{
	return _initModifiedPinAssignments;
}

// Declare all the PinAssignments resource-crap required by the Nextest
// runtime in order to create a new PinAssignments table.
PIN_ASSIGNMENTS( __pModifiedPinAssignments__ )
 {
    SITES_PER_CONTROLLER(1);
	getPinAssignmentsFunctor()( obj );
 }

#if 0 // Use this to create a PinAssignments table dynamically

// Creates an empty `PinAssignments` table.
static PinAssignments* create( const std::string& name )
{
	_resource_create( local::initfunc, name.c_str(), __FILE__, __LINE__ );
}

#endif

namespace detail
{
	// This is the maximum number of DUTs that PinAssignmentsCreator
	// supports when adding a new row to a given PinAssignments table.
	// This number doesn't need to be larger than 64 since the largest
	// ASSIGN_xxDUTS macro is for 64 DUTs.
	static const unsigned int maxNumDuts = 64;

	template< unsigned int N >
	struct PinAssignmentsCreator
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
				PinAssignmentsCreator< N - 1 >{}( numDuts, permutation, hdTesterPinMap, pPinAssignments, pDutPin, hdPins..., hdPinNext );
			}
		}
	};

	// End condition
	template<>
	struct PinAssignmentsCreator< 1 >
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
}

// Helper for adding a DutPin to a PinAssignments table, using a given permutation vector.
struct PinAssignmentsCreator
{
	void operator()( unsigned int numDuts, const std::vector< DutNum >& permutation, const std::map< DutNum, HDTesterPin >& hdTesterPinMap, 
		PinAssignments* pPinAssignments, DutPin* pDutPin )
	{
		// Check if valid number of columns
		if( numDuts <= detail::maxNumDuts )
			throw std::exception( "A maximum of 128 DUTs is supported for this PinAssignments table!" );

		detail::PinAssignmentsCreator< detail::maxNumDuts >{}( numDuts, permutation, hdTesterPinMap, pPinAssignments, pDutPin );
	}
};

// Swap the columns of the provided PinAssignments table.
//
// Returns true if the PinAssignments columns were swapped and the PinAssignments table
// was re-initialized. Else returns false.
static bool swapPinAssignmentsColumns( const std::vector< DutNum > permutation,
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
	_initModifiedPinAssignments = [ &pinsPerDut, numDuts, permutation ] ( PinAssignments* pPinAssignments )
	{
		// Iterate over all DutPins
		for( auto it : pinsPerDut )
		{
			DutPin* pDutPin = it.first;
			const auto& hdTesterPinMap = it.second;

			// Get the HDTesterPin associated with each column (DutNum)
			PinAssignmentsCreator{}( static_cast< unsigned int >( numDuts ), permutation, hdTesterPinMap, pPinAssignments, pDutPin );
		}
	};

	// Initialize the modified PinAssignments.
	apps::pin_assignments::select( name.c_str() );
	
	// After re-initialiaing PinAssignments, we must also re-initialize a bunch of other resources...
	// so just to be safe, we re-initialize all resources.
	BOOL ok = resource_initialize();
	if( !ok ) throw std::exception( vFormat( "swapPinAssignmentsColumns() -- Unable to re-initialize all Resources" ) );

	return true;
}

bool apps::pin_assignments::swap_columns( const std::vector< DutNum >& permutation )
{
	// Extract currently selected PinAssignments
	std::map< DutPin*, std::map< DutNum, HDTesterPin > > pinsPerDut;
	capture( pinsPerDut );

	// Swap the columns
	return swapPinAssignmentsColumns( permutation, pinsPerDut, "__pModifiedPinAssignments__" );
}
void apps::pin_assignments::save_restore( bool save )
{
	static std::string originalPinAssignmentsName;

	if( save )
		originalPinAssignmentsName = resource_name( get() );

	else if( originalPinAssignmentsName.empty() )
		throw std::exception( "apps::pin_assignments::save_restore() -- No PinAssignments was previously stored" );	

	else
	{
		if( originalPinAssignmentsName != resource_name( get() ) )
			select( originalPinAssignmentsName );
	}
}

#if 0 // For testing

//////////////////////////////////////////////////////////////////////////////////////

// Helpers for iterating through permutations of ordered sampling w/o replacement

//////////////////////////////////////////////////////////////////////////////////////

// Compute the factorial of a given number
static size_t factorial( size_t n )
{
	if( n == 0 || n == 1 )
		return 1;
	else
		return n * factorial( n - 1 );
}

// Permutes the input vector, |in|, based on the provided |permutationNumber|, and returns
// the permutation via the output vector, |out|. Given an input vector size of N, if 
// |permutationNumber| is strictly smaller than N! (N factorial), returns true, else returns false.
template< typename T >
static bool permute( size_t permutationNumber, const std::vector< T >& in, std::vector< T >& out )
{
	const size_t n = in.size();

	// Avoid no-op
	if( permutationNumber >= factorial( n ) ) return false;

	// Iterate through each position
	for( size_t pos = 0; pos < n; ++pos )
	{
		// Choose object for this position based on the user-provided |permutationNumber|.
		out.emplace_back( in[ ( pos + permutationNumber ) % n ] );
	}
	return true;
}

// Assuming the number of DUTs declared in the PinAssignments table is N, this
// function returns true if |permutationNumber| is greater than or equal to N.
// Else returns false.
static bool rotatePinAssignmentsColumns( size_t permutationNumber )
{
	// Avoid no-op
	if( permutationNumber == 0 ) return true;

	// Extract currently selected PinAssignments
	std::map< DutPin*, std::map< DutNum, HDTesterPin > > pinsPerDut;
	capturePinAssignments( pinsPerDut );

	// Compute the number of possible permutations
	size_t numDuts = pinsPerDut.begin()->second.size();
	auto numPermutations = numDuts;
	if( permutationNumber >= numPermutations ) return false;

	// Generate permutation vector to rotate by one column
	std::vector< DutNum > permutation;
	size_t rotateBy = 1;
	for( size_t i = 0; i < numDuts; ++i )
	{
		permutation.push_back( static_cast< DutNum >( ( i + rotateBy ) % numDuts ) );
	}

	// Swap columns
	swapPinAssignmentsColumns( permutation, pinsPerDut );
	return true;
}

#endif