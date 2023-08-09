#pragma once
#include "TestProgApp/public.h"

////////////////////
// DUT_PIN
////////////////////
#define DECLARE_DUT_PIN( name ) \
  DutPin* name();

#define DEFINE_DUT_PIN( name )        \
  DutPin* dutpin::name()              \
  {                                   \
    DutPin* p = DutPin_find( #name ); \
    return p;                         \
  }                                   \
  DUT_PIN( name )

////////////////////
// PINLIST
////////////////////
#define DECLARE_PINLIST( name )                         \
  PinList* name( DutNum dut = t_dut_na );

#define DEFINE_PINLIST( name )                          \
  DEFINE_PINLIST_FUNC( name )                           \
  PINLIST( name )

#define DEFINE_PER_DUT_PINLIST( name )                  \
  DEFINE_PINLIST_FUNC( name )                           \
  AFTER_INITIALIZING( PinList, name )                   \
  {                                                     \
    EXTERN_PINLIST( name )                              \
    extern bool CreatePerDutPinlist( PinList* );        \
    CreatePerDutPinlist( name );                        \
  }                                                     \
  PINLIST( name )

#define DEFINE_PINLIST_FUNC( name )                     \
  PinList* pinlist::name( DutNum dut )                  \
  {                                                     \
    PinList* pl;                                        \
    if ( dut == t_dut_na ) {                            \
      pl = PinList_find( #name );                       \
    }                                                   \
    else {                                              \
      CString pinlist_name;                             \
      pinlist_name.Format("%s_Dut%d", #name, dut + 1 ); \
      pl = PinList_find( pinlist_name );                \
    }                                                   \
    return pl;                                          \
  }

