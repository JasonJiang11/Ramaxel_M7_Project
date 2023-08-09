#include "PatternIncludes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Programs one page on multiple planes, on given LUN in SLC mode

///////////////////////////////////////////////////////////////////////////////////////////////////

//Program page in SLC mode
//AUX1 has initial y address for plane 0
//AUX2 has initial x address for plane 0
//AUX3 has byte data to write to page plane 0
//AUX4 has initial y address for plane 1
//AUX5 has initial x address for plane 1
//AUX6 has byte data to write to page plane 1
//AUX7 has initial y address for plane 2
//AUX8 has initial x address for plane 2
//AUX9 has byte data to write to page plane 2
//AUX10 has initial y address for plane 3
//AUX11 has initial x address for plane 3
//AUX12 has byte data to write to page plane 3

PATTERN(PAT__ProgramOnePage_MultiPlane_SLC)

% AssertCEn			: BUS_IDLE_WAIT
// enable SLC mode
% Send0xDA			: SEND_CMD( DA )																		
% Wait_tSLC			: BUS_IDLE_WAIT														LOOP( C_tSLC, Wait_tSLC )	

//plane 0
% LoadData_0		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_0, MAIN)
% ProgPagePln_0		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10_0		: SEND_CMD(11)																				
% Wait_tWB_0		: WRITE_POSTAMBLE													LOOP(C_tWB, Wait_tWB_0)
% Wait_tDBSY_0		: WRITE_POSTAMBLE													LOOP(C_tDBSY, Wait_tDBSY_0)

//plane 1
% LoadData_1		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_1, MAIN)
% ProgPagePln_1		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10_1		: SEND_CMD(11)									
% Wait_tWB_1		: WRITE_POSTAMBLE													LOOP(C_tWB, Wait_tWB_1)
% Wait_tDBSY_1		: WRITE_POSTAMBLE													LOOP(C_tDBSY, Wait_tDBSY_1)

//plane 2
% LoadData_2		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_2, MAIN)
% ProgPagePln_2		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10_2		: SEND_CMD(11)
% Wait_tWB_2		: WRITE_POSTAMBLE													LOOP(C_tWB, Wait_tWB_2)
% Wait_tDBSY_2		: WRITE_POSTAMBLE													LOOP(C_tDBSY, Wait_tDBSY_2)

//plane 3
% LoadData_3		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_3, MAIN)
% ProgPagePln_3		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% Send0x10_3		: SEND_CMD(10)									
% Wait_tWB_3		: WRITE_POSTAMBLE													LOOP(C_tWB, Wait_tWB_3)
% Wait_tPROG_3		: WRITE_POSTAMBLE													LOOP(C_tPROG, Wait_tPROG_3)

% End				: WRITE_POSTAMBLE													END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Programs one page on multiple planes, on given LUN in TLC mode
///////////////////////////////////////////////////////////////////////////////////////////////////
PATTERN(PAT__ProgramOnePage_MultiPlane_TLC)

% AssertCEn1		: BUS_IDLE_WAIT																					

//lower
//plane 0
% LoadDataLP_0		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_0, MAIN)
% ProgLP_Pln_0		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% LP_Send0x10_0		: SEND_CMD( 11 )	
% LP_Wait_tWB_0		: WRITE_POSTAMBLE													LOOP( C_tWB, LP_Wait_tWB_0)
% LP_Wait_tDBSY_0	: WRITE_POSTAMBLE													LOOP( C_tDBSY, LP_Wait_tDBSY_0)
% LP_Next_page_0	: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

//plane 1
% LoadDataLP_1		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_1, MAIN)
% ProgLP_Pln_1		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% LP_Send0x10_1		: SEND_CMD( 11 )	
% LP_Wait_tWB_1		: WRITE_POSTAMBLE													LOOP( C_tWB, LP_Wait_tWB_1)
% LP_Wait_tDBSY_1	: WRITE_POSTAMBLE													LOOP(C_tDBSY, LP_Wait_tDBSY_1)
% LP_Next_page_1	: WRITE_POSTAMBLE	AUXINC(X_PLANE_1)

//plane 2
% LoadDataLP_2		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_2, MAIN)
% ProgLP_Pln_2		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% LP_Send0x10_2		: SEND_CMD( 11 )	
% LP_Wait_tWB_2		: WRITE_POSTAMBLE													LOOP( C_tWB, LP_Wait_tWB_2)
% LP_Wait_tDBSY_2	: WRITE_POSTAMBLE													LOOP(C_tDBSY, LP_Wait_tDBSY_2)
% LP_Next_page_2	: WRITE_POSTAMBLE	AUXINC(DATA_PLANE_2)

//plane 3
% LoadDataLP_3		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_3, MAIN)
% ProgLP_Pln_3		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% LP_Send0x10_3		: SEND_CMD( 1A )	
% LP_Wait_tWB_3		: WRITE_POSTAMBLE													LOOP( C_tWB, LP_Wait_tWB_3)
% LP_Wait_tPROG_3	: WRITE_POSTAMBLE													LOOP(C_tPROG, LP_Wait_tPROG_3)
% LP_Next_page_3	: WRITE_POSTAMBLE	AUXINC(X_PLANE_3)

// Mid_Page			

//plane 0
% LoadDataMP_0		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_0, MAIN)
% ProgMP_Pln_0		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% MP_Send0x10_0		: SEND_CMD( 11 )	
% MP_Wait_tWB_0		: WRITE_POSTAMBLE													LOOP( C_tWB, MP_Wait_tWB_0)
% MP_Wait_tDBSY_0	: WRITE_POSTAMBLE													LOOP( C_tDBSY, MP_Wait_tDBSY_0)
% MP_Next_page_0	: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

//plane 1
% LoadDataMP_1		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_1, MAIN)
% ProgMP_Pln_1		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% MP_Send0x10_1		: SEND_CMD( 11 )	
% MP_Wait_tWB_1		: WRITE_POSTAMBLE													LOOP( C_tWB, MP_Wait_tWB_1)
% MP_Wait_tDBSY_1	: WRITE_POSTAMBLE													LOOP(C_tDBSY, MP_Wait_tDBSY_1)
% MP_Next_page_1	: WRITE_POSTAMBLE	AUXINC(X_PLANE_1)

//plane 2
% LoadDataMP_2		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_2, MAIN)
% ProgMP_Pln_2		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% MP_Send0x10_2		: SEND_CMD( 11 )	
% MP_Wait_tWB_2		: WRITE_POSTAMBLE													LOOP( C_tWB, MP_Wait_tWB_2)
% MP_Wait_tDBSY_2	: WRITE_POSTAMBLE													LOOP(C_tDBSY, MP_Wait_tDBSY_2)
% MP_Next_page_2	: WRITE_POSTAMBLE	AUXINC(X_PLANE_2)

//plane 3
% LoadDataMP_3		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_3, MAIN)
% ProgMP_Pln_3		: BUS_IDLE_WAIT		XYALU_AUX(X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% MP_Send0x10_3		: SEND_CMD( 1A )	
% MP_Wait_tWB_3		: WRITE_POSTAMBLE													LOOP( C_tWB, MP_Wait_tWB_3)
% MP_Wait_tPROG_3	: WRITE_POSTAMBLE													LOOP(C_tPROG, MP_Wait_tPROG_3)
% MP_Next_page_3	: WRITE_POSTAMBLE	AUXINC(X_PLANE_3)

//upper page
//plane 0
% LoadDataUP_0		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_0, MAIN)
% ProgUP_Pln_0		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% UP_Send0x10_0		: SEND_CMD( 11 )	
% UP_Wait_tWB_0		: WRITE_POSTAMBLE													LOOP( C_tWB, UP_Wait_tWB_0)
% UP_Wait_tDBSY_0	: WRITE_POSTAMBLE													LOOP(C_tDBSY, UP_Wait_tDBSY_0)
% UP_Next_page_0	: WRITE_POSTAMBLE	AUXINC(X_PLANE_0)

//plane 1
% LoadDataUP_1		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_1, MAIN)
% ProgUP_Pln_1		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% UP_Send0x10_1		: SEND_CMD( 11 )	
% UP_Wait_tWB_1		: WRITE_POSTAMBLE													LOOP( C_tWB, UP_Wait_tWB_1)
% UP_Wait_tDBSY_1	: WRITE_POSTAMBLE													LOOP(C_tDBSY, UP_Wait_tDBSY_1)
% UP_Next_page_1	: WRITE_POSTAMBLE	AUXINC(X_PLANE_1)

//plane 2
% LoadDataUP_2		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_2, MAIN)
% ProgUP_Pln_2		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% UP_Send0x10_2		: SEND_CMD( 11 )	
% UP_Wait_tWB_2		: WRITE_POSTAMBLE													LOOP( C_tWB, UP_Wait_tWB_2)
% UP_Wait_tDBSY_2	: WRITE_POSTAMBLE													LOOP(C_tDBSY, UP_Wait_tDBSY_2)
% UP_Next_page_2	: WRITE_POSTAMBLE	AUXINC(X_PLANE_2)

//plane 3
% LoadDataUP_3		: BUS_IDLE_WAIT		DATGEN_AUX( DATA_PLANE_3, MAIN)
% ProgUP_Pln_3		: BUS_IDLE_WAIT		XYALU_AUX( X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN(SUB__PROG_PAGE)
% UP_Send0x10_3		: SEND_CMD( 10 )	
% UP_Wait_tWB_3		: WRITE_POSTAMBLE													LOOP( C_tWB, UP_Wait_tWB_3)
% UP_Wait_tPROG_3	: WRITE_POSTAMBLE													LOOP(C_tPROG, UP_Wait_tPROG_3)
% UP_Next_page_3	: WRITE_POSTAMBLE	AUXINC(X_PLANE_3)

% End				: WRITE_POSTAMBLE													END_PATTERN

///////////////////////////////////////////////////////////////////////////////////////////////////
