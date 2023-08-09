#include "PatternIncludes.h"

PATTERN( PAT__ReadDqTraining )
@{
@}
% AssertCEn  : BUS_IDLE_WAIT                                               
% Send0x62   : SEND_CMD( 62 )                                              
% LUN        : SEND_ADDR_JAMRAM                                            
% InverseData: SEND_ADDR_JAMRAM                                            
% FirstPat   : SEND_ADDR_JAMRAM                                            
% SecondPat  : SEND_ADDR_JAMRAM                                            
% Wait_tWHRT : BUS_IDLE_WAIT							LOOP( C_tWHRT, Wait_tWHRT )  
% Preamble1  : READ_PREAMBLE                                               
% Preamble2  : READ_PREAMBLE        
//first 16 bytes
% DataOut_1   : RE_TOGGLE_16_INC_JAMRAM															STROBE_EN   
//second 16 bytes
% DataOut_2  : RE_TOGGLE_16_INC_JAMRAM															STROBE_EN   
% End        : READ_POSTAMBLE                           END_PATTERN
  
PATTERN( PAT__WriteDqTrainingTxSide )
@{
@}
% AssertCEn  : BUS_IDLE_WAIT                                                           
% Send0x63   : SEND_CMD( 63 )                                                          
% LUN_Write  : SEND_ADDR_JAMRAM                                                        
% Wait_tADL  : BUS_IDLE_WAIT							LOOP( C_tADL, Wait_tADL )               
% PreambleW  : WRITE_PREAMBLE                                                               
% DataIn     : DQS_TOGGLE_INC_JAMRAM                                             
% Wait_tWPST : WRITE_POSTAMBLE							LOOP( C_tWPST, Wait_tWPST )                 
% Wait_tWPSTH: WRITE_POSTAMBLE							LOOP( C_tWPSTH, Wait_tWPSTH )               
% Send0x64   : SEND_CMD( 64 )                                                           
% LUN_Read   : SEND_ADDR_JAMRAM                                                         
% Wait_tWHRT : BUS_IDLE_WAIT							LOOP( C_tWHRT, Wait_tWHRT )             
% Preamble1  : READ_PREAMBLE							             
% DataOut    : RE_TOGGLE_16_INC_JAMRAM															STROBE_EN 
% End        : READ_POSTAMBLE                           END_PATTERN
