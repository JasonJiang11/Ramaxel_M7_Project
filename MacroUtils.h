#ifndef _MACROUTILS_H_
#define _MACROUTILS_H_

// To concatenate 
#define CONCAT4( w, x, y , z ) w##x##y##z
#define CONCAT3( x, y , z ) x##y##z
#define CONCAT2( x, y ) x##y
#define CONCAT( x, y ) CONCAT2( x, y )

// To help create a unique label
#define MAKE_LABEL( name ) CONCAT( name, CONCAT( __Line, __LINE__ ) )

// To help create macro calls
//
// For example, if we have the following macros defined:
//
//   INVOKE_1
//   INVOKE_2
//   INVOKE_3
//   INVOKE_4
//
// and we wish to define another macro which expands to one of
// these macros based on the argument passed to it. To define
// such a macro, we would do the follwing:
//
// #define INVOKE_X( num ) TOKENPASTE( INVOKE_ , num )
//
#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#endif _MACROUTILS_H_