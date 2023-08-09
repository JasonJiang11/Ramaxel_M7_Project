#include "PatternIncludes.h"

//
// Subroutine to execute "Change Read Column Enhanced"
// Column and Rowm addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
//
PATTERN( SUB__ChangeReadColumnEnhanced )
@{
@}
% Send0x06  : SEND_CMD( 06 )																				F_NO
% ADDR1     : SEND_COL1( MAIN )																				F_NO
% ADDR2		: SEND_COL2( MAIN )																				F_NO
% ADDR3     : SEND_ROW1(MAIN)																				F_NO
% ADDR4		: SEND_ROW2(MAIN)																				F_NO
% ADDR5		: SEND_ROW3( MAIN )																				F_NO
% Send0xE0  : SEND_CMD( E0 )																				F_NO
% Wait_tCCS : BUS_IDLE_WAIT												LOOP( C_tCCS, Wait_tCCS )			F_NO
% SubDone	: BUS_IDLE_WAIT												RETURN_PATTERN						F_NO

//
// Subroutine to execute "Change Read Column"
// Column and Rowm addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
//

PATTERN( SUB__ChangeReadColumn )
@{
@}
% Send0x06			: SEND_CMD( 05 )															F_NO
% ADDR1				: SEND_COL1( MAIN )															F_NO
% ADDR2				: SEND_COL2( MAIN )															F_NO
% ADDR3				: SEND_ROW1(MAIN)                                           				F_NO
% ADDR4				: SEND_ROW2(MAIN)                                           				F_NO
% ADDR5				: SEND_ROW3( MAIN )                                         				F_NO
% Send0xE0			: SEND_CMD( E0 )															F_NO
% Wait_tCCS			: BUS_IDLE_WAIT							LOOP( C_tCCS, Wait_tCCS )			F_NO
% SubDone			: BUS_IDLE_WAIT							RETURN_PATTERN						F_NO

//
// Subroutine to execute read page multi-plane
// CMD 00, 5 address cycles, CMD 00, CMD 32
// Column and Row addresses are sourced from X/YMAIN which must be set appropriately from the calling pattern
//

PATTERN(SUB__ReadPageMP)
% Send0x00			: SEND_CMD(00)																F_NO
% ADDR1				: SEND_COL1(MAIN)															F_NO
% ADDR2				: SEND_COL2(MAIN)															F_NO
% ADDR3				: SEND_ROW1(MAIN)															F_NO
% ADDR4				: SEND_ROW2(MAIN)															F_NO
% ADDR5				: SEND_ROW3(MAIN)															F_NO
% Send0x30			: SEND_CMD( 32 )															F_NO	
% Wait_tWB			: BUS_IDLE_WAIT							LOOP( C_tWB, Wait_tWB)				F_NO
% Wait_tDBSY		: BUS_IDLE_WAIT							LOOP( C_tDBSY, Wait_tDBSY)			F_NO
% SubDone			: BUS_IDLE_WAIT							RETURN_PATTERN						F_NO

//Read page 
//AUX register Y_PLANE_0 has initial y address for plane 0
//AUX register X_PLANE_0 has initial x address for plane 0
//AUX register DATA_PLANE_0 has byte data to read from page in plane 0
//AUX register Y_PLANE_1 has initial y address for plane 1
//AUX register X_PLANE_1 has initial x address for plane 1
//AUX register DATA_PLANE_1 has byte data to read from page in plane 1
//AUX register Y_PLANE_2 has initial x address for plane 2
//AUX register X_PLANE_2 has initial y address for plane 2
//AUX register DATA_PLANE_2 has byte data to read from page in plane 2
//AUX register Y_PLANE_3 has initial x address for plane 3
//AUX register X_PLANE_3 has initial y address for plane 3
//AUX register DATA_PLANE_3 has byte data to read from page in plane 3

PATTERN( PAT__ReadOnePage_MultiPlane_SLC )
@{
@}

% AssertCEn			: BUS_IDLE_WAIT
//if SLC mode, execute DA command, otherwise skip to label Send0x60_0
% Check_SLC_Mode	: BUS_IDLE_WAIT															JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, ReadMP_0)
% Send0xDA			: SEND_CMD( DA )
% Wait_tSLC			: BUS_IDLE_WAIT															LOOP( C_tSLC, Wait_tSLC )

//plane 0
% ReadMP_0			: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN(SUB__ReadPageMP)
//plane 1
% ReadMP_1			: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN(SUB__ReadPageMP)
//plane 2
% ReadMP_2			: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN(SUB__ReadPageMP)
//plane 3
% Read_3			: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN(SUB__ReadPage)

//plane 0
% ChangeCol_0		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_0, Y_PLANE_0, MAIN)			GOSUB_PATTERN( SUB__ChangeReadColumnEnhanced )
% DataOut_0			: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_0, MAIN)					GOSUB_PATTERN(SUB__ReadPageDataOut)
% Next_Page_0		: BUS_IDLE_WAIT			AUXINC(X_PLANE_0)

//plane 1
% ChangeCol_1		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_1, Y_PLANE_1, MAIN)			GOSUB_PATTERN( SUB__ChangeReadColumnEnhanced )
% DataOut_1			: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_1, MAIN)					GOSUB_PATTERN(SUB__ReadPageDataOut)
% Next_Page_1		: BUS_IDLE_WAIT			AUXINC(X_PLANE_1)

//plane 2
% ChangeCol_2		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_2, Y_PLANE_2, MAIN)			GOSUB_PATTERN( SUB__ChangeReadColumnEnhanced )
% DataOut_2			: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_2, MAIN)					GOSUB_PATTERN(SUB__ReadPageDataOut)
% Next_Page_2		: BUS_IDLE_WAIT			AUXINC(X_PLANE_2)

//plane 3
% ChangeCol_3		: BUS_IDLE_WAIT			XYALU_AUX(X_PLANE_3, Y_PLANE_3, MAIN)			GOSUB_PATTERN( SUB__ChangeReadColumnEnhanced )
% DataOut_3			: BUS_IDLE_WAIT			DATGEN_AUX(DATA_PLANE_3, MAIN)					GOSUB_PATTERN(SUB__ReadPageDataOut)
% Next_Page_3		: BUS_IDLE_WAIT			AUXINC(X_PLANE_3)

% End				: BUS_IDLE_WAIT															END_PATTERN

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
