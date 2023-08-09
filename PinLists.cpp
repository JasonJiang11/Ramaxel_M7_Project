#include "stdafx.h"
#include "PinLists.h"
#include "DeviceSpecs.h"

#define DEFINE_PINLIST_FUNC( name )                     \
  PinList* pinlist::name( DutNum dut )                  \
  {                                                     \
    PinList* pl;                                        \
    if ( dut == t_dut_na )                              \
	{                                                   \
      pl = PinList_find( #name );                       \
    }                                                   \
    else                                                \
	{                                                   \
      CString pinlist_name;                             \
      pinlist_name.Format("%s_Dut%d", #name, dut + 1 ); \
      pl = PinList_find( pinlist_name );                \
    }                                                   \
    return pl;                                          \
  }

#define DEFINE_PINLIST( name )                          \
  DEFINE_PINLIST_FUNC( name )                           \
  PINLIST( name )

bool CreatePerDutPinlist(PinList* pl)
{
	if (!OnSite()) return false;

	// For all DUTs
	for (DutNum dut = t_dut1; dut <= max_dut(); ++dut)
	{
		DutPin* dpin;
		Array< TesterPin > result;
		// For all DutPins in a pin list, |pl|
		for (int i = 0; pin_info(pl, i, &dpin); ++i)
		{
			TesterPin tpin;
			if (pin_info(dpin, dut, &tpin))
				result.Add(tpin); // Add TesterPin
		}
		CString name;
		name.Format("%s_Dut%d", resource_name(pl), dut + 1);

		// Dynamically create a per DUT pinlist
		PinList* dpl = pinlist_create(result.GetData(), static_cast<int>(result.GetCount()), name);
	}

	return true;
}

#define DEFINE_PER_DUT_PINLIST( name )                  \
  DEFINE_PINLIST_FUNC( name )                           \
  AFTER_INITIALIZING( PinList, name )                   \
  {                                                     \
    EXTERN_PINLIST( name )                              \
    extern bool CreatePerDutPinlist( PinList* );        \
    CreatePerDutPinlist( name );                        \
  }                                                     \
  PINLIST( name )

DEFINE_PER_DUT_PINLIST(AllDataPins)
{
	PINS(DQ0, DQ1, DQ2, DQ3, DQ4, DQ5, DQ6, DQ7)
}

DEFINE_PER_DUT_PINLIST( pl_DQS_t )
{
	PINS1(DQS_t)
}

DEFINE_PER_DUT_PINLIST( pl_DQS_c )
{
	PINS1(DQS_c)
}

DEFINE_PER_DUT_PINLIST(pl_DQ0)
{
	PIN(DQ0)
}
DEFINE_PER_DUT_PINLIST(pl_DQ1)
{
	PIN(DQ1)
}
DEFINE_PER_DUT_PINLIST(pl_DQ2)
{
	PIN(DQ2)
}
DEFINE_PER_DUT_PINLIST(pl_DQ3)
{
	PIN(DQ3)
}
DEFINE_PER_DUT_PINLIST(pl_DQ4)
{
	PIN(DQ4)
}
DEFINE_PER_DUT_PINLIST(pl_DQ5)
{
	PIN(DQ5)
}
DEFINE_PER_DUT_PINLIST(pl_DQ6)
{
	PIN(DQ6)
}
DEFINE_PER_DUT_PINLIST(pl_DQ7)
{
	PIN(DQ7)
}

DEFINE_PER_DUT_PINLIST( AllDataStrobePins )
{
	INCLUDE_PINLIST( pl_DQS_t )
	INCLUDE_PINLIST( pl_DQS_c )
}

DEFINE_PINLIST(AllDataAndDataStrobePins)
{
	INCLUDE_PINLIST(AllDataPins)
	INCLUDE_PINLIST(AllDataStrobePins)
}

DEFINE_PER_DUT_PINLIST( pl_RE_t )
{
	PINS1( RE_t )
}

DEFINE_PER_DUT_PINLIST( pl_RE_c )
{
	PINS1( RE_c )
}

DEFINE_PER_DUT_PINLIST( AllReadEnablePins )
{
	INCLUDE_PINLIST( pl_RE_t );
	INCLUDE_PINLIST( pl_RE_c );
}

DEFINE_PER_DUT_PINLIST( pl_RB0 )
{
	PINS1( RB0n )
}

DEFINE_PER_DUT_PINLIST( pl_RB1 )
{
	//each onfi channel is one tester dut
	//if nand has 2 CEs then there is 1 RB per channel or 1 CE per tester dut
	//if nand has 4 CEs then there is 2 RB per channel or 2 CE per tester dut
	if (device::num_chip_enables / 2 > 1)
	{
		PINS1(RB1n)
	}
}

DEFINE_PER_DUT_PINLIST( AllReadyBusyPins )
{
	//each onfi channel is one tester dut
	//if nand has 2 CEs then there is 1 RB per channel or 1 CE per tester dut
	//if nand has 4 CEs then there is 2 RB per channel or 2 CE per tester dut
	INCLUDE_PINLIST( pl_RB0 );
	if (device::num_chip_enables / 2 > 1)
	{
		INCLUDE_PINLIST(pl_RB1);
	}
}

DEFINE_PER_DUT_PINLIST(AllDataDataStrobeReadyBusyPins)
{
	INCLUDE_PINLIST(AllDataAndDataStrobePins)
	INCLUDE_PINLIST(AllReadyBusyPins)
}


DEFINE_PINLIST( AllWriteEnablePins )
{
	PINS1( WEn )
}

DEFINE_PINLIST( AllCommandLatchEnablePins )
{
	PINS1( CLE )
}

DEFINE_PINLIST( AllAddressLatchEnablePins )
{
	PINS1( ALE )
}

DEFINE_PINLIST(ALE_CLE)
{
	PINS2(ALE, CLE)
}

DEFINE_PINLIST( AllWriteProtectPins )
{
	PINS1( WPn )
}

DEFINE_PINLIST( pl_CE0 )
{
	PINS1( CE0n )
}

DEFINE_PINLIST( pl_CE1 )
{
	//each onfi channel is one tester dut
	//if nand has 2 CEs then there is 1 RB per channel or 1 CE per tester dut
	//if nand has 4 CEs then there is 2 RB per channel or 2 CE per tester dut
	if (device::num_chip_enables / 2 > 1)
	{
		PINS1(CE1n)
	}
}

DEFINE_PINLIST( AllChipEnablePins )
{
	//each onfi channel is one tester dut
	//if nand has 2 CEs then there is 1 RB per channel or 1 CE per tester dut
	//if nand has 4 CEs then there is 2 RB per channel or 2 CE per tester dut
	INCLUDE_PINLIST( pl_CE0 )
	if (device::num_chip_enables / 2 > 1)
	{
		INCLUDE_PINLIST(pl_CE1)
	}
}

DEFINE_PINLIST( pl_ENi )
{
	PINS1( ENi )
}

DEFINE_PINLIST( pl_ENo )
{
	PINS1( ENo )
}

DEFINE_PINLIST( AllEnumeratedPins )
{
	PINS2( ENi, ENo )
}

DEFINE_PINLIST( VSP )
{
	PINS3(VSP0, VSP1, VSP2)
}

DEFINE_PINLIST( AllInputPins )
{
	INCLUDE_PINLIST( AllChipEnablePins )
	INCLUDE_PINLIST( AllWriteProtectPins )
	INCLUDE_PINLIST( AllCommandLatchEnablePins )
	INCLUDE_PINLIST( AllAddressLatchEnablePins )
	INCLUDE_PINLIST( AllWriteEnablePins )
	INCLUDE_PINLIST( AllReadEnablePins )
	INCLUDE_PINLIST( pl_ENi )
	INCLUDE_PINLIST( VSP )
}

DEFINE_PINLIST(AllInputPins_minusRE)
{
	INCLUDE_PINLIST(AllChipEnablePins)
	INCLUDE_PINLIST(AllWriteProtectPins)
	INCLUDE_PINLIST(AllCommandLatchEnablePins)
	INCLUDE_PINLIST(AllAddressLatchEnablePins)
	INCLUDE_PINLIST(AllWriteEnablePins)
	INCLUDE_PINLIST(pl_ENi)
	INCLUDE_PINLIST(VSP)
}

DEFINE_PINLIST( AllInputPins_Leakage )
{
	INCLUDE_PINLIST( AllChipEnablePins )
	INCLUDE_PINLIST( AllWriteProtectPins )
	INCLUDE_PINLIST( AllCommandLatchEnablePins )
	INCLUDE_PINLIST( AllAddressLatchEnablePins )
	INCLUDE_PINLIST( AllWriteEnablePins )
	INCLUDE_PINLIST( AllReadEnablePins )
}

DEFINE_PINLIST(AllRB_ENo)
{
	INCLUDE_PINLIST(AllReadyBusyPins)
	INCLUDE_PINLIST(pl_ENo)
}

DEFINE_PINLIST(Continuity_Lower_Diode)
{
	INCLUDE_PINLIST(AllChipEnablePins)
	INCLUDE_PINLIST(AllWriteProtectPins)
	INCLUDE_PINLIST(AllCommandLatchEnablePins)
	INCLUDE_PINLIST(AllAddressLatchEnablePins)
	INCLUDE_PINLIST(AllWriteEnablePins)
	INCLUDE_PINLIST(AllReadEnablePins)
	INCLUDE_PINLIST(AllDataPins)
	INCLUDE_PINLIST(AllDataStrobePins)
	INCLUDE_PINLIST(AllReadyBusyPins)
}

DEFINE_PINLIST(Continuity_Upper_Diode)
{
	INCLUDE_PINLIST(AllReadEnablePins)
	INCLUDE_PINLIST(AllDataPins)
	INCLUDE_PINLIST(AllDataStrobePins)
	INCLUDE_PINLIST(AllReadyBusyPins)
}

DEFINE_PINLIST( pl_VCC )
{
	PINS1( VCC )
}

DEFINE_PINLIST( pl_VREFQ )
{
	PINS1( VREFQ )
}

DEFINE_PINLIST( pl_VCCQ ){
	PINS1( VCCQ )
}

DEFINE_PINLIST( AllDpsSupplies )
{
	INCLUDE_PINLIST( pl_VCC )
	INCLUDE_PINLIST( pl_VCCQ )
}

DEFINE_PINLIST(AllHvSupplies)
{
	INCLUDE_PINLIST(pl_VREFQ)
	INCLUDE_PINLIST(pl_VPP)
}

DEFINE_PINLIST( AllPowerSupplies )
{
	INCLUDE_PINLIST( pl_VCC )
	INCLUDE_PINLIST( pl_VCCQ )
}

DEFINE_PINLIST( pl_VPP )
{
	PINS1( VPP )
}

DEFINE_PINLIST(AllPins)
{
	INCLUDE_PINLIST(AllInputPins)
	INCLUDE_PINLIST(AllRB_ENo)
	INCLUDE_PINLIST(AllDataStrobePins)
	INCLUDE_PINLIST(AllDataPins)
}

PINLIST(pl_pin1){ PIN(DQS_t) }



