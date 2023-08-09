#include "PatternIncludes.h"

#if 0
PATTERN(SUB__POLL_TR)

% RB_or_loop	: CHIP_ENABLE				JUMP_TO_IF_COUNT_NOT_ZERO(C_POLL_EN, InitCnt)	
% Wait_tR		: CHIP_ENABLE				LOOP(C_tR, Wait_tR)
% LoopDone		: CHIP_ENABLE																 RETURN_PATTERN

//reset error flag
% InitCnt		: RELOAD_CNT(C_tR)															

% Reset_err		: CHIP_ENABLE				RESET_ERROR										
% Pipeline0		: CHIP_ENABLE				LOOP(C_PIPE, Pipeline0)

//poll RB for High
% RBPoll		: RB_POLL					SOFT_ERROR										
% Pipeline		: CHIP_ENABLE				LOOP( C_PIPE, Pipeline)
% error_eval	: CHIP_ENABLE				JUMP_IF_NO_ERROR(SubDone)						
% time_eval		: CHIP_ENABLE				LOOP(C_tR, Reset_err), RESET					
% SubDone		: CHIP_ENABLE																RETURN_PATTERN
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

// Read ONFI Parameters from DUT

///////////////////////////////////////////////////////////////////////////////////////////////////

//BOCx8

PATTERN( PAT__ReadParameters )

@{
apg_ymain_set(1, ymax() - 15);
apg_ymain_set(2, ymax() - 7);
@}

% AssertCEn		: BUS_IDLE_WAIT																											F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadParameters_ID)
% Send0xEC		: SEND_CMD( EC )																										F_NO
% Send0x40		: SEND_ADDR( 40 )																										F_NO
% Wait_tWB		: BUS_IDLE_WAIT															LOOP( C_tWB, Wait_tWB)							F_NO
% Wait_tR		: BUS_IDLE_WAIT															LOOP( C_tR, Wait_tR)							F_NO
% Wait_tRR		: BUS_IDLE_WAIT															LOOP(C_tRR, Wait_tRR)							F_NO
% Preamble		: READ_PREAMBLE																											F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_PARAM)
% ReadParams	: RE_TOGGLE_16						YADD(MAIN, MAIN, 16)				LOOP(C_PARAMETERS, ReadParams)					STROBE_EN F_CAP_REG
% Postamble		: READ_POSTAMBLE																										F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadParameters_ID) F2_FLUSH
% End			: BUS_IDLE_WAIT															END_PATTERN										F_NO

//BOCx4

PATTERN( PAT__ReadParameters_x4 )

@{
@}

% AssertCEn		: BUS_IDLE_WAIT																											F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadParameters_x4_ID)
% Send0xEC		: SEND_CMD( EC )																										F_NO
% Send0x40		: SEND_ADDR( 40 )																										F_NO
% Wait_tWB		: BUS_IDLE_WAIT															LOOP( C_tWB, Wait_tWB)							F_NO
% Wait_tR		: BUS_IDLE_WAIT															LOOP( C_tR, Wait_tR)							F_NO
% Wait_tRR		: BUS_IDLE_WAIT															LOOP(C_tRR, Wait_tRR)							F_NO
% Preamble		: READ_PREAMBLE																											F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_PARAM)
% ReadParams	: RE_TOGGLE_8						YADD(MAIN, MAIN, 8)					LOOP(C_PARAMETERS, ReadParams)					STROBE_EN F_CAP_REG
% Postamble		: READ_POSTAMBLE																										F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadParameters_x4_ID) F2_FLUSH
% End			: BUS_IDLE_WAIT															END_PATTERN										F_NO

///////////////////////////////////////////////////////////////////////////////////////////////////



