#pragma once
#include <tester.h>
#include "PinLists.h"
#include <array>

class FlsConfig
{
public:
	virtual void getConfig(fls_config_struct &fls_config) {
		return;
	}
	virtual unsigned int numTecRegions() = 0;
	virtual DWORD flsaddr_to_X(const uint64_t flsaddr) = 0;
	virtual DWORD flsaddr_to_Y(const uint64_t flsaddr) = 0;
	virtual DWORD flsaddr_to_Z(const uint64_t flsaddr) = 0;
	virtual DWORD flsaddr_to_TecRegion(const uint64_t flsaddr) = 0;
	virtual void flsdata_to_dut_pin(const DWORD databit, DWORD& dut, DWORD& pinnum) = 0;
	virtual void flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_words) = 0;
	//More to come
};

inline unsigned long ciel_bit(unsigned long in);

//24 X bits and 14 Y
//page lsb's of X
class FlsConfig_x8 : public FlsConfig {
public:
	DWORD num_datapins;
	uint64_t num_datapins_mask;
	void getConfig(fls_config_struct &fls_config);

	inline unsigned int numTecRegions();
	inline DWORD flsaddr_to_X(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Y(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Z(const uint64_t flsaddr);
	inline DWORD flsaddr_to_TecRegion(const uint64_t flsaddr);
	inline void flsdata_to_dut_pin(const DWORD databit, DWORD& flsdutnum, DWORD& pinnum);
	inline void flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_words);
};
extern FlsConfig_x8 NandFlsConfig_x8;

class FlsConfig_x10 : public FlsConfig {
public:
	DWORD num_datapins;
	DWORD num_datapins_cielb;
	uint64_t datapins_mask;
	uint64_t datapins_cielb_mask;
	void getConfig(fls_config_struct &fls_config);

	inline unsigned int numTecRegions();
	inline DWORD flsaddr_to_X(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Y(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Z(const uint64_t flsaddr);
	inline DWORD flsaddr_to_TecRegion(const uint64_t flsaddr);
	inline void flsdata_to_dut_pin(const DWORD databit, DWORD& flsdutnum, DWORD& pinnum);
	inline void flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_words);
};
extern FlsConfig_x10 NandFlsConfig_x10;

class FlsConfig_2x10 : public FlsConfig {
public:
	DWORD num_datapins;
	DWORD num_datapins_cielb;
	uint64_t datapins_mask;
	uint64_t datapins_cielb_mask;
	void getConfig(fls_config_struct& fls_config);

	inline unsigned int numTecRegions();
	inline DWORD flsaddr_to_X(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Y(const uint64_t flsaddr);
	inline DWORD flsaddr_to_Z(const uint64_t flsaddr);
	inline DWORD flsaddr_to_TecRegion(const uint64_t flsaddr);
	inline void flsdata_to_dut_pin(const DWORD databit, DWORD& flsdutnum, DWORD& pinnum);
	inline void flsfails_to_dut_words(const uint64_t flsword, std::array<uint64_t, 64> & dut_words);
};
extern FlsConfig_2x10 NandFlsConfig_2x10;