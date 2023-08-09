#include "stdafx.h" 
#include "Datalogs.h"
#include "Levels.h"
#include "PinLists.h"
#include "DutPins.h"
#include "Timings.h"
#include "DefineCounters.h"
#include "FlsTask.h"

// Type used to extract per-DUT from the ECR
using EcrDataType = std::map< DutNum, std::vector< BYTE >>;

// Helper for ecr_read()
static void _ecr_read(size_t numBytes, size_t rowStart, size_t colStart, EcrDataType& data)
{
	// TODO:
	// Read ECR, Store to data per DUT
}

static void ecr_read(size_t numBytes, size_t xAddr, size_t yAddr, EcrDataType& data)
{
	// Read data from the ECR
	_ecr_read(numBytes, xAddr, yAddr, data);
}

// Compares ECR data with specified expect-data
static void compare_ecr_data(const std::vector< unsigned __int64 >& expectData, size_t x, size_t y)
{
	// Extract data from the ECR
	EcrDataType ecrData;
	ecr_read(expectData.size(), x, y, ecrData);

	// Compare each byte of data on each DUT
	results_set(PASS);

	for (auto it : ecrData)
	{
		auto dut = it.first;
		auto& vec = it.second;
		bool isFail = false;

		for (auto jt = std::begin(vec); jt != std::end(vec); ++jt)
		{
			auto ix = jt - begin(vec);
			auto expected = expectData[ix];
			auto actual = expectData[ix] ^ *jt;
			auto str = vFormat("BYTE %04d -- Expect = 0x%02X, Actual = 0x%02X", ix, expected, actual);

			if (expected != actual)
			{
				isFail = true;
				str = "<COLOR=RED>" + str + "</COLOR>";
			}

			output(str);
		}
		output("");
		if (isFail) result_set(dut, FAIL);
	}
}

static void compare_ecr_data(const Int64Array& expectData, size_t x, size_t y)
{
	// Place data from CArray into an std::vector, then call other overload
	std::vector< unsigned __int64 > temp;
	temp.reserve(expectData.GetCount());
	for (auto i = 0; i < expectData.GetCount(); ++i)
		temp.push_back(expectData[i]);

	compare_ecr_data(temp, x, y);
}

// Issue a READ ID with Address 20h or 40h.
static void read_id_using_jam_ram(Pattern* pPattern, const Int64Array& valsJamRam, const Int64Array& valsEcr, bool compareDataOnFail)
{
	std::set< DutNum > failedDuts;

	// TODO: Disable even and enable odd DUTs
	//       Disable odd and enable even DUTs

	// Write the address byte and parameter bytes to JAM RAM
	apg_jam_mode_set(ApgJamMode::t_jam_mode_ram); // configure JAM as RAM (instead of reg)
	apg_jam_ram_set(0x00, const_cast<Int64Array&>(valsJamRam)); // Write data to JAM RAM
	apg_jam_ram_address_set(0x3FFF); // reset address pointer

	// Load the num bytes MAR counter
	count(C_DATA_OUT_NUM_BYTES, static_cast<int>(valsJamRam.GetCount() - 1));

	// Execute pattern with capturing to ECR
	funtest(pPattern, PatStopCond::fullec);

	// Get failed DUT results
	DutNumArray temp;
	results_get(&temp, FAIL);
	for (auto i = 0; i < temp.GetCount(); ++i)
		failedDuts.insert(temp[i]);

	// Compare expected vs actual data
	if (compareDataOnFail && any_results_match(FAIL))
	{
		compare_ecr_data(valsEcr, 0, 0);
	}

	// Set results
	results_set(PASS);
	for (auto it : failedDuts)
		result_set(it, FAIL);
}

// Generate JAM RAM and ECR expect data
template< typename T >
static void generate_jam_ram_and_ecr_expect_data(const T& expectData, Int64Array& valsJamRam, Int64Array& valsEcr)
{
	valsJamRam.RemoveAll();
	valsEcr.RemoveAll();

	for (auto& it : expectData)
	{
		valsJamRam.Add(it);
		valsEcr.Add(it);
	}
}

static void read_id_with_address_20h()
{
	// Define the address byte and parameter bytes for the JAM RAM
	std::array< unsigned long, 5 > payloadData = { 0x4f, 0x4e, 0x46, 0x49, 0x1 };

	Int64Array valsJamRam, valsEcr;
	generate_jam_ram_and_ecr_expect_data(payloadData, valsJamRam, valsEcr);
}

TEST_BLOCK(TB__FlsBasic)
{
	read_id_with_address_20h();

	return MULTI_DUT;
}