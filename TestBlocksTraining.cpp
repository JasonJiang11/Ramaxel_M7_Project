#include "stdafx.h"
#include "DeviceStatus.h"
#include "Timings.h"
#include "TestBlocks.h"

TEST_BLOCK(TB__ReadDqTraining_BOCx4)
{
	output("%s", current_test_block());

	const double start = 4.5 NS;
	const double stop = 8.5 NS;
	const double step = 10 PS;

	//initialize apg registers
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_xmain_set(apg, 0); 
		apg_ymain_set(apg, 0);

		apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, 0xaa55);		//DATA_PLANE_0 in pattern
	}

	search_and_set_tDQSRE(get_timing_mode(), start, stop, step, PAT__WriteReadCache_x4);
	setup_src_sync();

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadDqTraining)
{
	output("%s", current_test_block());

	const double start = 4.5 NS;
	const double stop = 8.5 NS;
	const double step = 10 PS;

	//initialize apg registers
	//initialize apg registers
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_xmain_set(apg, 0);
		apg_ymain_set(apg, 0);

		apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, 0xaa55);		//DATA_PLANE_0 in pattern
	}

	search_and_set_tDQSRE(get_timing_mode(), start, stop, step, PAT__WriteReadCache);
	setup_src_sync();

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadDqTraining_RND_BOCx4)
{
	output("%s", current_test_block());

	const double start = 4.5 NS;
	const double stop = 8.5 NS;
	const double step = 10 PS;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx4);
	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx4);

	//initialize apg registers
	//initialize apg registers
	for (auto apg = enApgNumberMag7::t_APG1; apg <= enApgNumberMag7::t_APG2; ++apg)
	{
		apg_xmain_set(apg, 0);
		apg_ymain_set(apg, 0);

		apg_auxreg_set(apg, enAuxRegNumberMag7::t_AUXREG3, 0xaa55);		//DATA_PLANE_0 in pattern
	}

	search_and_set_tDQSRE(get_timing_mode(), start, stop, step, PAT__WriteReadCache_RND_x4);
	setup_src_sync();

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadDqTraining_RND)
{
	output("%s", current_test_block());

	const double start = 4.5 NS;
	const double stop = 8.5 NS;
	const double step = 10 PS;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx8);
	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx8);

	search_and_set_tDQSRE(get_timing_mode(), start, stop, step, PAT__WriteReadCache_RND);
	setup_src_sync();

	return MULTI_DUT;
}

TEST_BLOCK(TB__ReadDqHwTraining_RND)
{
	output("%s", current_test_block());

	const double start = 4.5 NS;
	const double stop = 8.5 NS;
	const double step = 10 PS;

	load_jam_ram(g_buffer1, enPinModesMag7::t_PM_BOCx8);
	//init apg registers
	sp_prog_read_apg_init(uv_page, uv_plane, uv_block, uv_lun, uv_page_payload_byte, true, enPinModesMag7::t_PM_BOCx8);

	Hwsearch_and_shmoo(get_timing_mode(), start, stop, step, PAT__HwTraining_RND);
	setup_src_sync();

	return MULTI_DUT;
}

