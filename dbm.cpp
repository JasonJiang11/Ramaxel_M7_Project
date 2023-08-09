#include "tester.h"

INT_VARIABLE(uv_dbm_segment, 4, " ") {}

//used to store random data which will be written to dbm
std::vector<DWORD> dbm_rnd_data;

//create block of ramdom data and write to dbm
void dbm_write_random()
{
	dbm_rnd_data.resize((ymax()+1) * 1024);	//buffer with 0s for data, 1024 pages of data
	output("dbm_rnd_data size 0x%x", dbm_rnd_data.size());

	//inject random data
	//srand(static_cast<unsigned int>(time(NULL)));
	srand(1);

	for (auto idx = 0; idx < dbm_rnd_data.size(); ++idx)
	{
		//dbm_rnd_data[idx] = rand() & 0xff;
		dbm_rnd_data[idx] = rand() & 0xff;

			//if ((idx >= 15)
			//	&& (dbm_rnd_data[idx] == 0x1b)
			//	&& (dbm_rnd_data[idx - 1] == 0x8f)
			//	&& (dbm_rnd_data[idx - 2] == 0xd6)
			//	&& (dbm_rnd_data[idx - 3] == 0x94)
			//	&& (dbm_rnd_data[idx - 4] == 0x9d)
			//	&& (dbm_rnd_data[idx - 5] == 0xfd)
			//	&& (dbm_rnd_data[idx - 6] == 0x93)
			//	&& (dbm_rnd_data[idx - 7] == 0x1f)
			//	&& (dbm_rnd_data[idx - 8] == 0xcf)
			//	&& (dbm_rnd_data[idx - 9] == 0x1a)
			//	&& (dbm_rnd_data[idx - 10] == 0xac)
			//	&& (dbm_rnd_data[idx - 11] == 0x3d)
			//	&& (dbm_rnd_data[idx - 12] == 0x23)
			//	&& (dbm_rnd_data[idx - 13] == 0x11)
			//	&& (dbm_rnd_data[idx - 14] == 0xf7)
			//	&& (dbm_rnd_data[idx - 15] == 0x21)
			//	)
			////if ((idx >= 7)
			////	&& (dbm_rnd_data[idx] == 0xcf)
			////	&& (dbm_rnd_data[idx - 1] == 0x1a)
			////	&& (dbm_rnd_data[idx - 2] == 0xac)
			////	&& (dbm_rnd_data[idx - 3] == 0x3d)
			////	&& (dbm_rnd_data[idx - 4] == 0x23)
			////	&& (dbm_rnd_data[idx - 5] == 0x11)
			////	&& (dbm_rnd_data[idx - 6] == 0xf7)
			////	&& (dbm_rnd_data[idx - 7] == 0x21)
			////	)
			////if ((idx >= 2)
			////	//&& (dbm_rnd_data[idx] == 0x11)
			////	&& (dbm_rnd_data[idx - 1] == 0xf7)
			////	&& (dbm_rnd_data[idx - 2] == 0x21)
			////	)
			//{
			//	output("found match idx = 0x%x x=0x%x, y=0x%x", idx, idx / (ymax() + 1), idx % (ymax() + 1));
			//}
		//
	}

	//for (auto idx = 0; idx <= ymax(); ++idx)
	//{
	//	dbm_rnd_data[idx] = rand() & 0xff;
	//	//dbm_rnd_data[idx] = idx+1;
	//}

	dbm_write(0, &(dbm_rnd_data[0]), static_cast<int>(dbm_rnd_data.size()));
	//dbm_write(0, &(dbm_rnd_data[0]), ymax()+1);

	//for (auto idx = 0; idx <= ymax(); ++idx)
	//{
	//	//dbm_rnd_data[idx] = rand() & 0xff;
	//	dbm_rnd_data[idx] = ymax() + 1 - idx;
	//}

	//dbm_write(ymax()+1, &(dbm_rnd_data[0]), ymax() + 1);

}

//
//loads dbm with multiple patterns
//loads:
//	segment 1 - preformat data	
//	segment 2 - firmware upload	
//	segment 3 - device parameter
//	
void dbm_load(enDbmUINumberMag7 ui_per_boc)
{
	x_fast_axis( FALSE );

	//shortest read interval is shortest tset cycle in dbm pattern (not shortest dbm cycle)
	double shortest_read_interval = get_IDLE_CYC();

	//_putenv("DBM_XPT_DEBUG=1");

	//configure dbm
	dbm_config_set( 10, numy(), 8, shortest_read_interval, 16, ui_per_boc);

	//
	dbm_segment_set(1);
	output("dbm_segment_set %d", dbm_segment_get());
	output("loading dbm with incrementing data pattern");

	//dump dbm data
	std::vector<DWORD> dataout_evn, dataout_odd;
	size_t num_cols = static_cast<size_t>(ymax() + 1);
	dataout_evn.resize(num_cols);
	dataout_odd.resize(num_cols);

	//write incrementing pattern starting at 1
	for (auto idx = 0; idx < dataout_evn.size(); ++idx)
	{
		dataout_evn[idx] = (idx + 1);
		dataout_odd[idx] = (ymax() + 1 - idx);
	}
	output("dbm_write to 8 rows");
	for (auto row = 0; row < 16; row += 2)
	{
		dbm_write(num_cols * row,		&dataout_evn[0], static_cast<int>(dataout_evn.size()));
		dbm_write(num_cols * (row +1),	&dataout_odd[0], static_cast<int>(dataout_odd.size()));
	}

	output("dbm_read");
	dbm_read(0, 0, 0, 15, &dataout_evn[0], 256); // (0,0) to (0,15)
	output("dumping dbm....");
	for (auto idx = 0; idx < 8; ++idx)
	{
		output("%d 0x%x", idx, dataout_evn[idx]);
	}

	//
	dbm_segment_set(2);
	for (auto idx = 0; idx < dataout_evn.size(); ++idx)
	{
		dataout_evn[idx] = idx % 2 ? 0x51 : 0xaa;
		dataout_odd[idx] = idx % 2 ? 0xaa : 0x57;
	}
	for (auto row = 0; row < 1024; row += 2)
	{
		dbm_write((ymax() + 1) * row,		&dataout_evn[0], static_cast<int>(dataout_evn.size()));
		dbm_write((ymax() + 1) * (row + 1), &dataout_odd[0], static_cast<int>(dataout_odd.size()));
	}

	dbm_segment_set(4);
	output("loading dbm with 1 block of random....");
	dbm_write_random();

	//reset default segment
	//dbm_segment_set(uv_dbm_segment);
	dbm_segment_set(uv_dbm_segment);

	output("dbm_segment_set %d", dbm_segment_get());
}