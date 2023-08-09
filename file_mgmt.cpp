#include "tester.h"
#include <iostream>
#include <fstream>
#include <iomanip>

//read binary data from file and store in buffer	
//function allocates space in buffer for binary data
void read_binary(std::string s_bin_file, std::vector<char> &buffer)
{	
	std::ifstream file(s_bin_file, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		output("reading binary file %s", s_bin_file.c_str());

		//allocate space in buffer
		buffer.resize(size);

		if (file.read(buffer.data(), size))
		{
			output("successfully read %d bytes from file %s", size, s_bin_file.c_str());
		}
		else
		{
			fatal("failed to read file %s", s_bin_file.c_str());
		}
	}
	else
	{
		warning("could not open binary file %s for random jamram data", s_bin_file.c_str());
		buffer.resize(0x4000);	//buffer with 0s for data

		//inject random data
		srand(static_cast<unsigned int>(time(NULL)));

		for ( auto idx = 0; idx < buffer.size(); ++idx)
		{
			buffer[idx] = rand() & 0xff;
		}
	}
}



