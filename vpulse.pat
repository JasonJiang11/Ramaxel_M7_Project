#include "PatternIncludes.h"

// test vpulse feature
PATTERN(PAT__vpulse)

@{
count(C_VPULSE, 2000000);
@}

%						BUS_IDLE_WAIT
%						BUS_IDLE_WAIT
%						BUS_IDLE_WAIT

% Wait_vpulse_enable:	BUS_IDLE_WAIT_VPULSE	LOOP(C_VPULSE, Wait_vpulse_enable)
% Wait_vpulse_disable:	BUS_IDLE_WAIT			LOOP(C_VPULSE, Wait_vpulse_disable)

%						BUS_IDLE_WAIT
%						BUS_IDLE_WAIT
%						BUS_IDLE_WAIT

% End:					BUS_IDLE_WAIT											END_PATTERN





