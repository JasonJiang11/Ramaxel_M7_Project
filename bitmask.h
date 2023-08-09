#pragma once

#include "TestProgApp/public.h"

namespace apps
{
	namespace bitmask
	{
		//////////////////////////////////////////////////////////////////

		// Generates a bitmask with a single 1 bit 

		//////////////////////////////////////////////////////////////////

		// MaskType is intended to be any integral number (e.g. int, DWORD)
		// but can technically be any struct with operator<< overloaded.
		template< typename MaskType, size_t N >
		struct SingleBitShiftedLeftBy
		{
			enum { value = SingleBitShiftedLeftBy< MaskType, N - 1 >::value << 1 };
		};

		template< typename MaskType >
		struct SingleBitShiftedLeftBy< MaskType, 0 >
		{
			enum { value = static_cast<MaskType>(1) };
		};

		//////////////////////////////////////////////////////////////////

		// Shifts left by N bits

		//////////////////////////////////////////////////////////////////

		template< typename MaskType, MaskType Mask, size_t N >
		struct ShiftLeftBy
		{
			enum : MaskType { value = ShiftLeftBy< MaskType, Mask, N - 1 >::value << 1 };
		};

		template< typename MaskType, MaskType Mask >
		struct ShiftLeftBy< MaskType, Mask, 0 >
		{
			enum : MaskType { value = Mask };
		};

		//////////////////////////////////////////////////////////////////////////

		// Generates a bitmask with the lower N bits set to 1

		//////////////////////////////////////////////////////////////////////////

		template< typename MaskType, size_t EndBit, size_t StartBit >
		struct BitMask
		{
			enum : MaskType { value = ShiftLeftBy< MaskType, LowerBitMask< MaskType, EndBit - StartBit + 1 >::value, StartBit >::value };
		};


		//////////////////////////////////////////////////////////////////////////

		// Generates a bitmask with the lower N bits set to 1

		//////////////////////////////////////////////////////////////////////////

		template< typename MaskType, size_t N >
		struct LowerBitMask
		{
			enum : MaskType { value = SingleBitShiftedLeftBy< MaskType, N - 1 >::value | LowerBitMask< MaskType, N - 1 >::value };
		};

		template< typename MaskType >
		struct LowerBitMask< MaskType, 1 >
		{
			enum : MaskType { value = SingleBitShiftedLeftBy< MaskType, 0 >::value };
		};

		//////////////////////////////////////////////////////////////////////////

		// Returns the highest bit set in |_Word|

		//////////////////////////////////////////////////////////////////////////

		namespace detail
		{
			// Implementation type for HighestBitSet
			template< typename _WordType, _WordType _Word, size_t N >
			struct _HighestBitSet
			{
				enum {
					value =
					std::conditional<
					(_Word & (1ULL << N)) != 0,              // If bit N is a 1
					std::integral_constant< size_t, N >,         // set value to N, else
					_HighestBitSet< _WordType, _Word, N - 1 >    // check bit N-1.
					>::type::value
				};
			};

			// If we have reached the _NumBits = 0, declare the hightest bit position as 0.
			template< typename _WordType, _WordType _Word >
			struct _HighestBitSet< _WordType, _Word, 0 >
			{
				enum { value = static_cast<size_t>(0) };
			};
		}

		template< typename _WordType, _WordType _Word >
		struct HighestBitSet
		{
			enum { value = detail::_HighestBitSet< _WordType, _Word, (sizeof( _WordType ) * 8) - 1 >::value };
		};

		//////////////////////////////////////////////////////////////////////////////////////////

		// Returns the number of bits required to represent |_NumPermutations| different values.

		//////////////////////////////////////////////////////////////////////////////////////////

		template< unsigned long long _NumPermutations >
		struct NumberOfBitsRequiredToRepresent
		{
			// Convert _NumPermutations to a zero-based value before finding its highest
			// bit position that is a 1. Reminder: an N-bit word can hold 2^N different values
			static const auto N = HighestBitSet< size_t, _NumPermutations - 1 >::value + 1;

			enum {
				value =
				std::conditional<
				(1 << N) >= _NumPermutations,
				std::integral_constant< size_t, N     >,
				std::integral_constant< size_t, N + 1 >
				>::type::value
			};
		};
	}
}

