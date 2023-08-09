#include "stdafx.h"
#include "Levels.h"
#include "PinLists.h"
#include "DutPins.h"
#include "i2c.h"

DOUBLE_VARIABLE(uv_dps_ramp_rate, 2, " ") {}

static double _dps_cap = 22 * 1e-6;
ONEOF_VARIABLE(DPS_CAPACITANCE, "C22UF, NONE, C0NF, C50NF, C83NF, C138NF, C229NF, C380NF, C630NF, C1p1UF, C1p7UF, C2p9UF, C4p8UF, C7p9UF, C13UF, C36UF, C60UF", " ")
{
	if (	 DPS_CAPACITANCE == "NONE")		_dps_cap = -1;
	else if (DPS_CAPACITANCE == "C0NF")		_dps_cap = 0;
	else if (DPS_CAPACITANCE == "C50NF")	_dps_cap = 50 * 1e-9;
	else if (DPS_CAPACITANCE == "C83NF")	_dps_cap = 83 * 1e-9;
	else if (DPS_CAPACITANCE == "C138NF")	_dps_cap = 138 * 1e-9;
	else if (DPS_CAPACITANCE == "C229NF")	_dps_cap = 229 * 1e-9;
	else if (DPS_CAPACITANCE == "C380NF")	_dps_cap = 380 * 1e-9;
	else if (DPS_CAPACITANCE == "C630NF")	_dps_cap = 630 * 1e-9;
	else if (DPS_CAPACITANCE == "C1p1UF")	_dps_cap = 1.1 * 1e-6;
	else if (DPS_CAPACITANCE == "C1p7UF")	_dps_cap = 1.7 * 1e-6;
	else if (DPS_CAPACITANCE == "C2p9UF")	_dps_cap = 2.9 * 1e-6;
	else if (DPS_CAPACITANCE == "C4p8UF")	_dps_cap = 4.8 * 1e-6;
	else if (DPS_CAPACITANCE == "C7p9UF")	_dps_cap = 7.9 * 1e-6;
	else if (DPS_CAPACITANCE == "C13UF")	_dps_cap = 13 * 1e-6;
	else if (DPS_CAPACITANCE == "C22UF")	_dps_cap = 22 * 1e-6;
	else if (DPS_CAPACITANCE == "C36UF")	_dps_cap = 36 * 1e-6;
	else if (DPS_CAPACITANCE == "C60UF")	_dps_cap = 60 * 1e-6;
}

static double _esr = 0;
ONEOF_VARIABLE(ESR, "R0mOhm, R1mOhm, R1p8mOhm, R3p4mOhm, R6p3mOhm, R12mOhm, R21mOhm, R40mOhm, R74mOhm, R140mOhm, R250mOhm, R460mOhm, R860mOhm, R1500mOhm, R2900mOhm, R6400mOhm", " ")
{
	if (ESR == "R0mOhm")			_esr = 0;
	else if (ESR == "R1mOhm")		_esr = 1 * 1e-3;
	else if (ESR == "R1p8mOhm")		_esr = 1.8 * 1e-3;
	else if (ESR == "R3p4mOhm")		_esr = 3.4 * 1e-3;
	else if (ESR == "R6p3mOhm")		_esr = 6.3 * 1e-3;
	else if (ESR == "R12mOhm")		_esr = 12 * 1e-3;
	else if (ESR == "R21mOhm")		_esr = 21 * 1e-3;
	else if (ESR == "R40mOhm")		_esr = 40 * 1e-3;
	else if (ESR == "R74mOhm")		_esr = 74 * 1e-3;
	else if (ESR == "R140mOhm")		_esr = 140 * 1e-3;
	else if (ESR == "R250mOhm")		_esr = 250 * 1e-3;
	else if (ESR == "R460mOhm")		_esr = 460 * 1e-3;
	else if (ESR == "R860mOhm")		_esr = 860 * 1e-3;
	else if (ESR == "R1500mOhm")	_esr = 1500 * 1e-3;
	else if (ESR == "R2900mOhm")	_esr = 2900 * 1e-3;
	else if (ESR == "R6400mOhm")	_esr = 6400 * 1e-3;
}


// Core power supply (VCC)

const double vCC_min =  2.35 V;
const double vCC_typ =  2.5 V;
const double vCC_max =  3.6 V;

// I/O power supply (VCCQ)
const double vCCQ_min = 1.14 V;
const double vCCQ_typ = 1.2 V;
const double vCCQ_max = 1.26 V;

// Optional external high voltage power supply (VPP)
const double vPP_min = 10.08 V;
const double vPP_typ = 12.0 V;
const double vPP_max = 13.2 V;

//the tester has a minimum seperation of vih and vil 
const double min_vil_vih = 0.2 V;

double levels::get_min_vil_vih()
{
	return min_vil_vih;
}

void _pe_driver_set(PinList* pinlist, const double vih, const double vil, const double vterm, const double rterm, const double vtt, enPEDrvPeakingMag7 peaking)
{
	int status = pe_driver_set(pinlist, vih, vil, vterm, rterm, vtt, peaking);

	if (status != 0)
	{
		output("status not 0");

		switch (status)
		{
			// always check the return code, non-zero means trouble
			  //   Returns a bitmask, where
		case 0x01:
			warning("0x01  bit 0 - VTT VIL min Swing 0.150: do nothing  (isVttMax true)");
			break;
		case 0x02:
			warning("0x02  bit 1 - VIH VTT min Swing 0.150: do nothing  (isVttMin true)");
			break;
		case 0x04:
			warning("0x04  bit 2   VIH VIL min Swing 0.150: do nothing");
			break;
		case 0x08:
			warning("0x08  bit 3 - Peaking Level exceeded limit. And has been adjusted to max. Still Set Levels");
			break;
		case 0x10:
			warning("0x10  bit 4 - Internal Driver VDH Level unattainable. Nothing done");
			break;
		case 0x20:
			warning("0x20  bit 5 - Internal Driver VDL Level unattainable. Nothing done");
			break;
		case 0x40:
			warning("0x40  bit 6 - Internal Driver Error. Nothing done");
			break;
		}
	}
}

void levels::power_down()
{
	const double testerpin_vih = 0.0 V;
	const double testerpin_vil = 0.0 V;
	const double dutpin_vterm_low = 0.0 V;
	const double vtt = 0.0;
	const double fast_peaking_voltage = 0.0 V;
	const double slow_peaking_voltage = 0.0 V;

	//set signal pins to 0 V with pe_driver_set
	_pe_driver_set(pinlist::AllDataAndDataStrobePins(), min_vil_vih, 0, 0, levels::HIGH_IMPEDANCE, vtt, enPEDrvPeakingMag7::t_PEAK_NONE);
	_pe_driver_set(pinlist::AllInputPins(), min_vil_vih, 0, 0, levels::HIGH_IMPEDANCE, vtt, enPEDrvPeakingMag7::t_PEAK_NONE);

	// Must keep RE_t/RE_c, DQS_t/DQS_c, and DQ[7:0] LOW while VCCQ is 0V.
	pin_dc_state_set( pinlist::AllDataAndDataStrobePins(), PEDriverState::t_vil, FALSE );
	pin_dc_state_set( pinlist::AllInputPins(), PEDriverState::t_vil, FALSE );

	//set supplies to 0V
	dps( 0 V, dutpin::VCC() );
	dps( 0 V, dutpin::VCCQ() );

	hv_voltage_set( 0 V, dutpin::VREFQ() );
	hv_voltage_set( 0 V, dutpin::VPP() );
}

DOUBLE_VARIABLE( uv_voh_rb, 0.45, " " ) {}
DOUBLE_VARIABLE( uv_vol_rb, 0.45, " " ) {}
DOUBLE_VARIABLE(uv_voh, 0.55, " ") {}
DOUBLE_VARIABLE(uv_vol, 0.55, " ") {}
DOUBLE_VARIABLE(uv_vih, 1.1, " ") {}
DOUBLE_VARIABLE(uv_vtt, 0.55, " ") {}

DOUBLE_VARIABLE(uv_vpulse_volts, 1000, "uv_ripple_volts(MV) ") {}

BOOL_VARIABLE(OPUS_MODE, FALSE, "opus_mode") {}

//setup dps ripple parameters
void dps_vpulse_setup(DutPin *dp, double volts)
{
	dps_vpulse_mode_set(enVPulseModeMag7::t_VPulse);
	dps_vpulse(volts, pinlist::pl_VCC());
}

static void apply_levels(double vCCQ_level)
{
	voh_vol_pf_table_mode_set(pinlist::AllDataAndDataStrobePins(), false);
	voh_vol_pf_table_mode_set(pinlist::AllReadyBusyPins(), false);

	// Assert WP#, and de-assert CLE and ALE to prevent inadvertent command latch
	pin_dc_state_set( pinlist::AllWriteProtectPins(), PEDriverState::t_vil );
	pin_dc_state_set( pinlist::AllAddressLatchEnablePins(), PEDriverState::t_vil );
	pin_dc_state_set( pinlist::AllCommandLatchEnablePins(), PEDriverState::t_vil );

	// Set all other control signals to "de-asserted" states. Setting CE# to Vih
	// puts the bus in standby mode.
	pin_dc_state_set( pinlist::AllChipEnablePins(), PEDriverState::t_vih );
	pin_dc_state_set( pinlist::AllDataPins(), PEDriverState::t_vil );
	pin_dc_state_set( pinlist::AllReadEnablePins(), PEDriverState::t_vih );
	pin_dc_state_set( pinlist::AllWriteEnablePins(), PEDriverState::t_vih );
	pin_dc_state_set( pinlist::AllDataStrobePins(), PEDriverState::t_vil );

	const double testerpin_vih = uv_vih V;
	const double testerpin_vil = 0.1 V;
	const double dutpin_vterm_low = 0.0 V;
	const double testerpin_vtt = vCCQ_level / 2;
	const enPEDrvPeakingMag7 peaking = enPEDrvPeakingMag7::t_PEAK_NONE;

	// Let ENi be pulled high by the part. 
	_pe_driver_set( pinlist::pl_ENi(),						testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, testerpin_vtt, peaking);

	//// Set pin levels
	_pe_driver_set( pinlist::AllInputPins(),				testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, testerpin_vtt, peaking);

	_pe_driver_set( pinlist::AllDataAndDataStrobePins(),	testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, uv_vtt V, peaking);

	_pe_driver_set( pinlist::AllChipEnablePins(),			testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, testerpin_vtt, peaking);
	_pe_driver_set( pinlist::AllWriteProtectPins(),			testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, testerpin_vtt, peaking);
	_pe_driver_set( pinlist::AllReadEnablePins(),			testerpin_vih, testerpin_vil, testerpin_vtt, levels::HIGH_IMPEDANCE, testerpin_vtt, peaking);

	// The ready/busy# is pulled low when the target is busy.
	// Otherwise, this pin is terminated at high.
	_pe_driver_set( pinlist::AllReadyBusyPins(),			testerpin_vih, testerpin_vil, vCCQ_level, levels::HIGH_IMPEDANCE, testerpin_vih, peaking );

	pe_driver_rSrc_set(pinlist::AllDataAndDataStrobePins(), 50);  // Set Driver RSource resistance in Ohms

	// De-assert WP#
	pin_dc_state_set( pinlist::AllWriteProtectPins(), PEDriverState::t_vih, TRUE );
	
	//pe_driver_peaking_timeconst_set( pinlist::AllPins(), PeakingTauFast::TAU_FAST_50PS, PeakingTauSlow::TAU_SLOW_300PS );
	
	voh(testerpin_vih / 2, pinlist::AllPins());
	vol(testerpin_vih / 2, pinlist::AllPins());

	voh(testerpin_vih / 2 - 100 MV, pinlist::AllReadyBusyPins());
	vol(testerpin_vih / 2 - 100 MV, pinlist::AllReadyBusyPins());

	voh( uv_voh_rb V, pinlist::AllReadyBusyPins() );
	vol( uv_vol_rb V, pinlist::AllReadyBusyPins() );
	
	disconnect( dutpin::ENi() );
	disconnect( dutpin::ENo() );
}

BOOL_VARIABLE(uv_disconnect_slaves, FALSE, " ") {};

void levels::power_up( double vCC_level, double vCCQ_level, double vPP_level, DutPin* dp )
{
	//apply levels to PE channels
	apply_levels(vCCQ_level);

	//if (uv_disconnect_slaves)
	//{
	//	//dps_slaves_disconnect(pinlist::pl_VCC()); //doesn't work
	//	resource_deallocate(S_CurrentShare);
	//	CurrentShare_use("ONFI_No_Share");
	//	resource_initialize(S_CurrentShare);
	//}
	//else
	//{
	//	resource_deallocate(S_CurrentShare);
	//	CurrentShare_use("ONFI_X_Share");
	//	resource_initialize(S_CurrentShare);
	//}

	dps( 0, dutpin::VCC() );
	dps( 0, dutpin::VCCQ() );

	hv_voltage_set( 0, dutpin::VREFQ() );
	hv_voltage_set( 0, dutpin::VPP() );

	dps_connect( pinlist::AllPowerSupplies() );
	hv_connect( pinlist::AllHvSupplies() );

	if( dp != nullptr )
		adc_dps_samples_trigger( dp, DCForceMeasType::t_measV );

	// Ramp VCC, VCCQ, then VREFQ.
	dps( vCCQ_level, uv_dps_ramp_rate, dutpin::VCCQ() );
	Delay( 100 US );
	dps( vCC_level, uv_dps_ramp_rate, dutpin::VCC() );
	Delay( 2 MS);

	hv_voltage_set( 0.5 * vCCQ_level, dutpin::VREFQ() );
	hv_voltage_set(vPP_level, dutpin::VPP());

	// Wait at least 100us for R/B# to be valid and HIGH
	Delay( 500 US );


	Delay(200 MS);				//this is necessary, not sure why

	DpsCompCapInfo info;
	info.capacitance = _dps_cap;
	info.esr = _esr;
	info.mode = _dps_cap == -1 ? DpsCompCapMode::t_comp_safe_mode : DpsCompCapMode::t_comp_auto_mode;
	dps_comp_cap_set(info, pinlist::pl_VCC());
	dps_comp_cap_set(info, pinlist::pl_VCCQ());

	i2c::connect_all_vcc_vccq_caps();

	dps_vpulse_setup(dutpin::VCC(),  uv_vpulse_volts MV);
	dps_vpulse_setup(dutpin::VCCQ(), uv_vpulse_volts MV);
}

void levels::restore_volvoh()
{
	double testerpin_vih = vih(dutpin::DQ0());
	voh(testerpin_vih / 2, pinlist::AllPins());
	vol(testerpin_vih / 2, pinlist::AllPins());
}

void levels::update_rterm(double rterm)
{
	output("update_rterm: updating termination for DQ, DQS, and RE ");

	double vCCQ_level = dps(dutpin::VCCQ());	//get level from first dut in the ADS

	const double testerpin_vih = uv_vih V;
	const double testerpin_vil = 0.1 V;
	const double dutpin_vterm_low = 0.0 V;
	const double testerpin_vtt = vCCQ_level / 2;

	enPEDrvPeakingMag7 vpeak;
	pe_driver_peaking_get(dutpin::DQ0(), vpeak);
	_pe_driver_set(pinlist::AllDataAndDataStrobePins(),	testerpin_vih, testerpin_vil, testerpin_vtt, rterm, testerpin_vtt, vpeak);

	pe_driver_peaking_get(dutpin::RE_t(), vpeak);
	_pe_driver_set(pinlist::AllReadEnablePins(),		testerpin_vih, testerpin_vil, testerpin_vtt, rterm, testerpin_vtt, vpeak);
}



