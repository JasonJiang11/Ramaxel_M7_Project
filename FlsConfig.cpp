#include "FlsConfig.h"


inline unsigned long ciel_bit(unsigned long in) {
	unsigned long msb;
	if (in < 2) return in;
	_BitScanReverse(&msb, in - 1);
	return 1ULL << (msb + 1);  //Round up to power of 2
}

void get_flsConfig_defaults(fls_config_struct& fconfig)
{
	unsigned int bit = 0;
	//// FLS Address Bit Assignments
	//FLS_Address[15:0] = Y[15:0]
	for (int tf = (int)TesterFunc::t_y0; tf <= (int)TesterFunc::t_y15; tf++, bit++) {
		fconfig.address_order[bit] = (TesterFunc(tf));
	}

	//FLS_Address[39:16] = X[23:0]
	for (int tf = (int)TesterFunc::t_x0; tf <= (int)TesterFunc::t_x23; tf++, bit++) {
		fconfig.address_order[bit] = (TesterFunc(tf));
	}

	//FLS_Address[51:40] = Z[11:0]
	for (int tf = (int)TesterFunc::t_z0; tf <= (int)TesterFunc::t_z11; tf++, bit++) {
		fconfig.address_order[bit] = (TesterFunc(tf));
	}

	//// FLS TEC Region Assignments
	//This config assumes that the TEC regions will be within the Y address space.
	//We are going to assume that there are 4 regions of 4KBytes each.
	//Typically, when dealing with non-power-of-2 sectors, the user would use
	//the TSFE as a "sector decode" and use these bits as the region bits.
	fconfig.TEC_region_control[0] = t_y12;
	fconfig.TEC_region_control[1] = t_y13;

	fconfig.filter_on = false;     //Turn on filters for array reads
	fconfig.TEC_threshold = 128;  //Only report fails on regions that have less than 128 fails

	fconfig.scan_immediate = true; //Default to immediate mode, pattern will change during reads

//	fconfig.flow_ctrl_max_components = 100000; //This should support 8 DUTs, 4 Regions, BOCx8 at full speed
	fconfig.flow_ctrl_max_components = 1000000; //This should support 8 DUTs, 4 Regions, BOCx8 at full speed
	fconfig.flow_ctrl_max_commands = (96 * 1024) - 1; //max is 96*1024 commands. This is the number of minor cycles that need to be available in the internal buffer.

	return;
}

//24 X bits and 14 Y
//page lsb's of X
void FlsConfig_x8::getConfig(fls_config_struct& fconfig) {
	get_flsConfig_defaults(fconfig);
	fconfig.datapins = pinlist::AllDataPins();

	//Save number of data pins
	DutPin* pin;
	for (num_datapins = 0; pin_info(fconfig.datapins, num_datapins, &pin); num_datapins++) {}
	num_datapins = ciel_bit(num_datapins);

	if (num_datapins == 64) num_datapins_mask = 0xffffffffffffffff;
	else num_datapins_mask = (1ULL << num_datapins) - 1ULL;

	return;
}
inline unsigned int FlsConfig_x8::numTecRegions() { return 4; }
inline DWORD FlsConfig_x8::flsaddr_to_X(const uint64_t flsaddr) { return DWORD((flsaddr & 0x000ffffff0000llu) >> 16); }
inline DWORD FlsConfig_x8::flsaddr_to_Y(const uint64_t flsaddr) { return DWORD((flsaddr & 0x000000000ffffllu) >> 0); }
inline DWORD FlsConfig_x8::flsaddr_to_Z(const uint64_t flsaddr) { return DWORD((flsaddr & 0xfff0000000000llu) >> 40); }
inline DWORD FlsConfig_x8::flsaddr_to_TecRegion(const uint64_t flsaddr) { return DWORD((flsaddr & 0x03000llu) >> 12); }
inline void FlsConfig_x8::flsdata_to_dut_pin(const DWORD databit, DWORD& flsdutnum, DWORD& pinnum) {
	pinnum = databit % num_datapins;
	flsdutnum = databit / num_datapins;
	return;
}
inline void FlsConfig_x8::flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_words)
{
	uint64_t word = flsword;
	for (DWORD i = 0; i < (64 / num_datapins); i++)
	{
		dut_words.at(i) = (word & num_datapins_mask);
		word >>= uint64_t(num_datapins);
	}
}

FlsConfig_x8 NandFlsConfig_x8;


//24 X bits and 14 Y
//includes dqs pins
void FlsConfig_x10::getConfig(fls_config_struct &fconfig) {
	get_flsConfig_defaults(fconfig);
	fconfig.datapins = pinlist::AllDataAndDataStrobePins();

	//Save number of data pins
	DutPin* pin;
	for (num_datapins = 0; pin_info(fconfig.datapins, num_datapins, &pin); num_datapins++) {}
	num_datapins_cielb = ciel_bit(num_datapins);

	if (num_datapins == 64) datapins_mask = 0xffffffffffffffff;
	else datapins_mask = (1ULL << num_datapins) - 1ULL;

	if (num_datapins_cielb == 64) datapins_cielb_mask = 0xffffffffffffffff;
	else datapins_cielb_mask = (1ULL << num_datapins_cielb) - 1ULL;


	return;
}

inline unsigned int FlsConfig_x10::numTecRegions() { return 4; }
inline DWORD FlsConfig_x10::flsaddr_to_X(const uint64_t flsaddr) { return DWORD((flsaddr & 0x000ffffff0000llu) >> 16); }
inline DWORD FlsConfig_x10::flsaddr_to_Y(const uint64_t flsaddr) { return DWORD((flsaddr & 0x000000000ffffllu) >> 0); }
inline DWORD FlsConfig_x10::flsaddr_to_Z(const uint64_t flsaddr) { return DWORD((flsaddr & 0xfff0000000000llu) >> 40); }
inline DWORD FlsConfig_x10::flsaddr_to_TecRegion(const uint64_t flsaddr) { return DWORD((flsaddr & 0x03000llu) >> 12); }

//Convert fail_fields_t.fails bit location to FLS DUT and Pin Number
inline void FlsConfig_x10::flsdata_to_dut_pin(const DWORD databit, DWORD& flsdutnum, DWORD& pinnum) {
	pinnum = databit % num_datapins_cielb;
	flsdutnum = databit / num_datapins_cielb;
	return;
}

//Segment fail_fields_t.fails into each DUT's fails (dut_fail_words)
inline void FlsConfig_x10::flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_fail_words)
{
	uint64_t word = flsword;
	for (DWORD i = 0; i < (64 / num_datapins_cielb); i++)
	{
		dut_fail_words.at(i) = (word & datapins_mask);
		word >>= uint64_t(num_datapins_cielb);
	}
}
FlsConfig_x10 NandFlsConfig_x10;

