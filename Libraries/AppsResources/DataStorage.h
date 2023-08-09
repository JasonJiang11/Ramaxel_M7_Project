#pragma once
#include <map>
#include <vector>
#include <typeinfo>

namespace apps
{
	/// To store and retrieve response structs from associative containers.
	struct DataStore
	{
		static const DWORD INVALID_KEY = 0x0; ///< key value defined as being invalid.

		/// stores a copy of the key-value pair, `key`-`obj`, for every DUT defined in `dutmask`.
		/// Returns `true`, if all copies of `obj` were stored successfully. Else, returns `false`.
		/// If `key` is equal to `this->INVALID_KEY`, `obj` will not be stored and the function will
		/// return `false`.
		///
		/// \param dutmask	an 8-bit mask specifying the DUTs for which key-value pair should be stored.
		/// \param key		key associated with `obj`.
		/// \param obj		object to be stored in memory.
		template< typename T >
		bool store( DWORD dutmask, DWORD key, const T& obj )
		{
			// Get vector and map
			auto&& _vec = _get_vector< T >( );
			auto&& _map = _get_map< T >( );
			return _store_object( dutmask, key, obj, _vec, _map );
		}

		/// stores a copy of the key-value pair, `key`-`obj`, for only one DUT, `DutNum`.
		/// Returns `true`, if all copies of `obj` were stored successfully. Else, returns `false`.
		/// If `key` is equal to `this->INVALID_KEY`, `obj` will not be stored and the function will
		/// return `false`.
		///
		/// \param dut		specifies the DUT for which key-value pair should be stored.
		/// \param key		key associated with `obj`.
		/// \param obj		object to be stored in memory.
		template< typename T >
		bool store( DutNum dut, DWORD key, const T& obj )
		{
			// Get vector and map
			auto&& _vec = _get_vector< T >( );
			auto&& _map = _get_map< T >( );
			return _store_object( dut, key, obj, _vec, _map );
		}

		/// searches for the object paired with `dut` and `key` (stored in this class),
		/// and makes `obj` point to it.  If the object is not found, assigns `obj` to
		/// `nullptr` and throws a `std::exception` as an exception.
		///
		/// \param dut			specifies the DUT for which the object (associated with `key`) is being retrieved.
		/// \param key			key associated with the object being retrieved.
		/// \param obj			pointer-reference to the retrieved object.
		/// \param context_str	string included in the thrown exception's error message in case of a missing key-value pair.
		///						This string is meant to provide *context* to the thrown error message. For example, if the
		///						`UTRD` object associated with the key 0x01, and representing a NOP OUT/NOP IN UPIU transaction
		///						is not found, a `context_str` like "NOP OUT/NOP IN UPIU" would be helpful to the reader of the
		///						thrown error message to identify the source of the error.
		template< typename T >
		void get( DutNum dut, DWORD key, T*& obj, const char *context_str = "" )
		{
			auto&& _vec = _get_vector< T >( );
			auto&& _map = _get_map< T >( );
			_get_object( dut, key, obj, _vec, _map, context_str );
		}

		/// Deletes all response structs currently stored, and
		/// resets the counters used for key generation.
		///
		/// Must be implemented by user.
		template< typename T >
		void reset( )
		{
			_get_vector< T >( ).clear( );
			_get_map< T >( ).clear( );
			_get_counter< T >( ) = 0;
		}

		/// Used to generate unique task IDs for UPB tasks.
		///
		/// \param type	specifies the type of response struct for which a key will be generated.
		template< typename T >
		DWORD new_key( )
		{
			// Find counter associated with type T
			// and increment it
			auto&& id = _get_counter< T >( );
			++id;

			// Reset counter if no longer valid
			if( id == INVALID_KEY )
				id = 1 + INVALID_KEY;

			return id;
		}

	private:

		// Vector sizes should all be 2^32 since all TaskID fields are 32 bits.
		//
		// But, because of limitations with the std::vector class, and
		// system memory, the actual vector size needs to be smaller.
		template< typename T >
		std::vector< T > _initialize_vector( )
		{
			std::vector< T > ret;
			ret.reserve( 1 << 10 );
			return ret;
		}

		// Returns the vector used to store objects of type T.
		// Serves as a type-to-vector map.
		template< typename T >
		inline std::vector< T >& _get_vector( )
		{
			static auto ret = _initialize_vector< T >( );
			return ret;
		}

		// Returns the map used to store the index of the objects associates with
		// the type T.
		//
		// Serves as a type-to-map map.
		template< typename T >
		inline std::map< UINT64, size_t >& _get_map( )
		{
			static std::map< UINT64, size_t > ret;
			return ret;
		}

		// counters for key generation. There is one counter for each type of object stored.
		template< typename T >
		inline DWORD& _get_counter( )
		{
			static DWORD ret = 0;
			return ret;
		}

		// Function for getting a pointer to a stored object associated with DUT |_dut| and key |_key|.
		template <typename object_pointer_T, typename vec_T, typename map_T>
		inline void _get_object( DutNum _dut, DWORD _key, object_pointer_T &_obj, vec_T &_vec, map_T &_map, std::string context_str )
		{
			try
			{
				// Generate unique key for DUT |_dut| and the 'generic' key |_key|, and look for the
				// associated element in |_map|. Throws exception if element is not found.
				object_pointer_T _obj_ptr = &_vec.at( _map.at( _generate_key( _dut, _key ) ) );

				// object found!
				_obj = _obj_ptr;
			}
			catch( const std::out_of_range )
			{
				// object not found. Exception was thrown.
				_obj = nullptr;
				auto what = apps::datalog::format(
					"<COLOR=RED>"
					"<b>(DUT_%02d) ERROR : </b> "
					"DataStore::get() -- %s object with key 0x%08X [ %s ] not found!"
					"</COLOR>",
					_dut + 1,
					typeid( std::remove_pointer< object_pointer_T >::type ).name( ),
					_key,
					context_str.length( ) == 0 ? "<No Context Provided>" : apps::datalog::format( "Context : %s", context_str )
					);
				throw std::exception( what.c_str() );
			}
		}

		// function for generating a unique 64-bit key based on |dut| and |key|.
		inline UINT64 _generate_key( DutNum dut, DWORD key )
		{
			UINT64 _dut = dut;
			return ( _dut << 32 ) | key;
		}

		// Calls _store_object(DutNum, ...) for each DUT defined in |dutmask|.  
		// The return value of each function call is accumulated, then returned.
		template <typename storage_T, typename vec_T, typename map_T>
		inline bool _store_object( DWORD dutmask, DWORD _key, const storage_T &_obj, vec_T &_vec, map_T &_map )
		{
			// Store a copy of |_obj| for each DUT in the dutmask.
			bool _ret = true;
			for( int i = 0; i < 32; i++ )
			{
				if( ( 0x1 << i ) & dutmask )
				{
					_ret &= _store_object( static_cast< DutNum >( i ), _key, _obj, _vec, _map );
				}
			}
			return _ret;
		}

		// generic function for creating a unique key based on the values
		// of |_dut| and |_key|. Then storing |_obj| in |_vec| using this unique key.
		template <typename storage_T, typename vec_T, typename map_T>
		inline bool _store_object( DutNum _dut, DWORD _key, const storage_T &_obj, vec_T &_vec, map_T &_map )
		{
			if( _key == INVALID_KEY )
				return false;

			// Store |_obj_ptr| in |_map|, using a unique key. Returns TRUE
			// if the index was stored successfully in |_map|. Returns
			// FALSE if an equivalent key already existed in |_map|. For the
			// latter case, the previously mapped value associated with the key
			// will be overwritten by |_obj_ptr|.
			map_T::iterator it = _map.find( _generate_key( _dut, _key ) );
			if( it != _map.end( ) )
			{
				output( "<COLOR=RED><b>(DUT_%02d) ERROR : </b> "
					"DataStore::_store_object() -- %s object associated with "
					"key 0x%08X has been overwritten!</COLOR>", _dut + 1, typeid( _obj ).name( ), _key );

				// Overwrite current element stored with |_key|
				map_T::mapped_type index = it->second;
				_vec [ index ] = _obj;
				return FALSE;
			}
			else
			{
				// Add |_obj| to the end of |_vec|
				_vec.push_back( _obj );

				// Get index of this newly added element
				map_T::mapped_type index = _vec.size( ) - 1;

				return _map.insert( map_T::value_type( _generate_key( _dut, _key ), index ) ).second;
			}
		}
	};
}


