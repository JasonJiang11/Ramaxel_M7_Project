#include "PatternIncludes.h"

//Polling subroutine for erase.
//This is executed after the erase command and polls on the RB

PATTERN(SUB__POLL_TBERS)

% RB_or_loop	: BUS_IDLE_WAIT			JUMP_TO_IF_COUNT_NOT_ZERO(C_POLL_EN, InitCnt)
% Wait_tBERS	: BUS_IDLE_WAIT			LOOP(C_tBERS, Wait_tBERS)
% LoopDone		: BUS_IDLE_WAIT															RETURN_PATTERN

//reset error flag
% InitCnt		: RELOAD_CNT(C_tBERS)

% Reset_err		: BUS_IDLE_WAIT			RESET_ERROR
% Pipeline0		: BUS_IDLE_WAIT			LOOP(C_PIPE, Pipeline0)

//poll RB for High
% PollRB		: POLL_RBSY				DATGEN_UDATA(MAIN, 0)			
					SOFT_ERROR
% Pipeline		: BUS_IDLE_WAIT			LOOP( C_PIPE, Pipeline), ERR_ALL
% error_eval	: BUS_IDLE_WAIT			JUMP_IF_ERROR(SubDone)			count count2, incr	
% time_eval		: BUS_IDLE_WAIT			LOOP(C_tBERS, PollRB)
% SubDone		: BUS_IDLE_WAIT															RETURN_PATTERN


///////////////////////////////////////////////////////////////////////////////////////////////////

// Issue an ERASE BLOCK command for a given block and LUN on SINGLE plane.
// Erase Operation: ERASE BLOCK (60h-D0h)
//Supports SLC and TLC
/////////////////////////////////////////////////////////////////////////////////////////////////////
//Erase block in SLC or TLC mode
//AUX register X_PLANE_0 has initial x address for plane 0
//AUX register is referenced with a "#define", eg X_PLANE_0 specifies aux register to load with initial X for plane 0
PATTERN(PAT__EraseOneBlock_SinglePlane)
@{
count(2, 0);
@}

% AssertCEn		: BUS_IDLE_WAIT					RESET_ERROR
//if SLC mode, execute DA command, otherwise skip to label Send0x60
%				  BUS_IDLE_WAIT					JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Send0x60)
// Enable SLC mode
% Send0xDA		: SEND_CMD( DA )                                       
% Wait_tSLC		: BUS_IDLE_WAIT					LOOP( C_tSLC, Wait_tSLC )

//send cmd 60 and init xmain with aux register referenced by X_PLANE_0
% Send0x60		: SEND_CMD( 60 )				XALU_AUX( X_PLANE_0, MAIN)
% ADDR3			: SEND_ROW1( MAIN )				
% ADDR4			: SEND_ROW2( MAIN )
% ADDR5			: SEND_ROW3( MAIN )
% Send0xD0		: SEND_CMD( D0 )
% Wait_tWB_D0	: BUS_IDLE_WAIT					LOOP( C_tWB, Wait_tWB_D0 )
% Wait_tBERS	: BUS_IDLE_WAIT					GOSUB_PATTERN(SUB__POLL_TBERS)

% End			: BUS_IDLE_WAIT					END_PATTERN

////////////////////////////////////////////////////////////////////////////////////////////



