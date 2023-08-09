#include "stdafx.h"

#define TESTT( tbName ) TEST( tbName, NEXT, STOP )

SEQUENCE_TABLE( ONFI_BOCx4_seq )
{
	SEQUENCE_TABLE_INIT

	CALL( PE_CompModeTriState													)
	TEST( TB__ContactUpperDiode_PE,						NEXT,		cont_fail	)
	TEST( TB__ContactLowerDiode_PE,						NEXT,		cont_fail	)
	//TEST(TB__Input_Leakage,							NEXT,		leak_fail	)	
	TEST( TB__Leakage_PE_ALE,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_CEn,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_CLE,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_REn,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_WEn,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_RB,							NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_DQ_DQS_H,						NEXT,		leak_fail	)
	TEST( TB__Leakage_PE_DQ_DQS_L,						NEXT,		leak_fail	)
	CALL( PE_CompModeNormal														)
	TEST( TB__PowerOn,									NEXT,		func_fail	)
	TEST( TB__Reset_0xFF,								NEXT,		func_fail	)
	TEST( TB__ReadStatus,								NEXT,		func_fail	)
	//TEST( TB__ReadStatusEnhanced,						NEXT,		func_fail )	
	TEST(TB__SetFeaturesNvddr3Config,					NEXT,		func_fail	)
	TEST(TB__GetFeaturesNvddr3Config,					NEXT,		func_fail	)
	TEST(TB__ReadId90hWithAddress20h_dmain_BOCx4,		NEXT,		func_fail	)
	//TEST(TB__ReadParameters_BOCx4,						NEXT,		NEXT		)
	TEST(TB__SetFeaturesZqCalSwitching,					NEXT,		func_fail	)
	TEST(TB__GetFeaturesZqCalSwitching,					NEXT,		func_fail	)
	TEST(TB__SetFeaturesTimingMode,						NEXT,		func_fail	)
	TEST(TB__ReadDqTraining_RND_BOCx4,					NEXT,		func_fail	)
	//TEST(TB__ReadDqTraining_BOCx4,						NEXT,		func_fail	)
	TEST(TB__WriteReadCache_BOCx4,						NEXT,		func_fail	)
	TEST(TB__WriteReadCache_RND_BOCx4,					NEXT,		func_fail	)
	//TEST(TB__WriteCache_RND_BOCx4,					NEXT,		func_fail	)
	//TEST(TB__ReadCache_RND_BOCx4,						NEXT,		func_fail	)
	//TEST(TB__WriteReadCache_DBM_BOCx4,					NEXT,		func_fail	)
	TEST(TB__GetFeaturesTimingMode,						NEXT,		func_fail	)
	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramSinglePage_SinglePlane_SLC_BOCx4,	NEXT,		func_fail	)
	TEST(TB__ReadSinglePage_SinglePlane_SLC_BOCx4,		NEXT,		func_fail	)

	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramBlock_SP_SLC_BOCx4,					NEXT,		func_fail	)
	TEST(TB__ReadBlock_SP_SLC_BOCx4,					NEXT,		func_fail	)

	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramBlock_SP_SLC_DBM_BOCx4,				NEXT,		func_fail	)
	TEST(TB__ReadBlock_SP_SLC_DBM_BOCx4,				NEXT,		func_fail	)	

	TEST(TB__WriteReadCache_EBM_REPL_BOCx4,				NEXT,		func_fail	)
	TEST(TB__WriteCache_BOCx4,							NEXT,		func_fail	)
	TEST(TB__ReadCache_EBM_MASK_BOCx4,					NEXT,		func_fail	)
	TEST(TB__ReadCache_EBM_REPL_MASK_BOCx4,				NEXT,		func_fail	)

	BIN(												pass_bin,	STOP		)
	BINL(cont_fail,										cont_bin,	STOP		)
	BINL(leak_fail,										leak_bin,	STOP		)
	BINL(func_fail,										func_bin,	STOP		)
}

SEQUENCE_TABLE(ONFI_seq)
{
	SEQUENCE_TABLE_INIT

	CALL( PE_CompModeTriState													)
	TEST( TB__ContactUpperDiode_PE,						NEXT,		cont_fail	)
	TEST( TB__ContactLowerDiode_PE,						NEXT,		cont_fail	)
	//TEST(TB__Input_Leakage,							NEXT,		leak_fail	)	
	//TEST( TB__Leakage_PE_ALE,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_CEn,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_CLE,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_REn,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_WEn,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_RB,							NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_DQ_DQS_H,						NEXT,		leak_fail	)
	//TEST( TB__Leakage_PE_DQ_DQS_L,						NEXT,		leak_fail	)
	CALL( PE_CompModeNormal														)
	TEST( TB__PowerOn,									NEXT,		func_fail	)
	TEST( TB__Reset_0xFF,								NEXT,		func_fail	)
	TEST( TB__ReadStatus,								NEXT,		func_fail	)
	TEST( TB__isb_vcc,									NEXT,		func_fail)

	//TEST( TB__ReadStatusEnhanced,						NEXT,		func_fail )
	TEST(TB__SetFeaturesNvddr3Config,					NEXT,		func_fail	)
	TEST(TB__GetFeaturesNvddr3Config,					NEXT,		func_fail	)
	TEST(TB__ReadId90hWithAddress20h_dmain,				NEXT,		func_fail	)
	TEST(TB__ReadParameters,							NEXT,		NEXT		)
	TEST(TB__SetFeaturesZqCalSwitching,					NEXT,		func_fail	)
	TEST(TB__GetFeaturesZqCalSwitching,					NEXT,		func_fail	)
	TEST(TB__SetFeaturesTimingMode,						NEXT,		func_fail	)
	TEST(TB__ReadDqHwTraining_RND,						NEXT,		func_fail	)
	TEST(TB__ReadDqTraining_RND,						NEXT,		func_fail	)
	//TEST(TB__ReadDqTraining,							NEXT,		func_fail	)
	//TEST(TB__ReadCache_RND, NEXT, func_fail)
	TEST(TB__WriteReadCache,							NEXT,		func_fail	)
	TEST(TB__WriteReadCache_INV,						NEXT,		func_fail	)
	TEST(TB__WriteReadCache_RND,						NEXT,		func_fail	)
	TEST(TB__WriteReadCache_DBI,						NEXT,		func_fail	)	
	TEST(TB__WriteReadCache_DBM,						NEXT,		func_fail	)	
	TEST(TB__WriteCache_RND,							NEXT,		func_fail	)
	TEST(TB__ReadCache_RND,								NEXT,		func_fail	)
	TEST(TB__GetFeaturesTimingMode,						NEXT,		func_fail	)
	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramSinglePage_SinglePlane_SLC,			NEXT,		func_fail	)
	TEST(TB__ReadSinglePage_SinglePlane_SLC,			NEXT,		func_fail	)
	////TEST(TB__ODT_MEASURE,								NEXT,		func_fail	)

	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramBlock_SP_SLC,						NEXT,		func_fail	)
	TEST(TB__ReadBlock_SP_SLC,							NEXT,		func_fail	)	
	TEST(TB__EraseOneBlock_SinglePlane_SLC,				NEXT,		func_fail	)
	TEST(TB__ProgramBlock_SP_SLC_DBM,					NEXT,		func_fail	)
	TEST(TB__ReadBlock_SP_SLC_DBM,						NEXT,		func_fail	)	
	TEST(TB__WriteReadCache_EBM_REPL,					NEXT,		func_fail	)
	TEST(TB__WriteReadCache_LFSR,						NEXT,		func_fail	)

	BIN( pass_bin,										STOP					)
	BINL(cont_fail,										cont_bin,	STOP		)
	BINL(leak_fail,										leak_bin,	STOP		)
	BINL(func_fail,										func_bin,	STOP		)
}

SEQUENCE_TABLE(Simulator_seq)
{
	SEQUENCE_TABLE_INIT

	TESTT(datgen_add_hd7)
	TESTT(tb_Sim_Reset_0xFF)
}

SEQUENCE_TABLE(voltage_force_seq)
{
	SEQUENCE_TABLE_INIT

	TESTT(TB__ContactUpperDiode_PE)
	TESTT(TB__ContactLowerDiode_PE)

	TESTT(TB__PowerOn)
	TESTT(TB__voltage_force)
}

SEQUENCE_TABLE(vpulse_seq)
{
	SEQUENCE_TABLE_INIT

	TESTT(TB__PowerOn)
	TESTT(TB__vpulse_VCC)
	TESTT(TB__ripple_VCC)
}

SEQUENCE_TABLE(levelset_seq)
{
	SEQUENCE_TABLE_INIT

	TESTT(TB__PowerOn)
	TESTT(TB__PE_LevelSets)
	TESTT(TB_DPS_LevelSets_set)
	TESTT(TB_DPS_LevelSets_twk)
	TESTT(TB_HV_LevelSets_set)
	TESTT(TB_HV_LevelSets_twk)
}

SEQUENCE_TABLE(WatchDog_seq)
{
	SEQUENCE_TABLE_INIT

	TESTT(TB__WatchDogTest)
}

TEST_BIN(cont_bin){  }
TEST_BIN(leak_bin){  }
TEST_BIN(func_bin){  }
TEST_BIN(pass_bin){  }





