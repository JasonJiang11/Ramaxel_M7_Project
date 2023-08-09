#include "PatternIncludes.h"

//Write Read Cache
//Y_PLANE_0(AUX1) has initial y address
//X_PLANE_0(AUX2) has initial x address
//DATA_PLANE_0(AUX3) has byte data to write to cache
//BOCx4

PATTERN(PAT__WriteReadCache_x4)

@{
@}

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																				F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_x4)									F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																													F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)						F_NO
% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut_x4)								F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% End			: BUS_IDLE_WAIT													END_PATTERN															F_NO

//Write Read Cache
//Y_PLANE_0(AUX1) has initial y address
//X_PLANE_0(AUX2) has initial x address
//DATA_PLANE_0(AUX3) has byte data to write to cache
//BOCx8
PATTERN(PAT__WriteReadCache)

@{
@}

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																				F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)										F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																					
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)						F_NO
% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut)									F_NO

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% End			: BUS_IDLE_WAIT													END_PATTERN															F_NO

//Write Read Cache
//Y_PLANE_0(AUX1) has initial y address
//X_PLANE_0(AUX2) has initial x address
//DATA_PLANE_0(AUX3) has byte data to write to cache
//BOCx8
//for measuring odt with pmu
PATTERN(PAT__ODT_SETUP)

@{
@}

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																				F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)																		F_NO

% Send0x80		: SEND_CMD( 80 )																													F_NO
% ADDR1			: SEND_COL1( MAIN )																													F_NO
% ADDR2			: SEND_COL2( MAIN )																													F_NO
% ADDR3			: SEND_ROW1( MAIN )																													F_NO
% ADDR4			: SEND_ROW2( MAIN )																													F_NO
% ADDR5			: SEND_ROW3( MAIN )																													F_NO
% Wait_tADL		: BUS_IDLE_WAIT											LOOP( C_tADL, Wait_tADL)													F_NO
% Preamble		: WRITE_PREAMBLE																													F_NO
% DataIn		: DQS_TOGGLE_16											LOOP(C_DATA, DataIn)														F_NO
%				  WRITE_PREAMBLE										END_PATTERN																	F_NO

//Write Read Cache
//Y_PLANE_0(AUX1) has initial y address
//X_PLANE_0(AUX2) has initial x address
//DATA_PLANE_0(AUX3) has byte data to write to cache
//BOCx8
PATTERN(PAT__WriteReadCache_INV)

@{
@}

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																				F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_INV)									F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																													F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)						F_NO
% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut_INV)								F_NO

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% End			: BUS_IDLE_WAIT													END_PATTERN															F_NO

//
//Write Read Cache with random data from the jamram
//
PATTERN(PAT__WriteReadCache_RND_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																							F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			/*DATGEN_AUX( DATA_PLANE_0, MAIN)*/																			F_NO
//% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
//% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE)
% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE_x4_JAMINC)																		F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																													F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)							F_NO
//% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)		GOSUB_PATTERN(SUB__ReadPageDataOut)																		
//% DataOut		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__ReadPageDataOut)
% DataOut		: BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD		 			GOSUB_PATTERN(SUB__ReadPageDataOut_x4_JAMINC)							F_NO

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																F_NO

//
//Write Read Cache with random data from the jamram
//
PATTERN(PAT__WriteReadCache_RND)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																							F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			/*DATGEN_AUX( DATA_PLANE_0, MAIN)*/																			F_NO
//% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
//% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE)
% WritePage		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__PROG_PAGE_JAMINC)									F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																													F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)							F_NO
//% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)		GOSUB_PATTERN(SUB__ReadPageDataOut)																		
//% DataOut		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__ReadPageDataOut)
% DataOut		: BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD		 			GOSUB_PATTERN(SUB__ReadPageDataOut_JAMINC)								F_NO

%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																F_NO


PATTERN(PAT__HwTraining_RND)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																														F1_FRAME(FLSF_PAT_TASK_START, PAT_WriteReadCache_x8_ID)
%l_start		: BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																							F_NO
%				  BUS_IDLE_WAIT																														F_NO

% LoadData		: BUS_IDLE_WAIT			/*DATGEN_AUX( DATA_PLANE_0, MAIN)*/																			F_NO
//% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
//% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE)
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)		GOSUB_PATTERN(SUB__PROG_PAGE_JAMINC)							F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																													F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										        GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)					F_NO
//% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)		        GOSUB_PATTERN(SUB__ReadPageDataOut)																		
//% DataOut		: BUS_IDLE_WAIT												        GOSUB_PATTERN(SUB__ReadPageDataOut)
% DataOut		: BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD 		 			        GOSUB_PATTERN(SUB__Hw_training_ReadPageDataOut_JAMINC)			F_NO //AUXINC(X_PLANE_0)
% STEP_STRB     : PINFUNC1 PIN_SWEEP_ADJ 
	              AUXINC(X_PLANE_0) LOOP(C_PIN_SWEEP, l_start)
%				  BUS_IDLE_WAIT																														F1_FLUSH F2_FLUSH
%				  BUS_IDLE_WAIT																														F_NO
%				  BUS_IDLE_WAIT																														F1_FRAME(FLSF_PAT_TASK_STOP, PAT_WriteReadCache_x8_ID) F2_FLUSH

% End			: BUS_IDLE_WAIT






//
//Write Read Cache with random data from the jamram
// Enable DBI
//
PATTERN(PAT__WriteReadCache_DBI)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

% BUS_IDLE_WAIT							JAMRAM_ADDR_RELOAD																									F_NO
% BUS_IDLE_WAIT																																				F_NO
% BUS_IDLE_WAIT																																				F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)																						F_NO
% WritePage		: BUS_IDLE_WAIT									XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_JAMINC_DBI)			F_NO
% Wait_tWPST	: WRITE_POSTAMBLE
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST																	GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)		F_NO
% DataOut		: BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD		XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__ReadPageDataOut_JAMINC_DBI)		F_NO

% Disable_DBI_R : BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT																						END_PATTERN								F_NO

//
//Write Read Cache with random data from the DBM, BOCx8
//
PATTERN(PAT__WriteReadCache_DBM)

@{
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
% 				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT																																F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_DBMINC)										F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST	XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)								F_NO
% DataOut		: BUS_IDLE_WAIT													GOSUB_PATTERN(SUB__ReadPageDataOut_DBMINC)									F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Read Cache with random data from the DBM, BOCx4
//
PATTERN(PAT__WriteReadCache_DBM_x4)

@{
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
% 				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT																																F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_DBMINC_x4)										F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST	XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)								F_NO
% DataOut		: BUS_IDLE_WAIT													GOSUB_PATTERN(SUB__ReadPageDataOut_DBMINC_x4)								F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Read Cache with random data from the jamram
//
PATTERN(PAT__WriteReadCache_LFSR)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT			LFSR(1, LOAD)	DATGEN_AUX( DATA_PLANE_0, MAIN)																		F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_LFSR)											F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)								F_NO
% DataOut		: BUS_IDLE_WAIT			LFSR(1, LOAD)		 					GOSUB_PATTERN(SUB__ReadPageDataOut_LFSR)									F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Cache with random data from the jamram
//
PATTERN(PAT__WriteCache_RND_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT			/*DATGEN_AUX( DATA_PLANE_0, MAIN)*/																					F_NO
//% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
//% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE)
% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE_x4_JAMINC)																				F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Cache in BOCx4
//
PATTERN(PAT__WriteCache_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																						F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_x4)											F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Read Cache with random data from the jamram
//
PATTERN(PAT__ReadCache_RND_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																									F_NO
																					
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)									F_NO
//% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)		GOSUB_PATTERN(SUB__ReadPageDataOut)																		
//% DataOut		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__ReadPageDataOut)
% DataOut		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__ReadPageDataOut_x4_JAMINC)									F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																															

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Cache with random data from the jamram
//
PATTERN(PAT__WriteCache_RND)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																									F_NO

% LoadData		: BUS_IDLE_WAIT			/*DATGEN_AUX( DATA_PLANE_0, MAIN)*/																					F_NO
//% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)
//% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE)
% WritePage		: BUS_IDLE_WAIT			GOSUB_PATTERN(SUB__PROG_PAGE_JAMINC)																				F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Read Cache with random data from the jamram
//
PATTERN(PAT__ReadCache_RND)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT			JAMRAM_ADDR_RELOAD																									F_NO
																					
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)									F_NO
% DataOut		: BUS_IDLE_WAIT												GOSUB_PATTERN(SUB__ReadPageDataOut_JAMINC)										F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Read Cache with strobe mask data from ebm
//
PATTERN(PAT__ReadCache_EBM_MASK_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
																					
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST										GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)									F_NO
% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)		GOSUB_PATTERN(SUB__ReadPageDataOut_x4_EBMINC)									F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Read Cache with data from the ebm
// BOCx4
//
PATTERN(PAT__WriteReadCache_EBM_x4)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT				DATGEN_AUX( DATA_PLANE_0, MAIN)																					F_NO
% WritePage		: EBM_PTR_RELOAD			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_x4_EBMINC)									F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST												GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)							F_NO
% DataOut		: EBM_PTR_RELOAD									 				GOSUB_PATTERN(SUB__ReadPageDataOut_x4_EBMINC)							F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT														END_PATTERN																F_NO

//
//Write Read Cache with data from the ebm
// BOCx8
//
PATTERN(PAT__WriteReadCache_EBM)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																						F_NO
% WritePage		: EBM_PTR_RELOAD		XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE_EBMINC)										F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)								F_NO
% DataOut		: EBM_PTR_RELOAD									 			GOSUB_PATTERN(SUB__ReadPageDataOut_EBMINC)									F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT												END_PATTERN																		F_NO

//
//Write Cache with data from aux register
//
PATTERN(PAT__WriteCache)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% LoadData		: BUS_IDLE_WAIT			DATGEN_AUX( DATA_PLANE_0, MAIN)																						F_NO
% WritePage		: BUS_IDLE_WAIT			XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)	GOSUB_PATTERN(SUB__PROG_PAGE)												F_NO
% Wait_tWPST	: WRITE_POSTAMBLE																															F_NO

% End			: BUS_IDLE_WAIT													END_PATTERN																	F_NO

//
//Read Cache with random data from the ebm
//
PATTERN(PAT__ReadCache_EBM)

@{
apg_xmain_set(1, 0);
apg_ymain_set(1, 0);
@}

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
																					
% ChangeCol		: BUS_IDLE_WAIT_DQS_RST											GOSUB_PATTERN(SUB__ChangeReadColumnEnhanced)								F_NO
% DataOut		: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageDataOut)											F_NO

%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO
%				  BUS_IDLE_WAIT																																F_NO

% End			: BUS_IDLE_WAIT													END_PATTERN																	F_NO