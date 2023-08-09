#include "stdafx.h" 
#include "Levels.h"
#include "PatternInitializations.h"
#include "PinLists.h"
#include "TestBlocks.h"
#include "Timings.h"
#include "Datalogs.h"
#include "FlsUserTasks.h"

EXTERN_DOUBLE_VARIABLE(uv_stb)
INT_VARIABLE(uv_srch_cnt, 1, " ") {}

// Issues a READ STATUS command
TEST_BLOCK( TB__ReadStatus )
{
	output("%s", current_test_block());

	// Execute
	Delay(100 MS);				//pattern fails without this delay
	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(PAT__ReadStatus_0x70);
	output("%s", fls_capture_data.sr_dump().c_str());
	//funtest(PAT__ReadStatus_0x70, finish);
	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

// Issues a READ STATUS Enhanced command
TEST_BLOCK(TB__ReadStatusEnhanced)
{
	output("%s", current_test_block());

	//init apg counters
	set_counters_init();
	//init xmain
	apg_xmain_set(enApgNumberMag7::t_APG1, set_addr_xalu(uv_lun, uv_block, uv_plane, uv_page));	//X_PLANE_0 in pattern

	// Execute
	funtest(PAT__ReadStatusEnhanced_0x78, finish);

	return MULTI_DUT;
}