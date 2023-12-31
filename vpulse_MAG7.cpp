// This file has been automatically generated by the pattern compiler.
// DO NOT EDIT!

#ifndef MAGNUM7
#define MAGNUM7
#endif 

#define PATCOM_OUTPUT
#include "tester.h"
#include "vpulse_MAG7.h"

#ifndef PATTERN_INSTRUCTION
#error PATTERN_INSTRUCTION not #defined ("tester.h" should not have "#pragma once").
#endif



/************ pattern "PAT__vpulse" ************/

PATTERN_INITIAL_CONDITIONS( PAT__vpulse )   /* C Code to set initial conditions */
{
count(35, 2000000);
}

PATTERN_INSTRUCTIONS( PAT__vpulse )   /* Microcode */
{
	/* 0 */
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00022000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0000030C,0x00022100,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00C0018C,0x00880500,0x00005010,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x01000198,0x00880400,0x00004010,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0000230E,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x020001D8,0x00000000,0x00004400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
}

PATTERN_LABELS( PAT__vpulse )   /* List of all labels with their associated microinstructions */
{
	PATTERN_LABEL( 3, "Wait_vpulse_enable" )
	PATTERN_LABEL( 4, "Wait_vpulse_disable" )
	PATTERN_LABEL( 8, "End" )
}

PATTERN_MAR_RELOC_UINSTS( PAT__vpulse )   /* Microinstructions that need MAR address relocated */
{
	PATTERN_MAR_RELOC_UINST( 3 )
	PATTERN_MAR_RELOC_UINST( 4 )
	PATTERN_MAR_RELOC_UINST( 8 )
}

PATTERN_MAR_FUNCB_UINSTS( PAT__vpulse )   /* Instructions requiring MAR_FUNCB fixup */
{
	PATTERN_MAR_FUNCB_UINST( 8, PAT__vpulse, 8 )
	PATTERN_MAR_FUNCB_UINST( 3, PAT__vpulse, 3 )
	PATTERN_MAR_FUNCB_UINST( 4, PAT__vpulse, 4 )
}

PATTERN_COUNTER_UINSTS( PAT__vpulse )   /* Instructions requiring Counter func/addr fixup */
{
	PATTERN_COUNTER_UINST( 3, PAT__vpulse, 3 )
	PATTERN_COUNTER_UINST( 4, PAT__vpulse, 4 )
}

PATTERN_VERSIONS( PAT__vpulse )   /* Version of the compiler output */
{
	PATTERN_VERSION( PATTERN_VAR_GOSUB, 1 )
}

PATTERN_USED_TSETS( PAT__vpulse )   /* TSETS used by this pattern */
{
#ifdef INCLUDE_HD7
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET10 )
#endif
}

PATTERN_USED_VIHHS( PAT__vpulse )   /* VIHH used by this pattern */
{
	PATTERN_USED_VIHH( VIHH1 )
}

PATTERN_ATTRIBUTES( PAT__vpulse )   /* Pattern attributes */
{
	PATTERN_ATTRIBUTE( MEM )
	PATTERN_ATTRIBUTE( SINGLE )
}

PATTERN_HW_REQUIREMENTS( PAT__vpulse )   /* Hardware requirements */
{
	PATTERN_HW_REQUIREMENT( mag, 7 )
}

PATTERN( PAT__vpulse )   /* Process the pattern specific information */
{
	INITIAL_CONDITIONS( PAT__vpulse )
	INSTRUCTIONS( PAT__vpulse )
	LABELS( PAT__vpulse )
	MAR_RELOC_UINSTS( PAT__vpulse )
	MAR_FUNCB_UINSTS( PAT__vpulse )
	COUNTER_UINSTS( PAT__vpulse )
	VERSIONS( PAT__vpulse )
	USED_TSETS( PAT__vpulse )
	USED_VIHHS( PAT__vpulse )
	ATTRIBUTES( PAT__vpulse )
	HW_REQUIREMENTS( PAT__vpulse )
}
