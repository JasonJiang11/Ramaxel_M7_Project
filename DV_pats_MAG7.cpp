// This file has been automatically generated by the pattern compiler.
// DO NOT EDIT!

#ifndef MAGNUM7
#define MAGNUM7
#endif 

#define PATCOM_OUTPUT
#include "tester.h"
#include "DV_pats_MAG7.h"

#ifndef PATTERN_INSTRUCTION
#error PATTERN_INSTRUCTION not #defined ("tester.h" should not have "#pragma once").
#endif



/************ pattern "SUB__FHS" ************/

PATTERN_INITIAL_CONDITIONS( SUB__FHS )   /* C Code to set initial conditions */
{
}

PATTERN_INSTRUCTIONS( SUB__FHS )   /* Microcode */
{
	/* 0 */
	PATTERN_INSTRUCTION(0x00000318,0x0001D000,0x00004000,0x01000016,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x01100D10,0x01100110,0x01100110,0x14900680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x17100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0000030C,0x0001D100,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x000000C0,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0080018C,0x00740500,0x00004010,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00C00189,0x00740400,0x00404010,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00C00136,0x00000000,0x00404000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x000002D8,0x00000000,0x00004000,0x02000016,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x01100D10,0x01100110,0x01100110,0x14900680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x17100680,0x14024800,0x00000000,0x00000000)
}

PATTERN_LABELS( SUB__FHS )   /* List of all labels with their associated microinstructions */
{
	PATTERN_LABEL( 2, "LookAhead1" )
	PATTERN_LABEL( 3, "LookAhead2" )
}

PATTERN_MAR_RELOC_UINSTS( SUB__FHS )   /* Microinstructions that need MAR address relocated */
{
	PATTERN_MAR_RELOC_UINST( 2 )
	PATTERN_MAR_RELOC_UINST( 3 )
	PATTERN_MAR_RELOC_UINST( 4 )
}

PATTERN_MAR_FUNCB_UINSTS( SUB__FHS )   /* Instructions requiring MAR_FUNCB fixup */
{
	PATTERN_MAR_FUNCB_UINST( 2, SUB__FHS, 2 )
	PATTERN_MAR_FUNCB_UINST( 3, SUB__FHS, 3 )
	PATTERN_MAR_FUNCB_UINST( 4, SUB__FHS, 3 )
}

PATTERN_COUNTER_UINSTS( SUB__FHS )   /* Instructions requiring Counter func/addr fixup */
{
	PATTERN_COUNTER_UINST( 2, SUB__FHS, 2 )
	PATTERN_COUNTER_UINST( 3, SUB__FHS, 3 )
}

PATTERN_VERSIONS( SUB__FHS )   /* Version of the compiler output */
{
	PATTERN_VERSION( PATTERN_VAR_GOSUB, 1 )
}

PATTERN_USED_TSETS( SUB__FHS )   /* TSETS used by this pattern */
{
#ifdef INCLUDE_HD7
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET10 )
#endif
}

PATTERN_USED_VIHHS( SUB__FHS )   /* VIHH used by this pattern */
{
	PATTERN_USED_VIHH( VIHH1 )
}

PATTERN_ATTRIBUTES( SUB__FHS )   /* Pattern attributes */
{
	PATTERN_ATTRIBUTE( MEM )
	PATTERN_ATTRIBUTE( SINGLE )
}

PATTERN_HW_REQUIREMENTS( SUB__FHS )   /* Hardware requirements */
{
	PATTERN_HW_REQUIREMENT( mag, 7 )
}

PATTERN( SUB__FHS )   /* Process the pattern specific information */
{
	INITIAL_CONDITIONS( SUB__FHS )
	INSTRUCTIONS( SUB__FHS )
	LABELS( SUB__FHS )
	MAR_RELOC_UINSTS( SUB__FHS )
	MAR_FUNCB_UINSTS( SUB__FHS )
	COUNTER_UINSTS( SUB__FHS )
	VERSIONS( SUB__FHS )
	USED_TSETS( SUB__FHS )
	USED_VIHHS( SUB__FHS )
	ATTRIBUTES( SUB__FHS )
	HW_REQUIREMENTS( SUB__FHS )
}



/************ pattern "PAT__FlagHandshake" ************/

PATTERN_INITIAL_CONDITIONS( PAT__FlagHandshake )   /* C Code to set initial conditions */
{
	count(30, 4);
}

PATTERN_INSTRUCTIONS( PAT__FlagHandshake )   /* Microcode */
{
	/* 0 */
	PATTERN_INSTRUCTION(0x00000306,0x00000000,0x00004000,0x0C000014,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x00000D10,0x01100110,0x01100110,0x11900680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x000000C6,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x000000C6,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x000000D8,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0000170E,0x00000000,0x00004000,0x0C000015,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x00000D10,0x01100110,0x01100110,0x11900680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x10100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x014001D8,0x00000000,0x00004400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
}

PATTERN_LABELS( PAT__FlagHandshake )   /* List of all labels with their associated microinstructions */
{
	PATTERN_LABEL( 5, "End" )
}

PATTERN_MAR_RELOC_UINSTS( PAT__FlagHandshake )   /* Microinstructions that need MAR address relocated */
{
	PATTERN_MAR_RELOC_UINST( 1 )
	PATTERN_MAR_RELOC_UINST( 2 )
	PATTERN_MAR_RELOC_UINST( 3 )
	PATTERN_MAR_RELOC_UINST( 5 )
}

PATTERN_GOSUB_UINSTS( PAT__FlagHandshake )   /* Microinstructions that call other patterns */
{
	PATTERN_GOSUB_UINST( 1, SUB__FHS )
	PATTERN_GOSUB_UINST( 2, SUB__FHS )
	PATTERN_GOSUB_UINST( 3, SUB__FHS )
}

PATTERN_MAR_FUNCB_UINSTS( PAT__FlagHandshake )   /* Instructions requiring MAR_FUNCB fixup */
{
	PATTERN_MAR_FUNCB_UINST( 5, PAT__FlagHandshake, 5 )
	PATTERN_MAR_FUNCB_UINST( 1, SUB__FHS, 0 )
	PATTERN_MAR_FUNCB_UINST( 2, SUB__FHS, 0 )
	PATTERN_MAR_FUNCB_UINST( 3, SUB__FHS, 0 )
}

PATTERN_VERSIONS( PAT__FlagHandshake )   /* Version of the compiler output */
{
	PATTERN_VERSION( PATTERN_VAR_GOSUB, 1 )
}

PATTERN_USED_TSETS( PAT__FlagHandshake )   /* TSETS used by this pattern */
{
#ifdef INCLUDE_HD7
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET10 )
#endif
}

PATTERN_USED_VIHHS( PAT__FlagHandshake )   /* VIHH used by this pattern */
{
	PATTERN_USED_VIHH( VIHH1 )
}

PATTERN_ATTRIBUTES( PAT__FlagHandshake )   /* Pattern attributes */
{
	PATTERN_ATTRIBUTE( MEM )
	PATTERN_ATTRIBUTE( SINGLE )
}

PATTERN_HW_REQUIREMENTS( PAT__FlagHandshake )   /* Hardware requirements */
{
	PATTERN_HW_REQUIREMENT( mag, 7 )
}

PATTERN( PAT__FlagHandshake )   /* Process the pattern specific information */
{
	INITIAL_CONDITIONS( PAT__FlagHandshake )
	INSTRUCTIONS( PAT__FlagHandshake )
	LABELS( PAT__FlagHandshake )
	MAR_RELOC_UINSTS( PAT__FlagHandshake )
	GOSUB_UINSTS( PAT__FlagHandshake )
	MAR_FUNCB_UINSTS( PAT__FlagHandshake )
	VERSIONS( PAT__FlagHandshake )
	USED_TSETS( PAT__FlagHandshake )
	USED_VIHHS( PAT__FlagHandshake )
	ATTRIBUTES( PAT__FlagHandshake )
	HW_REQUIREMENTS( PAT__FlagHandshake )
}



/************ pattern "PAT__userramaddress" ************/

PATTERN_INITIAL_CONDITIONS( PAT__userramaddress )   /* C Code to set initial conditions */
{
}

PATTERN_INSTRUCTIONS( PAT__userramaddress )   /* Microcode */
{
	/* 0 */
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x01000014,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x00000D10,0x01100110,0x01100110,0x11800680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000009,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x80200000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x15000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x0000000B,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x80200000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x15000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	/* 10 */
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x0000000D,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x80200000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x15000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x0000000F,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x80200000,0x00000001,0x00000000,0x00000000,0x00000000,0x00000001,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x15000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x01000015,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x00000D10,0x01100110,0x01100110,0x11800680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00000680,0x00000000,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x0000530E,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x10000680,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x10000680,0x00000000,0x00000000,0x00000000)
	/* 20 */
	PATTERN_INSTRUCTION(0x050001D8,0x00000000,0x00004400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
}

PATTERN_LABELS( PAT__userramaddress )   /* List of all labels with their associated microinstructions */
{
	PATTERN_LABEL( 20, "End" )
}

PATTERN_MAR_RELOC_UINSTS( PAT__userramaddress )   /* Microinstructions that need MAR address relocated */
{
	PATTERN_MAR_RELOC_UINST( 20 )
}

PATTERN_MAR_FUNCB_UINSTS( PAT__userramaddress )   /* Instructions requiring MAR_FUNCB fixup */
{
	PATTERN_MAR_FUNCB_UINST( 20, PAT__userramaddress, 20 )
}

PATTERN_VERSIONS( PAT__userramaddress )   /* Version of the compiler output */
{
	PATTERN_VERSION( PATTERN_VAR_GOSUB, 1 )
}

PATTERN_USED_TSETS( PAT__userramaddress )   /* TSETS used by this pattern */
{
#ifdef INCLUDE_HD7
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET1 )
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET10 )
#endif
}

PATTERN_USED_VIHHS( PAT__userramaddress )   /* VIHH used by this pattern */
{
	PATTERN_USED_VIHH( VIHH1 )
}

PATTERN_ATTRIBUTES( PAT__userramaddress )   /* Pattern attributes */
{
	PATTERN_ATTRIBUTE( MEM )
	PATTERN_ATTRIBUTE( SINGLE )
}

PATTERN_HW_REQUIREMENTS( PAT__userramaddress )   /* Hardware requirements */
{
	PATTERN_HW_REQUIREMENT( mag, 7 )
}

PATTERN( PAT__userramaddress )   /* Process the pattern specific information */
{
	INITIAL_CONDITIONS( PAT__userramaddress )
	INSTRUCTIONS( PAT__userramaddress )
	LABELS( PAT__userramaddress )
	MAR_RELOC_UINSTS( PAT__userramaddress )
	MAR_FUNCB_UINSTS( PAT__userramaddress )
	VERSIONS( PAT__userramaddress )
	USED_TSETS( PAT__userramaddress )
	USED_VIHHS( PAT__userramaddress )
	ATTRIBUTES( PAT__userramaddress )
	HW_REQUIREMENTS( PAT__userramaddress )
}



/************ pattern "PAT__FlsShutdown" ************/

PATTERN_INITIAL_CONDITIONS( PAT__FlsShutdown )   /* C Code to set initial conditions */
{
}

PATTERN_INSTRUCTIONS( PAT__FlsShutdown )   /* Microcode */
{
	/* 0 */
	PATTERN_INSTRUCTION(0x00000318,0x00000000,0x00004000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x00000B0E,0x00000000,0x00004000,0xFE000BAD,0x000000CA,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x01140D1B,0x01140D1B,0x00000D10,0x01100110,0x01100110,0x10900680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x00100680,0x14024800,0x00000000,0x00000000)
	PATTERN_INSTRUCTION(0x008001D8,0x00000000,0x00004400,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000,0x00000D10,0x00000D10,0x00000D10,0x01100110,0x01100110,0x04100680,0x14024800,0x00000000,0x00000000)
}

PATTERN_LABELS( PAT__FlsShutdown )   /* List of all labels with their associated microinstructions */
{
	PATTERN_LABEL( 2, "End" )
}

PATTERN_MAR_RELOC_UINSTS( PAT__FlsShutdown )   /* Microinstructions that need MAR address relocated */
{
	PATTERN_MAR_RELOC_UINST( 2 )
}

PATTERN_MAR_FUNCB_UINSTS( PAT__FlsShutdown )   /* Instructions requiring MAR_FUNCB fixup */
{
	PATTERN_MAR_FUNCB_UINST( 2, PAT__FlsShutdown, 2 )
}

PATTERN_VERSIONS( PAT__FlsShutdown )   /* Version of the compiler output */
{
	PATTERN_VERSION( PATTERN_VAR_GOSUB, 1 )
}

PATTERN_USED_TSETS( PAT__FlsShutdown )   /* TSETS used by this pattern */
{
#ifdef INCLUDE_HD7
	PATTERN_USED_TSET( enTSETNumberMag7::t_TSET10 )
#endif
}

PATTERN_USED_VIHHS( PAT__FlsShutdown )   /* VIHH used by this pattern */
{
	PATTERN_USED_VIHH( VIHH1 )
}

PATTERN_ATTRIBUTES( PAT__FlsShutdown )   /* Pattern attributes */
{
	PATTERN_ATTRIBUTE( MEM )
	PATTERN_ATTRIBUTE( SINGLE )
}

PATTERN_HW_REQUIREMENTS( PAT__FlsShutdown )   /* Hardware requirements */
{
	PATTERN_HW_REQUIREMENT( mag, 7 )
}

PATTERN( PAT__FlsShutdown )   /* Process the pattern specific information */
{
	INITIAL_CONDITIONS( PAT__FlsShutdown )
	INSTRUCTIONS( PAT__FlsShutdown )
	LABELS( PAT__FlsShutdown )
	MAR_RELOC_UINSTS( PAT__FlsShutdown )
	MAR_FUNCB_UINSTS( PAT__FlsShutdown )
	VERSIONS( PAT__FlsShutdown )
	USED_TSETS( PAT__FlsShutdown )
	USED_VIHHS( PAT__FlsShutdown )
	ATTRIBUTES( PAT__FlsShutdown )
	HW_REQUIREMENTS( PAT__FlsShutdown )
}