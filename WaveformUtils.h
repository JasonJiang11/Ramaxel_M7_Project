#pragma once

#include "TestProgApp/public.h"

namespace waveform
{
	void trigger_capture(PinList* pPinList, BOOL measureCurrent, int sampleCount, double sampleInterval);
	void read(PinList* pPinList, BOOL measureCurrent, CString wfName);
}

