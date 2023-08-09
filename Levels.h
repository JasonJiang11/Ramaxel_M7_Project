#pragma once
#include "TestProgApp/public.h"

namespace levels
{
	constexpr int HIGH_IMPEDANCE = 5000000;

	enum class TerminateVoltage
	{
		VIL, VIH
	};

	//this member function returns the tester minimum seperation of vih and vil 
	double get_min_vil_vih();

	// Power-down device
	void power_down();

	// Power-up device and place in standby mode. Also set the levels of all signals.
	// If |dp| is provided, trigger DC A/D converter before |dp| power supply ramps up.
	void power_up( double vCC_level, double vCCQ_level, double vPP_level, DutPin* dp = nullptr );

	//restore vol/voh after user variable shmoo
	void restore_volvoh();

	//update rterm for DQ, DQS, RE after set feature
	void update_rterm(double rterm);
		
	using ChipEnableBitMask = unsigned int;

	constexpr ChipEnableBitMask ASSERT_CE0 = 1 << 0; // CKE0
	constexpr ChipEnableBitMask ASSERT_CE1 = 1 << 2; // CKE2 
	constexpr ChipEnableBitMask ASSERT_CE2 = 1 << 4; // CKE4
	constexpr ChipEnableBitMask ASSERT_CE3 = 1 << 6; // CKE6
}

EXTERN_DOUBLE_VARIABLE(uv_voh) 
EXTERN_DOUBLE_VARIABLE(uv_vol) 
