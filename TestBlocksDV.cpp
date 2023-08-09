#include "stdafx.h" 
#include "Datalogs.h"
#include "Levels.h"
#include "PinLists.h"
#include "DutPins.h"
#include "Timings.h"
#include "DefineCounters.h"
#include "FlsUserTasks.h"

// Issues a READ STATUS command

void flag_cb()
{
	output("Shook hands");
	pattern_flag1_set(FALSE);
}

//#define USE_PAT_CB
TEST_BLOCK(TB__FlagHandshake)
{
	// Execute
	Delay(100 MS);				//pattern fails without this delay
	FlsStoreRegCnt fls_capture_data(4);
#ifdef USE_PAT_CB
	pattern_callback_func_set(enPatternFlagNumberMag7::t_FLAG1, flag_cb);
	fls_capture_data.funtest(PAT__FlagHandshake);
#else
	pattern_flag1_set(FALSE);
	fls_capture_data.queue();
	start_pattern(PAT__FlagHandshake);
	while (1) {
		 PatternState pfs = pattern_state();
		 if (pfs == PatternState::PATTERN_RUNNING) {
			 if (pattern_flag1_get()) {
				 output("Shook hands");
				 pattern_flag1_set(FALSE);
			 }
		 }
		 else {
			 break;
		 }
		 Sleep(1);
	}
	fls_capture_data.wait_for_taskstop();
#endif
	output("%s", fls_capture_data.sr_dump().c_str());
	//funtest(PAT__ReadStatus_0x70, finish);
	fdatalog(pinlist::AllDataDataStrobeReadyBusyPins());

	return MULTI_DUT;
}

TEST_BLOCK(TB__userramaddress)
{

	Int64Array UserRAM_buf;

	for (int val = 1; val <= 10; val++) { UserRAM_buf.Add(val); };
	apg_userram_value_set(1, UserRAM_buf, (int)UserRAM_buf.GetSize());
	//At this point, URAM_0=0, URAM_1=1, etc. up to URAM_9=9
	apg_user_ram_address_set(0, 2);
	apg_user_ram_address_set(1, 3);
	apg_user_ram_address_set(2, 4);
	apg_user_ram_address_set(3, 5);


	FlsStoreRegCnt fls_capture_data(4);
	fls_capture_data.funtest(PAT__userramaddress);
	output("%s", fls_capture_data.sr_dump().c_str());
	fdatalog(pinlist::AllDataPins());

	return MULTI_DUT;
}
