#pragma once
#include <set>

namespace apps
{
	// Defines a Gallois-style Linear Feedback Shift Register (LFSR) (as implemented in MagV)
	template< typename T >
	struct lfsr
	{
		using word_type = T;

		/// Defines the LFSR's with a polynomial.
		///
		/// The maximum polynomial order supported is 64. If any exponent
		/// specified is greater than 64, it will be coerced to 64.
		///
		/// \param polynomial    exponents of the polynomial describing the LFSR
		///                       excluding the zero-th order term (i.e. exponent 0).
		///                       If exponent 0 is specified, it will be ignored.
		/// \param seed          seed. If zero, will be coerced to 1.
		///
		lfsr( const std::set< size_t >& polynomial, word_type seed )
		{
			// Remove the x^0 = 1 term in case it was added
			_exponents.erase( 0 );
			
			// Verify that the polynomial is not empty
			if( polynomial.empty() )
			{
				throw std::exception( "Empty polynomial" );
			}
	
			for( auto& it : polynomial )
			{
				if( it > _maxExponentCap )
					_exponents.emplace( _maxExponentCap );
				else
					_exponents.emplace( it );
			}
	
			// Cache the highest order/exponent of the polynomial
			_highestExponent = *std::max_element( std::begin( polynomial ), std::end( polynomial ) );
	
			// Set the overflow bit if the polynomial's order is larger
			// than the word length.
			_overflow = _highestExponent > ( sizeof( word_type ) * 8 );

			// Also cache the mask used to zero-out higher-order bits
			// from the state variable holding the register value.
			_mask = ( 1 << _highestExponent ) - 1;
	
			// Sanitize seed. Make sure that |_mask| is initialized
			// before this step since it is used by seed().
			this->seed( seed );
	
			// Reset the LFSR
			reset();
		}
	
		/// Returns the current state of the LFSR.
		///
		word_type state() const
		{
			return _state;
		}
	
		/// Advances the state of the LFSR.
		///
		void advance()
		{
			// From a schematics point-of-view, this is a NOR of all
			// the bits in the register. By XOR'ing the output of this gate
			// with the MSB, we don't get stuck if we reach the value of 0.
			word_type isZero = _state == 0;
	
			// The feedback bit is comprised of the XOR of the MSB with
			// the output of the NOR gate (see above).
			word_type feedbackBit = isZero ^ ( _state >> ( _highestExponent - 1 ) );
	
			// Apply all the internal feedback XORs
			auto origState = _state;
			auto it = std::rbegin( _exponents );
			++it;
			for( ; it != std::rend( _exponents ); ++it )
			{
				auto&& exp = *it;
	
				// Compute XOR of feedback bit and bit i-1, and assign to bit i-1.
				_state ^= ( feedbackBit << ( exp - 1 ) );
			}
	
			// Shift the register left by one, and set the LSB to the feedback bit.
			_state = ( _state << 1 ) | feedbackBit;
	
			// Zero-out the bits that are out-of-bounds.
			_state &= _mask;
		}
		
		/// Returns the current state of the LFSR and then advances it.
		///
		size_t operator()()
		{
			auto ret = state();
			advance();
			return ret;
		}
	
		/// Resets the LFSR to its seed.
		///
		void reset()
		{
			_state = _seed;
		}
	
		/// Sets the seed.
		///
		/// \param seed   seed. If zero, will be coerced to 1. Any bits
		///                outside the width of the LFSR will be masked out.
		///
		void seed( word_type val )
		{
			if( val == 0 )
				_seed = 1;
			else
				_seed = val;

			// Mask out extra bits
			_seed &= _mask;
		}
	
		/// Returns the seed.
		///
		size_t seed() const
		{
			return _seed;
		}
	
		/// Returns the order of the polynomial.
		///
		size_t order() const
		{
			return _highestExponent;
		}

		/// Returns true if the order of the polynomial is too large for the word length.
		///
		bool overflow() const
		{
			return _overflow;
		}

		/// Returns the maximum polynomial order supported.
		///
		static size_t max_order()
		{
			return _maxExponentCap;
		}
	
	
	private:
		static const size_t _maxExponentCap = sizeof( T ) * 8;
		size_t _highestExponent, _mask;
		word_type _state, _seed;
		std::set< size_t > _exponents;
		bool _overflow;
	};

	/// Splits a widget-delimited string and stores each field in |out|
	///
	/// \param str     string to be split
	/// \param delim   delimited used in string
	/// \param out     delimited fields extracted from str
	///
	inline size_t split( const std::string& str, const std::string& delim, std::vector< std::string >& out )
	{
	  out.clear();
	  if( str.empty() )
	    return 0;
	
	  auto delimSize = delim.size();
	  size_t pos = 0;
	  size_t prevPos = 0;
	  while( 1 )
	  {
	    // Find start of next delimiter
	    pos = str.find( delim, prevPos );
	    if( pos == std::string::npos )
	    {
	      // No other delimiter found, copy remainder of string
	      auto sub = str.substr( prevPos );
	      if( !sub.empty() )
	        out.push_back( sub );
	      break;
	    }
	    else
	    {
	      auto sub = str.substr( prevPos, pos - prevPos );
	      if( !sub.empty() )
	        out.push_back( sub );
	    }
	
	    // Save start of next field
	    prevPos = pos + delimSize;
	  }
	  return out.size();
	}
}