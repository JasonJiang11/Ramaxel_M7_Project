#include "stdafx.h"
#include "DefineCounters.h"
#include "Timings.h"
#include "PatternInitializations.h"

BOOL_VARIABLE(PatSimLog2File, FALSE, " ") {}

TEST_BLOCK(datgen_add_hd7) {
	PatSimConfigStructMagnum7 conf;
	conf.pPattern = PAT__SIM_DG_ADD;  // pattern name
	conf.count[0] = 3;   // COUNT1 is set to 3
	conf.count[1] = 0;   // COUNT2
	conf.count[2] = 0;   // COUNT3
	conf.count[3] = 0;   // COUNT4
	conf.count[4] = 0;   // COUNT5
	conf.count[5] = 0;   // COUNT6

	conf.numx = 4;
	conf.xmain[0] = 0xF;      // APG1, X address
	conf.xbase[0] = 0x0;      // APG1, X address
	conf.xfield[0] = 0x0;      // APG1, X address

	conf.numy = 4;
	conf.ymain[0] = 0xF;      // APG1, Y address
	conf.ybase[0] = 0x0;      // APG1, Y address
	conf.yfield[0] = 0x0;      // APG1, Y address

	conf.datawidth = 32;

	conf.dmain[0][0] = 1;      // APG1, SubDG1
	conf.dbase[0][0] = 8;      // APG1, SubDG1
	conf.dmain[0][1] = 2;      // APG1, SubDG2
	conf.dbase[0][1] = 8;      // APG1, SubDG2
	conf.dmain[0][2] = 3;      // APG1, SubDG3
	conf.dbase[0][2] = 8;      // APG1, SubDG3
	conf.dmain[0][3] = 4;      // APG1, SubDG4
	conf.dbase[0][3] = 8;      // APG1, SubDG4

	conf.dmain[1][0] = 5;      // APG2, SubDG1
	conf.dbase[1][0] = 8;      // APG2, SubDG1
	conf.dmain[1][1] = 6;      // APG2, SubDG2
	conf.dbase[1][1] = 8;      // APG2, SubDG2
	conf.dmain[1][2] = 7;      // APG2, SubDG3
	conf.dbase[1][2] = 8;      // APG2, SubDG3
	conf.dmain[1][3] = 8;      // APG2, SubDG4
	conf.dbase[1][3] = 8;      // APG2, SubDG4

	if (PatSimLog2File == TRUE) conf.output_file_name = "datgen_add.csv";

	pat_sim(&conf); // run pattern simulation

	results_set(PASS);
	return MULTI_DUT;
}

TEST_BLOCK(tb_Sim_Reset_0xFF) 
{
	PatSimConfigStructMagnum7 conf;
	conf.pPattern = PAT__Reset_0xFF;  // pattern name

	set_counters_init(enPinModesMag7::t_PM_BOCx8, &conf);

	conf.datawidth = 32;

	if (PatSimLog2File == TRUE) conf.output_file_name = "datgen_add.csv";

	pat_sim(&conf); // run pattern simulation

	results_set(PASS);
	return MULTI_DUT;
}

