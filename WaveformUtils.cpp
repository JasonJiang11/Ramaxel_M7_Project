#include "stdafx.h"
#include <map>
#include "WaveformUtils.h"

// Creates and formats Waveform objects (using the ADC samples stored in the PMU for the specified DutPin/PinList)
// for each DUT in the Active DUT Set, and stores a pointer to the resultant Waveform in the DUT map |wfmap|.
static void _store_waveforms(PassCond pass_cond, DutPin* pin, CString tl_name, std::map< DutNum, Waveform* >& wfmap)
{
	// Get ADC measurement interval
	double meas_interval = adc_sample_interval_get(pin);

	// Choose the 'yunits' for the Waveform objects
	LPCTSTR yunits = "";
	CString measType = "";
	switch(pass_cond)
	{
	case pass_ncl:
	case pass_nicl:
	case pass_pcl:
		yunits = "nanoamperes";
		measType = "CURRENT";
		break;

	default:
	case pass_vg:
	case pass_vl:
	case pass_nivl:
		yunits = "microvolts";
		measType = "VOLTAGE";
		break;
	}

	// Get DutPin name and use for the Waveform name
	CString pin_name = resource_name(pin);

	// Empty the DutNum-Waveform map before proceeding
	wfmap.clear();

	SOFT_DUT_LOOP(dut)
	{
		// Get ADC measurements.
		DoubleArray data;
		adc_measurements_get(pin, dut, data);

		// Create a new Waveform object using all the samples stored in |data|.
		CString wf_name = vFormat("%s__%s__%s__DUT%02d", tl_name, pin_name, measType, dut + 1);
		Waveform* wf = Waveform_find(wf_name);
		if(wf == NULL) wf = waveform_create(wf_name);

		// Store measurments in |wf|
		waveform_set_rrect(wf, data, yunits, 0, meas_interval, "picoseconds");

		// Store |wf| in a DUT map
		wfmap[dut] = wf;
	}
}
static void _store_waveforms(PassCond pass_cond, PinList* pl, CString tl_name, std::map< DutPin*, std::map< DutNum, Waveform* > >& wfmap)
{
	DutPin* pin;
	wfmap.clear();
	for(int i = 0; pin_info(pl, i, &pin); ++i)
	{
		std::map< DutNum, Waveform* > pin_wfmap;
		_store_waveforms(pass_cond, pin, tl_name, pin_wfmap);
		wfmap[pin] = pin_wfmap;
	}
}

void waveform::trigger_capture(PinList* pPinList, BOOL measureCurrent, int sampleCount, double sampleInterval)
{
	adc_average_count_set(1, pPinList);
	adc_sample_count_set(sampleCount, pPinList);
	adc_sample_interval_set(sampleInterval, pPinList);
	adc_vcomp_delay_set(0, pPinList);

	adc_dps_samples_trigger(pPinList, measureCurrent ? DCForceMeasType::t_measI : DCForceMeasType::t_measV);
}
void waveform::read(PinList* pPinList, BOOL measureCurrent, CString wfName)
{
	auto passCond = measureCurrent ? PassCond::pass_nicl : PassCond::pass_vg;
	std::map< DutPin*, std::map< DutNum, Waveform* > > wfmap;
	_store_waveforms(passCond, pPinList, wfName, wfmap);
}