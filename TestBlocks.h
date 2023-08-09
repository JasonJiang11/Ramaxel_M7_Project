#pragma once

extern std::vector<char> g_buffer1;

EXTERN_DWORD_VARIABLE(uv_page)
EXTERN_DWORD_VARIABLE(uv_bytes_in_page)

//user variables for single plane ERP patterns
EXTERN_DWORD_VARIABLE(uv_block)
EXTERN_DWORD_VARIABLE(uv_plane)
EXTERN_DWORD_VARIABLE(uv_lun)
EXTERN_DWORD_VARIABLE(uv_page_payload_byte)

//user variables for multi plane ERP patterns
EXTERN_DWORD_VARIABLE(uv_block_pln0)
EXTERN_DWORD_VARIABLE(uv_block_pln1)
EXTERN_DWORD_VARIABLE(uv_block_pln2)
EXTERN_DWORD_VARIABLE(uv_block_pln3)
EXTERN_DWORD_VARIABLE(uv_page_payload_byte_pln0)
EXTERN_DWORD_VARIABLE(uv_page_payload_byte_pln1)
EXTERN_DWORD_VARIABLE(uv_page_payload_byte_pln2)
EXTERN_DWORD_VARIABLE(uv_page_payload_byte_pln3)

//get Read Parameter data from binary file and store in static global param_data
void read_param_data_from_file();

//load jamram with random data from file
//can only load up to max size of jamram which is 1024
void load_jam_ram(std::vector<char> buf, enPinModesMag7 mode);

//single plane prog/read apg init
//this function loads the aux registers for use with the single plane ERP patterns
//AUX1 register Y_PLANE_0 has initial y address for plane 0
//AUX2 register X_PLANE_0 has initial x address for plane 0
//AUX3 register DATA_PLANE_0 has byte data to read from page in plane 0
void sp_prog_read_apg_init(DWORD page, DWORD plane, DWORD block, DWORD lun, DWORD byte_data, bool slc_mode = true, enPinModesMag7 mode = enPinModesMag7::t_PM_BOCx8);



