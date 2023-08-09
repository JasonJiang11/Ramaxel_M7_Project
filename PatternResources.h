#ifndef _RESERVEDPATTERNRESOURCES_H_
#define _RESERVEDPATTERNRESOURCES_H_

#include "MacroUtils.h" // CONCAT()

//////////////////////////////////////////////////////////////////////////

// Timing Sets

//////////////////////////////////////////////////////////////////////////

// Reserved Timing Set numbers
#define TS_IDLE						TSET1 
#define TS_DATA_OUT					TSET2 
#define TS_DATA_IN					TSET3 
#define TS_DATA_IN_x2				TSET5
#define TS_DATA_IN_x4				TSET6
#define TS_DATA_OUT_x2				TSET7
#define TS_DATA_OUT_x4				TSET8
#define TS_DATA_OUT_x5				TSET9
#define TS_BUS_IDLE_WAIT			TSET10
#define TS_CMD						TSET11 
#define TS_ADDR						TSET12
#define TS_READ_PREAMBLE_1			TSET13
#define TS_READ_PREAMBLE_2			TSET14 
#define TS_WRITE_PREAMBLE_1			TSET15 
#define TS_WRITE_PREAMBLE_2			TSET16 
#define TS_RBSY						TSET17
#define TS_READ_POSTAMBLE			TSET18
#define TS_WRITE_POSTAMBLE			TSET19
#define TS_IDLE_DQZ					TSET20 
#define TS_BUS_IDLE_WAIT_DQS_RST	TSET21
#define TS_LEVELSET					TSET22

//////////////////////////////////////////////////////////////////////////

// Pin Scrambles

//////////////////////////////////////////////////////////////////////////

#define	PS_NOOP					PS1
#define PS_DATA_IN				PS2
#define PS_DATA_OUT				PS3
#define PS_DATA_IN_PREAMBLE		PS4
#define PS_DATA_OUT_PREAMBLE	PS5
#define	PS_COL1					PS6
#define	PS_COL2					PS7
#define	PS_ROW1					PS8
#define	PS_ROW2					PS9
#define	PS_ROW3					PS10
#define	PS_ROW4					PS11
#define	PS_ROW5					PS12
#define PS_POLL_RBSY			PS13
#define PS_DATA_IN_LFSR			PS14
#define PS_DATA_OUT_LFSR		PS15
#define PS_DBM_IN				PS16
#define PS_DBM_OUT				PS17
#define PS_DATA_IN_DBI			PS18

//////////////////////////////////////////////////////////////////////////

// CHIPS for Dut Control pins

//////////////////////////////////////////////////////////////////////////

#define	DQSt_L			CS1L
#define	DQSc_L			CS2L
#define	DQSt_H			CS1H
#define	DQSc_H			CS2H
#define	DQSt_RD_L		CS1RDL
#define	DQSc_RD_L		CS2RDL
#define	DQSt_RD_H		CS1RDH
#define	DQSc_RD_H		CS2RDH

#define CE0_L			CS3L					
#define CE1_L			CS4L					
#define CE2_L			CS5L					
#define CE3_L			CS6L	
#define CE0_H			CS3H					
#define CE1_H			CS4H					
#define CE2_H			CS5H					
#define CE3_H			CS6H
#define CE_ALL_L		CE0_L, CE1_L, CE2_L, CE3_L			//all CEs low
#define CE_ALL_H		CE0_H, CE1_H, CE2_H, CE3_H			//all CEs high

#define CLE_L			CS7L
#define CLE_H			CS7H

#define WE_L			CS8L
#define WE_H			CS8H

#define ALE_L			CS9L
#define ALE_H			CS9H

#define REn_L			CS10L
#define REn_H			CS10H

#define REc_L			CS11L
#define REc_H			CS11H

#define WPN_L			CS12L
#define WPN_H			CS12H

//////////////////////////////////////////////////////////////////////////

// AUX REGISTERS FOR ERASE/PROGRAM/READ

//////////////////////////////////////////////////////////////////////////

#define Y_PLANE_0		AUX1
#define X_PLANE_0		AUX2
#define DATA_PLANE_0	AUX3
#define Y_PLANE_1		AUX4
#define X_PLANE_1		AUX5
#define DATA_PLANE_1	AUX6
#define Y_PLANE_2		AUX7
#define X_PLANE_2		AUX8
#define DATA_PLANE_2	AUX9
#define Y_PLANE_3		AUX10
#define X_PLANE_3		AUX11
#define DATA_PLANE_3	AUX12

#endif _RESERVEDPATTERNRESOURCES_H_