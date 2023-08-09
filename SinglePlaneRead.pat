#include "PatternIncludes.h"

//poll using RB
PATTERN(SUB__POLL_TR)

//the next 2 lines are executed in a loop when polling(branch-on-fail) is disabled by setting a counter(C_POLL_EN) to 1
% RB_or_loop	: BUS_IDLE_WAIT				JUMP_TO_IF_COUNT_NOT_ZERO(C_POLL_EN, InitCnt)											F_NO
% Wait_tR		: BUS_IDLE_WAIT				LOOP(C_tR, Wait_tR)																		F_NO //loop on counter											F_NO
% LoopDone		: BUS_IDLE_WAIT																 RETURN_PATTERN							F_NO

//reset error flag
% InitCnt		: RELOAD_CNT(C_tR)																									F_NO
% BUS_IDLE_WAIT COUNT COUNT50, COUNTUDATA																							F_NO
	UDATA1 0x0
% Reset_err		: BUS_IDLE_WAIT				RESET_ERROR					/*RESET_ERROR = MAR RESET*/									F_NO								
% Pipeline0		: BUS_IDLE_WAIT				LOOP(C_PIPE, Pipeline0)																	F_NO

//poll RB for High
% PollRB		: POLL_RBSY					DATGEN_UDATA(MAIN, 0)		/*read for 0*/												F_NO
					SOFT_ERROR											//MAR NOLATCH
% Pipeline		: BUS_IDLE_WAIT				LOOP( C_PIPE, Pipeline), ERR_ALL														F_NO
% error_eval	: BUS_IDLE_WAIT				JUMP_IF_ERROR(Reset_err_done) COUNT COUNT50, INCR										F_NO
% time_eval		: BUS_IDLE_WAIT				LOOP(C_tR, PollRB)																		F_NO
% Reset_err_done: BUS_IDLE_WAIT				RESET_ERROR																				F_NO
% SubDone		: BUS_IDLE_WAIT																RETURN_PATTERN							F_NO

//poll using read status DQ6
PATTERN(SUB__POLL_Read_status)

% Poll_or_loop	: BUS_IDLE_WAIT				JUMP_TO_IF_COUNT_NOT_ZERO(C_POLL_EN, InitCnt)											F_NO
% Wait_tR		: BUS_IDLE_WAIT				LOOP(C_tR, Wait_tR)																		F_NO
% LoopDone		: BUS_IDLE_WAIT																 RETURN_PATTERN							F_NO

//reset error flag
% InitCnt		: RELOAD_CNT(C_tR)																									F_NO

% Reset_err		: BUS_IDLE_WAIT				RESET_ERROR																				F_NO
% Pipeline0		: BUS_IDLE_WAIT				LOOP(C_PIPE, Pipeline0)																	F_NO

//backup dmain
//poll Read Status
%				  BUS_IDLE_WAIT																										F_NO
% Send0x70		: SEND_CMD(70)																										F_NO

% Wait_tWHR		: BUS_IDLE_WAIT				DG_INIT(MAIN, BASE)								LOOP( C_tWHR, Wait_tWHR )				F_NO
					

% Preamble		: READ_PREAMBLE				DATGEN_UDATA(MAIN, 0)																	F_NO
% PollRS		: READ_ONE_BURST													SOFT_STROBE_MASK(40)							F_NO
% Postamble		: READ_POSTAMBLE			DG_INIT(BASE, MAIN)																		F_NO
//restore dmain
% Pipeline		: BUS_IDLE_WAIT				LOOP( C_PIPE, Pipeline), ERR_ALL														F_NO
% error_eval	: BUS_IDLE_WAIT				JUMP_IF_ERROR(SubDone)																	F_NO
% time_eval		: BUS_IDLE_WAIT				LOOP(C_tR, Send0x70)																	F_NO
% SubDone		: BUS_IDLE_WAIT																RETURN_PATTERN							F_NO


//
// Subroutine to execute read page 
// CMD 00, 5 address cycles, CMD 00, CMD 30
// Column and Row addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
//

PATTERN(SUB__ReadPage)
% Send0x00		: SEND_CMD(00)																								F_NO
% ADDR1			: SEND_COL1(MAIN)																							F_NO
% ADDR2			: SEND_COL2(MAIN)																							F_NO
% ADDR3			: SEND_ROW1(MAIN)																							F_NO
% ADDR4			: SEND_ROW2(MAIN)																							F_NO
% ADDR5			: SEND_ROW3(MAIN)																							F_NO
% Send0x30		: SEND_CMD( 30 )																							F_NO
% Wait_tWB		: BUS_IDLE_WAIT															LOOP( C_tWB, Wait_tWB)				F_NO
% Wait_tR		: BUS_IDLE_WAIT															GOSUB_PATTERN(SUB__POLL_TR)			F_NO
% Wait_tRR		: BUS_IDLE_WAIT															LOOP(C_tRR, Wait_tRR)				F_NO
% SubDone		: BUS_IDLE_WAIT															RETURN_PATTERN						F_NO

//
// Subroutine to execute read page, toggling RE to access all page bytes
// Column and Row addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
// uses BOCx8 mode
//

PATTERN(SUB__ReadPageDataOut)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_16		 														DECR(C_DATA)											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_16		 			YADD(MAIN, MAIN, 16)						LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE 															RETURN_PATTERN																	F_NO

//
// Subroutine to execute read page, toggling RE to access all page bytes
// Column and Row addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
// uses BOCx8 mode
//
PATTERN(SUB__ReadPageDataOut_INV)

% Preamble	   : READ_PREAMBLE																																				F_NO
% DataOut	   : RE_TOGGLE_16_INV		 		YADD(MAIN, MAIN, 16)						LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone	   : READ_POSTAMBLE 															RETURN_PATTERN																	F_NO

PATTERN(SUB__ReadPageDataOut_2)

% Preamble		: READ_PREAMBLE		
% DataOut		: BUS_IDLE_WAIT		 			YADD(MAIN, MAIN, 16)						LOOP(C_DATA, DataOut)									//STROBE_EN
% SubDone		: READ_POSTAMBLE 															RETURN_PATTERN

//
// Subroutine to execute read page, toggling RE to access all page bytes
// Column and Row addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
// uses BOCx4 mode
//
PATTERN(SUB__ReadPageDataOut_x4)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut		: RE_TOGGLE_8					YADD(MAIN, MAIN, 8)							LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx4
PATTERN(SUB__ReadPageDataOut_x4_JAMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_8					DATGEN(JAM, JAM)							DECR(C_DATA)											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_8					INC_JAMRAM_ADDR		YADD(MAIN, MAIN, 8)		LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx8
PATTERN(SUB__ReadPageDataOut_EBMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% EbmEnable		: RE_TOGGLE_16_EBM( HOLD )							YADD(MAIN, MAIN, 8)		DECR(C_DATA )											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_16_EBM( INC )							YADD(MAIN, MAIN, 8)		LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx4
PATTERN(SUB__ReadPageDataOut_x4_EBMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% EbmEnable		: RE_TOGGLE_8_EBM( HOLD )													DECR(C_DATA )											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_8_EBM( INC )							YADD(MAIN, MAIN, 8)		LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx8
PATTERN(SUB__ReadPageDataOut_JAMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_16					DATGEN(JAM, JAM)	YADD(MAIN, MAIN, 16)	DECR(C_DATA)											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_16					INC_JAMRAM_ADDR		YADD(MAIN, MAIN, 16)	LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO


PATTERN(SUB__Hw_training_ReadPageDataOut_JAMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_16					DATGEN(JAM, JAM)							DECR(C_DATA)											STROBE_EN				F1_CAP_ARRAY
//% DataOut		: RE_TOGGLE_16					INC_JAMRAM_ADDR		YADD(MAIN, MAIN, 16)															STROBE_EN				F1_CAP_ARRAY
% DataOut		: RE_TOGGLE_16					INC_JAMRAM_ADDR		YADD(MAIN, MAIN, 16)	LOOP(C_DATA, DataOut)									STROBE_EN				F1_CAP_ARRAY
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO


// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx8
PATTERN(SUB__ReadPageDataOut_JAMINC_DBI)																			

% Enable_DBI	: BUS_IDLE_WAIT_DBI(DBI_WRITE_DISABLE_READ_ENABLE)																														F_NO
% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_16					DATGEN(JAM, JAM)	/*YADD(MAIN, MAIN, 16)*/	DECR(C_DATA)											STROBE_EN			F_CNT //dbi enable uses udata bits
% DataOut		: RE_TOGGLE_16					INC_JAMRAM_ADDR		/*YADD(MAIN, MAIN, 16)*/	LOOP(C_DATA, DataOut)									STROBE_EN			F_CNT //dbi enable uses udata bits
% PostAmble		: READ_POSTAMBLE																																			F_NO
% SubDone		: BUS_IDLE_WAIT_DBI(DBI_WRITE_DISABLE_READ_DISABLE)											RETURN_PATTERN																F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses DBM
// BOCx8
PATTERN(SUB__ReadPageDataOut_DBMINC)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_16_DBM				DATGEN(BUF, BUF)							DECR(C_DATA)											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_16_DBM				DATGEN(BUF, BUF)	YADD(MAIN, MAIN, 16)	LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT

% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

// Subroutine to execute read page, toggling RE to access all page bytes
// uses DBM
// BOCx8
PATTERN(SUB__ReadPageDataOut_DBMINC_x4)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut_0		: RE_TOGGLE_8_DBM				DATGEN(BUF, BUF)							DECR(C_DATA)											STROBE_EN				F_CNT
% DataOut		: RE_TOGGLE_8_DBM				DATGEN(BUF, BUF)	YADD(MAIN, MAIN, 8)		LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT

% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO


// Subroutine to execute read page, toggling RE to access all page bytes
// uses jamram
// BOCx8
PATTERN(SUB__ReadPageDataOut_LFSR)

% Preamble		: READ_PREAMBLE																																				F_NO
% DataOut		: RE_TOGGLE_16_LFSR				LFSR(1, ADVANCE)	YADD(MAIN, MAIN, 16)	LOOP(C_DATA, DataOut)									STROBE_EN				F_CNT
% SubDone		: READ_POSTAMBLE															RETURN_PATTERN																	F_NO

///////////////////////////////////////////////////////////////////////////////////////////////////

// Read one page at a given block and plane address, on given LUN
// Support SLC and TLC

///////////////////////////////////////////////////////////////////////////////////////////////////

//Read page in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//AUX register DATA_PLANE_0 has byte data to read from page
//BOCx8
PATTERN( PAT__ReadOnePage_SinglePlane )

% AssertCEn		: BUS_IDLE_WAIT
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA		: SEND_CMD( DA )															
% Wait_tSLC		: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

% Read_Page		: BUS_IDLE_WAIT_DQS_RST			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__ReadPage)
% DataOut		: BUS_IDLE_WAIT					DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut)

% Next_Page		: READ_POSTAMBLE				AUXINC(X_PLANE_0)

% End			: READ_POSTAMBLE														END_PATTERN

//Read page in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//AUX register DATA_PLANE_0 has byte data to read from page
//BOCx4
PATTERN( PAT__ReadOnePage_SinglePlane_x4 )
@{
count(2, 0);
@}

% AssertCEn		: BUS_IDLE_WAIT
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode : BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA : SEND_CMD(DA)
% Wait_tSLC : BUS_IDLE_WAIT															LOOP(C_tSLC, Wait_tSLC)

% Read_Page : BUS_IDLE_WAIT					XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__ReadPage)
% DataOut : BUS_IDLE_WAIT					DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut_x4)

% Next_Page : READ_POSTAMBLE				AUXINC(X_PLANE_0)

% End : READ_POSTAMBLE														END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Read block in SLC mode using DBM as expect data
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte expect data to read from blk
//BOCx8
PATTERN(PAT__Read_Blk_SP)
@{
apg_ymain_set(1, 0);
apg_ymain_set(2, 8);
@}

%				  BUS_IDLE_WAIT
%				  BUS_IDLE_WAIT
%				  BUS_IDLE_WAIT

% AssertCEn		: BUS_IDLE_WAIT					XALU_AUX(X_PLANE_0, MAIN)
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA		: SEND_CMD( DA )															
% Wait_tSLC		: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

% Read_Page		: BUS_IDLE_WAIT_DQS_RST			YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__ReadPage)
% DataOut		: BUS_IDLE_WAIT					DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut)

% Next_Page		: READ_POSTAMBLE				XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)

% End			: READ_POSTAMBLE														END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Read block in SLC mode using DBM as expect data
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte expect data to read from blk
//BOCx4
PATTERN(PAT__Read_Blk_SP_BOCx4)
@{
apg_ymain_set(1, 0);
apg_ymain_set(2, 8);
@}

%				  BUS_IDLE_WAIT
%				  BUS_IDLE_WAIT
%				  BUS_IDLE_WAIT

% AssertCEn		: BUS_IDLE_WAIT					XALU_AUX(X_PLANE_0, MAIN)
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA		: SEND_CMD( DA )															
% Wait_tSLC		: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

% Read_Page		: BUS_IDLE_WAIT_DQS_RST			YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__ReadPage)
% DataOut		: BUS_IDLE_WAIT					DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut_x4)

% Next_Page		: READ_POSTAMBLE				XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)

% End			: READ_POSTAMBLE														END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Read block in SLC mode using DBM as expect data
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte expect data to read from blk
//BOCx8
PATTERN( PAT__Read_RND_Blk_SP )

% AssertCEn		: BUS_IDLE_WAIT					XALU_AUX(X_PLANE_0, MAIN)				RESET_ERROR
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA		: SEND_CMD( DA )															
% Wait_tSLC		: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

% Read_Page		: BUS_IDLE_WAIT_DQS_RST			YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__ReadPage)
% DataOut		: BUS_IDLE_WAIT															GOSUB_PATTERN(SUB__ReadPageDataOut_DBMINC)

% Next_Page		: READ_POSTAMBLE				XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)

% End			: READ_POSTAMBLE														END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Read block in SLC mode using DBM as expect data
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte expect data to read from blk
//BOCx4
PATTERN( PAT__Read_RND_Blk_SP_BOCx4)

% AssertCEn		: BUS_IDLE_WAIT					XALU_AUX(X_PLANE_0, MAIN)				RESET_ERROR
//if SLC mode, execute DA command, otherwise skip to label Read_Page
% Check_SLC_Mode: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Read_Page)

// enable SLC mode
% Send0xDA		: SEND_CMD( DA )															
% Wait_tSLC		: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

% Read_Page		: BUS_IDLE_WAIT_DQS_RST			YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__ReadPage)
% DataOut		: BUS_IDLE_WAIT															GOSUB_PATTERN(SUB__ReadPageDataOut_DBMINC_x4)

% Next_Page		: READ_POSTAMBLE				XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)

% End			: READ_POSTAMBLE														END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////


