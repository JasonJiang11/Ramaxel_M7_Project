#pragma once
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <memory>
#include <ctime> // for std::strftime
#include <regex>
#include <functional> // for std::function
#include "../AppsResources/DutPin.h" // for DutPin utils
#include "../AppsResources/PinList.h" // for PinList utils

// Because we don't want those silly (lowercase) min/max macros!
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

/// Apps libraries
namespace apps
{
	/// For all your datalogging needs
	namespace datalog
	{
		/// \cond skip
		namespace detail
		{
			// Predicate to identify an std::string
			template< typename T >
			struct IsStdString
			{
				static const bool value = false; 
			};
			template<>
			struct IsStdString< std::string >
			{
				static const bool value = true; 
			};

			// If the type is std::string, converts it to a pointer-to-const-char
			// via the member function c_str(), else forwards the type.
			template< typename T, bool = IsStdString< T >::value >
			struct ConvertToCharPointerIfStdString
			{
				T&& operator()( T&& arg )
				{
					return static_cast< T&& >( arg );
				}
			};
			template<>
			struct ConvertToCharPointerIfStdString< std::string, true >
			{
				const char* operator()( const std::string&& arg )
				{
					return arg.c_str();
				}
			};
		}
		/// \endcond

		/// Converts a wchar string to a multibyte (narrow) char string
		///
		/// Returns true if an error occurred during the conversion, else
		/// returns false.
		///
		inline bool wcstombs( const std::wstring& wstr, std::string& ret )
		{
			// Allocate two narrow chars for each wchar, plus two
			// more narrow chars for a wide null char.
			ret.clear();
			ret.resize( ( wstr.size() * 2 ) + 1, ' ' );

			// Convert to a narrow char array
			auto numCharsConverted = std::wcstombs( const_cast< char* >( ret.data() ), wstr.data(), wstr.size() );
			if( numCharsConverted != wstr.size() )
			{
				return true;
			}
			else
			{
				ret.resize( numCharsConverted );
				return false;
			}
		}

		/// Same as ::vFormat() (part of the Nextest runtime environment) except
		/// that it automatically converts all std::strings to `const char*` via
		/// the c_str() member function.
		///
		/// Refer to the vFormat() and sprintf() documentation for details on the
		/// accepted format specification.
		template< typename... Args >
		inline std::string format( Args... args )
		{
			return std::string( ::vFormat( detail::ConvertToCharPointerIfStdString< Args >{}( std::forward< Args >( args ) )... ) );
		}

		/// A class that implements a minimalistic logger-handler entity
		/// for creating logging entries, filtering them based on their
		/// verbosity level, and handling them (e.g. printing or writing to
		/// a file) according to one's needs.
		struct Logger
		{
			/// Enumeration object to specify the verbosity level of a log entry
			enum class VerbosityLevel : int
			{
				Critical = 0,
				Error = 1,
				Warning = 2,
				Info = 3,
				Debug = 4
			};

			/// Sets the maximum verbosity level accepted by the handler.
			///
			/// \param lvl    verbosity level
			void set_verbosity( VerbosityLevel lvl )
			{
				_maxVerbosityLevel = lvl;
			}

			/// Returns the maximum verbosity level accepted by the handler.
			VerbosityLevel get_verbosity()
			{
				return _maxVerbosityLevel;
			}
		
			/// Sets the format of each log entry.
			///
			/// Format specifiers:
			///
			/// {time}     time stamp
			/// {level}    log level (e.g. error, info, critical)
			/// {message}  log message
			///
			/// The default format is "{time} : {level} : {message}".
			/// 
			void set_log_format( const std::string& str )
			{
				_logFormat = str;
			}
		
			/// Sets the format of each DutNum-specific log entry.
			///
			/// Format specifiers:
			///
			/// {time}     time stamp
			/// {dut}      DutNum
			/// {level}    log level (e.g. error, info, critical)
			/// {message}  log message
			///
			/// The default format is "{time} : {level} : {dut} : {message}".
			/// 
			void set_dut_log_format( const std::string& str )
			{
				_dutLogFormat = str;
			}

			/// Sets the format of the {time} specifier in log entries.
			///
			/// The {time} format specifiers are the same as those used in std::strftime().
			///
			/// The default format is "%Y-%m-%d %H:%M:%S".
			///
			void set_time_format( const std::string& str )
			{
				_timeFormat = str;
			}

			/// Sets the format of the {dut} specifier in log entries.
			///
			/// The specifier should contain exactly 1 numerical format-specifier
			/// as used in std::printf(), CString::Format(), vFormat(), etc.
			///
			/// Examples for DutNum::t_dut10:
			///
			/// [default format] (1) "DUT %2d" prints "DUT 10"
			///                  (2) "t_dut%d" prints "t_dut10"
			///                  (3) "DUT 0x%02X" prints "DUT 0x0A"
			void set_dut_format( const std::string& str )
			{
				_dutFormat = str;
			}

			/// Sets the format of multi-line log records' line headers.
			///
			/// In multi-line log records, every line but the first will
			/// be prepended by this formatted string. will be replaced
			/// by this formatted string.
			///
			/// The default format is ">  ".
			///
			void set_multi_line_format( const std::string& str )
			{
				_multiLineFormat = str;
			}

			/// Enables/disables multi-line log formatting.
			///
			void enable_multi_line_format( bool assert )
			{
				_multiLineFormatEnabled = assert;
			}

			/// Sets the handler of all log records generated by *this.
			///
			/// For example, in order to print all log entries in blue, the handler could
			/// be changed as follows:
			///
			/// \code
			/// Logger myLogger;
			/// myLogger.set_handler( []( VerbosityLevel, const std::string& str ){ output( "<COLOR=BLUE>%s</COLOR>", str.c_str() ); } );
			/// \endcode
			///
			/// The default handler is defined as follows: `[]( VerbosityLevel, const std::string& str ){ output( str.c_str() ); }`
			///
			/// \param handler    a callable object with signature `void handler( VerbosityLevel lvl, const std::string& str )`
			///                     that handles a given formatted log record, `str`, with verbosity level, `lvl`.
			void set_handler( std::function< void( VerbosityLevel, const std::string& ) > handler )
			{
				_handler = handler;
			}

			/// Logs a message without the {dut} specifier.
			template< typename... Args >
			void log( VerbosityLevel level, Args&&... args )
			{
				// Skip rendering of log record if verbosity level doesn't allow it.
				if( static_cast< int >( level ) > static_cast< int >( _maxVerbosityLevel ) )
					return;
			
				_create_log_entry( level, format( std::forward< Args >( args )... ) );
			}

			/// Logs a message with the {dut} specifier.
			template< typename... Args >
			void log( VerbosityLevel level, DutNum dut, Args&&... args )
			{
				// Skip rendering of log record if verbosity level doesn't allow it.
				if( static_cast< int >( level ) > static_cast< int >( _maxVerbosityLevel ) )
					return;

				_create_log_entry( level, dut, format( std::forward< Args >( args )... ) );
			}

			/// Logs a debug message.
			///
			/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
			template< typename... Args >
			void debug( Args&&... args )
			{
				log( VerbosityLevel::Debug, std::forward< Args >( args )... );
			}

			/// Logs an information message.
			///
			/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
			template< typename... Args >
			void info( Args&&... args )
			{
				log( VerbosityLevel::Info, std::forward< Args >( args )... );
			}
		
			/// Logs a warning message.
			///
			/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
			template< typename... Args >
			void warning( Args&&... args )
			{
				log( VerbosityLevel::Warning, std::forward< Args >( args )... );
			}

			/// Logs an error message.
			///
			/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
			template< typename... Args >
			void error( Args&&... args )
			{
				log( VerbosityLevel::Error, std::forward< Args >( args )... );
			}

			/// Logs a critical message.
			///
			/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
			template< typename... Args >
			void critical( Args&&... args )
			{
				log( VerbosityLevel::Critical, std::forward< Args >( args )... );
			}

		private:

			// Returns the default xxx format
			std::string _get_default_time_format()
			{
				return "%Y-%m-%d %H:%M:%S";
			}
			std::string _get_default_dut_format()
			{
				return "DUT %2d";
			}
			std::string _get_default_log_format()
			{
				return "{time} : {level} : {message}";
			}
			std::string _get_default_dut_log_format()
			{
				return "{time} : {level} : {dut} : {message}";
			}
			std::string _get_default_multi_line_format()
			{
				return ">  ";
			}

			// Stores various format strings used by apps::datalog
			std::string _timeFormat = _get_default_time_format();
			std::string _dutFormat = _get_default_dut_format();
			std::string _logFormat = _get_default_log_format();
			std::string _dutLogFormat = _get_default_dut_log_format();
			std::string _multiLineFormat = _get_default_multi_line_format();

			// Other state vars
			bool _multiLineFormatEnabled = true;

			// Returns a xxx-formatted string
			std::string _format_time( const std::string& fmt )
			{
				// Thanks knavero for the code!
				size_t bufsize = 0;
				size_t bufsize_incr = 256;
				std::time_t rawTime = std::time(nullptr);
				auto timeinfo = std::localtime( &rawTime );
				std::unique_ptr< char[] > buffer;
				do
				{
					bufsize += bufsize_incr;
					buffer = std::make_unique< char[] >( bufsize );
				}
				while( !std::strftime( buffer.get(), bufsize, fmt.c_str(), timeinfo ) );

				return std::string( buffer.get() );
			}
			std::string _format_dut( const std::string& fmt, DutNum dut )
			{
				return format( fmt.c_str(), dut + 1 );
			}
			std::string _format_level( VerbosityLevel lvl )
			{
				using namespace apps::datalog;
				std::string ret;
				switch( lvl )
				{
				case VerbosityLevel::Critical:
					ret = "CRITICAL";
					break;
				case VerbosityLevel::Error:
					ret = "ERROR";
					break;
				case VerbosityLevel::Warning:
					ret = "WARNING";
					break;
				case VerbosityLevel::Info:
					ret = "INFO";
					break;
				case VerbosityLevel::Debug:
					ret = "DEBUG";
					break;
				default:
					ret = "UNKNOWN";
					break;
				}
				return format( "%-8s", ret );
			}

			// Replaces the {xxx} specifier with the corresponding formatted string
			void _replace( std::string& format, const std::string& specifier, const std::string& newStr )
			{
				size_t start = 0;
				while( 1 )
				{
					auto pos = format.find( specifier, start );
					if( pos != std::string::npos )
					{
						format.replace( pos, specifier.size(), newStr.c_str() );
						start = pos + newStr.size();
					}
					else
						break;
				}
			}
			void _replace_time( std::string& format )
			{
				auto time = _format_time( _timeFormat );
				_replace( format, "{time}", time );
			}
			void _replace_dut( std::string& format, DutNum dut )
			{
				auto dutStr = _format_dut( _dutFormat, dut );
				_replace( format, "{dut}", dutStr );
			}
			void _replace_level( std::string& format, VerbosityLevel lvl )
			{
				auto lvlStr = _format_level( lvl );
				_replace( format, "{level}", lvlStr );
			}
			void _replace_line_breaks( std::string& format )
			{
				_replace( format, "\n", '\n' + _multiLineFormat );
			}
			void _replace_message( std::string& format, const std::string& msg )
			{
				_replace( format, "{message}", msg );
			}
		
			// State vars
			VerbosityLevel _maxVerbosityLevel = VerbosityLevel::Debug;
			std::function< void( VerbosityLevel, const std::string& ) > _handler = []( VerbosityLevel, const std::string& str ){ output( str.c_str() ); };

			// Generates a formatted log entry
			std::string _generate_formatted_log_entry( const std::string& fmt, DutNum dut, VerbosityLevel lvl, const std::string& msg )
			{
				auto ret( fmt );
				_replace_time( ret );
				_replace_dut( ret, dut );
				_replace_level( ret, lvl );
				_replace_message( ret, msg );

				if( _multiLineFormatEnabled )
					_replace_line_breaks( ret );

				return ret;
			}

			// Logs an entry
			void _create_log_entry( const std::string& fmt, DutNum dut, VerbosityLevel level, const std::string& msg )
			{
				// Generate the log entry
				auto entry = _generate_formatted_log_entry( fmt, dut, level, std::string( msg ) );
				_handler( level, entry.c_str() );
			}

			// Logs a message with a specified verbosity level. The log entry
			// will ignore the {dut} format specifier.
			void _create_log_entry( VerbosityLevel level, const std::string& msg )
			{
				_create_log_entry( _logFormat, t_dut_na, level, msg );
			}

			// Logs a message associated with one DutNum with a specified verbosity level.
			void _create_log_entry( VerbosityLevel level, DutNum dut, const std::string& msg )
			{
				_create_log_entry( _dutLogFormat, dut, level, msg );
			}
		};


		/// Returns a reference to the default `Logger` instance for all Apps libraries.
		///
		/// This function must be defined by the user in one, and only one, translation unit (i.e. cpp file ).
		/// Failure to define this function will lead to link failures during the build.
		Logger& get_default_logger();

		/// Logs a message using the default `Logger`.
		///
		/// \sa Logger::debug()
		template< typename... Args >
		void log( Logger::VerbosityLevel level, Args&&... args )
		{
			get_default_logger().log( level, std::forward< Args >( args )... );
		}

		/// Logs a debug message using the default `Logger`.
		///
		/// \sa Logger::debug()
		template< typename... Args >
		void debug( Args&&... args )
		{
			get_default_logger().debug( std::forward< Args >( args )... );
		}

		/// Logs an information message using the default `Logger`.
		///
		/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
		template< typename... Args >
		void info( Args&&... args )
		{
			get_default_logger().info( std::forward< Args >( args )... );
		}
		
		/// Logs a warning message using the default `Logger`.
		///
		/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
		template< typename... Args >
		void warning( Args&&... args )
		{
			get_default_logger().warning( std::forward< Args >( args )... );
		}

		/// Logs an error message using the default `Logger`.
		///
		/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
		template< typename... Args >
		void error( Args&&... args )
		{
			get_default_logger().error( std::forward< Args >( args )... );
		}

		/// Logs a critical message using the default `Logger`.
		///
		/// If the first argument is a DutNum, the log entry will include the {dut} specifier.
		template< typename... Args >
		void critical( Args&&... args )
		{
			get_default_logger().critical( std::forward< Args >( args )... );
		}

			
		/////////////////////////////////////////////

		// Miscellaneous

		/////////////////////////////////////////////

		/// \cond skip
		namespace detail
		{
			// Helpers for generate_table()
			inline std::string generate_table_row( const std::vector< size_t > &col_widths, const std::vector< std::string > &data, std::string delimiter )
			{
				typedef std::vector< std::string > col_type;
				std::string hstr = "";

				// Add columns, one by one.
				for(col_type::const_iterator it = data.cbegin(); it != data.cend(); ++it)
				{
					// Pad string if needed
					col_type::difference_type col_index = it - data.begin();
					int pad_len = int( col_widths[col_index] - it->size() );
					if( pad_len >= 0 )
					{
						// Add entire string...
						hstr += *it;
			
						// ... plus padding ...
						if( pad_len > 0 )
							hstr += std::string( pad_len, ' ' );
					}
					else
					{
						// Add partial string since it exceeds the specified column width.
						// Here, |pad_len| should be negative.
						hstr.insert(hstr.end(), it->begin(), it->end() + pad_len);
					}

					// Add delimiter if not last string in vector
					if( it != data.cend() - 1 )
						hstr += delimiter;
				}
				return hstr;
			}
			inline void transpose_table( const std::vector< std::string > &headers, const std::vector< std::vector< std::string > > &rows, std::vector< std::vector< std::string > > &cols )
			{
				const auto numCols = headers.size();
				const auto numRows = rows.size();

				cols.clear();
				cols.resize( numCols );
				for( auto it = std::begin( rows ); it != std::end( rows ); ++it )
				{
					auto& rowElement = *it;
					auto rowIndex = it - std::begin( rows );

					for( auto jt = std::begin( rowElement ); jt != std::end( rowElement ); ++jt )
					{
						auto& str = *jt;
						auto colIndex = jt - std::begin( rowElement );

						if( colIndex == cols.size() ) break;

						cols[ colIndex ].emplace_back( str );
					}
				}
			}
		}
		/// \endcond

		/// Generates a plain text table from a vector of columns.
		///
		/// The overload without the `column_widths` parameter automatically adjusts
		/// the width of each column based on their largest entry.
		///
		/// \param headers         column headers
		/// \param columns         table's columns. Each element represents one column.
		/// \param column_widths   the width (specified as a number of characters) for each column. 
		///                         A column width of 0 will cause the column to be as large as its
		///                         largest entry.
		/// \param delimiter       string delimiting each columns
		///
		inline std::string generate_table_from_columns( const std::vector< std::string >& headers, const std::vector< std::vector< std::string > >& columns, const std::vector< size_t > &column_widths, std::string delimiter = " | " )
		{
			typedef std::vector< std::string > col_type;
			typedef std::vector< col_type > body_type;

			// Create copy of |cols_input| for modification
			body_type cols( columns );

			// Make sure that |cols| has the same size as |headers|
			cols.resize( headers.size() );

			// Make sure that each column in |cols| has the same number of strings
			size_t max_num_rows = 0;
			for( auto it = cols.begin(); it != cols.end(); ++it )
				max_num_rows = std::max( max_num_rows, it->size() );
			for( auto it = cols.begin(); it != cols.end(); ++it )
				it->resize(max_num_rows);

			// For |col_widths| entries of zero, replace with the length of the longest string in the corresponding column.
			std::vector< size_t > col_widths( column_widths );
			for( auto it = col_widths.begin(); it != col_widths.end(); ++it )
			{
				if( *it != 0 ) continue;
				auto col_index = it - col_widths.begin();

				// Check column header string length
				*it = std::max( headers[ col_index ].size(), *it ) ;

				// Check length of each string in the column body
				for( auto itc = cols[ col_index ].cbegin(); itc != cols[ col_index ].cend(); ++itc )
					*it = std::max( itc->size(), *it );
			}

			// Print table header
			auto hstr = detail::generate_table_row( col_widths, headers, delimiter );

			// Print separator before and after column headers;
			const auto sep = std::string( hstr.size(), '-' ) ;
			auto ret = sep + "\n" + hstr + "\n" + sep + "\n";

			// Print table body
			for( size_t ix = 0; ix < max_num_rows; ++ix )
			{
				col_type temp;
				for( auto it = cols.begin(); it != cols.end(); ++it )
					temp.emplace_back( it->at( ix ) );

				ret += detail::generate_table_row( col_widths, temp, delimiter ) + "\n";
			}
			return ret;
		}
		
		/// \overload
		inline std::string generate_table_from_columns( const std::vector< std::string >& headers, const std::vector< std::vector< std::string > >& columns, std::string delimiter = " | " )
		{
			typedef std::vector< std::string > col_type;
			typedef std::vector< col_type > body_type;

			const auto numCols = headers.size();

			// Find the length of the longest string in each column |headers| and |cols|
			std::vector< size_t > col_widths( numCols, 0 );
			for( auto it = col_widths.begin(); it != col_widths.end(); ++it )
			{
				auto col_index = it - col_widths.begin();

				// Check column header string length
				*it = std::max( headers[ col_index ].size(), headers[ col_index ].size() ) ;

				// Check length of each string in the column body
				if( col_index == columns.size() ) break;
				for( auto itc = columns[ col_index ].cbegin(); itc != columns[ col_index ].cend(); ++itc )
					*it = std::max( itc->size(), *it );
			}
			return generate_table_from_columns( headers, columns, col_widths, delimiter );
		}

		/// Generates a plain text table from a vector of rows.
		///
		/// The overload without the `column_widths` parameter automatically adjusts
		/// the width of each column based on their largest entry.
		///
		/// \param headers         column headers
		/// \param rows            table's rows. Each element represents one row.
		/// \param column_widths   the width (specified as a number of characters) for each column. 
		///                         A column width of 0 will cause the column to be as large as its
		///                         largest entry.
		/// \param delimiter       string delimiting each columns
		///
		inline std::string generate_table_from_rows( const std::vector< std::string >& headers, const std::vector< std::vector< std::string > >& rows, const std::vector< size_t >& column_widths, std::string delimiter = " | " )
		{
			std::vector< std::vector< std::string > > cols;
			detail::transpose_table( headers, rows, cols );
			return generate_table_from_columns( headers, cols, column_widths, delimiter );
		}
		
		/// \overload
		inline std::string generate_table_from_rows( const std::vector< std::string >& headers, const std::vector< std::vector< std::string > >& rows, std::string delimiter = " | " )
		{
			std::vector< std::vector< std::string > > cols;
			detail::transpose_table( headers, rows, cols );
			return generate_table_from_columns( headers, cols, delimiter );
		}
		
		/// Returns a formatted string with voltage units.
		///
		/// \param value   voltage expressed in Nextest units.
		inline std::string voltage( double value )
		{
			if ( fabs( value ) >= ( 1 V ) )
				return format( "%4.3f V", value / ( 1 V ) );
			else
				return format( "%1.0f mV", value / ( 1 MV ) );
		}

		/// Returns a formatted string with current units.
		///
		/// \param value   current expressed in Nextest units.
		inline std::string current( double value )
		{
			if ( fabs( value ) >= ( 1 MA ) )
				return format( "%4.3f mA", value / ( 1 MA ) );
			else if ( fabs( value ) >= ( 1 UA ) )
				return format( "%4.3f uA", value / ( 1 UA ) );
			else
				return format( "%1.0f nA", value / ( 1 NA ) );
		}

		/// Returns a formatted string with time units.
		///
		/// \param val   time expressed in Nextest units.
		inline std::string time( double val )
		{
			if( val == 0.0 )
				return "0 s";
			else if( val >= 1 S || val == 0.0 )
				return format( "%4.3f s", val / (1 S) );
			else if ( val >= 1 MS )
				return format( "%4.3f ms", val / (1 MS) );
			else if ( val >= 1 US )
				return format( "%4.3f us", val / (1 US) );
			else if ( val >= 1 NS )
				return format( "%4.3f ns", val / (1 NS) );
			else if ( val >= 1 PS )
				return format( "%4.3f ps", val / (1 PS) );
			else
				return format( "%3.0f fs", val / (1 FS) );
		}

		/// Converts a UPB timestamp to a CString with units
		///
		/// \param timestamp    UPB timestamp value
		inline std::string upb_timestamp( UINT64 timestamp )
		{
			const UINT64 ns_per_s = 1000000000; // 1e9
			const UINT64 ns_per_ms = 1000000; // 1e6
			const UINT64 ns_per_us = 1000; // 1e3

			UINT64 timestamp_ns = 10 * timestamp; // 10ns per clock cycle
			UINT64 time_s = timestamp_ns / ns_per_s;
			UINT64 time_ms = (timestamp_ns - (time_s * ns_per_s)) / ns_per_ms;
			UINT64 time_us = (timestamp_ns - (time_s * ns_per_s) - (time_ms * ns_per_ms)) / ns_per_us;
			UINT64 time_ns = timestamp_ns - (time_s * ns_per_s) - (time_ms * ns_per_ms) - (time_us * ns_per_us);
	
			auto s = format( "%llu s", time_s );
			auto ms = format( "%llu ms", time_ms );
			auto us = format( "%llu us", time_us );
			auto ns = format( "%llu ns", time_ns );

			// Return in units of seconds
			if( timestamp_ns > ns_per_s ) return format( "%s : %s : %s : %s", s, ms, us, ns );

			// Return in units of miliseconds
			if( timestamp_ns > ns_per_ms ) return format( "%s : %s : %s", ms, us, ns );
	
			// Return in units of microseconds
			if( timestamp_ns > ns_per_us ) return format( "%s : %s", us, ns );

			// Return in units of nanoseconds
			return ns;
		}

		/// Creates a list from the contents of a container.
		///
		/// Here are some examples of printing a comma-seperated list of integers
		/// stored inside a vector.
		///
		/// \code
		/// using namespace apps::datalog;
		/// std::vector< int > ints = { 45, 8, 9 };
		/// std::cout << list( std::begin( ints ), std::end( ints ), ", ",
		///    []( int val, size_t ){ return format( "%d", val ); } ) << '\n';
		///
		/// // This time, include the index of each integer
		/// std::cout << list( std::begin( ints ), std::end( ints ), ", ",
		///    []( int val, size_t ix ){ return format( "[%d] %d", ix, val ); } ) << '\n';
		///
		/// // Output:
		/// // 45, 8, 9
		/// // [0] 45, [1] 8, [2] 9
		/// \endcode
		///
		/// \param delim         string seperating each element
		/// \param first, last   the range of elements to generate the list
		/// \param printer       a callable object that accepts an element from |Container| (along
		///                        with that element's index) and returns a type convertible to std::string.
		///                        In other words, the object must have the following signature:
		///                        `string_type operator()( Container::value_type, size_t )`
		///
		template< typename ForwardIt, typename Printer >
		inline std::string list( ForwardIt first, ForwardIt last, std::string delim, Printer printer )
		{
			std::string ret;
			auto it = first;
			while( it != last )
			{
				auto ix = std::distance( first, it );
				ret += printer( *it, ix );
				ret += delim;
				++it;
			}

			// Remove trailing delimiter
			if( !ret.empty() )
				ret.resize( ret.size() - delim.size() );

			return ret;
		}

		/// Returns a string representing a given TesterFunc.
		///
		inline std::string testerfunc( TesterFunc tfn )
		{
			// Macro for defining the static map below
			#define ADD_MAP_ENTRY( val ) ret.emplace( val, #val );

			static auto ret = []
			{
				std::map< TesterFunc, std::string > ret;
				ADD_MAP_ENTRY( t_cs1 );
				ADD_MAP_ENTRY( t_cs2 );
				ADD_MAP_ENTRY( t_cs3 );
				ADD_MAP_ENTRY( t_cs4 );
				ADD_MAP_ENTRY( t_cs5 );
				ADD_MAP_ENTRY( t_cs6 );
				ADD_MAP_ENTRY( t_cs7 );
				ADD_MAP_ENTRY( t_cs8 );
				ADD_MAP_ENTRY( t_cs9 );
				ADD_MAP_ENTRY( t_cs10 );
				ADD_MAP_ENTRY( t_cs11 );
				ADD_MAP_ENTRY( t_cs12 );
				ADD_MAP_ENTRY( t_cs13 );
				ADD_MAP_ENTRY( t_cs14 );
				ADD_MAP_ENTRY( t_cs15 );
				ADD_MAP_ENTRY( t_cs16 );
				ADD_MAP_ENTRY( t_cs17 );
				ADD_MAP_ENTRY( t_cs18 );
	
				ADD_MAP_ENTRY( t_d0 );
				ADD_MAP_ENTRY( t_d1 );
				ADD_MAP_ENTRY( t_d2 );
				ADD_MAP_ENTRY( t_d3 );
				ADD_MAP_ENTRY( t_d4 );
				ADD_MAP_ENTRY( t_d5 );
				ADD_MAP_ENTRY( t_d6 );
				ADD_MAP_ENTRY( t_d7 );
				ADD_MAP_ENTRY( t_d8 );
				ADD_MAP_ENTRY( t_d9 );
				ADD_MAP_ENTRY( t_d10 );
				ADD_MAP_ENTRY( t_d11 );
				ADD_MAP_ENTRY( t_d12 );
				ADD_MAP_ENTRY( t_d13 );
				ADD_MAP_ENTRY( t_d14 );
				ADD_MAP_ENTRY( t_d15 );
				ADD_MAP_ENTRY( t_d16 );
				ADD_MAP_ENTRY( t_d17 );
				ADD_MAP_ENTRY( t_d18 );
				ADD_MAP_ENTRY( t_d19 );
				ADD_MAP_ENTRY( t_d20 );
				ADD_MAP_ENTRY( t_d21 );
				ADD_MAP_ENTRY( t_d22 );
				ADD_MAP_ENTRY( t_d23 );
				ADD_MAP_ENTRY( t_d24 );
				ADD_MAP_ENTRY( t_d25 );
				ADD_MAP_ENTRY( t_d26 );
				ADD_MAP_ENTRY( t_d27 );
				ADD_MAP_ENTRY( t_d28 );
				ADD_MAP_ENTRY( t_d29 );
				ADD_MAP_ENTRY( t_d30 );
				ADD_MAP_ENTRY( t_d31 );
				ADD_MAP_ENTRY( t_d32 );
				ADD_MAP_ENTRY( t_d33 );
				ADD_MAP_ENTRY( t_d34 );
				ADD_MAP_ENTRY( t_d35 );
	
				ADD_MAP_ENTRY( t_y0 );
				ADD_MAP_ENTRY( t_y1 );
				ADD_MAP_ENTRY( t_y2 );
				ADD_MAP_ENTRY( t_y3 );
				ADD_MAP_ENTRY( t_y4 );
				ADD_MAP_ENTRY( t_y5 );
				ADD_MAP_ENTRY( t_y6 );
				ADD_MAP_ENTRY( t_y7 );
				ADD_MAP_ENTRY( t_y8 );
				ADD_MAP_ENTRY( t_y9 );
				ADD_MAP_ENTRY( t_y10 );
				ADD_MAP_ENTRY( t_y11 );
				ADD_MAP_ENTRY( t_y12 );
				ADD_MAP_ENTRY( t_y13 );
				ADD_MAP_ENTRY( t_y14 );
				ADD_MAP_ENTRY( t_y15 );
				ADD_MAP_ENTRY( t_y16 );
				ADD_MAP_ENTRY( t_y17 );
				ADD_MAP_ENTRY( t_y18 );
				ADD_MAP_ENTRY( t_y19 );
				ADD_MAP_ENTRY( t_y20 );
				ADD_MAP_ENTRY( t_y21 );
				ADD_MAP_ENTRY( t_y22 );
				ADD_MAP_ENTRY( t_y23 );
				ADD_MAP_ENTRY( t_y24 );
				ADD_MAP_ENTRY( t_y25 );
				ADD_MAP_ENTRY( t_y26 );
				ADD_MAP_ENTRY( t_y27 );
				ADD_MAP_ENTRY( t_y28 );
				ADD_MAP_ENTRY( t_y29 );
				ADD_MAP_ENTRY( t_y30 );
				ADD_MAP_ENTRY( t_y31 );
	
				ADD_MAP_ENTRY( t_x0 );
				ADD_MAP_ENTRY( t_x1 );
				ADD_MAP_ENTRY( t_x2 );
				ADD_MAP_ENTRY( t_x3 );
				ADD_MAP_ENTRY( t_x4 );
				ADD_MAP_ENTRY( t_x5 );
				ADD_MAP_ENTRY( t_x6 );
				ADD_MAP_ENTRY( t_x7 );
				ADD_MAP_ENTRY( t_x8 );
				ADD_MAP_ENTRY( t_x9 );
				ADD_MAP_ENTRY( t_x10 );
				ADD_MAP_ENTRY( t_x11 );
				ADD_MAP_ENTRY( t_x12 );
				ADD_MAP_ENTRY( t_x13 );
				ADD_MAP_ENTRY( t_x14 );
				ADD_MAP_ENTRY( t_x15 );
				ADD_MAP_ENTRY( t_x16 );
				ADD_MAP_ENTRY( t_x17 );
				ADD_MAP_ENTRY( t_x18 );
				ADD_MAP_ENTRY( t_x19 );
				ADD_MAP_ENTRY( t_x20 );
				ADD_MAP_ENTRY( t_x21 );
				ADD_MAP_ENTRY( t_x22 );
				ADD_MAP_ENTRY( t_x23 );
				ADD_MAP_ENTRY( t_x24 );
				ADD_MAP_ENTRY( t_x25 );
				ADD_MAP_ENTRY( t_x26 );
				ADD_MAP_ENTRY( t_x27 );
				ADD_MAP_ENTRY( t_x28 );
				ADD_MAP_ENTRY( t_x29 );
				ADD_MAP_ENTRY( t_x30 );
				ADD_MAP_ENTRY( t_x31 );
	
				ADD_MAP_ENTRY( t_z0 );
				ADD_MAP_ENTRY( t_z1 );
				ADD_MAP_ENTRY( t_z2 );
				ADD_MAP_ENTRY( t_z3 );
				ADD_MAP_ENTRY( t_z4 );
				ADD_MAP_ENTRY( t_z5 );
				ADD_MAP_ENTRY( t_z6 );
				ADD_MAP_ENTRY( t_z7 );
				ADD_MAP_ENTRY( t_z8 );
				ADD_MAP_ENTRY( t_z9 );
				ADD_MAP_ENTRY( t_z10 );
				ADD_MAP_ENTRY( t_z11 );
				ADD_MAP_ENTRY( t_z12 );
				ADD_MAP_ENTRY( t_z13 );
				ADD_MAP_ENTRY( t_z14 );
				ADD_MAP_ENTRY( t_z15 );
				ADD_MAP_ENTRY( t_z16 );
				ADD_MAP_ENTRY( t_z17 );
				ADD_MAP_ENTRY( t_z18 );
				ADD_MAP_ENTRY( t_z19 );
				ADD_MAP_ENTRY( t_z20 );
				ADD_MAP_ENTRY( t_z21 );
				ADD_MAP_ENTRY( t_z22 );
				ADD_MAP_ENTRY( t_z23 );
	
				ADD_MAP_ENTRY( t_drive_low );
				ADD_MAP_ENTRY( t_drive_high );
				ADD_MAP_ENTRY( t_strobe_low );
				ADD_MAP_ENTRY( t_strobe_high );
				ADD_MAP_ENTRY( t_strobe_valid );
				ADD_MAP_ENTRY( t_strobe_mid );
				ADD_MAP_ENTRY( t_tri_state );
	
				ADD_MAP_ENTRY( t_dsc_src_1 );
				ADD_MAP_ENTRY( t_dsc_src_2 );
				ADD_MAP_ENTRY( t_dsc_src_3 );
				ADD_MAP_ENTRY( t_dsc_src_4 );
				ADD_MAP_ENTRY( t_dsc_src_5 );
				ADD_MAP_ENTRY( t_dsc_src_6 );
				ADD_MAP_ENTRY( t_dsc_src_7 );
				ADD_MAP_ENTRY( t_dsc_src_8 );
				ADD_MAP_ENTRY( t_dsc_src_9 );
				ADD_MAP_ENTRY( t_dsc_src_20 );
				ADD_MAP_ENTRY( t_dsc_src_21 );
				ADD_MAP_ENTRY( t_dsc_src_22 );
				ADD_MAP_ENTRY( t_dsc_src_23 );
				ADD_MAP_ENTRY( t_dsc_src_24 );
				ADD_MAP_ENTRY( t_dsc_src_25 );
				ADD_MAP_ENTRY( t_dsc_src_26 );
				ADD_MAP_ENTRY( t_dsc_src_27 );
				ADD_MAP_ENTRY( t_dsc_src_28 );
				ADD_MAP_ENTRY( t_dsc_src_29 );
				ADD_MAP_ENTRY( t_dsc_src_30 );
				ADD_MAP_ENTRY( t_dsc_src_31 );
				ADD_MAP_ENTRY( t_dsc_src_32 );
	
				ADD_MAP_ENTRY( t_dutctrl1 );
				ADD_MAP_ENTRY( t_dutctrl2 );
				ADD_MAP_ENTRY( t_dutctrl3 );
				ADD_MAP_ENTRY( t_dutctrl4 );
				ADD_MAP_ENTRY( t_dutctrl5 );
				ADD_MAP_ENTRY( t_dutctrl6 );
				ADD_MAP_ENTRY( t_dutctrl7 );
				ADD_MAP_ENTRY( t_dutctrl8 );
				ADD_MAP_ENTRY( t_dutctrl9 );
				ADD_MAP_ENTRY( t_dutctrl20 );
				ADD_MAP_ENTRY( t_dutctrl21 );
				ADD_MAP_ENTRY( t_dutctrl22 );
				ADD_MAP_ENTRY( t_dutctrl23 );
				ADD_MAP_ENTRY( t_dutctrl24 );
				ADD_MAP_ENTRY( t_dutctrl25 );
				ADD_MAP_ENTRY( t_dutctrl26 );
				ADD_MAP_ENTRY( t_dutctrl27 );
				ADD_MAP_ENTRY( t_dutctrl28 );
				ADD_MAP_ENTRY( t_dutctrl29 );
				ADD_MAP_ENTRY( t_dutctrl30 );
				ADD_MAP_ENTRY( t_dutctrl31 );
				ADD_MAP_ENTRY( t_dutctrl32 );
				ADD_MAP_ENTRY( t_dutctrl33 );
				ADD_MAP_ENTRY( t_dutctrl34 );
				ADD_MAP_ENTRY( t_dutctrl35 );
				ADD_MAP_ENTRY( t_dutctrl36 );
				ADD_MAP_ENTRY( t_dutctrl37 );
				ADD_MAP_ENTRY( t_dutctrl38 );
				ADD_MAP_ENTRY( t_dutctrl39 );
				ADD_MAP_ENTRY( t_dutctrl40 );
	
				ADD_MAP_ENTRY( t_clk_req_dut1 );
				ADD_MAP_ENTRY( t_clk_req_dut2 );
				ADD_MAP_ENTRY( t_clk_req_dut3 );
				ADD_MAP_ENTRY( t_clk_req_dut4 );
				ADD_MAP_ENTRY( t_clk_req_dut5 );
				ADD_MAP_ENTRY( t_clk_req_dut5 );
				ADD_MAP_ENTRY( t_clk_req_dut6 );
				ADD_MAP_ENTRY( t_clk_req_dut7 );
				ADD_MAP_ENTRY( t_clk_req_dut8 );
	
				ADD_MAP_ENTRY( t_scan );
				ADD_MAP_ENTRY( t_scan1 );
				ADD_MAP_ENTRY( t_scan2 );
				ADD_MAP_ENTRY( t_scan3 );
				ADD_MAP_ENTRY( t_scan4 );
	
				ADD_MAP_ENTRY( t_wt_trigger );
				ADD_MAP_ENTRY( t_lvm );
				ADD_MAP_ENTRY( t_dbm );
				ADD_MAP_ENTRY( t_tf_na );
	
				return ret;
			}();
	
			auto it = ret.find( tfn );
			if( it == std::end( ret ) )
				return "UNDEFINED TesterFunc";
			else
				return it->second;

			// Undefine macro
			#undef ADD_MAP_ENTRY
		}

		/// Returns a table representation of the specified PinScramble.
		///
		/// \param psNum    PinScramble number
		/// \perDut         if true, generates one column for Cycle A and another column
		///                  for Cycle B of each DutNum in the PinAssignments table. Otherwise,
		///                  only generates Cycle A and Cycle B columns for t_dut1.
		///
		inline std::string pinscramble( PSNumber psNum, bool perDut = false )
		{
			// Get all DutPins defined by user.
			static const auto dutPins = []
			{
				CStringArray dutPinNames;
				resource_all_names( S_DutPin, &dutPinNames );
		
				std::vector< DutPin* > ret;
				for( auto i = 0; i < dutPinNames.GetCount(); ++i )
				{
					auto pDutPin = DutPin_find( dutPinNames[ i ] );
					ret.emplace_back( pDutPin );
				}
		
				return ret;
			}();
		
			// Create table headers
			std::vector< std::string > headers = { "DutPin" };
			const auto endCond = perDut ? max_dut() + 1 : 1;
			for( auto dut = t_dut1; dut < endCond; ++dut )
			{
				// For Cycle A and B...
				for( auto i = 0; i < 2; ++i )
				{
					if( perDut )
					{
						headers.emplace_back(
							datalog::format( "DUT %3d - Cycle %s",
							dut + 1,
							i == 0 ? "A" : "B" )
							);
					}
					else
					{
						headers.emplace_back(
							datalog::format( "Cycle %s",
							i == 0 ? "A" : "B" )
							);
					}
				}
			}
		
			// Define table contents
			std::vector< std::vector< std::string > > table;
		
			// For each DutPin...
			for( auto& dutPin : dutPins )
			{
				std::vector< std::string > row;
				row.emplace_back( resource_name( dutPin ) );

				// For each DutNum
				bool isValidRow = false;
				for( auto dut = t_dut1; dut < endCond; ++dut )
				{
					auto tPin = apps::dutpin::get_hd_tester_pin( dutPin, dut );

					// For cycle A and B...
					for( auto cycleB = 0; cycleB < 2; ++cycleB )
					{
						// Since all_pe() doesn't accept a pointer-to-DutPin as an
						// argument, we look at each DutNum's assigned TesterFunc to
						// determine whether the DutPin is assigned to a PE channel or not.
						if( ( tPin >= t_1 ) && ( tPin < t_pe_max  ) )
						{
							isValidRow = true;
							auto tFn = pinscramble_lookup( psNum, tPin, !cycleB );
							row.emplace_back( datalog::testerfunc( tFn ) );
						}
						else
							row.emplace_back( "ERROR" );
					}
				}

				if( isValidRow )
					table.emplace_back( std::move( row ) );
			}

			// Generate table
			return generate_table_from_rows( headers, table );
		}

		/// Function type used to perform the last parametric test
		enum class ParametricTestFunction
		{
			partest,
			test_supply,
			hv_test_supply
		};

		// \cond skip
		namespace detail
		{
			// Returns the force value, and test limits based on the ParametricTestFunction
			// and whether the test measure current or voltage.
			inline void get_test_conditions( datalog::ParametricTestFunction ptf, bool isMeasCurr, double& force, double& low, double& high, DutPin* pDutPin )
			{
				using namespace datalog;

				switch( ptf )
				{
				case ParametricTestFunction::partest:	
					if( isMeasCurr )
					{
						force = vpar_force( pDutPin );
						low = ipar_low( pDutPin );
						high = ipar_high( pDutPin );
					}
					else
					{
						force = ipar_force( pDutPin );
						low = vpar_low( pDutPin );
						high = vpar_high( pDutPin );
					}
					break;

				case ParametricTestFunction::test_supply:
					force = dps( pDutPin );
					low = dps_current_low( pDutPin );
					high = dps_current_high( pDutPin );
					break;

				case ParametricTestFunction::hv_test_supply:
					force = hv_voltage_get( pDutPin );
					if( isMeasCurr )
					{
						low = hv_ipar_low( pDutPin );
						high = hv_ipar_high( pDutPin );
					}
					else
					{
						low = hv_vpar_low( pDutPin );
						high = hv_vpar_high( pDutPin );
					}
					break;

				default:
					auto str = format( "datalog::get_test_conditions() -- Invalid ParametricTestFunction value." );
					warning( str );
					throw std::exception( str.c_str() );
				}
			}
		}
		/// \endcond

		/// Logs a table with all the results of the previous parametric test for the given PinList
		///
		/// \param pPinList   PinList for which to log results
		/// \param ptf        parametric test function for which results will be logged
		inline void parametric( PinList* pPinList, ParametricTestFunction ptf = ParametricTestFunction::partest )
		{
			const std::vector< std::string > headers = 
			{
				" ",
				"Pass/Fail",
				"Test Number",
				"Test Name",
				"DUT #",
				"Pin Name",
				"Force Value",
				"Low Limit",
				"Measured Value",
				"High Limit",
			};

			const std::string testNumberString = format( "%d", test_number() );
			const std::string testName( current_test_block() );

			// Check if valid PinList
			const auto allDps = all_dps( pPinList );
			const auto allHv = all_hv( pPinList );
			const auto allPe = all_pe( pPinList );

			if( !( allDps || allHv || allPe ) )
			{
				warning( "datalog::parametric() -- PinList <%s> contains different tester channel types. Log will not be generated.", resource_name( pPinList ) );
				return;
			}
	
			// Get which type of test was executed.
			bool isValCurrent;
			switch( ptf )
			{
			case ParametricTestFunction::partest:	
				isValCurrent = parametric_mode() == 0 ? true : false;
				break;

			case ParametricTestFunction::test_supply:
			case ParametricTestFunction::hv_test_supply:
				isValCurrent = true;
				break;

			default:
				auto str = format( "datalog::parametric() -- Invalid ParametricTestFunction value." );
				warning( str );
				throw std::exception( str.c_str() );
			}

			// Convert PinList into a vector for easier iteration
			std::vector< DutPin* > pinVec;
			pinlist::get_pinlist_as_vector( pPinList, pinVec );

			// Get pass/fail and measurement results
			std::map< DutNum, std::deque< PFState > > pfMap;
			std::map< DutNum, std::deque< double > > valsMap;
			{
				SOFT_DUT_LOOP( dut )
				{
					CArray< PFState, PFState > pfTemp;
					CArray< double, double > valsTemp;

					if( allDps )
					{
						Dps_pf( pfTemp );
						Dps_meas( valsTemp );
					}
					else if( allHv )
					{
						Hv_pf( pfTemp );
						Hv_meas( valsTemp );
					}
					else
					{
						Pin_pf( pfTemp );
						Pin_meas( valsTemp );
					}

					// Copy CArrays to real data structures (i.e. lists)
					std::deque< PFState > pfVec;
					for( auto i = 0; i < pfTemp.GetCount(); ++i )
						pfVec.emplace_back( pfTemp[ i ] );

					std::deque< double > valsVec;
					for( auto i = 0; i < valsTemp.GetCount(); ++i )
						valsVec.emplace_back( valsTemp[ i ] );

					// 'Move' to Dut map
					pfMap.emplace(  dut, pfVec );
					valsMap.emplace( dut, valsVec );
				}
			}

			// Vector for building the table's contents
			std::vector< std::vector< std::string > > tableContents;

			// Build the log entry as a table, one row at a time.
			for( auto it = std::begin( pinVec ); it != std::end( pinVec ); ++it )
			{
				auto pDutPin = *it;
				auto pinIndex = it - std::begin( pinVec );

				SOFT_DUT_LOOP( dut )
				{
					// Skip this DUT pin if it is unassgined
					if( dutpin::get_hd_tester_pin( pDutPin, dut ) == t_na )
						continue;

					// Get pass/fail and measurement elements
					auto& pfElement = pfMap[ dut ];
					auto& valElement = valsMap[ dut ];
			
					// Check that there exists an entry for this DutPin.
					// If not, the user did not provide the correct PinList.
					if( pfElement.empty() || valElement.empty() )
					{
						warning( "datalog::parametric() -- Number of measurements stored in PMU does not match size of PinList <%s>.",
							resource_name( pPinList ) );
						return;
					}
			
					// Extract pass/fail result and discard
					auto pf = pfElement.front();
					pfMap[ dut ].pop_front();

					// Ditto, but for measurement value
					auto val = valElement.front();
					valElement.pop_front();

					// Get force value, and test limits.
					double forceVal, lowLimitVal, highLimitVal;
					detail::get_test_conditions( ptf, isValCurrent, forceVal, lowLimitVal, highLimitVal, pDutPin );

					std::string measFailString = pf == PFState::FAIL ? " * " : "";
					std::string pfString = pf ? "PASS" : "FAIL";
					std::string dutString = format( "%d", dut + 1 );
					std::string pinName = resource_name( pDutPin );

					auto valLogger = []( double val, bool isValCurrent )
						{
							auto ret = isValCurrent ? datalog::current( val ) : datalog::voltage( val );
							return static_cast< std::string >( ret );
						};
			
					auto forceValueString = ( ptf == ParametricTestFunction::hv_test_supply && !isValCurrent ) ? "N/A" : valLogger( forceVal, !isValCurrent );
					auto lowLimitString   = valLogger( lowLimitVal, isValCurrent );
					auto highLimitString  = valLogger( highLimitVal, isValCurrent );
					auto valString        = valLogger( val, isValCurrent );

					std::vector< std::string > tempStrVec =
					{
						measFailString,
						pfString,
						testNumberString,
						testName,
						dutString,
						pinName,
						forceValueString,
						lowLimitString,
						valString,
						highLimitString,
					};
					tableContents.emplace_back( std::move( tempStrVec ) );
				}
			}

			info( "\n%s", datalog::generate_table_from_rows( headers, tableContents, "    " ).c_str() );

			// Finally, print a summary of which DUTs passed and which failed.
			{
				SOFT_DUT_LOOP( dut )
				{
					if( result_get( dut ) == PASS )
						info( dut, "<COLOR=GREEN>PASS</COLOR>" );
					else
						info( dut, "<COLOR=RED>FAIL</COLOR>" );
				}
			}
		}

		/// Logs a table with all the results of the previous parametric test for the given PinList
		///
		/// \param pDutPin    DutPin for which to log results
		/// \param ptf        parametric test function for which results will be logged
		inline void parametric( DutPin* pDutPin, ParametricTestFunction ptf = ParametricTestFunction::partest )
		{
			auto pPinList = pinlist_create( pDutPin );
			parametric( pPinList, ptf );
			pinlist_destroy( pPinList );
		}	
	}
}