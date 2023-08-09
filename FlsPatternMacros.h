#ifndef _FLSPATTERNMACROS_H_
#define _FLSPATTERNMACROS_H_

#include "DefineCounters.h"
#include "PatternMacros.h"
#include "PatternResources.h"

//// FLS Pattern ID's
#define PAT_ReadStatus_ID							1
#define PAT_ReadStatusEnhanced_ID					2
#define PAT_ReadParameters_ID						3
#define PAT_ReadParameters_x4_ID					4
#define PAT_ReadId90hWithAddress20h_ID				5
#define PAT_ReadId90hWithAddress20h_dmain_x4_ID		6
#define PAT_ReadId90hWithAddress20h_dmain_x8_ID		7
#define PAT_GetFeatures_dmain_ID					8
#define PAT_GetFeatures_ID							9
#define PAT_WriteReadCache_x4_ID					10
#define PAT_WriteReadCache_x8_ID					11
#define PAT_FlagHandshake							12
#define PAT_ReadCache_RND_ID						13
#define PAT_WriteCache_RND_ID						14
#define PAT_ReadCache_RND_fails_ID					15
#define PAT_LoopReadCache_RND_fails_ID				16

// CID_USER1 Message Types
#define FLSF_PAT_START  0x10			//payload of Pat ID
#define FLSF_PAT_STOP   0x11			//payload of Pat ID
#define FLSF_TASK_START 0x12			//payload of Task ID
#define FLSF_TASK_STOP  0x13			//payload of Task ID
#define FLSF_PAT_TASK_START  0x14		//payload of Pat/Task ID (must be the same)
#define FLSF_PAT_TASK_STOP   0x15		//payload of Pat/Task ID (must be the same)
#define FLS_SET_REGREAD_TYPE 0x16				//payload of Register Read Types

// CID_USER2 / CID_USER3 Message Types
// CID_USER2 uses X as a TYPE and Y as a payload
// CID_USER3 uses Y as a TYPE and X as a payload
#define FLSU_COUNT_BANK_SET			0x10
#define FLSU_COUNT_SUBBANK_SET		0x11
#define FLSU_REG_BANK_SET		0x12    //register address from CID_FFAIL address
#define FLSU_REG_TYPE          0x13
#define FLSU_TIME_BANK_SET			0x14    
#define FLSU_TIME_SUBBANK_SET	    0x15
#define FLSU_TEC_TYPE				0x16
#define FLSU_CHARSTEP_ID1_SET		0x21
#define FLSU_CHARSTEP_ID2_SET		0x22
#define FLSU_CHARSTEP_ID3_SET		0x23


// CID_USER4 Message Types
#define FLSD_TIMESTAMP  0x16
#define FLSD_BPRESS_ID  0x19
#define FLSD_SLOT_ID         0x20
#define FLSD_OP_BPRESS_START 0x1
#define FLSD_OP_BPRESS_STOP  0x2

// CID_FLUSH Message Types
#define FLST_FLUSH_INIT_FLSMGR 0x1
#define FLST_FLUSH_STOP_FLSMGR 0x2

// FLSU_REG_TYPE Payloads
#define FLSP_REG_PARAM			0x1
#define FLSP_REG_SR				0x2
#define FLSP_REG_FEATURES	    0x3
#define FLSP_REG_READID         0x4

// FLSU_TEC_TYPE_SET
#define FLSP_TEC_COUNTS			0x1
#define FLSP_TEC_TIME			0x2

//Pattern Macros
#if 1

#define F1_FRAME( _MType, _Payload)							\n \
	xudataval1  _Payload										\n \
	yudataval1  _MType											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_USER1										\n

#define F1_FRAME_XMAIN2( _MType)								\n \
	yudataval1  _MType											\n \
	xalu1		oxmain2											\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_USER1										\n

#define F2_FRAME( _MType, _Payload)                           \n \
	xudataval2  _Payload										\n \
	yudataval2  _MType											\n \
	xalu2		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	adram2		atopo_bypass									\n \
	fls2		FLS_USER1										\n

#define F1_USER( _MType, _Payload)                            \n \
	xudataval1  _Payload										\n \
	yudataval1  _MType											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_USER2										\n

#define F2_USER( _MType, _Payload)                              \n \
	xudataval2  _Payload										\n \
	yudataval2  _MType											\n \
	xalu2		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	adram2		atopo_bypass									\n \
	fls2		FLS_USER2										\n

#define F1_USER_AUX( _MType, _AUX)                              \n \
	yudataval1  _MType											\n \
	xalu1		auxa, dxfield2, oxfield2						\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	auxreg1		_AUX, hold										\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_USER2										\n

#define F2_USER_AUX( _MType, _AUX)                              \n \
	yudataval2  _MType											\n \
	xalu2		auxa, dxfield2, oxfield2						\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	auxreg2		_AUX, hold										\n \
	adram2		atopo_bypass									\n \
	fls2		FLS_USER2										\n

//DBG types are never forwarded to the user's processing code. Debug only
#define F1_DBG( _MType, _Payload)                            \n \
	xudataval1  _Payload										\n \
	yudataval1  _MType											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	zalu1		ozfield2										\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_USER4										\n

#define F2_DBG( _MType, _Payload)                              \n \
	xudataval2  _Payload										\n \
	yudataval2  _MType											\n \
	xalu2		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	zalu2		ozfield2										\n \
	adram2		atopo_bypass									\n \
	fls2		FLS_USER4										\n

#define F1_FLS_SHUTDOWN											\n \
	xudataval1  0xCAFE											\n \
	yudataval1  0xBAD											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	adram1		atopo_bypass									\n \
	fls1		FLS_FLUSH										\n

#else

#define F1_FRAME( _MType, _Payload)							\n \
	xudataval1  _Payload										\n \
	yudataval1  _MType											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	fls1		FLS_USER1										\n

#define F2_FRAME( _MType, _Payload)                           \n \
	xudataval2  _Payload										\n \
	yudataval2  _MType											\n \
	xalu2		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	fls2		FLS_USER1										\n

#define F1_USER( _MType, _Payload)                            \n \
	xudataval1  _Payload										\n \
	yudataval1  _MType											\n \
	xalu1		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu1		yudata, yfield2, dyfield2, oyfield2				\n \
	fls1		FLS_USER2										\n

#define F2_USER( _MType, _Payload)                              \n \
	xudataval2  _Payload										\n \
	yudataval2  _MType											\n \
	xalu2		xudata, xfield2, dxfield2, oxfield2				\n \
	yalu2		yudata, yfield2, dyfield2, oyfield2				\n \
	fls2		FLS_USER2										\n

#endif

// FLS Capture Mode Macros

#define F1_NO  \n              fls1        FLS_NO_CAP           \n
#define F2_NO  \n              fls2        FLS_NO_CAP           \n
#define F_NO      F1_NO F2_NO

#define F1_CNT  \n             fls1        FLS_CAP_CNT_ONLY   \n
#define F2_CNT  \n             fls2        FLS_CAP_CNT_ONLY   \n
#define F_CNT      F1_CNT F2_CNT

#define F1_CAP_ARRAY   \n      fls1        FLS_CAP              \n
#define F2_CAP_ARRAY   \n      fls2        FLS_CAP              \n
#define F_CAP_ARRAY      F1_CAP_ARRAY F2_CAP_ARRAY

#define F1_CAP_REG     \n      fls1        FLS_CAP_FORCE        \n
#define F2_CAP_REG     \n      fls2        FLS_CAP_FORCE        \n
#define F_CAP_REG      F1_CAP_REG F2_CAP_REG

#define F1_FLUSH       \n      fls1        FLS_FLUSH            \n
#define F2_FLUSH       \n      fls2        FLS_FLUSH            \n

#define F1_CLRTEC     \n      fls1        FLS_CLR_ALLTEC     \n
#define F2_CLRTEC     \n      fls2        FLS_CLR_ALLTEC     \n

#define F1_SACTEC     \n      fls1        FLS_SAC_ALLTEC     \n
#define F2_SACTEC     \n      fls2        FLS_SAC_ALLTEC     \n

#define F_TStamp(_StampID) \n F1_DBG(FLSD_TIMESTAMP, _StampID )      \n    fls2 FLS_TIMER     \n
#endif