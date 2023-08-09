#include "PatternIncludes.h"

// Issue a RESET (FFh) command to all targets.
PATTERN(PAT__Reset_0xFF)

@{
@}

%			 BUS_IDLE_WAIT
%			 BUS_IDLE_WAIT
%			 BUS_IDLE_WAIT

% Send0xFF:  SEND_CMD(FF)
% Wait_tWB:  BUS_IDLE_WAIT  LOOP(C_tWB, Wait_tWB)
% Wait_tPOR: BUS_IDLE_WAIT  LOOP(C_tPOR, Wait_tPOR)

%			 BUS_IDLE_WAIT
%			 BUS_IDLE_WAIT
%			 BUS_IDLE_WAIT

% End:		 BUS_IDLE_WAIT											END_PATTERN





