#ifndef _PATTERNMACROS_H_
#define _PATTERNMACROS_H_

#include "MacroUtils.h"

#define STROBE_EN                                  \n \
MAR READ_APG1, READ_APG2

#define STROBE_APG1_EN                                  \n \
MAR READ_APG1

#define STROBE_APG2_EN                                  \n \
MAR READ_APG2

#define STROBE_MASK( mask )                         \n \
MAR READUDATA										\n \
UDATA 0x##mask

#define SOFT_STROBE_MASK( mask )                         \n \
MAR READUDATA, NOLATCH									\n \
UDATA 0x##mask

#define STROBE_MASK_AUX( aux_reg )				\n \
MAR READREG_APG1								\n \
DATGEN1 STROBEREGLOADBYAUXB						\n \
AUXREG1	aux_reg, HOLD, aux_reg					\n \

#define USE_STROBE_MASK								\n \
MAR READREG_APG1									\n \

#define SOFT_ERROR                                 \n \
MAR NOLATCH, READ_APG1

#define DECREMENT( count )                         \n \
COUNT CONCAT( COUNT, count ), DECR, AON            

#define LOOP( count, label )						\n \
COUNT CONCAT( COUNT, count ), DECR, AON				\n \
MAR CJMPNZ, label	

#define DECR( count)								\n \
COUNT CONCAT( COUNT, count ), DECR, AON				\n \

#define JUMP_TO_IF_COUNT_ZERO( count, label )      \n \
COUNT CONCAT( COUNT, count ), NOCOUNT              \n \
MAR CJMPZ, label

#define JUMP_TO_IF_COUNT_NOT_ZERO( count, label )  \n \
COUNT CONCAT( COUNT, count ), NOCOUNT              \n \
MAR CJMPNZ, label									\n \

#define JUMP_TO( label )						   \n \
MAR JUMP, label

#define JUMP_IF_ERROR( label )						\n \
MAR cjmpe_all, label								\n \

#define JUMP_IF_NO_ERROR( label )					\n \
MAR cjmpne, label, READ_APG1, READ_APG2									\n \

#define RESET_ERROR							       \n \
MAR RESET

#define LOAD_COUNTER( count, data )                \n \
COUNT CONCAT( COUNT, count ), COUNTUDATA           \n \
UDATA data

#define LOAD_RELOAD( count, data )                \n \
COUNT CONCAT( RELOAD, count ), COUNTUDATA           \n \
UDATA data

#define END_PATTERN                                \n \
MAR DONE

#define RETURN_PATTERN                             \n \
MAR RETURN

#define SETFLAG_1                 \n \
PINFUNC SETFLAG1 \n
#define SETFLAG_2                 \n \
PINFUNC SETFLAG2 \n
#define SETFLAG_3                 \n \
PINFUNC SETFLAG3 \n
#define SETFLAG_4                 \n \
PINFUNC SETFLAG4 \n
#define CLEARFLAG_1               \n \
PINFUNC CLRFLAG1 \n
#define CLEARFLAG_2               \n \
PINFUNC CLRFLAG2 \n
#define CLEARFLAG_3               \n \
PINFUNC CLRFLAG3 \n
#define CLEARFLAG_4               \n \
PINFUNC CLRFLAG4 \n
#define LOOKAHEAD_F1              \n \
MAR ERR_F1 \n
#define LOOKAHEAD_F2              \n \
MAR ERR_F2 \n
#define LOOKAHEAD_F3              \n \
MAR ERR_F3 \n
#define LOOKAHEAD_F4              \n \
MAR ERR_F4 \n
#define CJMP_FLAG1(label)         \n \
MAR CJMPF_1, label \n
#define CJMP_FLAG2(label)         \n \
MAR CJMPF_2, label \n
#define CJMP_FLAG3(label)         \n \
MAR CJMPF_3, label \n
#define CJMP_FLAG4(label)         \n \
MAR CJMPF_4, label \n

/************************************************************************************************************
  Pattern Macro Helpers
*************************************************************************************************************/
#define JAMRAM_ADDR_LOAD( addr )			\n \
DATGEN1 JAMRAMRELOADBYUDATA, JAMRAMPTRLOAD	\n \
DATGEN2 JAMRAMRELOADBYUDATA, JAMRAMPTRLOAD	\n \
UDATA1 addr									\n \
UDATA2 addr   

#define JAMRAM_ADDR_RELOAD			\n \
DATGEN1 JAMRAMPTRLOAD					\n \
DATGEN2 JAMRAMPTRLOAD	

/* Specify output Y address register to pin scrambles */
#define XALU( apg1_output, apg2_output ) \n \
XALU1 CONCAT2(OX, apg1_output)                                     \n \
XALU2 CONCAT2(OX, apg2_output) 

/* Specify output Y address register to pin scrambles */
#define YALU( apg1_output, apg2_output ) \n \
YALU1 CONCAT2(OY, apg1_output)                                     \n \
YALU2 CONCAT2(OY, apg2_output) 

#define YINIT( func1, func2, reg )											\n \
YALU1		XCARE, func1, COFF, CONCAT2(DY, reg), CONCAT2(OY, reg)			\n \
YALU2		XCARE, func2, COFF, CONCAT2(DY, reg), CONCAT2(OY, reg)			\n \

//init x addr reg function with multiple destinations
#define XINIT( func1, func2, reg )											\n \
XALU1		XCARE, func1, COFF, CONCAT2(DX, reg), CONCAT2(OX, reg)			\n \
XALU2		XCARE, func2, COFF, CONCAT2(DX, reg), CONCAT2(OX, reg)			\n \

#define XINC( apg1_output, apg2_output )																\n \
XALU1 CONCAT2(X, apg1_output), XCARE, CON, INCREMENT, CONCAT2(DX, apg1_output), CONCAT2(OX, apg1_output)	\n \
XALU2 CONCAT2(X, apg2_output), XCARE, CON, INCREMENT, CONCAT2(DX, apg2_output), CONCAT2(OX, apg2_output)	\n \

#define YINC( apg1_output, apg2_output )                                        \n \
YALU1 CONCAT2(Y, apg1_output), XCARE, CON, INCREMENT, CONCAT2(DY, apg1_output), CONCAT2(OY, apg1_output)  \n \
YALU2 CONCAT2(Y, apg2_output), XCARE, CON, INCREMENT, CONCAT2(DY, apg2_output), CONCAT2(OY, apg2_output) 

//increment aux_reg on 2 APGs
#define AUXINC( aux_reg )																	\n \
AUXREG1		aux_reg, INCREMENT, AUX1														\n \
AUXREG2		aux_reg, INCREMENT, AUX1														\n \

//add "dec_val" to apg register such as ymain/ybase/..
#define YADD( apg1_output, apg2_output, dec_val )															\n \
YALU1		CONCAT2(Y, apg1_output), YUDATA, COFF, ADD, CONCAT2(DY, apg1_output), CONCAT2(OY, apg1_output)	\n \
YALU2		CONCAT2(Y, apg2_output), YUDATA, COFF, ADD, CONCAT2(DY, apg2_output), CONCAT2(OY, apg2_output)	\n \
YUDATAVAL1	dec_val																							\n \
YUDATAVAL2	dec_val																							\n \

/* Load Y address register from aux register */
#define YALU_AUX( aux_reg, ag_reg )												\n \
YALU1 AUXA, CONCAT2(DY, ag_reg), CONCAT2(OY, ag_reg)							\n \
YALU2 AUXA, CONCAT2(DY, ag_reg), CONCAT2(OY, ag_reg)							\n \
AUXREG1		aux_reg, HOLD, aux_reg												\n \
AUXREG2		aux_reg, HOLD, aux_reg		

/* Load X address register from aux register */
#define XALU_AUX( aux_reg, ag_reg )												\n \
XALU1 AUXA, CONCAT2(DX, ag_reg), CONCAT2(OX, ag_reg)							\n \
XALU2 AUXA, CONCAT2(DX, ag_reg), CONCAT2(OX, ag_reg)							\n \
AUXREG1		aux_reg, HOLD, aux_reg												\n \
AUXREG2		aux_reg, HOLD, aux_reg			

// Load X and Y address register from aux registers 
// aux_reg_x loaded to X
// aux_reg_y loaded to Y
#define XYALU_AUX( aux_reg_x, aux_reg_y, ag_reg )								\n \
XALU1 AUXA, CONCAT2(DX, ag_reg), CONCAT2(OX, ag_reg)							\n \
XALU2 AUXA, CONCAT2(DX, ag_reg), CONCAT2(OX, ag_reg)							\n \
YALU1 AUXB, CONCAT2(DY, ag_reg), CONCAT2(OY, ag_reg)							\n \
YALU2 AUXB, CONCAT2(DY, ag_reg), CONCAT2(OY, ag_reg)							\n \
AUXREG1		aux_reg_x, HOLD, aux_reg_y											\n \
AUXREG2		aux_reg_x, HOLD, aux_reg_y											\n \

/* Increase JAM RAM address and output from JAM RAM */
#define INC_JAMRAM_ADDR															\n \
DATGEN1 JAMRAMINCR, OJAM														\n \
DATGEN2 JAMRAMINCR, OJAM														\n \

/* Decrease JAM RAM address and output from JAM RAM */
#define DEC_JAMRAM_ADDR															\n \
DATGEN1 JAMRAMDECR, OJAM														\n \
DATGEN2 JAMRAMDECR, OJAM

/* Hold JAM RAM address and output from JAM RAM */
#define HOLD_JAMRAM_ADDR														\n \
DATGEN1 JAMRAMHOLD, OJAM														\n \
DATGEN2 JAMRAMHOLD, OJAM

/* load initial jamram  address for apg1 and apg2*/
#define LOAD_JAMRAM_ADDR(hex_jam1, hex_jam2)									\n \
DATGEN1 JAMRAMRELOADBYUDATA														\n \
UDATA1 0x##hex_jam1																\n \
DATGEN2 JAMRAMRELOADBYUDATA														\n \
UDATA2 0x##hex_jam2																\n \

/* Specify output Data register to pin scrambles */
#define DATGEN( apg1_output, apg2_output ) \n \
DATGEN1 CONCAT2(O, apg1_output)												    \n \
DATGEN2 CONCAT2(O, apg2_output) 

#define DG_INIT( dg1_reg1, dg1_reg2 )														\n \
DATGEN1		CONCAT2( SD, dg1_reg1), HOLDDR, CONCAT2( D, dg1_reg2), CONCAT2( OD, dg1_reg2)	\n \
DATGEN2		CONCAT2( SD, dg1_reg1), HOLDDR, CONCAT2( D, dg1_reg2), CONCAT2( OD, dg1_reg2)	\n \

/* Store Data register, |dat_reg| from UDATA, |udata_val| */
#define DATGEN_UDATA( dat_reg, udata_val )										\n \
DATGEN1 SUDATA, CONCAT2(D, dat_reg), CONCAT2(OD, dat_reg), UDATADR				\n \
DATGEN2 SUDATA, CONCAT2(D, dat_reg), CONCAT2(OD, dat_reg), UDATADR				\n \
UDATA	0x##udata_val															\n \

/* Store Data register, |dat_reg| from aux register */
#define DATGEN_AUX( aux_reg, dat_reg )											\n \
DATGEN1 AUXA, CONCAT2(D, dat_reg), CONCAT2(OD, dat_reg)							\n \
DATGEN2 AUXA, CONCAT2(D, dat_reg), CONCAT2(OD, dat_reg)							\n \
AUXREG1		aux_reg, HOLD, aux_reg												\n \
AUXREG2		aux_reg, HOLD, aux_reg												

#define GOSUB_PATTERN( name ) \n \
MAR GOSUB, name

// Reload counter by initializng to 0 and decr
#define RELOAD_CNT( count )														\n \
BUS_IDLE_WAIT																	\n \
LOAD_COUNTER( count, 0 )														\n \
% BUS_IDLE_WAIT																	\n \
COUNT CONCAT( COUNT, count ), DECR, AON 

#define LFSR( lfsr, operation)													\n \
LFSR	CONCAT4(LFSR, lfsr, _, operation), CONCAT3(LFSR, lfsr, _OUT)			\n \

#endif _PATTERNMACROS_H_