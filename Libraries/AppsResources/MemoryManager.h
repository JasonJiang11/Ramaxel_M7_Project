#pragma once
#include <list>

namespace apps
{
	/// To manage a virtual pool of memory.
	///
	/// \param SectorNumBytes     number of bytes that comprise a sector
	///                            in the memory pool.
	/// \param SizeNumBytes       number of bytes available in the memory pool.
	/// \param BaseAddress        smallest valid address in the memory pool.
	///
	struct MemoryManager
	{
		const size_t N, _size, _baseAddr;
		MemoryManager( size_t SectorNumBytes, size_t SizeNumBytes, size_t BaseAddress = 0 ) :
			N( SectorNumBytes ),
			_size( SizeNumBytes ),
			_baseAddr( BaseAddress )
		{
			purge();
		}

		/// Returns the number of bytes in the memory pool.
		size_t size()
		{
			return _size;
		}

		/// Returns the smallest valid address in the memory pool.
		size_t base()
		{
			return _baseAddr;
		}

		/// Frees all memory.
		///
		void purge()
		{
			// Clear list of free blocks
			_free_blocks.clear();
		
			// Calculate the block of free memory at the end
			// of the memory pool, and add to the list.  Round
			// address up to the next N-byte aligned address.
			BlockInfo bi;
			bi.addr = round_up_to_valid_sector_addr( _baseAddr );
			bi.len = size() - bi.addr;
			_free_blocks.push_back( bi );
		
			// Append empty block at the end to symbolize end of the memory.
			_free_blocks.push_back( BlockInfo( _size, 0 ) );
		}

		/// Allocates a region of memory.
		///
		/// Finds a byte address in that can accomodate a chunk of
		/// `num_bytes` bytes, and marks that region of memory as being
		/// allocated. The returned address is always N-byte aligned.
		/// If no region in memory can accomodate a chunk of the specified length,
		/// returns `true`; else, returns `false`.
		///
		/// \param numBytes     length, in bytes, of the UCD requested to store in memory.
		/// \param addr         start byte address of allocated region of memory. Only
		///						  valid if return value is `false`.
		bool allocate( size_t numBytes, size_t& addr )
		{
			// If request block size is 0, return address
			// of current block, and print warning message.
			if( numBytes == 0 )
			{
				warning( "MemoryManager::allocate() -- Requested block size of 0 bytes!" );
				addr = 0;
				return false;
			}
		
			// Find a suitable free block of memory
			auto it = find_suitable_free_block( numBytes );
			if( it == _free_blocks.end() )
			{
				// No available space in SRAM
				return true;
			}
			else
			{
				// Record block before erasing it.
				BlockInfo chosen_block = *it;
		
				// Remove the block from list.
				it = _free_blocks.erase( it );
		
				// Update the list if only part of a 'free' block was allocated.
				auto allocated_len = numBytes;
				if( chosen_block.len != numBytes )
				{
					BlockInfo bi;
					bi.addr = round_up_to_valid_sector_addr( chosen_block.addr + numBytes );
		
					// Compute effective allocated length
					allocated_len = bi.addr - chosen_block.addr;
		
					// Compute number of free bytes remaining in block.
					bi.len = chosen_block.len - allocated_len;
		
					// Do not add block to list if effective allocated
					// length equals length of the chosen_block.
					if( bi.len != 0 )
						_free_blocks.insert( it, bi );
				}
		
				// Return the address of the block of memory
				// that was allocated for the UCD.
				addr = chosen_block.addr;
				return false;
			}
		}

		/// Deallocates a region of memory.
		///
		/// The provided address should always be N-byte aligned and correspond to
		/// the beginning of a region of memory that was previously allocated. If it
		/// isn't, returns `true`.
		///
		/// If the number of bytes specified is not an integer multiple of N, it is
		/// rounded up to the next integer multiple of N. As a consequence of this,
		/// the information provided to and received from `allocate()` for a given
		/// chunk of memory can also be used with `deallocate()`.
		///
		/// Returns `false` if the deallocation was successful.
		///
		/// \code
		/// MemoryManager< 128 > mm; // memory manager for 128-byte sectors
		///
		/// // Allocate a 32-byte chunk.
		/// DWORD num_bytes = 32; // Not an integer multiple of 128!
		/// DWORD address;
		/// ASSERT( mm.allocate( num_bytes, address ) == false );
		///
		/// // Under the hood, MemoryManager actually allocated a 128-byte block of
		/// // memory at address |address|, but we can simply re-use |num_bytes|
		/// // to deallocate the chunk via deallocate().
		/// ASSERT( mm.deallocate( num_bytes, address ) == false );
		/// \endcode
		///
		/// \param numBytes    size of the previously-allocated chunk of memory (in units of bytes)
		/// \param addr         byte address of the previously allocated to the chunk of memory
		bool deallocate( size_t numBytes, size_t addr )
		{
			// Verify that address is 128-byte aligned
			if( addr % 128 != 0 )
			{
				warning( "MemoryManager::deallocate() -- address provided was not %d-byte aligned.", N );
				return true;
			}
		
			// Verify that described region of memory is within SRAM bounds.
			if( addr + numBytes > _size )
			{
				warning( "MemoryManager::deallocate() -- region specified exceeds the memory pool's address space." );
				return true;
			}
		
			// Set |numBytes| to its effective allocation size.
			numBytes = round_up_to_the_next_N_integer_multiple( numBytes );
		
			// Search for allocated region of memory and deallocate if found.
			auto&& firstFreeBlock = _free_blocks.front();
			bool first = true;
			size_t busyAddr, busyNumBytes;
			for( auto it = std::begin( _free_blocks ); it != std::end( _free_blocks ); ++it )
			{
				auto&& nextFreeAddr = it->addr;
				auto&& nextFreeNumBytes = it->len;
		
				// Compute length of the busy block that precedes this
				if( first )
				{
					busyAddr = _baseAddr;
					busyNumBytes = firstFreeBlock.addr - _baseAddr;
					first = false;
				}
				else
				{
					// Compute the length of the previous busy block
					busyNumBytes = nextFreeAddr - busyAddr;
				}
		
				// Check if user's UCD could be in this busy block
				if( addr >= busyAddr	&& addr < nextFreeAddr )
				{
					// Check if user's UCD would fit in this block
					if( addr + numBytes <= nextFreeAddr )
					{
						// We found the allocated region of memory!
						// Deallocate it.
						
						// Case 1: UCD spans the entire busy block
						if( addr == busyAddr && numBytes == busyNumBytes )
						{
							// Join the previous block with the current one. If we are at 
							// the start of the block list, however, we simply need to modify
							// the current block.
							if( it == std::begin( _free_blocks ) )
							{
								it->addr = busyAddr;
								it->len += busyNumBytes;
							}
							else
							{
								// Modify the previous block, and delete current block, 
								(--it)->len += busyNumBytes + nextFreeNumBytes;
								_free_blocks.erase( ++it );
							}
							
							return false;
						}
						// Case 2: UCD is aligned with the start of the busy block
						else if( addr == busyAddr )
						{
							// Increase size of previous free block. If we are at the start of the block
							// list, however, we must create a new block.
							if( it == std::begin( _free_blocks ) )
							{
								BlockInfo temp( addr, numBytes );
								_free_blocks.push_front( temp );
							}
							else
							{
								--it;
								it->len += numBytes;
							}
							
							return false;
						}
						// Case 3: UCD is aligned with the end of the busy block
						else if( addr + numBytes == nextFreeAddr )
						{
							// Move the current's block address *back* by the size of the UCD
							// and adjust its size accordingly;
							it->addr -= numBytes;
							it->len += numBytes;
							
							return false;
						}
						// Case 4: UCD floats in the middle of the busy block
						else
						{
							// In all cases, we add a free block before the current one.
							BlockInfo temp( addr, numBytes );
							_free_blocks.insert( it, temp );
							
							return false;
						}
					}
				}
		
				// Get the address of the next busy block
				busyAddr = nextFreeAddr + nextFreeNumBytes; 
			}
		
			// Return now if the allocated region of memory was not found
			warning( "MemoryManager::deallocate() -- region specified was not previously allocated." );
			return true;
		}

		/// Verifies whether there is enough free space in the memory pool to allocate the requested number of bytes.
		///
		/// Returns true if there is enough free space in the memoru poll to allocate
		/// the requested number of bytes, else returns false.
		///
		/// \param numBytes	number of bytes to store.
		bool query( size_t numBytes )
		{
			return find_suitable_free_block( numBytes ) != _free_blocks.end();
		}

	private:

		size_t round_up_to_the_next_N_integer_multiple( size_t num )
		{
			return ( (num + N - 1) / N ) * N;
		}
		size_t round_up_to_valid_sector_addr( size_t addr )
		{
			return round_up_to_the_next_N_integer_multiple( addr );
		}
		
		// struct to represent a block of memory
		struct BlockInfo
		{
			size_t addr; // byte address
			size_t len; // byte length
		
			BlockInfo():
				addr( 0 ),
				len( 0 )
			{
			}
			BlockInfo( size_t _addr, size_t _len ) :
				addr( _addr ),
				len( _len )
			{
			}
		};
		
		// list to keep track of free blocks of memory.
		std::list< BlockInfo > _free_blocks;
		using list_it = std::list< BlockInfo >::iterator;

		// Returns the first BlockInfo in the |_free_blocks| list that can accomodate |num_bytes|
		// bytes of data. If none can be found, std::end( _free_blocks ) is returned.
		list_it find_suitable_free_block( size_t num_bytes )
		{
			// Go through list of free blocks until a suitable free
			// block of memory is found.
			auto it = _free_blocks.begin();
			while( it != _free_blocks.end() )
			{
				// Skip to next free block if length too small
				if( num_bytes > it->len )
				{
					++it;
					continue;
				}
				break;
			}
			return it;
		}
	};
}