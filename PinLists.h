#pragma once

#include "TestProgApp/public.h"
#include "Utilities.h"

namespace pinlist
{
	DECLARE_PINLIST( AllDataPins				)
	DECLARE_PINLIST( pl_DQ0						)
	DECLARE_PINLIST( pl_DQ1						)
	DECLARE_PINLIST( pl_DQ2						)
	DECLARE_PINLIST( pl_DQ3						)
	DECLARE_PINLIST( pl_DQ4						)
	DECLARE_PINLIST( pl_DQ5						)
	DECLARE_PINLIST( pl_DQ6						)
	DECLARE_PINLIST( pl_DQ7						)
	DECLARE_PINLIST( pl_DQS_t					)
	DECLARE_PINLIST( pl_DQS_c					)
	DECLARE_PINLIST( AllDataStrobePins			)
	DECLARE_PINLIST( pl_RE_t					)
	DECLARE_PINLIST( pl_RE_c					)
	DECLARE_PINLIST( AllReadEnablePins			)
	DECLARE_PINLIST( pl_RB0						)
	DECLARE_PINLIST( pl_RB1						)
	DECLARE_PINLIST( AllReadyBusyPins			)
	DECLARE_PINLIST(AllDataDataStrobeReadyBusyPins	)

	DECLARE_PINLIST( AllWriteEnablePins			)
	DECLARE_PINLIST( AllCommandLatchEnablePins	)
	DECLARE_PINLIST( AllAddressLatchEnablePins	)
	DECLARE_PINLIST( ALE_CLE					)
	DECLARE_PINLIST( AllWriteProtectPins		)
	DECLARE_PINLIST( pl_CE0						)
	DECLARE_PINLIST( pl_CE1						)
	DECLARE_PINLIST( AllChipEnablePins			)
	DECLARE_PINLIST( pl_ENi						)
	DECLARE_PINLIST( pl_ENo						)
	DECLARE_PINLIST( AllEnumeratedPins			)
	DECLARE_PINLIST( AllRB_ENo					)
	DECLARE_PINLIST( AllInputPins				)
	DECLARE_PINLIST( AllInputPins_Leakage		)
	DECLARE_PINLIST( AllInputPins_minusRE		)
	DECLARE_PINLIST( AllOutputPins				)
	DECLARE_PINLIST( AllPins					)
	DECLARE_PINLIST( Continuity_Upper_Diode		)
	DECLARE_PINLIST( Continuity_Lower_Diode		)
	DECLARE_PINLIST( pl_VCC						)
	DECLARE_PINLIST( pl_VREFQ					)
	DECLARE_PINLIST( pl_VCCQ					)
	DECLARE_PINLIST( AllDpsSupplies				)
	DECLARE_PINLIST( AllHvSupplies				)
	DECLARE_PINLIST( AllPowerSupplies			)
	DECLARE_PINLIST( pl_VPP						)
	DECLARE_PINLIST( VSP						)
	DECLARE_PINLIST( AllDataAndDataStrobePins	)

	// Returns a vector where each element contains one CE# per bus
	std::vector< std::vector< DutPin* > > get_single_chip_enables_per_bus();
}

EXTERN_PINLIST(pl_pin1) 


