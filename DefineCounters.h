#ifndef _DEFINECOUNTERS_H_
#define _DEFINECOUNTERS_H_

#define C_NUM_PLANES_PER_LUN	1
#define C_tPOR					2
#define C_tWB					3
#define C_tWHR					4
#define C_tFEAT					5
#define C_tADL					6
#define C_tRR					7
#define C_tWPST					8
#define C_tWPSTH				9

// Number of pattern instructions required
// for data in program or read patterns
#define C_DATA					10

#define C_tPROG					11
#define C_tCCS					12
#define C_tWHRT					13
#define C_tWTRN					14
#define C_tR					15

// Number of pattern instructions required
// to read page data bytes
#define C_PAGE_READ				16

#define C_tBERS					17
#define C_tWC					18
#define C_tDBSY					19
#define C_tSLC					20
#define C_SLC_WL				21
#define C_TLC_WL				22
#define C_TLC_PG				23
#define C_PRBS					24		//PRBS enable
#define C_PIPE					25		//PRBS enable
#define C_POLL_EN				26		//enable poll with RB
#define C_2PLN					27		//enable for 2 plane duts
#define C_DQS					28		//enable for DQSH/L measurements
#define C_DQSH					29		//enable for DQSH measurements
#define C_FlagLookAhead			30		//for pattern flag lookahead
#define C_TRAINING				31		//for pattern training

#define C_DATA_OUT_NUM_BYTES    32

#define C_SLC_MODE				33		//set if pattern is SLC mode

#define C_PARAMETERS			34		//number of bytes in Parameter Pages BOCx8

#define C_VPULSE				35		//number of cycles to wait for vpulse

#define C_SLC_PAGES_PER_BLK		36		//number of slc pages per block

#define C_PIN_SWEEP				37		//number of sweep

#endif _DEFINECOUNTERS_H_
