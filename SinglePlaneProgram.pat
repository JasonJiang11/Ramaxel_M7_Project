#include "PatternIncludes.h"

//Polling subroutine for Program.
//This is executed after the Program command and polls on the RB

PATTERN(SUB__POLL_TPROG)

% RB_or_loop	: BUS_IDLE_WAIT			JUMP_TO_IF_COUNT_NOT_ZERO(C_POLL_EN, InitCnt)	
% Wait_tPROG	: BUS_IDLE_WAIT			LOOP(C_tPROG, Wait_tPROG)
% LoopDone		: BUS_IDLE_WAIT															RETURN_PATTERN

% InitCnt		: RELOAD_CNT(C_tPROG)														

//reset error flag
% Reset_err		: BUS_IDLE_WAIT			RESET_ERROR										
% Pipeline0		: BUS_IDLE_WAIT			LOOP(C_PIPE, Pipeline0)

//poll RB for High
% PollRB		: POLL_RBSY				DATGEN_UDATA(MAIN, 0)	
					SOFT_ERROR
% Pipeline		: BUS_IDLE_WAIT			LOOP( C_PIPE, Pipeline), ERR_ALL
% error_eval	: BUS_IDLE_WAIT			JUMP_IF_ERROR(Reset_err_done)
% time_eval		: BUS_IDLE_WAIT			LOOP(C_tPROG, PollRB)
% Reset_err_done: BUS_IDLE_WAIT			RESET_ERROR										
% SubDone		: BUS_IDLE_WAIT															RETURN_PATTERN

//////////////////////////////////////////////////////////////////////////////////////////////////
// Subroutine to program one page
// column and page address are sourced from x/ymain
// data bytes are sourced from aux3
// uses BOCx8 mode
//////////////////////////////////////////////////////////////////////////////////////////////////

PATTERN(SUB__PROG_PAGE)

% Send0x80		: SEND_CMD( 80 )																			F_NO																			
% ADDR1			: SEND_COL1( MAIN )																			F_NO
% ADDR2			: SEND_COL2( MAIN )																			F_NO
% ADDR3			: SEND_ROW1( MAIN )																			F_NO
% ADDR4			: SEND_ROW2( MAIN )																			F_NO
% ADDR5			: SEND_ROW3( MAIN )																			F_NO
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)			F_NO
% Preamble		: WRITE_PREAMBLE																			F_NO
			
//uses dmain for payload
% DataIn		: DQS_TOGGLE_16											LOOP( C_DATA, DataIn)				F_NO

% Wait_tWPST	: WRITE_POSTAMBLE																			F_NO						

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN						F_NO

//////////////////////////////////////////////////////////////////////////////////////////////////
// Subroutine to program one page
// column and page address are sourced from x/ymain
// data bytes are sourced from aux3
// uses BOCx8 mode
// use INVSNS 
//////////////////////////////////////////////////////////////////////////////////////////////////

PATTERN(SUB__PROG_PAGE_INV)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses dmain for payload
% DataIn		: DQS_TOGGLE_16_INV										LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//////////////////////////////////////////////////////////////////////////////////////////////////
// Subroutine to program one page
// column and page address are sourced from x/ymain
// data bytes are sourced from aux3
// uses BOCx4 mode
//////////////////////////////////////////////////////////////////////////////////////////////////

PATTERN(SUB__PROG_PAGE_x4)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses dmain for payload
% DataIn		: DQS_TOGGLE_8											LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//this subroutine uses the jamram in BOCx4
PATTERN(SUB__PROG_PAGE_x4_JAMINC)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses dmain for payload
% DataIn_0		: DQS_TOGGLE_8		DATGEN(JAM, JAM)				 	DECR( C_DATA )
% DataIn		: DQS_TOGGLE_8		INC_JAMRAM_ADDR 					LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//this subroutine uses the jamram in BOCx4
PATTERN(SUB__PROG_PAGE_x4_EBMINC)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses dmain for payload
% EbmEnable		: DQS_TOGGLE_8_EBM( HOLD )		 						DECR( C_DATA )
% DataIn		: DQS_TOGGLE_8_EBM( INC )		 						LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//this subroutine uses the jamram in BOCx8
PATTERN(SUB__PROG_PAGE_EBMINC)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses dmain for payload
% EbmEnable		: DQS_TOGGLE_16_EBM( HOLD )		 						DECR( C_DATA )
% DataIn		: DQS_TOGGLE_16_EBM( INC )		 						LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//this subroutine uses the jamram in BOCx8
PATTERN(SUB__PROG_PAGE_JAMINC)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses jamram for payload
% DataIn_0		: DQS_TOGGLE_16		DATGEN(JAM, JAM)					DECR( C_DATA )
% DataIn		: DQS_TOGGLE_16		INC_JAMRAM_ADDR 					LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN

//this subroutine uses the jamram in BOCx8
//enable DBI
PATTERN(SUB__PROG_PAGE_JAMINC_DBI)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT_DBI(DBI_WRITE_ENABLE_READ_DISABLE)					LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses jamram for payload
% DataIn_0		: DQS_TOGGLE_16			DATGEN(JAM, JAM)				DECR(C_DATA )     
% DataIn		: DQS_TOGGLE_16			INC_JAMRAM_ADDR 				LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT_DBI(DBI_WRITE_DISABLE_READ_DISABLE)					RETURN_PATTERN

//this subroutine uses the DBM in BOCx8
PATTERN(SUB__PROG_PAGE_DBMINC)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT														LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses jamram for payload
% DataIn_0		: DQS_TOGGLE_16_DBM								DATGEN(BUF, BUF) 	DECR( C_DATA)
% DataIn		: DQS_TOGGLE_16_DBM		YADD(MAIN, MAIN, 16)	DATGEN(BUF, BUF) 	LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT														RETURN_PATTERN

//this subroutine uses the DBM in BOCx4
PATTERN(SUB__PROG_PAGE_DBMINC_x4)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT														LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses jamram for payload
% DataIn_0		: DQS_TOGGLE_8_DBM								DATGEN(BUF, BUF) 	DECR( C_DATA)
% DataIn		: DQS_TOGGLE_8_DBM		YADD(MAIN, MAIN, 8)		DATGEN(BUF, BUF) 	LOOP( C_DATA, DataIn)

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT														RETURN_PATTERN

//this subroutine uses the jamram in BOCx8
PATTERN(SUB__PROG_PAGE_LFSR)

% Send0x80		: SEND_CMD( 80 )																			
% ADDR1			: SEND_COL1( MAIN )
% ADDR2			: SEND_COL2( MAIN )
% ADDR3			: SEND_ROW1( MAIN )
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)
% Preamble		: WRITE_PREAMBLE										

//uses lfsr for payload
% DataIn		: DQS_TOGGLE_16_LFSR		LFSR(1, ADVANCE)			LOOP( C_DATA, DataIn)   //LFSR1_ADVANCE

% Wait_tWPST	: WRITE_POSTAMBLE										

% SubDone		: BUS_IDLE_WAIT											RETURN_PATTERN



///////////////////////////////////////////////////////////////////////////////////////////////////

// Programs one page at a given block and plane address, on given LUN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program page in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//AUX register DATA_PLANE_0 has byte data to write to page
//BOCx8

PATTERN( PAT__ProgramOnePage_SinglePlane_SLC )
@{
@}

% AssertCEn		: BUS_IDLE_WAIT
// enable SLC mode
% Send0xDA		: SEND_CMD( DA )																		
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP( C_tSLC, Wait_tSLC )				
	
% LoadData		: BUS_IDLE_WAIT		DATGEN_AUX(DATA_PLANE_0, MAIN)
% ProgPage		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10		: SEND_CMD( 10 )																				
% Wait_tWB		: WRITE_POSTAMBLE											LOOP( C_tWB, Wait_tWB )						
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)

% End			: WRITE_POSTAMBLE											END_PATTERN

//Program page in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//AUX register DATA_PLANE_0 has byte data to write to page
//BOCx4

PATTERN(PAT__ProgramOnePage_SinglePlane_SLC_x4)
@{
count(2, 0);
@}

% AssertCEn		: BUS_IDLE_WAIT
// enable SLC mode
% Send0xDA		: SEND_CMD(DA)
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP(C_tSLC, Wait_tSLC)

% LoadData		: BUS_IDLE_WAIT		DATGEN_AUX(DATA_PLANE_0, MAIN)
% ProgPage		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_x4)
% Send0x10		: SEND_CMD(10)
% Wait_tWB		: WRITE_POSTAMBLE											LOOP(C_tWB, Wait_tWB)
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)

% End			: WRITE_POSTAMBLE											END_PATTERN//Program page in SLC mode

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Programs one page at a given block and plane address, on given LUN
///////////////////////////////////////////////////////////////////////////////////////////////////

PATTERN(PAT__ProgramOnePage_SinglePlane_TLC)
@{
@}

% AssertCEn		: BUS_IDLE_WAIT				

//lower page
% LoadData		: BUS_IDLE_WAIT		DATGEN_AUX(DATA_PLANE_0, MAIN)
//call subroutine SUB__PROG_PAGE and init xmain/ymain with aux registers referenced by X_PLANE_0/Y_PLANE_0
% ProgLP		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
% LP_Send0x1A	: SEND_CMD( 1A )																			
% LP_Wait_tWB	: WRITE_POSTAMBLE											LOOP( C_tWB, LP_Wait_tWB )
% LP_Wait_tPROG	: WRITE_POSTAMBLE											LOOP(C_tPROG, LP_Wait_tPROG)

//increment aux register X_PLANE_0
% Mid_Page		: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

//mid page
% ProgMP		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__PROG_PAGE)
% MP_Send0x10	: SEND_CMD( 1A )																				
% MP_Wait_tWB	: WRITE_POSTAMBLE											LOOP( C_tWB, MP_Wait_tWB )
% MP_Wait_tPROG	: WRITE_POSTAMBLE											LOOP(C_tPROG, MP_Wait_tPROG)

//upper page
//increment aux register X_PLANE_0
% Upper_Page	: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

//upper page
% ProgUP		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__PROG_PAGE)
% UP_Send0x10	: SEND_CMD( 10 )																					
% UP_Wait_tWB	: WRITE_POSTAMBLE											LOOP( C_tWB, UP_Wait_tWB )
% UP_Wait_tPROG	: WRITE_POSTAMBLE											LOOP( C_tPROG, UP_Wait_tPROG)

% Next_Page		: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

% End			: WRITE_POSTAMBLE											END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program single block in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte data to write to blk
//BOCx8

PATTERN( PAT__Prog_Blk_SP_SLC )
@{
@}

% AssertCEn		: BUS_IDLE_WAIT		XALU_AUX(X_PLANE_0, MAIN)
// enable SLC mode
% Send0xDA		: SEND_CMD( DA )																		
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP( C_tSLC, Wait_tSLC )		

% LoadData		: BUS_IDLE_WAIT		DATGEN_AUX(DATA_PLANE_0, MAIN)
% ProgPage		: BUS_IDLE_WAIT		YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10		: SEND_CMD( 10 )																				
% Wait_tWB		: WRITE_POSTAMBLE											LOOP( C_tWB, Wait_tWB )						
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)
% Next_Page		: BUS_IDLE_WAIT		XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)
% End			: WRITE_POSTAMBLE											END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program single block in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte data to write to blk
//BOCx4

PATTERN( PAT__Prog_Blk_SP_SLC_BOCx4 )
@{
@}

% AssertCEn		: BUS_IDLE_WAIT		XALU_AUX(X_PLANE_0, MAIN)
// enable SLC mode
% Send0xDA		: SEND_CMD( DA )																		
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP( C_tSLC, Wait_tSLC )		

% LoadData		: BUS_IDLE_WAIT		DATGEN_AUX(DATA_PLANE_0, MAIN)
% ProgPage		: BUS_IDLE_WAIT		YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__PROG_PAGE_x4)
% Send0x10		: SEND_CMD( 10 )																				
% Wait_tWB		: WRITE_POSTAMBLE											LOOP( C_tWB, Wait_tWB )						
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)
% Next_Page		: BUS_IDLE_WAIT		XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)
% End			: WRITE_POSTAMBLE											END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program single block from DBM in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte data to write to blk
//BOCx8

PATTERN( PAT__Prog_RND_Blk_SP_SLC )
@{
apg_xmain_set(1, 0);
apg_xmain_set(2, 0);
@}

% AssertCEn		: BUS_IDLE_WAIT		XALU_AUX(X_PLANE_0, MAIN)				RESET_ERROR
// enable SLC mode
% Send0xDA		: SEND_CMD( DA )																		
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP( C_tSLC, Wait_tSLC )				
	
% ProgPage		: BUS_IDLE_WAIT		YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__PROG_PAGE_DBMINC)
% Send0x10		: SEND_CMD( 10 )																				
% Wait_tWB		: WRITE_POSTAMBLE											LOOP( C_tWB, Wait_tWB )						
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)
% Next_Page		: BUS_IDLE_WAIT		XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)
% End			: WRITE_POSTAMBLE											END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program single block from DBM in SLC mode
//AUX register Y_PLANE_0 has initial y address
//AUX register X_PLANE_0 has initial x address
//DBM has byte data to write to blk
//BOCx4

PATTERN( PAT__Prog_RND_Blk_SP_SLC_BOCx4 )
@{
apg_xmain_set(1, 0);
apg_xmain_set(2, 0);
@}

% AssertCEn		: BUS_IDLE_WAIT		XALU_AUX(X_PLANE_0, MAIN)				RESET_ERROR
// enable SLC mode
% Send0xDA		: SEND_CMD( DA )																		
% Wait_tSLC		: BUS_IDLE_WAIT												LOOP( C_tSLC, Wait_tSLC )				
	
% ProgPage		: BUS_IDLE_WAIT		YALU_AUX(Y_PLANE_0, MAIN)				GOSUB_PATTERN(SUB__PROG_PAGE_DBMINC_x4)
% Send0x10		: SEND_CMD( 10 )																				
% Wait_tWB		: WRITE_POSTAMBLE											LOOP( C_tWB, Wait_tWB )						
% Wait_tPROG	: WRITE_POSTAMBLE											GOSUB_PATTERN(SUB__POLL_TPROG)
% Next_Page		: BUS_IDLE_WAIT		XINC(MAIN, MAIN)						LOOP(C_SLC_PAGES_PER_BLK, Send0xDA)
% End			: WRITE_POSTAMBLE											END_PATTERN
