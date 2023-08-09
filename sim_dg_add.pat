#include "PatternIncludes.h"

PATTERN( PAT__SIM_DG_ADD )

% MAR_1:
	MAR        INC
% MAR_2 :
	MAR            CJMPNZ, MAR_2
	COUNT          COUNT1, DECR, AON
	DATGEN1SUB1    SDMAIN, SDBASE, ADD, DMAIN  // set individual subdgen
	DATGEN1SUB2    SDMAIN, SDBASE, ADD, DMAIN
	DATGEN1SUB3    SDMAIN, SDBASE, ADD, DMAIN
	DATGEN1SUB4    SDMAIN, SDBASE, ADD, DMAIN
	DATGEN2        SDMAIN, SDBASE, ADD, DMAIN  // make subdgens be identical
	% MAR_3:
MAR        INC
% MAR_DONE :
	MAR     DONE
	// **** end of pattern ****
