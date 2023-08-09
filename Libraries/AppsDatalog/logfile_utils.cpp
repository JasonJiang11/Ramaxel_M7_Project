#include "TestProgApp/public.h"
#include <algorithm>
#include "logfile_utils.h"

// Because we don't want those silly (lowercase) min/max macros!
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

// This handles <CHASSIS>, <SLOTNUM> and <SERIALNUM> tokens. Warn about the obsolete <ENV:SITENAME> token.
static CString format_filename_extended( CString name )
{
	if ( re_match( name, "<ENV:SITENAME>" ) )
	{
		warning( "format_filename_extended() passed obsolete \"<ENV:SITENAME>\" spec.  Use <CHASSIS>_<SITE> instead!" );
		name.Replace( "<ENV:SITENAME>", "<CHASSIS>_<SITE>" );
	}

	return format_filename( name );
}

CString initialize_folder_name_format()
{
	return "d:\\nextest\\logs\\<EXENAME>\\<DATE>";
}
static CString folder_name_format = initialize_folder_name_format();
void apps::logfile::set_folder_name_format( const std::string& format )
{
	folder_name_format = format.c_str();
}
std::string apps::logfile::get_folder_name_format()
{
	return std::string( folder_name_format );
}

static CString file_ext = "log";
void apps::logfile::set_file_extension( const std::string& ext )
{
	file_ext = ext.c_str();
}
std::string apps::logfile::get_file_extension()
{
	return std::string( file_ext );
}

// Replace the <EXT> format specifier with the user-specified extension
static CString replace_ext_specifier( const CString& name )
{
	auto ret = name;
	ret.Replace( "<EXT>", file_ext );
	return ret;
}

CString initialize_file_name_format()
{
	if ( OnHost() )
		return "Host.<EXT>";
	else
		return "Site<SITENUM:%03d>__<CHASSIS>_<SLOTNUM:%02d>_<SLICE>.<EXT>";
}
CSTRING_VARIABLE( file_name_format, initialize_file_name_format(), "" )
{
}
void apps::logfile::set_file_name_format( const std::string& format )
{
	file_name_format = format.c_str();
}
std::string apps::logfile::get_file_name_format()
{
	return std::string( file_name_format );
}

// Replaces the <DATE> format specifier with whatever <DATE> is rendered as on the
// Host process during program load time. This ensures that all sites end with the
// same date string. This is especially important in the case where the <DATE> specifier
// is used in the folder name format.
CSTRING_VARIABLE( date_string, ( site_num( ) == 0 ? format_filename( "<DATE>" ) : remote_get( "date_string", 0 ) ), "" )
{
}
static CString replace_date_specifier( const CString& name )
{
	auto ret = name;
	ret.Replace( "<DATE>", date_string );
	return ret;
}

// Intercepts output() and warning() strings and saves them until we set
// up UI's log files via logfile::initialize().
static CString prog_load_string = "";
struct SaveOutput : CStringArray 
{
	SaveOutput()
	{
		intercept( saveoutput, this );
	}
	~SaveOutput()
	{
		fumble( saveoutput );
		for ( int i = 0; i < GetSize( ); ++i )
		{
			CString c = (*this)[ i ];
			auto type = c.GetAt( 0 );
			auto str = c.Mid( 1 );
			( type == 'o' ? output : warning )( "%s\\", str );

			// Also store these strings statically for future reference
			if( type == 'w' )
				prog_load_string += "warning: ";
			prog_load_string += str;
		}
	}
	static BOOL saveoutput( char type, CString text, void *data )
	{
		static_cast< SaveOutput * >( data )->Add( type + text );
		return FALSE;
	}
} *savedOutput = new SaveOutput;


std::string apps::logfile::get_load_time_string()
{
	return std::string( prog_load_string );
}

static bool enableUiOutputFiles = true;
void apps::logfile::disable()
{
	enableUiOutputFiles = false;
}

// Conditionally enables the UI output files using the user-specified
// log folder and log file name format strings, and then dump all
// deferred output strings (captured via SaveOutput) to the UI.
static CString folder_name = "";
static void _initialize()
{
	if( enableUiOutputFiles )
	{
		// Construct the log file path
		auto path = replace_date_specifier( folder_name_format );
		folder_name = format_filename( path );
		path += "\\";
		path += file_name_format;
		path = replace_ext_specifier( path );

		// Make sure any other logfile is closed (paranoia)
		remote_set( "ui_OutputFile", "", -1, TRUE, INFINITE );

		// Assign a UI output file for this site process
		remote_set( "ui_OutputFile", format_filename_extended( path ), -1, TRUE, INFINITE );
	}

	// Now that UI knows the log files send it any deferred output.
	if( savedOutput != nullptr )
	{
		delete savedOutput;
		savedOutput = nullptr;
	}
}

std::string apps::logfile::get_folder_name()
{
	return std::string( folder_name );
}

// Automatically initialize the UI output files for the user
// right after the InitializationHook has finished executing.
AFTER_INITIALIZING( Resource, InitializationHook )
{
	apps::logfile::initialize();
}

// In case the test program wasn't loaded successfully, we want all the intercepted
// strings to be dumped to the UI.
BEFORE_DEALLOCATING( Resource, SiteEndBlock )
{
	if( savedOutput != nullptr )
		delete savedOutput;
}

void apps::logfile::initialize()
{
	if( savedOutput != nullptr )
		_initialize();
}
bool apps::logfile::create_directories( const std::string& path_str )
{
	CString path = path_str.c_str();
	CString orig = path; // keep a copy of the user's input for a potential error message

	// Check if no directory needs to be created
	int pos = std::max( path.ReverseFind( '/' ), path.ReverseFind( '\\' ) );
	if ( pos < 0 )
		return true;
	
	// Drop the filename, keep the trailing /
	path = path.Mid( 0, pos + 1 );

	// Handle UNC names (i.e. "\\server3\temp") first.
	// We identify UNC paths by the first 2 characters being both '\' (but of course,
	// as a string literatal, 2 backslashes are escaped and thus entered as "\\\\").
	// Then, we find the server name by finding the next '\' character in the path.
	// This delimits the server name (on the left of the '\') from the path inside
	// the server (to the right of '\').
	CString dir = "";
	if ( ( path.Find( "\\\\" ) == 0 ) && ( ( pos = path.Find( "\\", 2 ) ) >= 0 ) )
	{
		dir = dir + path.Left( pos + 1 );
		path = path.Mid( pos + 1 );
	}

	// Loop over each component of the path (i.e. each substring contained between
	// '\' characters), creating a directory for each, one at a time.
	while ( ( pos = path.FindOneOf( "/\\" ) ) >= 0 )
	{
		dir = dir + path.Left( pos + 1 );
		path = path.Mid( pos + 1 );
		if ( ! CreateDirectory( dir, 0 ) )
		{
			DWORD fa = GetFileAttributes( dir );
			if ( ( fa == 0xffffffff ) || ( fa & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			{
				AfxMessageBox( vFormat( "Cannot create directory %s (for output file %s)", dir, orig ) );
				return false;
			}
		}
	}
	return true;
}