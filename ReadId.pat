#include "PatternIncludes.h"

//  Issue the READ ID (90h) command followed by a 20h address.
//  Wait for tWHR
//  The target returns 5 bytes
//  the expected bytes are loaded into aux 1 on the subDGs
PATTERN( PAT__ReadId90hWithAddress20h )
% BUS_IDLE_WAIT													F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadId90hWithAddress20h_ID)
% Send0x90 : SEND_CMD(90)										F_NO
% Send0x20 : SEND_ADDR(20)										F_NO
% BUS_IDLE_WAIT					  DECREMENT(C_tWHR)				F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_READID)
% Wait_tWHR: BUS_IDLE_WAIT        LOOP( C_tWHR, Wait_tWHR )		F_NO
% Preamble : READ_PREAMBLE										F_NO
% Byte0_5  : DATA_OUT_READ_ID_90_AUX1							STROBE_EN F_CAP_REG
% Postamble: READ_POSTAMBLE										F_NO
% BUS_IDLE_WAIT													F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadId90hWithAddress20h_ID) F2_FLUSH
% End      : END_PATTERN

//  Issue the READ ID (90h) command followed by a 20h address.
//  Wait for tWHR
//  The target returns 5 bytes
//  the expected bytes are loaded into dmain on the subDGs
//	BOCx4
PATTERN( PAT__ReadId90hWithAddress20h_dmain_x4 )
@{
//x4
apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT1, 0x4f4f);
apg_dmain_set(enApgNumberMag7::t_APG1, enUnitNumberMag7::t_UNIT2, 0x4e4e);
apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT1, 0x4646);
apg_dmain_set(enApgNumberMag7::t_APG2, enUnitNumberMag7::t_UNIT2, 0x4949);
//	BOCx4
//
@}
%			 BUS_IDLE_WAIT										F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadId90hWithAddress20h_dmain_x4_ID)
% Send0x90 : SEND_CMD(90)										F_NO
% Send0x20 : SEND_ADDR(20)										F_NO
%			 BUS_IDLE_WAIT		  DECREMENT(C_tWHR)				F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_READID)
% Wait_tWHR: BUS_IDLE_WAIT        LOOP( C_tWHR, Wait_tWHR )		F_NO
% Preamble : READ_PREAMBLE										F_NO
% Byte0_5  : DATA_OUT_READ_ID_90_DMAIN_x4						STROBE_APG1_EN F_CAP_REG
% Postamble: READ_POSTAMBLE										F_NO
%			 BUS_IDLE_WAIT										F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadId90hWithAddress20h_dmain_x4_ID) F2_FLUSH
% End      : BUS_IDLE_WAIT		  END_PATTERN					F_NO

//  Issue the READ ID (90h) command followed by a 20h address.
//  Wait for tWHR
//  The target returns 6 bytes
//  the expected bytes are loaded into dmain on the subDGs
PATTERN( PAT__ReadId90hWithAddress20h_dmain )
@{
@}
%			 BUS_IDLE_WAIT										F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadId90hWithAddress20h_dmain_x8_ID)
% Send0x90 : SEND_CMD(90)										F_NO
% Send0x20 : SEND_ADDR(20)										F_NO
%			 BUS_IDLE_WAIT		  DECREMENT(C_tWHR)				F1_USER(FLSU_REG_BANK_SET, 0) F2_USER(FLSU_REG_TYPE, FLSP_REG_READID)
% Wait_tWHR: BUS_IDLE_WAIT        LOOP( C_tWHR, Wait_tWHR )		F_NO
% Preamble : READ_PREAMBLE										F_NO
% Byte0_5  : DATA_OUT_READ_ID_90_DMAIN							STROBE_EN F_CAP_REG
YALU1 YUDATA, DYMAIN	
YUDATAVAL1 0					
YALU2 YUDATA, DYMAIN	
YUDATAVAL2 8					
% Postamble: READ_POSTAMBLE										F_NO

%			BUS_IDLE_WAIT										F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadId90hWithAddress20h_dmain_x8_ID) F2_FLUSH

% End      : BUS_IDLE_WAIT			END_PATTERN					F_NO
