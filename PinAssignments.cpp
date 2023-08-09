#include "stdafx.h"
#include "DeviceSpecs.h"

//#define ASSIGN_8DUT_DOP ASSIGN_8DUT

PIN_ASSIGNMENTS( PA__Share_Mag7_Training ) 
{
	//						DUT15					DUT11					DUT7					DUT3			
	//						chn0		chn1		chn0		chn1		chn0		chn1		chn0		chn1
	ASSIGN_8DUT(		DQS_t,		b_1,		b_13,		a_1,		a_13,		b_37,		b_49,		a_37,		a_49		)
	ASSIGN_8DUT(		DQS_c,		b_2,		b_14,		a_2,		a_14,		b_38,		b_50,		a_38,		a_50		)
	ASSIGN_8DUT(		DQ0,		b_11,		b_18,		a_12,		a_20,		b_47,		b_54,		a_48,		a_56		)
	ASSIGN_8DUT(		DQ1,		b_9,		b_16,		a_10,		a_21,		b_45,		b_52,		a_46,		a_57		)
	ASSIGN_8DUT(		DQ2,		b_10,		b_17,		a_11,		a_19,		b_46,		b_53,		a_47,		a_55		)
	ASSIGN_8DUT(		DQ3,		b_12,		b_15,		a_9,		a_22,		b_48,		b_51,		a_45,		a_58		)
	ASSIGN_8DUT(		DQ4,		b_6,		b_24,		a_6,		a_15,		b_42,		b_60,		a_42,		a_51		)
	ASSIGN_8DUT(		DQ5,		b_5,		b_21,		a_5,		a_16,		b_41,		b_57,		a_41,		a_52		)
	ASSIGN_8DUT(		DQ6,		b_4,		b_22,		a_4,		a_17,		b_40,		b_58,		a_40,		a_53		)
	ASSIGN_8DUT(		DQ7,		b_3,		b_23,		a_3,		a_18,		b_39,		b_59,		a_39,		a_54		)
	ASSIGN_8DUT(		RE_t,		b_7,		b_19,		a_7,		a_23,		b_44,		b_55,		a_44,		a_60		)
	ASSIGN_8DUT(		RE_c,		b_8,		b_20,		a_8,		a_24,		b_43,		b_56,		a_43,		a_59		)
	ASSIGN_8DUT(		CLE,		a_25,		b_25,		a_25,		b_25,		a_61,		b_61,		a_61,		b_61		)	
	ASSIGN_8DUT(		ALE,		a_26,		b_26,		a_26,		b_26,		a_62,		b_62,		a_62,		b_62		)
	ASSIGN_8DUT(		RB0n,		a_27,		b_27,		a_27,		b_27,		a_63,		b_63,		a_63,		b_63		)
	ASSIGN_8DUT(		WEn,		b_28,		b_30,		a_28,		a_30,		b_64,		b_66,		a_64,		a_66		)
	ASSIGN_8DUT(		WPn,		a_29,		b_29,		a_29,		b_29,		a_65,		b_65,		a_65,		b_65		)
	//the following pins are driven from one PG and will have no APG BOE capability
	ASSIGN_8DUT_DOP(	ENi,		a_32,		a_32,		a_32,		a_32,		b_32,		b_32,		b_32,		b_32		)
	ASSIGN_8DUT_DOP(	ENo,		a_72,		a_72,		a_72,		a_72,		b_72,		b_72,		b_72,		b_72		)
	ASSIGN_8DUT_DOP(	CE0n,		a_34,		b_34,		a_34,		b_34,		a_34,		b_34,		a_34,		b_34		)

	if (device::num_chip_enables / 2 > 1)
	{
		ASSIGN_8DUT(RB1n,			a_31,		b_31,		a_31,		b_31,		a_67,		b_67,		a_67,		b_67)
		//the following pins are driven from one PG and will have no APG BOE capability
		ASSIGN_8DUT_DOP(CE1n,		a_35,		b_35,		a_35,		b_35,		a_35,		b_35,		a_35,		b_35)
	}

	ASSIGN_8DUT( VCC,				b_dps1a,	b_dps1a,	a_dps1a,	a_dps1a,	b_dps2a,	b_dps2a,	a_dps2a,	a_dps2a		)
	ASSIGN_8DUT( VCCQ,				b_dps3a,	b_dps3a,	a_dps3a,	a_dps3a,	b_dps4a,	b_dps4a,	a_dps4a,	a_dps4a		)
	ASSIGN_8DUT( VPP,				b_hv5,		b_hv5,		a_hv5,		a_hv5,		b_hv7,		b_hv7,		a_hv7,		a_hv7		)
	ASSIGN_8DUT( VREFQ,				b_hv1,		b_hv2,		a_hv1,		a_hv2,		b_hv3,		b_hv4,		a_hv3,		a_hv4		)

	//the following pins are driven from one PG and will have no APG BOE capability
	ASSIGN_8DUT_DOP( VSP0,			a_36,		a_36,		a_36,		a_36,		b_36,		b_36,		b_36,		b_36		)
	ASSIGN_8DUT_DOP( VSP1,			a_33,		a_33,		a_33,		a_33,		b_33,		b_33,		b_33,		b_33		)
	ASSIGN_8DUT_DOP( VSP2,			a_68,		a_68,		a_68,		a_68,		b_68,		b_68,		b_68,		b_68		)
}

PIN_ASSIGNMENTS( PA__NoShare_Mag7_Training )
{
	//						DUT13/14				DUT1/2			
	//						chn0		chn1		chn0		chn1
	ASSIGN_4DUT( DQS_t,		a_1,		a_13,		a_37,		a_49	)
	ASSIGN_4DUT( DQS_c,		a_2,		a_14,		a_38,		a_50	)
	ASSIGN_4DUT( DQ0,		a_3,		a_18,		a_48,		a_55	)
	ASSIGN_4DUT( DQ1,		a_4,		a_17,		a_46,		a_57	)
	ASSIGN_4DUT( DQ2,		a_5,		a_16,		a_47,		a_56	)
	ASSIGN_4DUT( DQ3,		a_6,		a_15,		a_45,		a_58	)
	ASSIGN_4DUT( DQ4,		a_9,		a_22,		a_42,		a_51	)
	ASSIGN_4DUT( DQ5,		a_11,		a_20,		a_41,		a_52	)
	ASSIGN_4DUT( DQ6,		a_10,		a_21,		a_40,		a_53	)
	ASSIGN_4DUT( DQ7,		a_12,		a_19,		a_39,		a_54	)
	ASSIGN_4DUT( RE_t,		a_7,		a_23,		a_44,		a_60	)
	ASSIGN_4DUT( RE_c,		a_8,		a_24,		a_43,		a_59	)
	ASSIGN_4DUT( CLE,		a_25,		b_25,		a_61,		b_61	)
	ASSIGN_4DUT( ALE,		a_26,		b_26,		a_62,		b_62	)
	ASSIGN_4DUT( RB0n,		a_27,		b_27,		a_63,		b_63	)
	ASSIGN_4DUT( WEn,		a_28,		b_28,		a_64,		b_64	)
	ASSIGN_4DUT( WPn,		a_29,		b_29,		a_65,		b_65	)
	ASSIGN_4DUT( ENi,		a_30,		a_30,		a_66,		a_66	)
	ASSIGN_4DUT( ENo,		b_30,		b_30,		b_66,		b_66	)
	ASSIGN_4DUT(CE0n,		a_32,		b_32,		a_68,		b_68	)

	if (device::num_chip_enables / 2 > 1)
	{
		ASSIGN_4DUT(RB1n,	a_31,		b_31,		a_67,		b_67	)
		ASSIGN_4DUT(CE1n,	a_33,		b_33,		a_69,		b_69	)
	}

	ASSIGN_4DUT( VCC,		a_dps1a,	a_dps1a,	a_dps2a,	a_dps2a	)
	ASSIGN_4DUT( VCCQ,		a_dps3a,	a_dps3a,	a_dps4a,	a_dps4a	)
	ASSIGN_4DUT( VPP,		a_hv5,		a_hv5,		a_hv7,		a_hv7	)
	ASSIGN_4DUT( VREFQ,		a_hv1,		a_hv2,		a_hv3,		a_hv4	)

	ASSIGN_4DUT(VSP0,		a_36,		a_36,		a_72,		a_72	)
	ASSIGN_4DUT(VSP1,		b_36,		b_36,		b_72,		b_72	)
	ASSIGN_4DUT(VSP2,		b_24,		b_24,		b_60,		b_60	)
}

CURRENT_SHARE( ONFI_X_Share )
{
	SHARE_X( VCC, 4 )
	SHARE_X( VCCQ, 4 )
}

CURRENT_SHARE( ONFI_Share )
{
	SHARE( VCC, 2 )
	SHARE( VCCQ, 2 )
}

CURRENT_SHARE( ONFI_No_Share )
{
	SHARE( VCC, 1 )
	SHARE( VCCQ, 1 )
}



















