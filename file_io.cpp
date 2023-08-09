#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "TestBlocks.h"

//read binary data from file and store in buffer	
//function allocates space in buffer for binary data
void read_binary(std::string s_bin_file, std::vector<BYTE>& buffer)
{
	std::ifstream file(s_bin_file, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	output("reading binary file %s", s_bin_file.c_str());

	//allocate space in buffer
	buffer.resize(size);

	if (file.read((char *)buffer.data(), size))
	{
		output("successfully read %d bytes from file %s", size, s_bin_file.c_str());
	}
	else
	{
		fatal("failed to read file %s", s_bin_file.c_str());
	}
}

//read binaries needed by test program
//for example random data for page porgram/read
void read_program_binaries()
{
	read_param_data_from_file();
}