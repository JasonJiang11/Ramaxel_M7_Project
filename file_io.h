#pragma once

//read binary data from file and store in buffer	
//function allocates space in buffer for binary data
void read_binary(std::string s_bin_file, std::vector<BYTE>& buffer);

//read binaries needed by test program
//for example random data for page porgram/read
void read_program_binaries();