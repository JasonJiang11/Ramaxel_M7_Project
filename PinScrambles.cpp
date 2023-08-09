#include "stdafx.h"
#include "PatternResources.h"
#include "DutPins.h"

#define HSN_PS(ps) enPSNumberMag7::CONCAT(t_, ps)

PIN_SCRAMBLE(ONFI_PINSCRAMBLE)
{
	PIN_SCRAMBLE_MAP(HSN_PS(PS_NOOP), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_IN), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1)		//scramble bug
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2)			//scramble bug
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DBM_IN), enDatgenStrobeMapNumberMag7::t_DSM2)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1)		//scramble bug
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2)			//scramble bug
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_IN_PREAMBLE), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high)		
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low)			
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_OUT), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DBM_OUT), enDatgenStrobeMapNumberMag7::t_DSM2)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_OUT_PREAMBLE), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_COL1), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y7)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_COL2), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_y15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_ROW1), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x7)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_ROW2), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x15)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_ROW3), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x16)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x17)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x18)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x19)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x20)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x21)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x22)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_x23)
		}
	}

	PIN_SCRAMBLE_MAP(HSN_PS(PS_POLL_RBSY), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
	}

	//with ADVANCE operation LFSR can generate new data on each ui 
	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_IN_LFSR), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs1)		//scramble bug
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2)			//scramble bug
		}
		//with ADVANCE operation LFSR can generate new data on each ui 
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7)
		}
	}

	//with ADVANCE operation LFSR can generate new data on each ui 
	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_OUT_LFSR), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			//SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low)

		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
				//SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_pg_output_high)

		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_cs1)
		}
		//with ADVANCE operation LFSR can generate new data on each ui 
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d0)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d2)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d5)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d6)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d7)
		}
	}

	//for debug of dbi function
	PIN_SCRAMBLE_MAP(HSN_PS(PS_DATA_IN_DBI), enDatgenStrobeMapNumberMag7::t_DSM1)
	{
		/*	DUT Pin		Tester		*/
		/*	 Name		Function	*/
		/*	-------		--------	*/
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs3)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CE1n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs4)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::CLE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs7)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ALE())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WEn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_cs8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10, enTesterFuncMag7::t_pg_output_high, enTesterFuncMag7::t_cs10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RE_c())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::WPn())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_high)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENi())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::ENo())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::RB0n())
		{
			SCRAMBLE_1_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low)
		}
		//let DQS be driven by DBI to view on logic analyzer
		//this dut does not have a DBI pin
		//these are the pin scrambles that the DBI pin would use
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_t())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQS_c())
		{
			//SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2, enTesterFuncMag7::t_pg_output_low, enTesterFuncMag7::t_cs2)			//scramble bug
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0, enTesterFuncMag7::t_DM1, enTesterFuncMag7::t_DM0 )
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ0())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8, enTesterFuncMag7::t_d0, enTesterFuncMag7::t_d8)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ1())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9, enTesterFuncMag7::t_d1, enTesterFuncMag7::t_d9)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ2())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10, enTesterFuncMag7::t_d2, enTesterFuncMag7::t_d10)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ3())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11, enTesterFuncMag7::t_d3, enTesterFuncMag7::t_d11)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ4())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12, enTesterFuncMag7::t_d4, enTesterFuncMag7::t_d12)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ5())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13, enTesterFuncMag7::t_d5, enTesterFuncMag7::t_d13)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ6())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14, enTesterFuncMag7::t_d6, enTesterFuncMag7::t_d14)
		}
		PIN_SCRAMBLE_ALL_DUTS(dutpin::DQ7())
		{
			SCRAMBLE_8_RESOURCE_ALL_DUTS(enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15, enTesterFuncMag7::t_d7, enTesterFuncMag7::t_d15)
		}
	}
}