#ifndef _ONFIMACROS_H_
#define _ONFIMACROS_H_

#include "DefineCounters.h"
#include "PatternMacros.h"
#include "PatternResources.h"

//RB will be driven by DG in polling RB and by PS 
#define RE_DIFF_H		REn_H, REc_L  
#define RE_DIFF_L		REn_L, REc_H  

#define DQS_DIFF_H		DQSt_H, DQSc_L  
#define DQS_DIFF_L		DQSt_L, DQSc_H  
#define DQS_DIFF_RD_H	DQSt_RD_H, DQSc_RD_L  
#define DQS_DIFF_RD_L	DQSt_RD_L, DQSc_RD_H  

#define CHIPS_IDLE			CE_ALL_L, CLE_L, ALE_L,	WE_H, RE_DIFF_H, DQS_DIFF_H  
#define CHIPS_CMD			CE_ALL_L, CLE_H, ALE_L,	WE_L, RE_DIFF_H, DQS_DIFF_H  
#define CHIPS_ADDR			CE_ALL_L, CLE_L, ALE_H,	WE_L, RE_DIFF_H, DQS_DIFF_H  
#define CHIPS_DATA_IN		CE_ALL_L, CLE_L, ALE_L,	WE_H, RE_DIFF_H, DQS_DIFF_L  
#define CHIPS_DATA_OUT		CE_ALL_L, CLE_L, ALE_L,	WE_H, RE_DIFF_L, DQS_DIFF_RD_H  
#define CHIPS_DATA_DQSRH	CE_ALL_L, CLE_L, ALE_L,	WE_H, RE_DIFF_L, DQS_DIFF_H  
#define CHIPS_IDLE_DQSZ		CE_ALL_L, CLE_L, ALE_L,	WE_H, RE_DIFF_H, DQS_DIFF_RD_H  


//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
#define BUS_IDLE_WAIT							\n \
PINFUNC1	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ	\n \
CHIPS1		CHIPS_IDLE							\n \
PINFUNC2	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ	\n \
CHIPS2		CHIPS_IDLE							\n \

//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
#define BUS_IDLE_WAIT_DQSZ						\n \
PINFUNC1	PS_NOOP, TS_IDLE_DQZ, ADHIZ			\n \
CHIPS1		CHIPS_IDLE_DQSZ						\n \
PINFUNC2	PS_NOOP, TS_IDLE_DQZ, ADHIZ			\n \
CHIPS2		CHIPS_IDLE_DQSZ						\n \

//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
#define BUS_IDLE_WAIT_DQS_RST					\n \
PINFUNC1	PS_NOOP, TS_BUS_IDLE_WAIT_DQS_RST, ADHIZ	\n \
CHIPS1		CHIPS_IDLE							\n \
PINFUNC2	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ	\n \
CHIPS2		CHIPS_IDLE							\n \

//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
//vpulse enabled
#define BUS_IDLE_WAIT_VPULSE							\n \
PINFUNC1	PS_NOOP, TS_BUS_IDLE_WAIT, VPULSE, ADHIZ	\n \
CHIPS1		CHIPS_IDLE									\n \
PINFUNC2	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ			\n \
CHIPS2		CHIPS_IDLE									\n \

//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
//turn on DBI sticky bit
#define BUS_IDLE_WAIT_DBI(dbi_mode)						\n \
PINFUNC1	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ, dbi_mode 	\n \
CHIPS1		CHIPS_IDLE									\n \
PINFUNC2	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ			\n \
CHIPS2		CHIPS_IDLE									\n \

//bus idle cycle - CE, CLE, ALE is low, DQs are tristated, DQS is diff H, RE is diff H
#define		EBM_PTR_RELOAD									\n \
PINFUNC1	PS_NOOP, TS_BUS_IDLE_WAIT, EBMPTRLOAD, ADHIZ	\n \
CHIPS1		CHIPS_IDLE										\n \
PINFUNC2	PS_NOOP, TS_BUS_IDLE_WAIT, ADHIZ				\n \
CHIPS2		CHIPS_IDLE										\n \

// A cycle during which a command byte is latched
#define SEND_CMD( hex_cmd_byte )			\n \
PINFUNC1	PS_DATA_IN, TS_CMD				\n \
CHIPS1		CHIPS_CMD						\n \
DATGEN1		SUDATA, DBASE, ODBASE, UDATADR	\n \
UDATA1		0x##hex_cmd_byte                \n \
PINFUNC2	PS_DATA_IN, TS_IDLE				\n \
CHIPS2		CHIPS_IDLE						\n \
DATGEN2		SUDATA, DBASE, ODBASE, UDATADR	\n \
UDATA2		0x##hex_cmd_byte                


// A cycle during which a address byte is latched
#define SEND_ADDR( hex_cmd_byte )			\n \
PINFUNC1	PS_DATA_IN, TS_ADDR				\n \
CHIPS1		CHIPS_ADDR						\n \
DATGEN1		SUDATA, DBASE, ODBASE, UDATADR	\n \
UDATA1		0x##hex_cmd_byte                \n \
PINFUNC2	PS_DATA_IN, TS_IDLE				\n \
CHIPS2		CHIPS_ADDR

// A cycle during which a address byte is latched
// byte is sourced from address generator xmain or ymain
#define SEND_ADDR_PS( ag_reg, ps_arg )		\n \
PINFUNC1	ps_arg, TS_ADDR					\n \
CHIPS1		CHIPS_ADDR						\n \
PINFUNC2	ps_arg, TS_IDLE					\n \
CHIPS2		CHIPS_ADDR						

// send column address 1
#define SEND_COL1( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_COL1 )				\n \
YALU(ag_reg, ag_reg)

// send column address 1 using aux as source
#define SEND_AUX_COL1( aux_reg, ag_reg )	\n \
SEND_ADDR_PS( ag_reg, PS_COL1 )				\n \
YALU_AUX(aux_reg, ag_reg)

// send column address 2
#define SEND_COL2( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_COL2 )				\n \
YALU(ag_reg, ag_reg)

// send row address 1
#define SEND_ROW1( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_ROW1 )				\n \
XALU(ag_reg, ag_reg)

// send row address 2
#define SEND_ROW2( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_ROW2 )				\n \
XALU(ag_reg, ag_reg)

// send row address 3
#define SEND_ROW3( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_ROW3 )				\n \
XALU(ag_reg, ag_reg)

// send row address 4
#define SEND_ROW4( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_ROW4 )				\n \
XALU(ag_reg, ag_reg)						\n \
ZALU(ag_reg, ag_reg)						

// send row address 5
#define SEND_ROW5( ag_reg )					\n \
SEND_ADDR_PS( ag_reg, PS_ROW5 )				\n \
XALU(ag_reg, ag_reg)

// send row address 1 using aux as source
#define SEND_AUX_ROW1( aux_reg, ag_reg )	\n \
SEND_ADDR_PS( ag_reg, PS_ROW1 )				\n \
XALU_AUX(aux_reg, ag_reg)

// Send address from XMAIN
#define SEND_ADDR_XMAIN						\n \
SEND_ADDR_PS( MAIN, PS_ROW1 )				\n \
XALU(MAIN, MAIN)					

// Send address from YMAIN
#define SEND_ADDR_YMAIN						\n \
SEND_ADDR_PS( MAIN, PS_COL1 )				\n \
YALU(MAIN, MAIN)

// Send address from AUX1 through XMAIN
#define SEND_AUX_ADDR( aux_reg )			\n \
PINFUNC1	PS_ROW1, TS_ADDR				\n \
CHIPS1		CHIPS_ADDR						\n \
XALU1		AUXA, DXMAIN, OXMAIN			\n \
AUXREG1		aux_reg, HOLD, aux_reg			\n \
PINFUNC2	PS_ROW1, TS_IDLE				\n \
CHIPS2		CHIPS_ADDR						

// Send address from JAM RAM
#define SEND_ADDR_JAMRAM					\n \
PINFUNC1	PS_DATA_IN, TS_ADDR 			\n \
CHIPS1		CHIPS_ADDR						\n \
DATGEN1     OJAM, JAMRAMINCR                \n \
PINFUNC2	PS_DATA_IN, TS_IDLE				\n \
CHIPS2		CHIPS_ADDR				        \n \
DATGEN2     OJAM, JAMRAMINCR                

// Assert CE# and de-assert WE#
// RE_t to low and RE_c to high										      
#define READ_PREAMBLE										\n \
PINFUNC1	PS_DATA_OUT_PREAMBLE, TS_READ_PREAMBLE_1, ADHIZ	\n \
CHIPS1		CHIPS_DATA_DQSRH								\n \
PINFUNC2	PS_DATA_OUT, TS_READ_PREAMBLE_2, ADHIZ			\n \
CHIPS2		CHIPS_DATA_OUT									\n \

// Assert CE# and de-assert WE#
// DQS_t to low and DQS_c to high                   
#define WRITE_PREAMBLE										\n \
PINFUNC1	PS_DATA_IN_PREAMBLE, TS_WRITE_PREAMBLE_1		\n \
CHIPS1		CHIPS_DATA_IN									\n \
PINFUNC2	PS_DATA_IN,	TS_WRITE_PREAMBLE_2					\n \
CHIPS2		CHIPS_DATA_IN

// tPRST                 
#define READ_POSTAMBLE										\n \
PINFUNC1	PS_DATA_OUT,		TS_READ_POSTAMBLE, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT									\n \
PINFUNC2	PS_DATA_OUT, TS_IDLE, ADHIZ						\n \
CHIPS2		CHIPS_DATA_OUT									\n \

// tWPST                 
#define WRITE_POSTAMBLE										\n \
PINFUNC1	PS_DATA_IN_PREAMBLE, TS_WRITE_POSTAMBLE			\n \
CHIPS1		CHIPS_DATA_IN									\n \
PINFUNC2	PS_DATA_IN_PREAMBLE, TS_IDLE					\n \
CHIPS2		CHIPS_DATA_IN									\n \

// Create a tDSC cycle
// PINFUNC1 - DQS_t/c toggle
// PINFUNC2 - no-op
#define WRITE_ONE_BURST						\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN_x2			\n \
CHIPS1		CHIPS_DATA_IN					\n \
PINFUNC2	PS_DATA_IN, TS_IDLE				\n \
CHIPS2		CHIPS_DATA_IN					

// Create a tRC cycle
// PINFUNC1 - RE_t/c toggle
// PINFUNC2 - no-op
#define READ_ONE_BURST							\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x2, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT						\n \
PINFUNC2	PS_DATA_OUT, TS_IDLE, ADHIZ			\n \
CHIPS2		CHIPS_DATA_OUT		

// strobe RB
// PINFUNC1 - RE_t/c toggle
// PINFUNC2 - no-op
#define POLL_RBSY							\n \
PINFUNC1	PS_POLL_RBSY, TS_RBSY, ADHIZ	\n \
CHIPS1		CHIPS_IDLE						\n \
PINFUNC2	PS_DATA_OUT, TS_IDLE, ADHIZ		\n \
CHIPS2		CHIPS_IDLE						

// Read Byte0, Byte1, Byte2, Byte3, and Byte4
// PINFUNC1 - Byte0, Byte1, Byte2, Byte3
// PINFUNC2 - Byte4, Byte5
#define DATA_OUT_READ_ID_90						\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT, ADHIZ		\n \
CHIPS1		CHIPS_DATA_OUT						\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT							

// Read Byte0, Byte1, Byte2, Byte3, and Byte4
// PINFUNC1 - Byte0 and Byte1
// PINFUNC2 - Byte2, Byte3, and Byte4
#define DATA_OUT_READ_ID_90_x4					\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT					\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT	

// Read Byte0, Byte1, Byte2, Byte3, and Byte4
// PINFUNC1 - Byte0 and Byte1
// PINFUNC2 - Byte2, Byte3, and Byte4
#define DATA_OUT_READ_ID_90_AUX1			\n \
DATA_OUT_READ_ID_90							\n \
DATGEN1SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN1SUB2	AUXA, DMAIN, ODMAIN				\n \
AUXREG1		AUX1, HOLD, AUX1				\n \
DATGEN2SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN2SUB2	AUXA, DMAIN, ODMAIN				\n \
DATGEN2SUB3	AUXA, DMAIN, ODMAIN				\n \
AUXREG2		AUX1, HOLD, AUX1	

// Read Byte0, Byte1, Byte2, Byte3, and Byte4
// PINFUNC1 - Byte0 and Byte1
// PINFUNC2 - Byte2, Byte3
//BOCx4
#define DATA_OUT_READ_ID_90_DMAIN_x4			\n \
DATA_OUT_READ_ID_90_x4							\n \
DATGEN1SUB1	DMAIN, ODMAIN					\n \
DATGEN1SUB2	DMAIN, ODMAIN					\n \
DATGEN1SUB3	DMAIN, ODMAIN					\n \
DATGEN1SUB4	DMAIN, ODMAIN					\n \
DATGEN2SUB1	DMAIN, ODMAIN					\n \
DATGEN2SUB2	DMAIN, ODMAIN					\n \
DATGEN2SUB3	DMAIN, ODMAIN					\n \
DATGEN2SUB4	DMAIN, ODMAIN					

// Read Byte0, Byte1, Byte2, Byte3, and Byte4
// PINFUNC1 - Byte0 and Byte1
// PINFUNC2 - Byte2, Byte3
//BOCx8
#define DATA_OUT_READ_ID_90_DMAIN			\n \
DATA_OUT_READ_ID_90							\n \
DATGEN1SUB1	DMAIN, ODMAIN					\n \
DATGEN1SUB2	DMAIN, ODMAIN					\n \
DATGEN1SUB3	DMAIN, ODMAIN					\n \
DATGEN1SUB4	DMAIN, ODMAIN					\n \
DATGEN2SUB1	DMAIN, ODMAIN					\n \
DATGEN2SUB2	DMAIN, ODMAIN					\n \
DATGEN2SUB3	DMAIN, ODMAIN					\n \
DATGEN2SUB4	DMAIN, ODMAIN	

// Set P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2
// PINFUNC2 - P3 and P4
#define SET_SUBFEATURES_JAMRAM				\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN_x4		\n \
CHIPS1		CHIPS_DATA_IN					\n \
DATGEN1     OJAM, JAMRAMINCR                \n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN_x4		\n \
CHIPS2		CHIPS_DATA_IN                   \n \
DATGEN2     OJAM, JAMRAMINCR                

// Get P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2
// PINFUNC2 - P3 and P4
#define GET_SUBFEATURES_JAMRAM					\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT					    \n \
DATGEN1     OJAM, JAMRAMINCR                    \n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT                      \n \
DATGEN2     OJAM, JAMRAMINCR

// Set P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2
// PINFUNC2 - P3 and P4
#define SET_SUBFEATURES						\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN_x4		\n \
CHIPS1		CHIPS_DATA_IN					\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN_x4		\n \
CHIPS2		CHIPS_DATA_IN

// Get P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2
// PINFUNC2 - P3 and P4
#define GET_SUBFEATURES					    	\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT					    \n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT

// Set P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2 sourced from DATGEN1SUB1/DATGEN1SUB2 dbase
// PINFUNC2 - P3 and P4 sourced from DATGEN2SUB1/DATGEN2SUB2 dbase
#define SET_SUBFEATURES_DMAIN				\n \
SET_SUBFEATURES								\n \
DATGEN( DMAIN, DMAIN )

// Get P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 and P2 sourced from DATGEN1SUB1/DATGEN1SUB2 dbase
// PINFUNC2 - P3 and P4 sourced from DATGEN2SUB1/DATGEN2SUB2 dbase
#define GET_SUBFEATURES_DMAIN				\n \
GET_SUBFEATURES								\n \
DATGEN( DMAIN, DMAIN )

// Set P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 sourced from DATGEN1SUB1 aux2
// PINFUNC1 - P2 sourced from DATGEN1SUB2 aux3
// PINFUNC2 - P3 sourced from DATGEN2SUB1 aux2
// PINFUNC2 - P4 sourced from DATGEN2SUB2 aux3
#define SET_SUBFEATURES_AUX				\n \
SET_SUBFEATURES								\n \
DATGEN1SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN1SUB2	AUXA, DMAIN, ODMAIN				\n \
AUXREG1		AUX2, HOLD, AUX3				\n \
DATGEN2SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN2SUB2	AUXA, DMAIN, ODMAIN				\n \
AUXREG2		AUX2, HOLD, AUX3

// Get P1, P2, P3, P4 subfeatures
// Must update timings before executing this macro
// PINFUNC1 - P1 sourced from DATGEN1SUB1 aux2
// PINFUNC1 - P2 sourced from DATGEN1SUB2 aux3
// PINFUNC2 - P3 sourced from DATGEN2SUB1 aux2
// PINFUNC2 - P4 sourced from DATGEN2SUB2 aux3
#define GET_SUBFEATURES_AUX				\n \
GET_SUBFEATURES								\n \
DATGEN1SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN1SUB2	AUXA, DMAIN, ODMAIN				\n \
AUXREG1		AUX2, HOLD, AUX3				\n \
DATGEN2SUB1	AUXA, DMAIN, ODMAIN				\n \
DATGEN2SUB2	AUXA, DMAIN, ODMAIN				\n \
AUXREG2		AUX2, HOLD, AUX3				

// Write 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define DQS_TOGGLE_16							\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN				\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN				\n \
CHIPS2		CHIPS_DATA_IN						\n \
DATGEN1		BCKFEN								\n \
DATGEN2		BCKFEN								\n \

// Write 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
// use INVSNS
#define DQS_TOGGLE_16_INV							\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN				\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN				\n \
CHIPS2		CHIPS_DATA_IN						\n \
DATGEN1		BCKFEN, INVSNS						\n \
DATGEN2		BCKFEN, INVSNS						\n \

// Write 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define DQS_TOGGLE_16_DBI						\n \
PINFUNC1	PS_DATA_IN_DBI, TS_DATA_IN			\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DATA_IN_DBI, TS_DATA_IN			\n \
CHIPS2		CHIPS_DATA_IN						\n \
DATGEN1		BCKFEN								\n \
DATGEN2		BCKFEN								\n \

// Write 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
// use dbm ps dsm_2
#define DQS_TOGGLE_16_DBM						\n \
PINFUNC1	PS_DBM_IN, TS_DATA_IN, STV			\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DBM_IN, TS_DATA_IN				\n \
CHIPS2		CHIPS_DATA_IN	

// Write 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
// uses LFSR pin scramble(change every ui)
#define DQS_TOGGLE_16_LFSR						\n \
PINFUNC1	PS_DATA_IN_LFSR, TS_DATA_IN			\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DATA_IN_LFSR, TS_DATA_IN			\n \
CHIPS2		CHIPS_DATA_IN	

// Write 16 bytes with ebm
// hold ebm counter
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define DQS_TOGGLE_16_EBM( op )													\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN, EBM_PS1, EBM_ENABLE, CONCAT2( EBM_, op)		\n \
CHIPS1		CHIPS_DATA_IN														\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN												\n \
CHIPS2		CHIPS_DATA_IN	

// Write 8 bytes
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define DQS_TOGGLE_8							\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN_x4			\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN_x4			\n \
CHIPS2		CHIPS_DATA_IN	

// Write 8 bytes
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define DQS_TOGGLE_8_DBM						\n \
PINFUNC1	PS_DBM_IN, TS_DATA_IN_x4			\n \
CHIPS1		CHIPS_DATA_IN						\n \
PINFUNC2	PS_DBM_IN, TS_DATA_IN_x4			\n \
CHIPS2		CHIPS_DATA_IN	

// Write 8 bytes with ebm
// hold ebm counter
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define DQS_TOGGLE_8_EBM( op )													\n \
PINFUNC1	PS_DATA_IN, TS_DATA_IN_x4, EBM_PS1, EBM_ENABLE, CONCAT2( EBM_, op)	\n \
CHIPS1		CHIPS_DATA_IN														\n \
PINFUNC2	PS_DATA_IN, TS_DATA_IN_x4											\n \
CHIPS2		CHIPS_DATA_IN	

//write 16 bytes
//data is sourced is from aux register
//odmain is used as output
#define DQS_TOGGLE_AUX(dg_reg)				\n \
DQS_TOGGLE_16									\n \
DATGEN_AUX(dg_reg, MAIN)

#define DQS_TOGGLE_INC_JAMRAM				\n \
DQS_TOGGLE_16									\n \
INC_JAMRAM_ADDR								\n \

// read 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define RE_TOGGLE_16								\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT, ADHIZ, STV	\n \
CHIPS1		CHIPS_DATA_OUT							\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT, ADHIZ			\n \
CHIPS2		CHIPS_DATA_OUT							\n \
DATGEN1		BCKFEN									\n \
DATGEN2		BCKFEN									\n \

// read 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define RE_TOGGLE_16_VP								\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT, ADHIZ, STV, VPULSE	\n \
CHIPS1		CHIPS_DATA_OUT							\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT, ADHIZ			\n \
CHIPS2		CHIPS_DATA_OUT							\n \
DATGEN1		BCKFEN									\n \
DATGEN2		BCKFEN									\n \

// read 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define RE_TOGGLE_16_INV						\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT					\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT					\n \
DATGEN1		BCKFEN, INVSNS					\n \
DATGEN2		BCKFEN, INVSNS					\n \

// read 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
// use dbm ps dsm_2
#define RE_TOGGLE_16_DBM					\n \
PINFUNC1	PS_DBM_OUT, TS_DATA_OUT, ADHIZ, STV	\n \
CHIPS1		CHIPS_DATA_OUT					\n \
PINFUNC2	PS_DBM_OUT, TS_DATA_OUT, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT	

// read 16 bytes
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
// uses LFSR pin scramble(change every ui)
#define RE_TOGGLE_16_LFSR							\n \
PINFUNC1	PS_DATA_OUT_LFSR, TS_DATA_OUT, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT							\n \
PINFUNC2	PS_DATA_OUT_LFSR, TS_DATA_OUT, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT	

// read 16 bytes using ebm
// PINFUNC1 - 8 bytes
// PINFUNC2 - 8 bytes
#define RE_TOGGLE_16_EBM( op )															\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT, ADHIZ, EBM_PS1, EBM_ENABLE, CONCAT2( EBM_, op)	\n \
CHIPS1		CHIPS_DATA_OUT																\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT, ADHIZ												\n \
CHIPS2		CHIPS_DATA_OUT																\n \

// read 8 bytes
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define RE_TOGGLE_8								\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT						\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT	

// read 8 bytes
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define RE_TOGGLE_8_DBM								\n \
PINFUNC1	PS_DBM_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS1		CHIPS_DATA_OUT						\n \
PINFUNC2	PS_DBM_OUT, TS_DATA_OUT_x4, ADHIZ	\n \
CHIPS2		CHIPS_DATA_OUT	

// read 8 bytes
// PINFUNC1 - 4 bytes
// PINFUNC2 - 4 bytes
#define RE_TOGGLE_8_EBM( op )															\n \
PINFUNC1	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ, EBM_PS1, EBM_ENABLE, CONCAT2( EBM_, op)	\n \
CHIPS1		CHIPS_DATA_OUT																\n \
PINFUNC2	PS_DATA_OUT, TS_DATA_OUT_x4, ADHIZ											\n \
CHIPS2		CHIPS_DATA_OUT	

//read 16 bytes
//data is sourced is from aux register
//odmain is used as output
#define RE_TOGGLE_AUX(dg_reg)			\n \
RE_TOGGLE_16								\n \
DATGEN_AUX(dg_reg, MAIN)				\n \

#define RE_TOGGLE_16_INC_JAMRAM			\n \
RE_TOGGLE_16								\n \
INC_JAMRAM_ADDR							\n \

#endif