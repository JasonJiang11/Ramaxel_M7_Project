#include "PatternIncludes.h"



// Set C_FlagLookAhead to 4 or higher
PATTERN(SUB__FHS)
%				  BUS_IDLE_WAIT																						F_TStamp(1)
%				  BUS_IDLE_WAIT		SETFLAG_1																		F_NO
% LookAhead1	: BUS_IDLE_WAIT												LOOP( C_FlagLookAhead, LookAhead1 )		F_NO
% LookAhead2	: BUS_IDLE_WAIT		LOOKAHEAD_F1							LOOP( C_FlagLookAhead, LookAhead2 )		F_NO
%				  BUS_IDLE_WAIT		CJMP_FLAG1( LookAhead2 )														F_NO
%				  BUS_IDLE_WAIT												RETURN_PATTERN							F_TStamp(2)


PATTERN(PAT__FlagHandshake)

@{
	count(C_FlagLookAhead, 4);
@}

%				  BUS_IDLE_WAIT																			F1_FRAME(FLSF_PAT_TASK_START, PAT_FlagHandshake)
%				  BUS_IDLE_WAIT											GOSUB_PATTERN(SUB__FHS)			F_NO
%				  BUS_IDLE_WAIT											GOSUB_PATTERN(SUB__FHS)			F_NO
%				  BUS_IDLE_WAIT											GOSUB_PATTERN(SUB__FHS)			F_NO
%				  BUS_IDLE_WAIT																			F1_FRAME(FLSF_PAT_TASK_STOP, PAT_FlagHandshake) F2_FLUSH
% End			: BUS_IDLE_WAIT											END_PATTERN						F_NO

PATTERN(PAT__userramaddress)

@{

@}

% MAR INC																			F1_FRAME(FLSF_PAT_TASK_START, PAT_ReadStatus_ID) //???
% MAR INC
% MAR INC
% MAR INC
% MAR INC
% MAR INC
% userram USE_PTR 0
% userram GET, URAMHOLD, FLS  //Get the UserRAM value pointed to by PTR 0 into the FLS ureg register. This is a same cycle GET
fls  FLS_USER5   //Output a FLS USER5 component
% userram USE_PTR 1
% userram GET, URAMHOLD, FLS
fls  FLS_USER5
% userram USE_PTR 2
% userram GET, URAMHOLD, FLS
fls  FLS_USER5
% userram USE_PTR 3
% userram GET, URAMHOLD, FLS
fls  FLS_USER5
% MAR INC
% MAR INC
% MAR INC
% MAR INC
% MAR INC																			F1_FRAME(FLSF_PAT_TASK_STOP, PAT_ReadStatus_ID)
% MAR INC F1_FLUSH F2_FLUSH

% End:		 BUS_IDLE_WAIT											END_PATTERN

PATTERN(PAT__FlsShutdown)
@{
@}

% BUS_IDLE_WAIT																														F_NO
% BUS_IDLE_WAIT															F1_FLS_SHUTDOWN
% End			: BUS_IDLE_WAIT												END_PATTERN	  F_NO

