#include "PatternIncludes.h"

// Sends a SET FEATURES (EFh) command whose associated 
// address and parameter bytes are stored in JAM RAM.
//
// Feature Address stored in JAM RAM at address 0
// P1              stored in JAM RAM at address 1 APG1 UNIT1 
// P2              stored in JAM RAM at address 1 APG1 UNIT2 
// P3              stored in JAM RAM at address 1 APG2 UNIT1 
// P4              stored in JAM RAM at address 1 APG2 UNIT2
PATTERN(PAT__SetFeaturesJamRam)
%					BUS_IDLE_WAIT											F_NO
%					BUS_IDLE_WAIT											F_NO
%					BUS_IDLE_WAIT											F_NO
% Send0xEF:			SEND_CMD(EF)											F_NO
% SendFeatAddr :	SEND_ADDR_JAMRAM										F_NO
% Wait_tADL :		BUS_IDLE_WAIT				LOOP(C_tADL, Wait_tADL)		F_NO
% Preamble :		WRITE_PREAMBLE											F_NO
% P1_P4 :			SET_SUBFEATURES_JAMRAM									F_NO
% Wait_tWB :		WRITE_POSTAMBLE				LOOP(C_tWB, Wait_tWB)		F_NO
% Wait_tFEAT :		WRITE_POSTAMBLE				LOOP(C_tFEAT, Wait_tFEAT)	F_NO
%					BUS_IDLE_WAIT											F_NO
%					BUS_IDLE_WAIT											F_NO
% End :				BUS_IDLE_WAIT											END_PATTERN F_NO

// Sends a GET FEATURES (EEh) command whose associated 
// address and expected parameter bytes are stored in JAM RAM.
//
// Address     stored in JAM RAM at address 0
// P1          stored in JAM RAM at address 1 APG1 UNIT1
// P2          stored in JAM RAM at address 1 APG1 UNIT2
// P3          stored in JAM RAM at address 1 APG2 UNIT1
// P4          stored in JAM RAM at address 1 APG2 UNIT2
PATTERN(PAT__GetFeaturesJamRam)
%					BUS_IDLE_WAIT									    		F1_FRAME(FLSF_PAT_TASK_START, PAT_GetFeatures_ID)
% Send0xEE:			SEND_CMD(EE)												F_NO
% SendFeatAddr :	SEND_ADDR_JAMRAM											F_NO
% DQS_RST		:	BUS_IDLE_WAIT_DQS_RST                      					F_NO
%					BUS_IDLE_WAIT	  					DECREMENT(C_tWB)		F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_FEATURES)
% Wait_tWB		:	BUS_IDLE_WAIT                      LOOP(C_tWB, Wait_tWB)	F_NO
% Wait_tFEAT	:	BUS_IDLE_WAIT                      LOOP(C_tFEAT, Wait_tFEAT) F_NO
% Wait_tRR		:	BUS_IDLE_WAIT                      LOOP(C_tRR, Wait_tRR)	F_NO
% Preamble		:	READ_PREAMBLE       										F_NO
% P1_P4			:	GET_SUBFEATURES_JAMRAM             STROBE_EN				F_CAP_REG
% Postamble		:	READ_POSTAMBLE												F1_FRAME(FLSF_PAT_TASK_STOP, PAT_GetFeatures_ID) F2_FLUSH
% End:				BUS_IDLE_WAIT					   END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////

// Alternative method to JamRam
// Sends a SET FEATURES (EFh) command whose associated 
// address is stored in xmain
// parameter bytes are stored in dmain.
PATTERN(PAT__SetFeaturesDmain)
%					BUS_IDLE_WAIT													F_NO
%					BUS_IDLE_WAIT													F_NO
%					BUS_IDLE_WAIT													F_NO
% Send0xEF:			SEND_CMD(EF)													F_NO
% SendFeatAddr :	SEND_ADDR_XMAIN													F_NO
% Wait_tADL :		BUS_IDLE_WAIT					LOOP(C_tADL, Wait_tADL)			F_NO
% Preamble :		WRITE_PREAMBLE													F_NO
% P1_P4 :			SET_SUBFEATURES_DMAIN											F_NO
% Wait_tWB :		WRITE_POSTAMBLE					LOOP(C_tWB, Wait_tWB)			F_NO
% Wait_tFEAT :		BUS_IDLE_WAIT					LOOP(C_tFEAT, Wait_tFEAT)		F_NO
%					BUS_IDLE_WAIT													F_NO
%					BUS_IDLE_WAIT													F_NO
% End :				BUS_IDLE_WAIT					END_PATTERN						F_NO

// Alternative method to JamRam
// Sends a GET FEATURES (EEh) command whose associated 
// address is stored in xmain
// parameter bytes are stored in dmain.
PATTERN(PAT__GetFeaturesDmain)
%				BUS_IDLE_WAIT														F1_FRAME(FLSF_PAT_TASK_START, PAT_GetFeatures_dmain_ID)
% Send0xEE		: SEND_CMD(EE)														F_NO
% SendFeatAddr	: SEND_ADDR_XMAIN													F_NO
% DQS_RST		: BUS_IDLE_WAIT_DQS_RST												F_NO
%				  BUS_IDLE_WAIT	  DECREMENT(C_tWB)									F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_FEATURES)
% Wait_tWB		: BUS_IDLE_WAIT                   LOOP( C_tWB,   Wait_tWB )			F_NO
% Wait_tFEAT	: BUS_IDLE_WAIT                   LOOP( C_tFEAT, Wait_tFEAT )		F_NO
% Wait_tRR		: BUS_IDLE_WAIT                   LOOP( C_tRR,   Wait_tRR )			F_NO
% Preamble		: READ_PREAMBLE														F_NO
% P1_P4			: GET_SUBFEATURES_DMAIN				STROBE_EN						F_CAP_REG
% Postamble		: READ_POSTAMBLE													F_NO
%				  BUS_IDLE_WAIT														F1_FRAME(FLSF_PAT_TASK_STOP, PAT_GetFeatures_dmain_ID) F2_FLUSH
% End			: BUS_IDLE_WAIT						END_PATTERN						F_NO


