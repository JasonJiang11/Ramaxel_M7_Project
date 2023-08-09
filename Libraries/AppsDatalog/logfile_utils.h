#pragma once
#include "afxstr.h"

/////////////////////////////////////////////////////////////////////

// The inclusion of this code in your test program will allow for
// automatic logging of the UI console to files on the Host computer's
// disk. Each site window in the UI console will have its contents
// stored to a separate file. To disable this behavior, you can either
// (a) not include this code in your test program, or (b) invoke
// apps::logfile::disable().

// Due to the nature of some of this code, it cannot be distributed
// in a header-only format. So instead, if you wish to use this code,
// you should copy logfile_utils.h and logfile_utils.cpp to your
// test program's directory.

/////////////////////////////////////////////////////////////////////

namespace apps
{
	namespace logfile
	{
		/// Forces the configuration of the UI output files before they are
		/// automatically configured after the InitializationHook.
		///
		/// Has no effect if the UI output files have already been configured. 
		///
		void initialize();

		/// Disables the generation of UI output files.
		///
		/// Must be invoked before the end of the InitializationHook.
		///
		void disable();

		/// Specifies the folder where the UI output files will be stored.
		///
		/// The folder name provided must be an absolute path that may or may
		/// not end with a directory delimiter symbol '/' or "\\". The folder
		/// name can use any format specifier defined by the Nextest format_filename()
		/// function.
		///
		/// The default folder name is "D:\\nextest\\logs\\<EXENAME>\\<DATE>".
		///
		/// \param format     format specifier for the log file folder name
		///
		void set_folder_name_format( const std::string& format );

		/// Returns the folder name format.
		///
		std::string get_folder_name_format();

		/// Returns the formatted folder name.
		///
		/// If invoked before the log folder and log files have been
		/// initialized, returns an empty string.
		///
		/// The folder name is returned without the trailing '\' character.
		///
		std::string get_folder_name();

		/// Specifies the log file name format for the UI output files.
		///
		/// The supported format specifiers are the same as those used in
		/// format_filename(). Refer to the documentation of format_filename()
		/// for more details.
		///
		/// One additional format specifier, <EXT>, is supported, which expands
		/// to the file extension specified via set_file_extension().
		///
		/// The default format specifier on the Host is "Host.<EXT>".
		/// The default format specifier on a site is "Site<SITENUM:%03d>__<CHASSIS>_<SLOTNUM:%02d>_<SLICE>.<EXT>".
		///
		/// \param format      log file format
		///
		void set_file_name_format( const std::string& format );

		/// Returns the file name format.
		///
		std::string get_file_name_format();

		/// Specifies the log file extension.
		///
		/// The default extension is "log".
		///
		/// \param ext     file extension
		///
		/// \code
		/// // Change log file extension to .txt
		/// apps::logfile::set_file_extension( "txt" );
		/// \endcode
		///
		void set_file_extension( const std::string& ext );

		/// Returns the log file extension
		///
		std::string get_file_extension();

		/// Returns the text intercepted at program load time, before the log files were initialized.
		///
		std::string get_load_time_string();

		/// Creates directories for the given path. Returns true if the directories
		/// were created successfully, else returns false.
		///
		/// The provided directory path must end with a directory delimiter symbol
		/// ('/' or "\\"). If a path to a filename is given instead, e.g. "D:\\users\\foo",
		/// the filename is dropped and the directory "D:\\users\\" is created.
		///
		/// \param path      directory
		///
		bool create_directories( const std::string& path );
	}
}