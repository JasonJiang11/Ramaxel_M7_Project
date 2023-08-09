#include "PatternIncludes.h"

//  Issue the READ STATUS (70h) command.
//  Wait for tWHR
//  Transfer the Status Register (SR) byte in one Data-Out cycle
#if 1
PATTERN(PAT__ReadStatus_0x70)

@{
@}

%				  BUS_IDLE_WAIT																			F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadStatus_ID)
% Send0x70		: SEND_CMD(70)																			F_NO
%                 BUS_IDLE_WAIT											DECREMENT( C_tWHR )				F1_USER(FLSU_REG_BANK_SET, 0 ) F2_USER(FLSU_REG_TYPE, FLSP_REG_SR)
% Wait_tWHR		: BUS_IDLE_WAIT											LOOP( C_tWHR, Wait_tWHR )		F_NO
% Preamble		: READ_PREAMBLE																			F_NO
% ReadByte0		: READ_ONE_BURST		DATGEN_UDATA( MAIN, E0)											STROBE_APG1_EN F1_CAP_REG F2_CAP_ARRAY
% Postamble		: READ_POSTAMBLE																		F_NO
%				  BUS_IDLE_WAIT																			F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadStatus_ID) F2_FLUSH
% End			: BUS_IDLE_WAIT											END_PATTERN						F_NO
#endif
#if 0
PATTERN(PAT__ReadStatus_0x70)

@{
@}

%				  BUS_IDLE_WAIT
% Send0x70		: SEND_CMD(70)	

% Wait_tWHR		: BUS_IDLE_WAIT											LOOP( C_tWHR, Wait_tWHR )

% Preamble		: READ_PREAMBLE			DATGEN_UDATA(MAIN, E0)
% ReadByte0		: READ_ONE_BURST													STROBE_MASK(40)
% Postamble		: READ_POSTAMBLE

%				  BUS_IDLE_WAIT

% End			: BUS_IDLE_WAIT											END_PATTERN
#endif

#if 0
PATTERN(PAT__ReadStatus_0x70)

@{
apg_auxreg_set(enApgNumberMag7::t_APG1, enAuxRegNumberMag7::t_AUXREG1, 0x40);	
output("apg1, auxreg1 0x%x", apg_auxreg_get(enApgNumberMag7::t_APG1, enAuxRegNumberMag7::t_AUXREG1));

@}


%				  BUS_IDLE_WAIT
% Send0x70		: SEND_CMD(70)	

% Wait_tWHR		: BUS_IDLE_WAIT											LOOP( C_tWHR, Wait_tWHR )

% Preamble		: READ_PREAMBLE	
datgen1 STROBEREGLOADBYAUXB
% ReadByte0		: READ_ONE_BURST		DATGEN_UDATA(MAIN, E0)											STROBE_MASK_AUX(AUX1)
% Postamble		: READ_POSTAMBLE

%				  BUS_IDLE_WAIT

% End			: BUS_IDLE_WAIT											END_PATTERN
#endif

//  Issue the READ STATUS ENHANCED(78h) command.
//  Wait for tWHR
//  Transfer the Status Register (SR) byte in one Data-Out cycle
PATTERN(PAT__ReadStatusEnhanced_0x78)

@{
@}

%				  BUS_IDLE_WAIT																			F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadStatusEnhanced_ID)
% Send0x78		: SEND_CMD(78)																			F_NO
% ROW1			: SEND_ROW1(MAIN)																		F_NO
% ROW2			: SEND_ROW2(MAIN)																		F_NO
% ROW3			: SEND_ROW3(MAIN)																		F_NO
%				  BUS_IDLE_WAIT											DECREMENT(C_tWHR)				F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_SR)
% Wait_tWHR		: BUS_IDLE_WAIT											LOOP(C_tWHR, Wait_tWHR)			F_NO
% Preamble		: READ_PREAMBLE																			F_NO
% ReadByte0		: READ_ONE_BURST		DATGEN_UDATA(MAIN, E0)											STROBE_APG1_EN F1_CAP_REG F2_CAP_ARRAY
% BUS_IDLE_WAIT																							F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadStatusEnhanced_ID) F2_FLUSH
% End			: BUS_IDLE_WAIT											END_PATTERN						F_NO
																																																			   
