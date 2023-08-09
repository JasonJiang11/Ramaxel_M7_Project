#include "PatternIncludes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

// Issue an ERASE BLOCK command for a given block and LUN on all planes.
// Erase Operation: ERASE BLOCK MULTI-PLANE

/////////////////////////////////////////////////////////////////////////////////////////////////////

// Arguments
// ------------
//
// BlockAddress          AUX2 contains the row address (ADDR3, ADDR4, ADDR5).
// NumPlanes             used to issue a command to each plane. Must already be loaded 
//                       in MAR counter defined by the C_NUM_PLANES_PER_LUN macro.
//
// Algorithm
// ------------
//
// -For each Plane:
//    1.  Issue the (MULTI-PLANE) ERASE BLOCK command (60h) command
//    2.  Issue ADDR3/ADDR4/ADDR5/ADDR6 address cycles (ADDR3 is ignored)  
//    3.   -Issue the D0h command
//         -Wait for tWB
//
// Issue a MULTI PLANE ERASE BLOCK command in SLC mode
// Erase Operation: ERASE BLOCK (60h-60-60-60-D0h)

/////////////////////////////////////////////////////////////////////////////////////////////////////

//Erase page in SLC mode
//AUX2 has initial x address for plane 0
//AUX5 has initial x address for plane 1
//AUX8 has initial x address for plane 2
//AUX11 has initial x address for plane 3

PATTERN( PAT__EraseOneBlock_MultiPlane )

% AssertCEn			: BUS_IDLE_WAIT
//if SLC mode, execute DA command, otherwise skip to label Send0x60_0
%					  BUS_IDLE_WAIT					JUMP_TO_IF_COUNT_ZERO(C_SLC_MODE, Send0x60_0)
// Enable SLC mode
% Send0xDA			: SEND_CMD( DA )                                       
% Wait_tSLC			: BUS_IDLE_WAIT					LOOP( C_tSLC, Wait_tSLC )

//plane 0
% Send0x60_0		: SEND_CMD( 60 )				XALU_AUX(AUX2, MAIN)
% ADDR3_0			: SEND_ROW1( MAIN )				
% ADDR4_0			: SEND_ROW2( MAIN )												
% ADDR5_0			: SEND_ROW3( MAIN )												

//plane1
% Send0x60_1		: SEND_CMD( 60 )				XALU_AUX(AUX5, MAIN)
% ADDR3_1			: SEND_ROW1( MAIN)				
% ADDR4_1			: SEND_ROW2( MAIN )													
% ADDR5_1			: SEND_ROW3( MAIN )		

//plane2
% Send0x60_2		: SEND_CMD( 60 )				XALU_AUX(AUX8, MAIN)
% ADDR3_2			: SEND_ROW1( MAIN)				
% ADDR4_2			: SEND_ROW2( MAIN )													
% ADDR5_2			: SEND_ROW3( MAIN )		

//plane3
% Send0x60_3		: SEND_CMD( 60 )				XALU_AUX(AUX11, MAIN)
% ADDR3_3			: SEND_ROW1( MAIN )				
% ADDR4_3			: SEND_ROW2( MAIN )													
% ADDR5_3			: SEND_ROW3( MAIN )		

% Send0xD0			: SEND_CMD( D0 )											
% Wait_tWB_D0		: BUS_IDLE_WAIT					LOOP( C_tWB, Wait_tWB_D0)						
% Wait_tBERS		: BUS_IDLE_WAIT					LOOP(C_tBERS, Wait_tBERS)

% End				: BUS_IDLE_WAIT					END_PATTERN

////////////////////////////////////////////////////////////////////////////////////////////////////////////






