#include "stdafx.h" 
#include "Datalogs.h"
#include "PinLists.h"
#include "Levels.h"
#include "timer.h"
#include "watchdog.h"

TEST_BLOCK( TB__WatchDogTest )
{

	for (int i = 0; i <= 400; i += 10) {
		output("Elapsed time : %4d", i);
		Sleep(10000);
	}

	results_set(FAIL);

	return MULTI_DUT;
}

