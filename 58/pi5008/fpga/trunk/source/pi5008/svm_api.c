/**
 * \file
 *
 * \brief	SVM APIs
 *
 * \author 	yhcho
 *
 * \version 0.1
 *
 * Revision History
 *
 *
 * Copyright (c) 2016 Pixelplus Co.,Ltd. All rights reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "error.h"
#include "utils.h"
#include "system.h"
#include "intr.h"
#include "svm_register.h"
#include "svm_api.h"
#include "debug.h"

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/
#define SVM_BASE_MEMORY					(DRAM_MEM_BASE_ADDR + 0x0A000000U)		// TBD

#define SVM_MIN_WIDTH					(320)
#define SVM_MIN_HEIGHT					(240)
#define SVM_MAX_WIDTH					(1920)
#define SVM_MAX_HEIGHT					(1080)

#define SVM_MAX_VIEWMODE_COUNT			(10)	// TBD

#define SVM_UNUSE_VIEWMODE				(0xFF)
#define SVM_UNUSE_ADDRESS				(0xFFFFFFFF)

//#define SVM_VECTOR_MSG					// debugging message for HW vector

#ifdef SVM_VECTOR_MSG
#define _REG(x, y)						svm_setReg((uint32)&x, y)
#define _RET(x)							(x)
#else
#define _REG(x, y)						((*(vuint32*)(&x)) = y)
	#ifdef SVM_DEBUG
	#define _RET(x)						svm_retMsg(x, __FUNCTION__, __LINE__)
	#else
	#define _RET(x)						(x)
	#endif
#endif


/***************************************************************************************************************************************************************
 * Enumeration
***************************************************************************************************************************************************************/
enum tagSVM_REFERENCE_VIEWMODE_E
{
	eSVM_REFERENCE_VIEWMODE_FB_LUT = 0,
	eSVM_REFERENCE_VIEWMODE_LR_LUT,
	eSVM_REFERENCE_VIEWMODE_BC_LUT,
	eSVM_REFERENCE_VIEWMODE_CAR,
	eSVM_REFERENCE_VIEWMODE_MAX,
};


/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef struct tagSVM_SIZE_T
{
	int16 width;
	int16 height;
} SVM_SIZE_T, *pSVM_SIZE_T;

typedef struct tagSVM_MEMINFO_T
{
	uint32 size;
	uint32 addr;
} SVM_MEMINFO_T, *pSVM_MEMINFO_T;

typedef struct tagSVM_CAR_T
{
	uint32 addr;														// dram address of car image
	SVM_RECT_T rect;													// rectangle area for car output
	uint32 maskColor;													// maskcolor for car output
	bool isEnable;														// enable or disable for car output
} SVM_CAR_T, *pSVM_CAR_T;

typedef struct tagSVM_COMPRESSION_T
{
	bool isEnable;
	uint8 ratio;														// ref. enum SVM_COMPRESSION_RATIO_E
} SVM_COMPRESSION_T, *pSVM_COMPRESSION;

typedef struct tagSVM_ROI_T
{
	bool isEnable;
	SVM_ROI_AREA_T area;
} SVM_ROI_T, *pSVM_ROI_T;

typedef struct tagSVM_SECTION_T
{
	bool isEnable;														// enable or disable
	SVM_RECT_T rect;													// output area
} SVM_SECTION_T, *pSVM_SECTION_T;

typedef struct tagSVM_VIEWMODE_T
{
	bool isEnable;														// enable or disable of this view mode

	/**** input control ****/
	SVM_COMPRESSION_T compression;										// structure for compression. Compression is used to reduce MBW(Memory Band Width).
	SVM_ROI_T roi[eSVM_CHANNEL_MAX];									// structure for ROI. ROI is used to reduce MBW(Memory Band Width).
	SVM_BOUNDARY_T boundary[eSVM_CHANNEL_MAX];							// structure for boundary. Boundary is used to reduce MBW(Memory Band Width).
	bool vscale[eSVM_CHANNEL_MAX];										// vertical half scale. vscale is used to reduce MBW(Memory Band Width).

	/**** Memory ****/
	uint32 baseAddr;
	uint32 memSize;

	/**** svm control ****/
	uint8 refViewMode[eSVM_REFERENCE_VIEWMODE_MAX];						// The number of the view mode that refers to the value. (fb lut, lr lut, bc lut, car image)
	uint8 pageNum4LUT[eSVM_OUTPUT_PART_MAX];							// page flipping number of LUT (ref. enum SVM_PAGEFLIPPING_NUMBER_E)
	uint32 lutAddr[eSVM_OUTPUT_PART_MAX][eSVM_PAGEFLIPPING_NUMBER_MAX];	// lut address for fb or lr

	uint32 bclutAddr;													// lut address for brightness control

	SVM_CAR_T car;														// structure for car model

	/**** output control ****/
	SVM_SECTION_T section[eSVM_SECTION_NUMBER_MAX];						// structure for section. Section is used to reduce MBW(Memory Band Width).
	uint8 outputMode;													// tagSVM_OUTPUTMODE_E
} SVM_VIEWMODE_T, *pSVM_VIEWMODE_T;


/***************************************************************************************************************************************************************
 * Global Variable
***************************************************************************************************************************************************************/
static bool gSVM_isInit = FALSE;

static SVM_MEMINFO_T gSVM_channelAddr[eSVM_CHANNEL_MAX];

static SVM_SIZE_T gSVM_inputSize;
static SVM_SIZE_T gSVM_outputSize;
static uint8 gSVM_samplingCnt;

static uint32 gSVM_lutSize;

static bool gSVM_isMorphing[eSVM_OUTPUT_PART_MAX] = {FALSE, FALSE};

static uint8 gSVM_viewModeCount = 0;
static int8 gSVM_curViewMode = -1;
static SVM_VIEWMODE_T gSVM_viewMode[SVM_MAX_VIEWMODE_COUNT];

static SVM_CALLBACK gSVM_CBVsync = NULL;


/*======================================================================================================================
 = callbackl function
======================================================================================================================*/

void SVMAPI_load(void){}

void SVMAPI_vsync_isr(int num)
{
	uint32 msk = (1 << num);

	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	if ( gSVM_CBVsync ) gSVM_CBVsync();

	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}

/*======================================================================================================================
 = Local function
======================================================================================================================*/

#if !defined(SVM_VECTOR_MSG) && defined(SVM_DEBUG)
/*
 * function				:	svm_retMsg

 * param ret			:	error code
 * param line			:	code line number

 * return				:	error code

 * brief
	debuging message.
*/
static int32 svm_retMsg(int32 ret, const char* function, uint32 line)
{
	char error[32];

	if ( ret == eSUCCESS )
		return ret;

	printf("[SVM_API(%s_%d)]: ", function, line);

	switch (ret)
	{
	case eERROR_INVALID_ARGUMENT:
		strcpy(error, "eERROR_INVALID_ARGUMENT");
		break;
	case eERROR_SVM_NOT_INITIALIZE:
		strcpy(error, "eERROR_SVM_NOT_INITIALIZE");
		break;
	case eERROR_SVM_VIEWMODE_NOT_SELECTED:
		strcpy(error, "eERROR_SVM_VIEWMODE_NOT_SELECTED");
		break;
	case eERROR_SVM_TOO_MAANY_LIMIT_VIEWMODE:
		strcpy(error, "eERROR_SVM_TOO_MAANY_LIMIT_VIEWMODE");
		break;
	case eERROR_SVM_UNUSE_VIEWMODE:
		strcpy(error, "eERROR_SVM_UNUSE_VIEWMODE");
		break;
	case eERROR_SVM_UNUSE_LUT_VIEWMODE:
		strcpy(error, "eERROR_SVM_UNUSE_LUT_VIEWMODE");
		break;
	case eERROR_SVM_UNUSE_CAR_VIEWMODE:
		strcpy(error, "eERROR_SVM_UNUSE_CAR_VIEWMODE");
		break;
	case eERROR_SVM_RUNNING:
		strcpy(error, "eERROR_SVM_RUNNING");
		break;
	}
	printf("%s\n", error);

	return ret;
}
#endif

#ifdef SVM_VECTOR_MSG
/*
 * function				:	svm_setReg

 * param ret			:	register address
 * param line			:	register value

 * return				:

 * brief
	sets register and vector message.
*/
static void svm_setReg(uint32 addr, uint32 value)
{
	uint32 addrMsg = addr - SVM_BASE_ADDR + 0x3000;
	uint32 valueMsg = value;
	switch (addrMsg)
	{
	case 0x3030:
	case 0x3034:
	case 0x3038:
	case 0x303C:
	case 0x3040:
	case 0x3044:
	case 0x3048:
	case 0x304C:
		break;
	default:
		if ( addrMsg == 0x3000 )
			valueMsg = utilClearBit(valueMsg, 2);
		printf("i2c_mst_byte_wr(7'h1e,32'h%x, 32'h%08x);\n", addrMsg, valueMsg);
		break;
	}

	(*((vuint32 *)(addr))) = value;
}
#endif

/*
 * function				:	SVM_setResolution

 * param inputWidth		:	input width
 * param inputHeight	:	input height
 * param outputWidth	:	output width
 * param outputHeight	:	output height
 * param distance		:	distance for down-sampling

 * return				:	error code

 * brief
	Sets the size information of svm.
*/
static int8 SVM_setResolution(uint16 inputWidth, uint16 inputHeight, uint16 outputWidth, uint16 outputHeight, uint8 distance)
{
	uint8 sOffset = 0x0, tOffset = 0x0;
	uint8 tXCnt = 0x0, tYCnt = 0x0;
	uint8 dstw = 0x0;
	uint16 incr = 0x0;
	uint16 hscl = 0x0, vscl = 0x0;

	if ( inputWidth < SVM_MIN_WIDTH || inputHeight < SVM_MIN_HEIGHT || inputWidth > SVM_MAX_WIDTH || inputHeight > SVM_MAX_HEIGHT ||
		  outputWidth < SVM_MIN_WIDTH || outputHeight < SVM_MIN_HEIGHT || outputWidth > SVM_MAX_WIDTH || outputHeight > SVM_MAX_HEIGHT )
	{
		return eERROR_INVALID_ARGUMENT;
	}

	sOffset = inputWidth % distance;
	tOffset = inputHeight % distance;

	tXCnt = ((inputWidth + sOffset) / 2 / distance * 2) + 1;
	tYCnt = ((inputHeight + tOffset) / 2 / distance * 2) + 1 - 1;

	incr = 32768 / distance;
	dstw = distance - 1;

	hscl = (uint16)((((double)inputWidth / (double)outputWidth) * 256.) + 0.5);
	vscl = (uint16)((((double)inputHeight / (double)outputHeight) * 256.) + 0.5);

	_REG(SVM_CTRL_REG->inputSize, (inputWidth & utilBitMask(11)) | ((inputHeight & utilBitMask(11)) << 16));
	_REG(SVM_CTRL_REG->outputSize, (outputWidth & utilBitMask(11)) | ((outputHeight & utilBitMask(11)) << 16));
	_REG(SVM_CTRL_REG->samplingCnt, (tXCnt & utilBitMask(8)) | ((tYCnt & utilBitMask(8)) << 8) | ((sOffset & utilBitMask(8)) << 16) | ((tOffset & utilBitMask(8)) << 24));
	_REG(SVM_CTRL_REG->samplingDistance, (dstw & utilBitMask(8)) | ((incr & utilBitMask(16)) << 16));
	_REG(SVM_CTRL_REG->sizeRatio, (hscl & utilBitMask(10)) | ((vscl & utilBitMask(10)) << 16));

	return eSUCCESS;
}

/*
 * function				:	SVM_initMemory

 * param inputWidth		:	input width
 * param inputHeight	:	input height
 * param frameCnt		:	frame count

 * return				:	error code

 * brief
	Allocates a block of memory.
	Setup the memory map of the svm.
	Memory size of each channel = inputWidth * inputHeight * pixel size of yuv422 format * frameCnt
*/
static int8 SVM_initMemory(uint16 inputWidth, uint16 inputHeight, float frameCnt)
{
	int i = 0;
	uint32 channelMemSize;
	uint32 memPos = SVM_BASE_MEMORY;

	channelMemSize = (inputWidth * inputHeight * 2 * frameCnt);
	channelMemSize = ((channelMemSize + 7) >> 3) << 3;	// 8byte align

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		gSVM_channelAddr[i].addr = memPos;
		gSVM_channelAddr[i].size = channelMemSize;

		_REG(SVM_CTRL_REG->camWmainAddr[i], gSVM_channelAddr[i].addr);

		memPos += channelMemSize;
	}

	return eSUCCESS;
}

/*
 * function				:	SVM_initViewMode

 * param void			:

 * return				:	error code

 * brief
	Initialize view modes. (ref. structure SVM_VIEWMODE_T)
*/
static int8 SVM_initViewMode(void)
{
	int i = 0, j = 0, viewIndex = 0, camIndex = 0;

	gSVM_isMorphing[eSVM_OUTPUT_PART_FB] = FALSE;
	gSVM_isMorphing[eSVM_OUTPUT_PART_LR] = FALSE;

	for ( viewIndex = 0; viewIndex < SVM_MAX_VIEWMODE_COUNT; viewIndex++ )
	{
		gSVM_viewMode[viewIndex].isEnable = FALSE;

		/**** input control ****/
		gSVM_viewMode[viewIndex].compression.isEnable = FALSE;
		gSVM_viewMode[viewIndex].compression.ratio = eSVM_COMPRESSION_RATIO_HALF;

		for ( camIndex = 0; camIndex < eSVM_CHANNEL_MAX; camIndex++ )
		{
			gSVM_viewMode[viewIndex].roi[camIndex].area.startYLine = 0;
			gSVM_viewMode[viewIndex].roi[camIndex].area.endYLine = 0;
			gSVM_viewMode[viewIndex].roi[camIndex].isEnable = FALSE;
		}

		for ( camIndex = 0; camIndex < eSVM_CHANNEL_MAX; camIndex++ )
		{
			for ( i = 0; i < eSVM_BOUNDARYAEAR_MAX; i++ )
			{
				gSVM_viewMode[viewIndex].boundary[camIndex].area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
				gSVM_viewMode[viewIndex].boundary[camIndex].area[i].scale = eSVM_BOUNDARYSCALE_ONE;
			}
		}

		for ( camIndex = 0; camIndex < eSVM_CHANNEL_MAX; camIndex++ )
		{
			gSVM_viewMode[viewIndex].vscale[camIndex] = FALSE;
		}

		/**** Memory ****/
		gSVM_viewMode[viewIndex].baseAddr = SVM_UNUSE_ADDRESS;
		gSVM_viewMode[viewIndex].memSize = 0;

		/**** svm control ****/
		for ( i = 0; i < eSVM_REFERENCE_VIEWMODE_MAX; i++ )
		{
			gSVM_viewMode[viewIndex].refViewMode[i] = SVM_UNUSE_LUT;
		}

		gSVM_viewMode[viewIndex].pageNum4LUT[eSVM_OUTPUT_PART_FB] = eSVM_PAGEFLIPPING_NUMBER_0;
		gSVM_viewMode[viewIndex].pageNum4LUT[eSVM_OUTPUT_PART_LR] = eSVM_PAGEFLIPPING_NUMBER_0;

		for ( i = 0; i < eSVM_OUTPUT_PART_MAX; i++ )
		{
			for ( j = 0; j < eSVM_PAGEFLIPPING_NUMBER_MAX; j++ )
			{
				gSVM_viewMode[viewIndex].lutAddr[i][j] = SVM_UNUSE_ADDRESS;
			}
		}

		gSVM_viewMode[viewIndex].bclutAddr = SVM_UNUSE_ADDRESS;

		gSVM_viewMode[viewIndex].car.isEnable = FALSE;
		gSVM_viewMode[viewIndex].car.addr = SVM_UNUSE_ADDRESS;
		gSVM_viewMode[viewIndex].car.rect.x = 0;
		gSVM_viewMode[viewIndex].car.rect.y = 0;
		gSVM_viewMode[viewIndex].car.rect.width = 0;
		gSVM_viewMode[viewIndex].car.rect.height = 0;
		gSVM_viewMode[viewIndex].car.maskColor = 0;

		/**** output control ****/
		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			gSVM_viewMode[viewIndex].section[i].rect.x = 0;
			gSVM_viewMode[viewIndex].section[i].rect.y = 0;
			gSVM_viewMode[viewIndex].section[i].rect.width = 0;
			gSVM_viewMode[viewIndex].section[i].rect.height = 0;
			gSVM_viewMode[viewIndex].section[i].isEnable = FALSE;
		}

		gSVM_viewMode[viewIndex].outputMode = eSVM_OUTPUTMODE_BYPASS_FRONT;
	}

	return eSUCCESS;
}

/*
 * function					:	SVMAPI_setOutputMode

 * param mode				:	output mode (ref. enum SVM_OUTPUTMODE_E)

 * return					:	error code

 * brief
	Sets the outputmode of the SVM.
*/
static int8 SVMAPI_setOutputMode(uint8 viewMode, uint8 outputMode)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || outputMode >= eSVM_OUTPUTMODE_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].outputMode = outputMode;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].outputMode = outputMode;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		switch (outputMode)
		{
		case eSVM_OUTPUTMODE_LUT_HALF_BLEND:
		case eSVM_OUTPUTMODE_LUT_BCLUT_ONLY_ALPHA:
		case eSVM_OUTPUTMODE_LUT_BCLUT:
			SVMAPI_setInputEnable(TRUE, TRUE, TRUE, TRUE);
			SVMAPI_setOutputEnable(TRUE, TRUE);
			break;
		}

		switch (outputMode)
		{
		case eSVM_OUTPUTMODE_BYPASS_FRONT:
			SVMAPI_setInputEnable(TRUE, FALSE, FALSE, FALSE);
			SVMAPI_setOutputEnable(TRUE, FALSE);
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x1));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x0));
			break;
		case eSVM_OUTPUTMODE_BYPASS_LEFT:
			SVMAPI_setInputEnable(FALSE, TRUE, FALSE, FALSE);
			SVMAPI_setOutputEnable(FALSE, TRUE);
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x4));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x1));
			break;
		case eSVM_OUTPUTMODE_BYPASS_RIGHT:
			SVMAPI_setInputEnable(FALSE, FALSE, TRUE, FALSE);
			SVMAPI_setOutputEnable(FALSE, TRUE);
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0xC));
			_REG(SVM_CTRL_REG->outMode,  utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x1));
			break;
		case eSVM_OUTPUTMODE_BYPASS_BACK:
			SVMAPI_setInputEnable(FALSE, FALSE, FALSE, TRUE);
			SVMAPI_setOutputEnable(TRUE, FALSE);
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x3));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x0));
			break;
		case eSVM_OUTPUTMODE_LUT_HALF_BLEND:
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x3));
			break;
		case eSVM_OUTPUTMODE_LUT_BCLUT_ONLY_ALPHA:
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0xF));
			break;
		case eSVM_OUTPUTMODE_LUT_BCLUT:
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0));
			_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 0, 6, 0x3F));
			break;
		}
	}

	return eSUCCESS;
}

/*======================================================================================================================
 = Export function
======================================================================================================================*/

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setChannelInputAddr

 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)

 * return					:	error code

 * brief
	Sets the memory address of input channel data.
	The memory address of each channel is determined by SVMAPI_initialize().
	This function can be used when it is necessary to change the address value of the channel for additional function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setChannelInputAddr(uint8 IN channel, uint32 IN addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( channel >= eSVM_CHANNEL_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->camWmainAddr[channel], addr);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getChannelInputAddr

 * param channel0Addr		:	address pointer of channel 0
 * param channel1Addr		:	address pointer of channel 1
 * param channel2Addr		:	address pointer of channel 2
 * param channel3Addr		:	address pointer of channel 3

 * return					:	error code

 * brief
	Gets the memory address of input channel data.
	The memory address of each channel is determined by SVMAPI_initialize().
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_getChannelInputAddr(uint32 OUT *channel0Addr, uint32 OUT *channel1Addr, uint32 OUT *channel2Addr, uint32 OUT *channel3Addr)
{
	if ( gSVM_isInit == FALSE )
	{
		return _RET(eERROR_SVM_NOT_INITIALIZE);
	}

	*channel0Addr = SVM_CTRL_REG->camWmainAddr[eSVM_CHANNEL_FRONT];
	*channel1Addr = SVM_CTRL_REG->camWmainAddr[eSVM_CHANNEL_LEFT];
	*channel2Addr = SVM_CTRL_REG->camWmainAddr[eSVM_CHANNEL_RIGHT];
	*channel3Addr = SVM_CTRL_REG->camWmainAddr[eSVM_CHANNEL_BACK];

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setChannelFilpping

 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param isHorizontal		:	horizontal flipping
 * param isVertical			:	vertical flipping

 * return					:	error code

 * brief
	Sets flipping of the camera.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setChannelFilpping(uint8 IN channel, bool IN isHorizontal, bool IN isVertical)
{
	uint8 pos = 0x0;
	uint8 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( channel >= eSVM_CHANNEL_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	pos = 0x8 + (channel * 0x2);
	if ( isHorizontal && isVertical )
	{
		value = 0x3;
	}
	else if ( isHorizontal && !isVertical )
	{
		value = 0x1;
	}
	else if ( !isHorizontal && isVertical )
	{
		value = 0x2;
	}
	else
	{
		value = 0x0;
	}

	_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, pos, 2, value));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCompression

 * param viewMode			:	number of viewmode
 * param ratio				:	ratio for compression

 * return					:	error code

 * brief
	Sets the compression of the SVM.
	Compression is used to reduce MBW(Memory Band Width).
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCompression(uint8 IN viewMode, uint8 IN ratio)
{
	int i = 0;
	uint32 pos = 0x0;
	uint32 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || ratio > eSVM_COMPRESSION_RATIO_QUARTER )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].compression.ratio = ratio;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].compression.ratio = ratio;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		value = SVM_CTRL_REG->compression;

		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			pos = (i * 0x2) + 0x1;
			value = utilPutBit(value, pos, ratio);
		}

		_REG(SVM_CTRL_REG->compression, value);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCompressionEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the compression.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCompressionEnable(uint8 IN viewMode, bool IN isEnable)
{
	int i = 0;
	uint32 pos = 0x0;
	uint32 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].compression.isEnable = isEnable;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].compression.isEnable = isEnable;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		value = SVM_CTRL_REG->compression;

		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			pos = i * 0x2;
			value = utilPutBit(value, pos, isEnable);
		}

		_REG(SVM_CTRL_REG->compression, value);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setROI

 * param viewMode			:	number of viewmode
 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param roi				:	information of roi

 * return					:	error code

 * brief
	Sets the ROI of the SVM.
	ROI is used to reduce MBW(Memory Band Width).
	Use only specific area.

	------------------------------
	|        not used area       |
	------------------------------	startYLine
	|                            |
	|          ROI area          |
	|                            |
	------------------------------	endYLine
	|        not used area       |
	------------------------------
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setROI(uint8 IN viewMode, uint8 IN channel, SVM_ROI_AREA_T IN roi)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || channel >= eSVM_CHANNEL_MAX ||
		 roi.startYLine >= gSVM_inputSize.height || roi.endYLine >= gSVM_inputSize.height || roi.startYLine > roi.endYLine )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].roi[channel].area.startYLine = roi.startYLine;
			gSVM_viewMode[i].roi[channel].area.endYLine = roi.endYLine;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].roi[channel].area.startYLine = roi.startYLine;
		gSVM_viewMode[viewMode].roi[channel].area.endYLine = roi.endYLine;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
		_REG(SVM_CTRL_REG->roi[channel], (roi.startYLine & utilBitMask(11)) | ((roi.endYLine & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->roi[channel], 31) << 31));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setROIEnable

 * param viewMode			:	number of viewmode
 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the ROI.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setROIEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || channel >= eSVM_CHANNEL_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].roi[channel].isEnable = isEnable;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].roi[channel].isEnable = isEnable;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
		_REG(SVM_CTRL_REG->roi[channel], utilPutBit(SVM_CTRL_REG->roi[channel], 31, isEnable));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setHorizontalScale

 * param viewMode			:	number of viewmode
 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param chBoundary			:	information of boundary

 * return					:	error code

 * brief
	Sets the boundary of the SVM.
	Boundary is used to reduce MBW(Memory Band Width).
	Divide the input image into 4 parts. and scale-down each area.

	ex)
	------------------------------										------------------------------
	|           area 0           |					not scale -->		|                            |
	------------------------------	startYLine 1       					------------------------------
	|           area 1           |					1/2 scale -->		|              |
	------------------------------	startYLine 2						----------------
	|           area 2           |					1/4 scale -->		|		|
	------------------------------	startYLine 3						---------
	|           area 3           |					1/8 scale -->		|	|
	------------------------------										-----
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setHorizontalScale(uint8 IN viewMode, uint8 IN channel, SVM_BOUNDARY_T IN chBoundary)
{
	int i = 0;
	uint8 pos = 0x0;
	uint16 startY[4];

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

 	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || channel >= eSVM_CHANNEL_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		if ( chBoundary.area[i].scale >= eSVM_BOUNDARYSCALE_MAX )
			return _RET(eERROR_INVALID_ARGUMENT);
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].boundary[channel] = chBoundary;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].boundary[channel] = chBoundary;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		/* The register set value must be a multiple of 4.*/
		for ( i = 1; i < 4; i++ )
			startY[i] = chBoundary.area[i].startYLine / 4;

		_REG(SVM_CTRL_REG->boundary[channel],	((startY[1] & utilBitMask(9)) << 0) |
												((startY[2] & utilBitMask(9)) << 9) |
												((startY[3] & utilBitMask(9)) << 18) |
												(utilGetBit(SVM_CTRL_REG->boundary[channel], 31) << 31));

		pos = channel * 0x8;
		_REG(SVM_CTRL_REG->boundaryScale, utilPutBits(SVM_CTRL_REG->boundaryScale, pos, 8,
												(chBoundary.area[0].scale) | (chBoundary.area[1].scale << 2) | (chBoundary.area[2].scale << 4) | (chBoundary.area[3].scale << 6)));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setHoldTime

 * param noScale			:	hold time of no scale
 * param oneOverTwoScale	:	hold time of 1/2 scale
 * param oneOverFourScale	:	hold time of 1/4 scale
 * param oneOverEightScale	:	hold time of 1/8 scale

 * return void				:

 * brief
 	Sets the hold time of the boundary(horizontal scale).
	It guarantees priority of read operation by delaying write.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setHoldTime(uint16 IN noScale, uint16 IN oneOverTwoScale, uint16 IN oneOverFourScale, uint16 IN oneOverEightScale)
{
	_REG(SVM_CTRL_REG->sclDownHoldCnt[0], (noScale & utilBitMask(11)) | ((oneOverTwoScale & utilBitMask(11)) << 16));
	_REG(SVM_CTRL_REG->sclDownHoldCnt[1], (oneOverFourScale & utilBitMask(11)) | ((oneOverEightScale & utilBitMask(11)) << 16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setVerticalHalfScale

 * param viewMode			:	number of viewmode
 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Sets the vertical half scale of the SVM.
	vscale is used to reduce MBW(Memory Band Width).

	ex)
	------------------------------
	|                            |    half scale  ------------------------------
	|                            |    ------->    |                            |
	|                            |                |                            |
	|                            |                ------------------------------
	------------------------------
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setVerticalHalfScale(uint8 IN viewMode, uint8 IN channel, bool IN isEnable)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

 	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || channel >= eSVM_CHANNEL_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].vscale[channel] = isEnable;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].vscale[channel] = isEnable;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		_REG(SVM_CTRL_REG->boundary[channel], utilPutBit(SVM_CTRL_REG->boundary[channel], 31, isEnable));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInputBurstLength

 * param burstLength		:	burst length (ref. tagSVM_INPUT_BURST_LENGTH [16 ~ 128])

 * return					:	error code

 * brief
 	Sets the burst size of the write main.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setInputBurstLength(uint8 IN burstLength)
{
	if ( burstLength < eSVM_BURST_LENGTH_16 || burstLength > eSVM_BURST_LENGTH_128 )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->wmain, utilPutBits(SVM_CTRL_REG->wmain, 4, 4, burstLength));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInputTestModeEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the input test mode.
	The test mode uses the input data of the DRAM.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setInputTestModeEnable(bool IN isEnable)
{
	_REG(SVM_CTRL_REG->enable, utilPutBit(SVM_CTRL_REG->enable, 3, isEnable));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInputWrappingInitEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Initialize wrapping address to start address at frame start.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setInputWrappingInitEnable(bool IN isEnable)
{
	_REG(SVM_CTRL_REG->wmain, utilPutBit(SVM_CTRL_REG->wmain, 2, isEnable));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInputEnable

 * param isFrontEnable		:	enable or disable of front input channel
 * param isLeftEnable		:	enable or disable of left input channel
 * param isRIghtEnable		:	enable or disable of right input channel
 * param isBackEnable		:	enable or disable of back input channel

 * return					:	error code

 * brief
	Enable or disable of the input data.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setInputEnable(bool IN isFrontEnable, bool IN isLeftEnable, bool IN isRightEnable, bool IN isBackEnable)
{
	uint8 burst = 0x0;
	uint8 initEn = 0x0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	burst = utilGetBits(SVM_CTRL_REG->wmain, 4, 4);
	initEn = utilGetBit(SVM_CTRL_REG->wmain, 2);

	_REG(SVM_CTRL_REG->wmain, (isFrontEnable << 0) | (0x1 << 1) | (initEn << 2 ) | ((burst & utilBitMask(4)) << 4) | (isLeftEnable << 8) | (isRightEnable << 9) | (isBackEnable << 10));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setSync

 * param pipLineDelay		:	delay for internal vsync generation (pre_vsync)
 * param outLineDaly		:	delay for internal vsync generation (out_vsync)

 * return					:	void

 * brief
	Generate an internal and output vsync by an input vsync delay.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setSync(uint8 IN pipLineDelay, uint16 IN outLineDaly)
{
	_REG(SVM_CTRL_REG->syncCtrl, (outLineDaly & utilBitMask(16)) | ((pipLineDelay & utilBitMask(8)) << 16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBGColor

 * param y					:	y
 * param cb					:	cb
 * param cr					:	cr

 * return					:	void

 * brief
	Sets the background color of the SVM.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setBGColor(uint8 IN y, uint8 IN cb, uint8 IN cr)
{
	_REG(SVM_CTRL_REG->bgColor, (cr) | (cb << 8) | (y <<16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setMaskColor

 * param y					:	Y
 * param cbcr				:	Cb & Cr

 * return					:	void

 * brief
	Sets the maskcolor of the SVM.
	Maskcolor is used in an invalid lut areas.
	Cb should be equal to Cr.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setMaskColor(uint8 IN y, uint8 IN cbcr)
{
	_REG(SVM_CTRL_REG->maskColor, (cbcr) | (cbcr << 8) | (y <<16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setColorCoef

 * param coefColor			:	information of color coefficient

 * return					:	void

 * brief
	Sets the color coefficient of the SVM.
	yuv to rgb or rgb to yuv
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setColorCoef(pSVM_COLORCOEF_T IN coefColor)
{
	_REG(SVM_COLOR_COEF_REG->r2y4y, (coefColor->r2y.r4y) | (coefColor->r2y.g4y << 8) | (coefColor->r2y.b4y << 16) | (coefColor->isWith128 << 24));
	_REG(SVM_COLOR_COEF_REG->r2y4cb, (coefColor->r2y.r4cb) | (coefColor->r2y.g4cb << 8) | (coefColor->r2y.b4cb << 16));
	_REG(SVM_COLOR_COEF_REG->r2y4cr, (coefColor->r2y.r4cr) | (coefColor->r2y.g4cr << 8) | (coefColor->r2y.b4cr << 16));

	_REG(SVM_COLOR_COEF_REG->y2r4g, (coefColor->y2r.cb4g) | (coefColor->y2r.cr4g << 8));
	_REG(SVM_COLOR_COEF_REG->y2r4rnb, (coefColor->y2r.cr4r & utilBitMask(9)) | ((coefColor->y2r.cb4b & utilBitMask(9)) << 16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCar

 * param viewMode			:	number of viewmode
 * param rect				:	position for drawing cars
 * param maskColorY			:	Y of maskColor
 * param maskColorCb		:	Cb of maskColor
 * param maskColorCr		:	Cr of maskColor

 * return					:	error code

 * brief
	Sets the car model of the SVM.
	X position vlaue should be an even number greater than or equal to 2.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCar(uint8 IN viewMode, SVM_RECT_T IN rect, uint8 IN maskColorY, uint8 IN maskColorCb, uint8 IN maskColorCr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount || rect.width < 2 || rect.height == 0 )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( rect.x != 0 && rect.x % 2 != 0 ) --rect.x;
	gSVM_viewMode[viewMode].car.rect = rect;

	gSVM_viewMode[viewMode].car.maskColor = (maskColorCr) | (maskColorCb << 8) | (maskColorY <<16);

	if ( gSVM_curViewMode == viewMode )
	{
		_REG(SVM_CTRL_REG->carCtrl, utilPutBits(SVM_CTRL_REG->carCtrl, 0, 24, gSVM_viewMode[viewMode].car.maskColor));
		_REG(SVM_CTRL_REG->carX, (rect.x & utilBitMask(11)) | (((rect.width - 1) & utilBitMask(9)) << 16));
		_REG(SVM_CTRL_REG->carY, (rect.y & utilBitMask(11)) | (((rect.height - 1) & utilBitMask(9)) << 16) | (utilGetBit(SVM_CTRL_REG->carY, 31) << 31));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCarReadBurstLength

 * param burstLength		:	burst length

 * return					:	error code

 * brief
 	Sets the burst size of the car image.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCarReadBurstLength(uint8 IN burstLength)
{
	if ( burstLength > 127 )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->carCtrl, utilPutBits(SVM_CTRL_REG->carCtrl, 24, 7, burstLength));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateCar

 * param viewMode			:	number of viewmode
 * param carImage			:	binary data of car image ( yuv422 data format )
 * param size				:	size of car image

 * return					:	error code

 * brief
	Update car image.
	The update of car image should be done before enable of SVM.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_updateCar(uint8 IN viewMode, uint16* IN carImage, uint32 IN size)
{
	int i;
	uint16 *src, *dst;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( (carImage == NULL) || (size == 0) || (size > gSVM_outputSize.width * gSVM_outputSize.height) || (viewMode >= gSVM_viewModeCount) )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( gSVM_viewMode[viewMode].car.addr == SVM_UNUSE_ADDRESS )
		return _RET(eERROR_SVM_UNUSE_CAR_VIEWMODE);

	if (  utilGetBit(SVM_CTRL_REG->enable, 0) == TRUE )
		return _RET(eERROR_SVM_RUNNING);

	src = carImage;
	dst = (uint16*)(gSVM_viewMode[viewMode].car.addr + 0x40000000);
	size = size / 2;	// 16bit count

	for ( i = 0; i < size; i++ )
	{
		*(dst+i) = *(src+i);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCarAddr

 * param viewMode			:	number of viewmode
 * param addr				:	address of car image memory

 * return					:	error code

 * brief
	Sets the memory address of car image.
	The memory address is determined by SVMAPI_createViewMode().
	This function can be used when it is necessary to change the address value of the car image for additional function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCarAddr(uint8 IN viewMode, uint32 IN addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	gSVM_viewMode[viewMode].car.addr = addr;

	if ( gSVM_curViewMode == viewMode )
	{
		_REG(SVM_CTRL_REG->carAddr, gSVM_viewMode[viewMode].car.addr);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getCarAddr

 * param viewMode			:	number of viewmode
 * param addr				:	address of car image memory

 * return					:	error code

 * brief
	Gets the memory address of car image.
	The memory address is determined by SVMAPI_createViewMode().
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_getCarAddr(uint8 IN viewMode, uint32 OUT *addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount || addr == NULL )
		return _RET(eERROR_INVALID_ARGUMENT);

	*addr = gSVM_viewMode[viewMode].car.addr;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCarEable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the car object.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setCarEnable(uint8 IN viewMode, bool IN isEnable)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].car.isEnable = isEnable;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].car.isEnable = isEnable;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		_REG(SVM_CTRL_REG->carY, utilPutBit(SVM_CTRL_REG->carY, 31, isEnable));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setEdgeEnhancementMode

 * param mode				:	mode (tagSVM_EDGE_MODE_E)

 * return					:	error code

 * brief
	Sets the mode of edge enhancement.
	mode 0 uses brightness control lut. ([31:26])
	mode 1 uses fixed gain.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setEdgeEnhancementMode(uint8 IN mode)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( mode >= eSVM_EDGE_MODE_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->outMode, utilPutBit(SVM_CTRL_REG->outMode, 24, mode));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setEdgeEnhancementFixedGain

 * param gain				:	fixed gain

 * return					:	error code

 * brief
	Sets the fixed gain of edge enhancement.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setEdgeEnhancementFixedGain(float IN gain)
{
	int i = 0;
	int integerBit = 0;
	int decimalBit = 0x0;
	float decimal = 0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( gain < 0.0f )
		return _RET(eERROR_INVALID_ARGUMENT);

	integerBit = (int)gain;
	decimalBit = 0x0;
	decimal = gain - integerBit;

	for ( i = 0; i < 3; i++ )
	{
		decimal *= 2;
		if ( decimal >= 1.0f )
		{
			decimalBit |= (0x1 << (2-i));
			decimal = decimal - (int)decimal;
		}
		else
		{
			decimalBit |= (0x0 << (2-i));
		}
	}

	_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 25, 6, ((decimalBit & utilBitMask(3)) << 0) | ((integerBit & utilBitMask(3)) << 3)));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setEdgeEnhancementEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of edge enhancement
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setEdgeEnhancementEnable(bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	_REG(SVM_CTRL_REG->outMode, utilPutBit(SVM_CTRL_REG->outMode, 23, isEnable));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setDynamicblendingCoef

 * param gain				:	fixed gain

 * return					:	error code

 * brief
	Sets the coefficient of dynamic blending.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setDynamicblendingCoef(uint8 coef1, uint8 coef3, uint8 coef6, uint8 coef8)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	_REG(SVM_DYNAMIC_BLEND_REG->var, (coef1 << 0) | (coef3 << 8) | (coef6 << 16) | (coef8 << 24));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setDynamicblendingEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of dynamic blending
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setDynamicblendingEnable(bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	_REG(SVM_CTRL_REG->outMode, utilPutBit(SVM_CTRL_REG->outMode, 31, isEnable));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setMorphingSpeed

 * param speed				:	speed of morphing (0~7)

 * return					:	error code

 * brief
	Sets the speed for morphing.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setMorphingSpeed(uint8 IN speed)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( speed > 7 )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 18, 3, speed));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateBCCoef

 * param coefBC				:	coefficient information of brightness control

 * return					:	error code

 * brief
	Update coefficient of brightness control.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_updateBCCoef(pSVM_BCCOEF_T IN coefBC)
{
	int i = 0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( coefBC == NULL )
		return _RET(eERROR_INVALID_ARGUMENT);

	for ( i = 0; i < eSVM_COLOR_MAX; i++ )
	{
		_REG(SVM_COEF_REG->coef1[i], (coefBC->coef1[i][eSVM_SUBPART_FL]) | (coefBC->coef1[i][eSVM_SUBPART_FR] << 8) | (coefBC->coef1[i][eSVM_SUBPART_BR] << 16) | (coefBC->coef1[i][eSVM_SUBPART_BL] << 24));
		_REG(SVM_COEF_REG->coef3[i], (coefBC->coef3[i][eSVM_SUBPART_FL]) | (coefBC->coef3[i][eSVM_SUBPART_FR] << 8) | (coefBC->coef3[i][eSVM_SUBPART_BR] << 16) | (coefBC->coef3[i][eSVM_SUBPART_BL] << 24));
	}

	for ( i = 0; i < eSVM_COLOR_MAX; i++ )
	{
		_REG(SVM_COEF_REG->coef2n4[i], (coefBC->coef2[i][eSVM_FB_PART_F]) | (coefBC->coef2[i][eSVM_FB_PART_B] << 8) | (coefBC->coef4[i][eSVM_LR_PART_L] << 16) | (coefBC->coef4[i][eSVM_LR_PART_R] << 24));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateBCRatio

 * param ratio				:	ratio of brightness control

 * return					:	void

 * brief
	Update ratio of brightness control.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_updateBCRatio(uint8 IN ratio)
{
	_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 8, 8, ratio));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getPageNum4Lut

 * param viewMode			:	number of viewmode
 * param outputPartNum		:	front/back lut or left/right lut (ref. tagSVM_OUTPUT_PART_E)

 * return					:	page number (ref. tagSVM_PAGEFLIPPING_NUMBER_E)
 								If the return value is eSVM_PAGEFLIPPING_NUMBER_MAX, it is an error.

 * brief
	Gets the page number of view mode.
	The page number is used for seamless modification of lut.
***************************************************************************************************************************************************************/
uint8 APIENTRY SVMAPI_getPageNum4Lut(uint8 IN viewMode, uint8 IN outputPartNum)
{
	if ( gSVM_isInit == FALSE )
	{
		_RET(eERROR_SVM_NOT_INITIALIZE);
		return eSVM_PAGEFLIPPING_NUMBER_MAX;
	}

	if ( viewMode >= gSVM_viewModeCount || outputPartNum >= eSVM_OUTPUT_PART_MAX )
	{
		_RET(eERROR_INVALID_ARGUMENT);
		return eSVM_PAGEFLIPPING_NUMBER_MAX;
	}

	return gSVM_viewMode[viewMode].pageNum4LUT[outputPartNum];
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateFBLUT

 * param viewMode			:	number of viewmode
 * param lutBin				:	binary data of lut
 * param size				:	size of lut data

 * return					:	error code

 * brief
	Update front/back LUT.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_updateFBLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size)
{
	int i;
	uint32 *src, *dst;
	uint32 lutAddr;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size != gSVM_lutSize )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_0] == SVM_UNUSE_ADDRESS )
		return _RET(eERROR_SVM_UNUSE_LUT_VIEWMODE);

	if ( gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB] == eSVM_PAGEFLIPPING_NUMBER_0 )		// page0 -> page1
	{
		lutAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_1];
		gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB] = eSVM_PAGEFLIPPING_NUMBER_1;
	}
	else																							// page1 -> page0
	{
		lutAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_0];
		gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB] = eSVM_PAGEFLIPPING_NUMBER_0;
	}

	for ( i = 0; i < gSVM_viewModeCount; i++ )
	{
		if ( gSVM_viewMode[i].refViewMode[eSVM_REFERENCE_VIEWMODE_FB_LUT] == viewMode )
		{
			gSVM_viewMode[i].pageNum4LUT[eSVM_OUTPUT_PART_FB] = gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB];
		}
	}

	src = lutBin;
	dst = (uint32*)(lutAddr + 0x40000000);
	size = size / 4;	// 32bit count

	for ( i = 0; i < size; i++ )
	{
		*(dst+i) = *(src+i);
	}

	if ( gSVM_curViewMode == viewMode )
	{
		if ( gSVM_isMorphing[eSVM_OUTPUT_PART_FB] )
		{
			_REG(SVM_CTRL_REG->fbLutAddr[1], lutAddr);
		}
		else
		{
			_REG(SVM_CTRL_REG->fbLutAddr[0], lutAddr);
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateLRLUT

 * param viewMode			:	number of viewmode
 * param lutBin				:	binary data of lut
 * param size				:	size of lut data

 * return					:	error code

 * brief
	Update left/right LUT.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_updateLRLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size)
{
	int i;
	uint32 *src, *dst;
	uint32 lutAddr;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size != gSVM_lutSize )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_0] == SVM_UNUSE_ADDRESS )
		return _RET(eERROR_SVM_UNUSE_LUT_VIEWMODE);

	if ( gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR] == eSVM_PAGEFLIPPING_NUMBER_0 )		// page0 -> page1
	{
		lutAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_1];
		gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR] = eSVM_PAGEFLIPPING_NUMBER_1;
	}
	else																			// page1 -> page0
	{
		lutAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_0];
		gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR] = eSVM_PAGEFLIPPING_NUMBER_0;
	}

	for ( i = 0; i < gSVM_viewModeCount; i++ )
	{
		if ( gSVM_viewMode[i].refViewMode[eSVM_REFERENCE_VIEWMODE_LR_LUT] == viewMode )
		{
			gSVM_viewMode[i].pageNum4LUT[eSVM_OUTPUT_PART_LR] = gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR];
		}
	}

	src = lutBin;
	dst = (uint32*)(lutAddr + 0x40000000);
	size = size / 4;	// 32bit count

	for ( i = 0; i < size; i++ )
	{
		*(dst+i) = *(src+i);
	}

	if ( gSVM_curViewMode == viewMode )
	{
		if ( gSVM_isMorphing[eSVM_OUTPUT_PART_LR] )
		{
			_REG(SVM_CTRL_REG->lrLutAddr[1], lutAddr);
		}
		else
		{
			_REG(SVM_CTRL_REG->lrLutAddr[0], lutAddr);
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateBCLUT

 * param viewMode			:	number of viewmode
 * param lutBin				:	binary data of lut
 * param size				:	size of lut data

 * return					:	error code

 * brief
	Update brightness control LUT.
	The update of BC LUT should be done before enable of SVM.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_updateBCLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size)
{
	int i;
	uint32 *src, *dst;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size != gSVM_lutSize )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( gSVM_viewMode[viewMode].bclutAddr == SVM_UNUSE_ADDRESS )
		return _RET(eERROR_SVM_UNUSE_LUT_VIEWMODE);

	if (  utilGetBit(SVM_CTRL_REG->enable, 0) == TRUE )
		return _RET(eERROR_SVM_RUNNING);

	src = lutBin;
	dst = (uint32*)(gSVM_viewMode[viewMode].bclutAddr + 0x40000000);
	size = size / 4;	// 32bit count

	for ( i = 0; i < size; i++ )
	{
		*(dst+i) = *(src+i);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setFBLUTAddr

 * param viewMode			:	number of viewmode
 * param pageflipping0Addr	:	address of page0 lut memory
 * param pageflipping1Addr	:	address of page1 lut memory

 * return					:	error code

 * brief
	Sets the memory address of front_back lut.
	The memory address is determined by SVMAPI_createViewMode().
	This function can be used when it is necessary to change the address value of the front_back lut for additional function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setFBLUTAddr(uint8 IN viewMode, uint32 IN pageflipping0Addr, uint32 IN pageflipping1Addr)
{
	uint8 pageNum;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_0] = pageflipping0Addr;
	gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_1] = pageflipping1Addr;

	if ( gSVM_curViewMode == viewMode )
	{
		pageNum = gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB];

		if ( gSVM_isMorphing[eSVM_OUTPUT_PART_FB] )
		{
			_REG(SVM_CTRL_REG->lrLutAddr[1], gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][pageNum]);
		}
		else
		{
			_REG(SVM_CTRL_REG->lrLutAddr[0], gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][pageNum]);
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setLRLUTAddr

 * param viewMode			:	number of viewmode
 * param pageflipping0Addr	:	address of page0 lut memory
 * param pageflipping1Addr	:	address of page1 lut memory

 * return					:	error code

 * brief
	Sets the memory address of left_right lut.
	The memory address is determined by SVMAPI_createViewMode().
	This function can be used when it is necessary to change the address value of the left_right lut for additional function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setLRLUTAddr(uint8 IN viewMode, uint32 IN pageflipping0Addr, uint32 IN pageflipping1Addr)
{
	uint8 pageNum;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_0] = pageflipping0Addr;
	gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_1] = pageflipping1Addr;

	if ( gSVM_curViewMode == viewMode )
	{
		pageNum = gSVM_viewMode[viewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR];

		if ( gSVM_isMorphing[eSVM_OUTPUT_PART_LR] )
		{
			_REG(SVM_CTRL_REG->lrLutAddr[1], gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][pageNum]);
		}
		else
		{
			_REG(SVM_CTRL_REG->lrLutAddr[0], gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][pageNum]);
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBCLUTAddr

 * param viewMode			:	number of viewmode
 * param addr				:	address of bc lut memory

 * return					:	error code

 * brief
	Sets the memory address of brightnees control lut.
	The memory address is determined by SVMAPI_createViewMode().
	This function can be used when it is necessary to change the address value of the brightness control lut for additional function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setBCLUTAddr(uint8 IN viewMode, uint32 IN addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	gSVM_viewMode[viewMode].bclutAddr = addr;

	if ( gSVM_curViewMode == viewMode )
	{
		_REG(SVM_CTRL_REG->bcLutAddr, gSVM_viewMode[viewMode].bclutAddr);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getFBLUTAddr

 * param viewMode			:	number of viewmode
 * param pageflipping0Addr	:	address of page0 lut memory
 * param pageflipping1Addr	:	address of page1 lut memory

 * return					:	error code

 * brief
	Gets the memory address of front_back lut.
	The memory address is determined by SVMAPI_createViewMode().
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_getFBLUTAddr(uint8 IN viewMode, uint32 OUT *pageflipping0Addr, uint32 OUT *pageflipping1Addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount || pageflipping0Addr == NULL || pageflipping1Addr == NULL )
		return _RET(eERROR_INVALID_ARGUMENT);

	*pageflipping0Addr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_0];
	*pageflipping1Addr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_FB][eSVM_PAGEFLIPPING_NUMBER_1];

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getLRLUTAddr

 * param viewMode			:	number of viewmode
 * param pageflipping0Addr	:	address of page0 lut memory
 * param pageflipping1Addr	:	address of page1 lut memory

 * return					:	error code

 * brief
	Gets the memory address of left_right lut.
	The memory address is determined by SVMAPI_createViewMode().
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_getLRLUTAddr(uint8 IN viewMode, uint32 OUT *pageflipping0Addr, uint32 OUT *pageflipping1Addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount || pageflipping0Addr == NULL || pageflipping1Addr == NULL )
		return _RET(eERROR_INVALID_ARGUMENT);

	*pageflipping0Addr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_0];
	*pageflipping1Addr = gSVM_viewMode[viewMode].lutAddr[eSVM_OUTPUT_PART_LR][eSVM_PAGEFLIPPING_NUMBER_1];

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getBCLUTAddr

 * param viewMode			:	number of viewmode
 * param addr				:	address of bc lut memory

 * return					:	error code

 * brief
	Gets the memory address of brightness control lut.
	The memory address is determined by SVMAPI_createViewMode().
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_getBCLUTAddr(uint8 IN viewMode, uint32 OUT *addr)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( viewMode >= gSVM_viewModeCount || addr == NULL )
		return _RET(eERROR_INVALID_ARGUMENT);

	*addr = gSVM_viewMode[viewMode].bclutAddr;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getRGBStatistics

 * param stats				:	porinter of statistics structur

 * return					:	void

 * brief
	Gets statistics value from the output of SVM.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_getRGBStatistics(pSVM_RGB_STATISTICS_T OUT stats)
{
	int i = 0, j = 0, k = 0;
	for ( i = 0; i < eSVM_OUTPUT_PART_MAX; i++ )
	{
		for ( j = 0; j < eSVM_SUBPART_MAX; j++ )
		{
			for ( k = 0; k < eSVM_COLOR_MAX; k++ )
				stats->value[i][j][eSVM_COLOR_R] = SVM_STAT_REG->stat[i][j][k] & utilBitMask(26);
		}
	}
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setSVMEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the SVM. If set to disable, it is front bypass.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setSVMEnable(bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( gSVM_curViewMode < 0 )
		return _RET(eERROR_SVM_VIEWMODE_NOT_SELECTED);

	_REG(SVM_CTRL_REG->enable, utilPutBit(SVM_CTRL_REG->enable, 0, isEnable));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBlank

 * param hBlank				:	horizontal blank
 * param vBlank				:	vertical blank

 * return					:	error code

 * brief
 	Sets horizontal and vertical blank of output.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setBlank(uint16 IN hBlank, uint16 IN vBlank)
{
	_REG(SVM_CTRL_REG->blank, (hBlank & utilBitMask(16)) | ((vBlank & utilBitMask(16)) << 16));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setSection

 * param viewMode			:	number of viewmode
 * param sectionNumber		:	number of section for output
 * param section			:	information of section

 * return					:	error code

 * brief
	Sets the section of the SVM.
	Section is a function that outputs only a specific rectangle.

	ex)
	------------------------------
	|                            | output area
	|  ------------              |
	|  |          |              |
	|  |section0  |  ------------|
	|  |          |  |           |
	|  ------------  | section1  | Output only the section area.
	|                |           |
	------------------------------
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setSection(uint8 IN viewMode, uint8 IN sectionNumber, SVM_RECT_T IN sectionRect)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

 	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || sectionNumber >= eSVM_SECTION_NUMBER_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].section[sectionNumber].rect = sectionRect;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].section[sectionNumber].rect = sectionRect;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		switch (sectionNumber)
		{
		case eSVM_SECTION_NUMBER_0:
			_REG(SVM_CTRL_REG->section0X, (sectionRect.x & utilBitMask(11)) | (((sectionRect.width - 1) & utilBitMask(11)) << 16));
			_REG(SVM_CTRL_REG->section0Y, (sectionRect.y & utilBitMask(11)) | (((sectionRect.height - 1) & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->section0Y, 31) << 31));
			break;
		case eSVM_SECTION_NUMBER_1:
			_REG(SVM_CTRL_REG->section1X, (sectionRect.x & utilBitMask(11)) | (((sectionRect.width - 1) & utilBitMask(11)) << 16));
			_REG(SVM_CTRL_REG->section1Y, (sectionRect.y & utilBitMask(11)) | (((sectionRect.height - 1) & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->section1Y, 31) << 31));
			break;
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setSectionEnable

 * param viewMode			:	number of viewmode
 * param sectionNumber		:	number of section for output
 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the section function.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setSectionEnable(uint8 IN viewMode, uint8 IN sectionNumber, bool IN isEnable)
{
	int i;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

 	if ( (viewMode != SVM_APPLY_ALL_VIEWMODE && viewMode >= gSVM_viewModeCount) || sectionNumber >= eSVM_SECTION_NUMBER_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE )
	{
		for ( i = 0; i < gSVM_viewModeCount; i++ )
		{
			gSVM_viewMode[i].section[sectionNumber].isEnable = isEnable;
		}
	}
	else
	{
		gSVM_viewMode[viewMode].section[sectionNumber].isEnable = isEnable;
	}

	if ( viewMode == SVM_APPLY_ALL_VIEWMODE || gSVM_curViewMode == viewMode )
	{
		switch (sectionNumber)
		{
		case eSVM_SECTION_NUMBER_0:
			_REG(SVM_CTRL_REG->section0Y, utilPutBit(SVM_CTRL_REG->section0Y, 31, isEnable));
			break;
		case eSVM_SECTION_NUMBER_1:
			_REG(SVM_CTRL_REG->section1Y, utilPutBit(SVM_CTRL_REG->section1Y, 31, isEnable));
			break;
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setOutputBurstLength

 * param burstLength		:	burst length (ref. tagSVM_OUTPUT_BURST_LENGTH [8 ~ 128])

 * return					:	error code

 * brief
 	Sets the burst size of the read main.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setOutputBurstLength(uint8 IN burstLength)
{
	if ( burstLength < eSVM_BURST_LENGTH_8 || burstLength > eSVM_BURST_LENGTH_128 )
		return _RET(eERROR_INVALID_ARGUMENT);

	_REG(SVM_CTRL_REG->rmain, utilPutBits(SVM_CTRL_REG->rmain, 4, 4, burstLength));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInvalidLUTEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the invalid lut.
	Invalid lut is not used for unnecessary parts using alpha of bc lut.
	Invalid lut is used to reduce MBW(Memory Band Width).
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setInvalidLUTEnable(uint8 IN isEnable)
{
	_REG(SVM_CTRL_REG->outMode, utilPutBit(SVM_CTRL_REG->outMode, 6, isEnable));
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setOutputEnable

 * param isFBEnable			:	enable or disable of front/back output data
 * param isLREnable			:	enable or disable of left/right output data

 * return					:	error code

 * brief
	Enable or disable of the output data.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setOutputEnable(bool IN isFBEnable, bool IN isLREnable)
{
	uint8 burst = 0x0;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	burst = utilGetBits(SVM_CTRL_REG->rmain, 4, 4);

	_REG(SVM_CTRL_REG->rmain, (isFBEnable << 0) | ((burst & utilBitMask(4)) << 4) | (isLREnable << 8));

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCBOutputVsync

 * param callback			:	pointer function for callback

 * return					:	error code

 * brief
	Sets pointer callback of the output vsync.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setCBOutputVsync(SVM_CALLBACK IN callback)
{
	gSVM_CBVsync = callback;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setViewMode

 * param viewMode			:	number of viewmode
 * param isFBMorphing		:	enable or disable of front/back morphing
 * param isLRMorphing		:	enable or disable of left/right morphing

 * return					:	error code

 * brief
	Sets the viewmode of the SVM.
	Morphing is only possible for one channel.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_setViewMode(uint8 IN viewMode, bool IN isFBMorphing, bool IN isLRMorphing)
{
	int i = 0;
	int curViewMode = gSVM_curViewMode;
	int dstViewMode = viewMode;

	uint32 lutAddrViewmodeFB = 0x0, lutAddrViewmodeLR = 0x0;
	int pageNumFB, pageNumLR;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( gSVM_curViewMode == viewMode )
		return eSUCCESS;

	if ( viewMode >= gSVM_viewModeCount )
		return _RET(eERROR_INVALID_ARGUMENT);

	if ( gSVM_viewMode[viewMode].isEnable == FALSE )
		return _RET(eERROR_SVM_UNUSE_VIEWMODE);

	if ( gSVM_curViewMode != viewMode )
	{
		gSVM_curViewMode = dstViewMode;

		/* Compression */
		SVMAPI_setCompression(dstViewMode, gSVM_viewMode[dstViewMode].compression.ratio);
		SVMAPI_setCompressionEnable(dstViewMode, gSVM_viewMode[dstViewMode].compression.isEnable);

		/* ROI */
		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			SVMAPI_setROI(dstViewMode, i, gSVM_viewMode[dstViewMode].roi[i].area);
			SVMAPI_setROIEnable(dstViewMode, i, gSVM_viewMode[dstViewMode].roi[i].isEnable);
		}

		/* hscale (boundary) */
		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			SVMAPI_setHorizontalScale(dstViewMode, i, gSVM_viewMode[dstViewMode].boundary[i]);
		}

		/* vscale */
		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			SVMAPI_setVerticalHalfScale(dstViewMode, i, gSVM_viewMode[dstViewMode].vscale[i]);
		}

		/* section */
		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			SVMAPI_setSection(dstViewMode, i, gSVM_viewMode[dstViewMode].section[i].rect);
			SVMAPI_setSectionEnable(dstViewMode, i, gSVM_viewMode[dstViewMode].section[i].isEnable);
		}

		/* current view mode */
		pageNumFB = gSVM_viewMode[curViewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB];
		pageNumLR = gSVM_viewMode[curViewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR];
		lutAddrViewmodeFB = gSVM_viewMode[curViewMode].lutAddr[eSVM_OUTPUT_PART_FB][pageNumFB];
		lutAddrViewmodeLR = gSVM_viewMode[curViewMode].lutAddr[eSVM_OUTPUT_PART_LR][pageNumLR];
		if ( lutAddrViewmodeFB != SVM_UNUSE_ADDRESS && lutAddrViewmodeLR != SVM_UNUSE_ADDRESS )
		{
			isFBMorphing = FALSE;
			isLRMorphing = FALSE;
		}

		/* dstination view mode : FB LUT & LR LUT & Morphing */
		pageNumFB = gSVM_viewMode[dstViewMode].pageNum4LUT[eSVM_OUTPUT_PART_FB];
		pageNumLR = gSVM_viewMode[dstViewMode].pageNum4LUT[eSVM_OUTPUT_PART_LR];
		lutAddrViewmodeFB = gSVM_viewMode[dstViewMode].lutAddr[eSVM_OUTPUT_PART_FB][pageNumFB];
		lutAddrViewmodeLR = gSVM_viewMode[dstViewMode].lutAddr[eSVM_OUTPUT_PART_LR][pageNumLR];
		if ( lutAddrViewmodeFB != SVM_UNUSE_ADDRESS && lutAddrViewmodeLR != SVM_UNUSE_ADDRESS )
		{
			bool isDecrementFB = FALSE, isDecrementLR = FALSE;
			uint8 ratioFB, ratioLR;
			uint8 speed;
			uint8 morphineEn = 0;

			if ( isFBMorphing )
			{
				if ( gSVM_isMorphing[eSVM_OUTPUT_PART_FB] )
				{
					_REG(SVM_CTRL_REG->fbLutAddr[0], lutAddrViewmodeFB);
					gSVM_isMorphing[eSVM_OUTPUT_PART_FB] = FALSE;
					isDecrementFB = TRUE;
					ratioFB = 0xFF;
				}
				else
				{
					_REG(SVM_CTRL_REG->fbLutAddr[1], lutAddrViewmodeFB);
					gSVM_isMorphing[eSVM_OUTPUT_PART_FB] = TRUE;
					isDecrementFB = FALSE;
					ratioFB = 0x00;
				}

				morphineEn = utilSetBit(morphineEn, 0);
			}
			else
			{
				if ( gSVM_isMorphing[eSVM_OUTPUT_PART_FB] )
				{
					_REG(SVM_CTRL_REG->fbLutAddr[1], lutAddrViewmodeFB);
					ratioFB = 0xFF;
				}
				else
				{
					_REG(SVM_CTRL_REG->fbLutAddr[0], lutAddrViewmodeFB);
					ratioFB = 0x00;
				}
			}

			if ( isLRMorphing )
			{
				if ( gSVM_isMorphing[eSVM_OUTPUT_PART_LR] )
				{
					_REG(SVM_CTRL_REG->lrLutAddr[0], lutAddrViewmodeLR);
					gSVM_isMorphing[eSVM_OUTPUT_PART_LR] = FALSE;
					isDecrementLR = TRUE;
					ratioLR = 0xFF;
				}
				else
				{
					_REG(SVM_CTRL_REG->lrLutAddr[1], lutAddrViewmodeLR);
					gSVM_isMorphing[eSVM_OUTPUT_PART_LR] = TRUE;
					isDecrementLR = FALSE;
					ratioLR = 0x00;
				}

				morphineEn = utilSetBit(morphineEn, 1);
			}
			else
			{
				if ( gSVM_isMorphing[eSVM_OUTPUT_PART_LR] )
				{
					_REG(SVM_CTRL_REG->lrLutAddr[1], lutAddrViewmodeLR);
					ratioLR = 0xFF;
				}
				else
				{
					_REG(SVM_CTRL_REG->lrLutAddr[0], lutAddrViewmodeLR);
					ratioLR = 0x00;
				}
			}

			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 16, 8, ratioFB));
			_REG(SVM_CTRL_REG->chCtrl, utilPutBits(SVM_CTRL_REG->chCtrl, 24, 8, ratioLR));

			speed = utilGetBits(SVM_CTRL_REG->outMode, 18, 3);

			if ( isFBMorphing || isLRMorphing )
			{
				_REG(SVM_CTRL_REG->outMode, utilPutBits(SVM_CTRL_REG->outMode, 16, 7, (morphineEn << 0) | ((speed & utilBitMask(3)) << 2) | (isDecrementFB << 5) | (isDecrementLR << 6)));
			}
		}

		/* LUT of brightness control */
		if ( gSVM_viewMode[dstViewMode].bclutAddr != SVM_UNUSE_ADDRESS )
			_REG(SVM_CTRL_REG->bcLutAddr, gSVM_viewMode[dstViewMode].bclutAddr);

		/* car object */
		if ( gSVM_viewMode[dstViewMode].car.addr != SVM_UNUSE_ADDRESS )
		{
			_REG(SVM_CTRL_REG->carAddr, gSVM_viewMode[dstViewMode].car.addr);
			_REG(SVM_CTRL_REG->carCtrl, utilPutBits(SVM_CTRL_REG->carCtrl, 0, 24, gSVM_viewMode[dstViewMode].car.maskColor));
			_REG(SVM_CTRL_REG->carX, (gSVM_viewMode[dstViewMode].car.rect.x & utilBitMask(11)) | (((gSVM_viewMode[dstViewMode].car.rect.width - 1) & utilBitMask(9)) << 16));
			_REG(SVM_CTRL_REG->carY, (gSVM_viewMode[dstViewMode].car.rect.y & utilBitMask(11)) | (((gSVM_viewMode[dstViewMode].car.rect.height - 1) & utilBitMask(9)) << 16) |
									 (gSVM_viewMode[dstViewMode].car.isEnable << 31));
		}

		SVMAPI_setOutputMode(dstViewMode, gSVM_viewMode[dstViewMode].outputMode);

		SVMAPI_setSVMEnable(TRUE);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_createViewMode

 * param setFBLUT			:	number of viewmode, SVM_USE_LUT, or SVM_UNUSE_LUT
 * param setLRLUT			:	number of viewmode, SVM_USE_LUT, or SVM_UNUSE_LUT
 * param setBCLUT			:	number of viewmode, SVM_USE_LUT, or SVM_UNUSE_LUT
 * param setCarImage		: 	number of viewmode, SVM_USE_CAR, or SVM_UNUSE_CAR
 * param outputMode			:	output mode (ref. enum SVM_OUTPUTMODE_E)
 * param viewMode			:	number of viewmode

 * return					:	Error code

 * brief
	Create view mode. View mode is a scene of SVM.
	LUT and car image
	- SVM_USE_LUT : Create a new lut area.
	- SVM_UNUSE_LUT : Unused a lut area.
	- number of viewmode : Use lut from the viewmode have already created.
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_createViewMode(uint8 IN setFBLUT, uint8 IN setLRLUT, uint8 IN setBCLUT, uint8 IN setCarImage, uint8 IN outputMode, uint8 OUT *viewMode)
{
	int i;
	uint8 findViewMode = SVM_UNUSE_VIEWMODE;
	uint8 preViewMode = SVM_UNUSE_VIEWMODE;

	if ( gSVM_isInit == FALSE )
		return _RET(eERROR_SVM_NOT_INITIALIZE);

	if ( outputMode >= eSVM_OUTPUTMODE_MAX )
		return _RET(eERROR_INVALID_ARGUMENT);

	for ( i = 0; i < SVM_MAX_VIEWMODE_COUNT; i++ )
	{
		if ( gSVM_viewMode[i].isEnable == FALSE )
		{
			findViewMode = i;
			break;
		}
	}

	*viewMode = findViewMode;
	if ( findViewMode != 0 ) preViewMode = findViewMode - 1;

	if ( findViewMode == SVM_UNUSE_VIEWMODE )
	{
		return _RET(eERROR_SVM_TOO_MAANY_LIMIT_VIEWMODE);
	}
	else
	{
		uint32 carMemSize;
		uint32 memPos = SVM_BASE_MEMORY;

		if ( preViewMode == SVM_UNUSE_VIEWMODE )
		{
			uint32 channelMemSize = 0;

			for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
			{
				channelMemSize += gSVM_channelAddr[i].size;
			}

			gSVM_viewMode[findViewMode].baseAddr = SVM_BASE_MEMORY + channelMemSize;
		}
		else
		{
			gSVM_viewMode[findViewMode].baseAddr = gSVM_viewMode[preViewMode].baseAddr + gSVM_viewMode[preViewMode].memSize;
		}

		carMemSize = gSVM_outputSize.width * gSVM_outputSize.height * 2;
		carMemSize = ((carMemSize + 7) >> 3) << 3;	// 8byte align

		memPos = gSVM_viewMode[findViewMode].baseAddr;

		switch (setFBLUT)
		{
		case SVM_UNUSE_LUT:
			break;
		case SVM_USE_LUT:
			for ( i = 0; i < eSVM_PAGEFLIPPING_NUMBER_MAX; i++ )
			{
				gSVM_viewMode[findViewMode].lutAddr[eSVM_OUTPUT_PART_FB][i] = memPos;
				memPos += gSVM_lutSize;
			}
			break;
		default:
			for ( i = 0; i < eSVM_PAGEFLIPPING_NUMBER_MAX; i++ )
			{
				gSVM_viewMode[findViewMode].lutAddr[eSVM_OUTPUT_PART_FB][i] = gSVM_viewMode[setFBLUT].lutAddr[eSVM_OUTPUT_PART_FB][i];
			}
			gSVM_viewMode[findViewMode].refViewMode[eSVM_REFERENCE_VIEWMODE_FB_LUT] = setFBLUT;
			break;
		}

		switch (setLRLUT)
		{
		case SVM_UNUSE_LUT:
			break;
		case SVM_USE_LUT:
			for ( i = 0; i < eSVM_PAGEFLIPPING_NUMBER_MAX; i++ )
			{
				gSVM_viewMode[findViewMode].lutAddr[eSVM_OUTPUT_PART_LR][i] = memPos;
				memPos += gSVM_lutSize;
			}
			break;
		default:
			for ( i = 0; i < eSVM_PAGEFLIPPING_NUMBER_MAX; i++ )
			{
				gSVM_viewMode[findViewMode].lutAddr[eSVM_OUTPUT_PART_LR][i] = gSVM_viewMode[setLRLUT].lutAddr[eSVM_OUTPUT_PART_LR][i];
			}
			gSVM_viewMode[findViewMode].refViewMode[eSVM_REFERENCE_VIEWMODE_LR_LUT] = setLRLUT;
			break;
		}

		switch (setBCLUT)
		{
		case SVM_UNUSE_LUT:
			break;
		case SVM_USE_LUT:
			gSVM_viewMode[findViewMode].bclutAddr = memPos;
			memPos += gSVM_lutSize;
			break;
		default:
			gSVM_viewMode[findViewMode].bclutAddr = gSVM_viewMode[setBCLUT].bclutAddr;
			gSVM_viewMode[findViewMode].refViewMode[eSVM_REFERENCE_VIEWMODE_BC_LUT] = setBCLUT;
			break;
		}

		switch (setCarImage)
		{
		case SVM_UNUSE_CAR:
			break;
		case SVM_USE_CAR:
			gSVM_viewMode[findViewMode].car.addr = memPos;
			memPos += carMemSize;
			break;
			break;
		default:
			gSVM_viewMode[findViewMode].car.addr = gSVM_viewMode[setCarImage].car.addr;
			gSVM_viewMode[findViewMode].refViewMode[eSVM_REFERENCE_VIEWMODE_CAR] = setCarImage;
			break;
		}

		gSVM_viewMode[findViewMode].memSize = memPos - gSVM_viewMode[findViewMode].baseAddr;

		gSVM_viewMode[findViewMode].outputMode = outputMode;

		gSVM_viewMode[findViewMode].isEnable = TRUE;

#if (0)//def __FPGA
		{
			int j;

			/* compression */
			gSVM_viewMode[findViewMode].compression.isEnable = FALSE;
			gSVM_viewMode[findViewMode].compression.ratio = eSVM_COMPRESSION_RATIO_HALF;

			if ( outputMode >= eSVM_OUTPUTMODE_LUT_HALF_BLEND )	// Temporary if
			{
				/* hscale */
				for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
				{
					gSVM_viewMode[findViewMode].boundary[i].area[eSVM_BOUNDARYAEAR_0].startYLine = 0;
					gSVM_viewMode[findViewMode].boundary[i].area[eSVM_BOUNDARYAEAR_1].startYLine = 200;
					gSVM_viewMode[findViewMode].boundary[i].area[eSVM_BOUNDARYAEAR_2].startYLine = 300;
					gSVM_viewMode[findViewMode].boundary[i].area[eSVM_BOUNDARYAEAR_3].startYLine = 500;

					for ( j = 0; j < eSVM_BOUNDARYAEAR_MAX; j++ )
					{
						gSVM_viewMode[findViewMode].boundary[i].area[j].scale = eSVM_BOUNDARYSCALE_ONE_OVER_TWO;
					}
				}

				/* vscale */
				for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
				{
					gSVM_viewMode[findViewMode].vscale[i] = TRUE;
				}
			}
		}
#endif

		gSVM_viewModeCount++;
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_deleteAllViewMode

 * param void				:

 * return void				:

 * brief
	Deleate all view mode.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_deleteAllViewMode(void)
{
	SVM_initViewMode();
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_initialize

 * param inputWidth			:	input width of camera
 * param inputHeight		:	input height of camera
 * param outputWidth		:	output width of svm
 * param outputWidth		:	output height of svm
 * param samplingCount		: 	distance for down-sampling
 * param inputFrameCount	: 	the frame count of the memory in which to store the input data.

 * return					:	Error code

 * brief
	Initializes the SVM(Surround View Monitor).
	Memory size of each channel = inputWidth * inputHeight * pixel size of yuv422 format * inputFrameCount
***************************************************************************************************************************************************************/
int32 APIENTRY SVMAPI_initialize(uint16 IN inputWidth, uint16 IN inputHeight, uint16 IN outputWidth, uint16 IN outputHeight, uint8 IN samplingCount, float IN inputFrameCount)
{
	int32 ret;
	uint16 outLineDaly = 0, pipLineDelay = 0;
	uint16 hblank = 0, vblank = 0;
	uint16 holdTimeNoScale = 0, holdTimeOneOverTwoScale = 0, holdTimeOneOverFourScale = 0, holdTimeOneOverEightScale = 0;
	uint16 inputLine = 0;
	SVM_SIZE_T lut;

	if ( gSVM_isInit ) return eSUCCESS;

	ret = SVM_setResolution(inputWidth, inputHeight, outputWidth, outputHeight, samplingCount);
	if ( ret != eSUCCESS ) return eSUCCESS;

	gSVM_inputSize.width = inputWidth;
	gSVM_inputSize.height = inputHeight;
	gSVM_outputSize.width = outputWidth;
	gSVM_outputSize.height = outputHeight;
	gSVM_samplingCnt = samplingCount;

	SVM_initMemory(inputWidth, inputHeight, inputFrameCount);
	SVM_initViewMode();

	lut.width = gSVM_inputSize.width / gSVM_samplingCnt + 1;
	lut.height = gSVM_inputSize.height / gSVM_samplingCnt + 1;
	if ( lut.width % 2 != 0 ) lut.width++;
	gSVM_lutSize = lut.width * lut.height * 4;
	gSVM_lutSize = ((gSVM_lutSize + 7) >> 3) << 3;	// 8byte align

	/* wrapping line */
	inputLine = inputHeight * inputFrameCount;
	inputLine = inputLine & utilBitMask(13);
	_REG(SVM_CTRL_REG->wline0, (inputLine << 0) | (inputLine << 16));
	_REG(SVM_CTRL_REG->wline1, (inputLine << 0) | (inputLine << 16));

#ifdef __FPGA
	/* burst size of write main */
	SVMAPI_setInputBurstLength(eSVM_BURST_LENGTH_64);

	/* burst size of read main */
	SVMAPI_setOutputBurstLength(eSVM_BURST_LENGTH_8);

	/* burst size of car */
	SVMAPI_setCarReadBurstLength(eSVM_BURST_LENGTH_64);

	/* sync	*/
	outLineDaly = 0;
	pipLineDelay = 19;
	SVMAPI_setSync(pipLineDelay, outLineDaly);
	_REG(SVM_CTRL_REG->syncCtrl, utilPutBits(SVM_CTRL_REG->syncCtrl, 24, 8, 128));

	/* output frame rate : 15fps */
	hblank = 2532;
	vblank = 60;
	SVMAPI_setBlank(hblank, vblank);

	/* invalid lut area to valid lut area*/
	//_REG(SVM_CTRL_REG->outMode, utilPutBit(SVM_CTRL_REG->outMode, 6, TRUE));

	/* write hold time for scale down case (boundary) */
	holdTimeNoScale = 300;
	holdTimeOneOverTwoScale = 600;
	holdTimeOneOverFourScale = 1200;
	holdTimeOneOverEightScale = 1500;
	SVMAPI_setHoldTime(holdTimeNoScale, holdTimeOneOverTwoScale, holdTimeOneOverFourScale, holdTimeOneOverEightScale);

	_REG(SVM_CTRL_REG->enable, utilSetBit(SVM_CTRL_REG->enable, 2));
#endif

	SVMAPI_setMaskColor(0x0, 0x0);
	SVMAPI_setBGColor(0x0, 0x80, 0x80);

	gSVM_isInit = TRUE;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_deInitialize

 * param 					:	void

 * return					:	void

 * brief
	Deinitializes the SVM(Surround View Monitor).
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_deInitialize(void)
{
	SVMAPI_deleteAllViewMode();

	gSVM_viewModeCount = 0;
	gSVM_curViewMode = -1;
	gSVM_isInit = FALSE;
}
