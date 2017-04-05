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
#include "error.h"
#include "utils.h"
#include "svm_register.h"
#include "svm_api.h"

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/
#define BASE_ADDR_DRAM_MEM				(0x20000000U)
#define BASE_MEMORY_SVM					(BASE_ADDR_DRAM_MEM + 0x0F000000U)		// TBD


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

typedef struct tagSVM_MEM_T
{
	uint16 wrappingLine;

	SVM_MEMINFO_T channel[eSVM_CHANNEL_MAX];							// 4channel

	pSVM_MEMINFO_T pFBLUT[eSVM_PAGEFLIPPING_NUMBER_MAX];				// 2 array for page flipping * viewMode count
	pSVM_MEMINFO_T pLRLUT[eSVM_PAGEFLIPPING_NUMBER_MAX];				// 2 array for page flipping * viewMode count

	pSVM_MEMINFO_T pBCLUT;												// viewMode count

	pSVM_MEMINFO_T pCar;												// viewMode count
} SVM_MEM_T, *pSVM_MEM_T;

/***************************************************************************************************************************************************************
 * Global Variable
***************************************************************************************************************************************************************/
static bool gSVM_isInit = FALSE;

static SVM_MEM_T gSVM_mem;

static SVM_SIZE_T gSVM_inputSize;
static SVM_SIZE_T gSVM_outputSize;

bool gSVM_isFBLUTAddr4Morphing = FALSE;
bool gSVM_isLRLUTAddr4Morphing = FALSE;

static uint8 gSVM_viewModeCount = 0;
static int8 gSVM_curViewMode = -1;
static pSVM_VIEWMODE_T gSVM_viewMode;

static bool gSVM_isInput = FALSE;
static bool gSVM_isOutput = FALSE;


/*======================================================================================================================
 = Local function
======================================================================================================================*/

/*
 * function				:	SVM_setResolution

 * param inputWidth		:	input width
 * param inputHeight	:	input height
 * param outputWidth	:	output width
 * param outputHeight	:	output height
 * param distance		:	distance for down-sampling

 * return				:	error code

 * brief
	Deallocates a block of memory. (LUT memory info, Car memory info)
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
		return eERROR_INVALIED_ARGUMENT;
	}

	sOffset = inputWidth % distance;
	tOffset = inputHeight % distance;

	tXCnt = ((inputWidth + sOffset) / 2 / distance * 2) + 1;
	tYCnt = ((inputHeight + tOffset) / 2 / distance * 2) + 1 - 1;

	incr = 32768 / distance;
	dstw = distance - 1;

	hscl = (inputWidth / outputWidth) * 256;
	vscl = (inputHeight / outputHeight) * 256;

	SVM_CTRL_REG->inputSize = (inputWidth & utilBitMask(11)) | ((inputHeight & utilBitMask(11)) << 16);
	SVM_CTRL_REG->outputSize = (inputWidth & utilBitMask(11)) | ((inputHeight & utilBitMask(11)) << 16);
	SVM_CTRL_REG->samplingCnt = (tXCnt & utilBitMask(8)) | ((tYCnt & utilBitMask(8)) << 8) | ((sOffset & utilBitMask(8)) << 16) | ((tOffset & utilBitMask(8)) << 24);
	SVM_CTRL_REG->samplingDistance = (dstw & utilBitMask(8)) | ((incr & utilBitMask(16)) << 16);
	SVM_CTRL_REG->sizeRatio = (hscl & utilBitMask(10)) | ((vscl & utilBitMask(10)) << 16);

	return eSUCCESS;
}

/*
 * function				:	SVM_clearMemory

 * param				:	void

 * return				:	void

 * brief
	Deallocates a block of memory. (LUT memory info, Car memory info)
*/
static void SVM_clearMemory(void)
{
	int i = 0;

	for ( i = 0; i < 2; i++ )
	{
		if ( gSVM_mem.pFBLUT[i] )
		{
			free(gSVM_mem.pFBLUT[i]);
			gSVM_mem.pFBLUT[i] = NULL;
		}

		if ( gSVM_mem.pLRLUT[i] )
		{
			free(gSVM_mem.pLRLUT[i]);
			gSVM_mem.pLRLUT[i] = NULL;
		}
	}

	if ( gSVM_mem.pBCLUT )
	{
		free(gSVM_mem.pBCLUT);
		gSVM_mem.pBCLUT = NULL;
	}

	if ( gSVM_mem.pCar )
	{
		free(gSVM_mem.pCar);
		gSVM_mem.pCar = NULL;
	}
}

/*
 * function				:	SVM_initMemory

 * param baseWidth		:	width for memory size
 * param baseHeight		:	height for memory size
 * param samplingCount	:	distance for sampling
 * param viewModeCnt	:	number of viewmode

 * return				:	error code

 * brief
	Allocates a block of memory. (LUTs memory info, Cars memory info)
	Setup the memory map of the svm.
	baseWidth x baseHeight : In order to use a fixed memory map should use the 1920x1080.
*/
static int8 SVM_initMemory(uint16 baseWidth, uint16 baseHeight, uint8 samplingCount, uint8 viewModeCnt)
{
	int i = 0, j = 0;
	uint16 wrappingSize;
	uint32 channelMemSize, lutMemSize, carMemSize;
	uint32 memPos = BASE_MEMORY_SVM;

	for ( i = 0; i < 2; i++ )
	{
		// FB LUT
		gSVM_mem.pFBLUT[i] = (pSVM_MEMINFO_T)malloc(sizeof(SVM_MEMINFO_T) * viewModeCnt);
		if ( !gSVM_mem.pFBLUT[i] ) goto Err;

		// LR LUT
		gSVM_mem.pLRLUT[i] = (pSVM_MEMINFO_T)malloc(sizeof(SVM_MEMINFO_T) * viewModeCnt);
		if ( !gSVM_mem.pLRLUT[i] ) goto Err;
	}

	// BC LUT
	gSVM_mem.pBCLUT = (pSVM_MEMINFO_T)malloc(sizeof(SVM_MEMINFO_T) * viewModeCnt);
	if ( !gSVM_mem.pBCLUT ) goto Err;

	gSVM_mem.pCar = (pSVM_MEMINFO_T)malloc(sizeof(SVM_MEMINFO_T) * viewModeCnt);
	if ( !gSVM_mem.pCar ) goto Err;

	/*
	 * Input channel memory size	: maxWidth * maxHight * yuv data size * (2frame + wrapping area) = 1920 * 1080 * 2 * (2 + 0.5) = 0x009E3400
	 * LUT size						: (maxWidth / distance + 1) * (maxHeight / distance + 1) = (1920 / 8 + 1) * (1080 / 8 + 1) =  0x04F1A000
	 * Car image size				: (maxWidth / 4) * (maxHiehgt / 4) * yuv data size = (1920 / 4) * (1080 / 4) * 2 = 0x0003F480
	 * Each item * viewMode Count
	 * LUT item * 2 (pageflipping)
	*/
	gSVM_mem.wrappingLine = (uint16)(baseHeight / 2);
	wrappingSize = gSVM_mem.wrappingLine * baseWidth;
	channelMemSize = (baseWidth * baseHeight * 2 * 2) + wrappingSize;
	lutMemSize = (baseWidth / samplingCount + 1) * (baseHeight / samplingCount + 1);
	carMemSize = (baseWidth / 4) * (baseHeight / 4) * 2;

	memPos = BASE_MEMORY_SVM;

	for ( i = 0; i < 4; i++ )
	{
		gSVM_mem.channel[i].size = channelMemSize;
		gSVM_mem.channel[i].addr = memPos;
		memPos += channelMemSize;

		SVM_CTRL_REG->camWmainAddr[i] = gSVM_mem.channel[i].addr;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < viewModeCnt; j++ )
		{
			gSVM_mem.pFBLUT[i][j].size = lutMemSize;
			gSVM_mem.pFBLUT[i][j].addr = memPos;
			memPos += lutMemSize;
		}
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < viewModeCnt; j++ )
		{
			gSVM_mem.pLRLUT[i][j].size = lutMemSize;
			gSVM_mem.pLRLUT[i][j].addr = memPos;
			memPos += lutMemSize;
		}
	}

	for ( j = 0; j < viewModeCnt; j++ )
	{
		gSVM_mem.pBCLUT[i].size = carMemSize;
		gSVM_mem.pBCLUT[i].addr = memPos;
		memPos += carMemSize;
	}

	for ( j = 0; j < viewModeCnt; j++ )
	{
		gSVM_mem.pCar[i].size = carMemSize;
		gSVM_mem.pCar[i].addr = memPos;
		memPos += carMemSize;
	}

	return eSUCCESS;

Err:
	SVM_clearMemory();

	return eERROR_MEM_ALLOC;
}

/*
 * function				:	SVM_initViewMode

 * param inputWidth		:	input width of camera
 * param inputHeight	:	input height of camera
 * param outputWidth	:	output width of svm
 * param outputHeight	:	output height of svm

 * return				:	error code

 * brief
	Initialize view modes. (ref. structure SVM_VIEWMODE_T)
*/
static int8 SVM_initViewMode(uint16 inputWidth, uint16 inputHeight, uint16 outputWidth, uint16 outputHeight)
{
	int i = 0, viewIndex = 0, camIndex = 0;

	if ( gSVM_viewMode != NULL )
	{
		free(gSVM_viewMode);
		gSVM_viewMode = NULL;
	}

	if ( gSVM_viewModeCount == 0 )
		return eERROR_SVM_VIEWMODE_COUNT_ZERO;

	gSVM_viewMode = (pSVM_VIEWMODE_T)malloc(sizeof(SVM_VIEWMODE_T) * gSVM_viewModeCount);
	if ( gSVM_viewMode == NULL )
		return eERROR_MEM_ALLOC;

	gSVM_isFBLUTAddr4Morphing = FALSE;
	gSVM_isLRLUTAddr4Morphing = FALSE;

	for ( viewIndex = 0; viewIndex < gSVM_viewModeCount; viewIndex++ )
	{
		gSVM_viewMode[viewIndex].pageNum4FBLUT = eSVM_PAGEFLIPPING_NUMBER_0;
		gSVM_viewMode[viewIndex].pageNum4LRLUT = eSVM_PAGEFLIPPING_NUMBER_0;

		gSVM_viewMode[viewIndex].lutAddr[eSVM_MAINPART_FB] = gSVM_mem.pFBLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewIndex].addr;
		gSVM_viewMode[viewIndex].lutAddr[eSVM_MAINPART_LR] = gSVM_mem.pLRLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewIndex].addr;

		gSVM_viewMode[viewIndex].bclutAddr = gSVM_mem.pBCLUT[viewIndex].addr;

		gSVM_viewMode[viewIndex].car.isEnable = FALSE;
		gSVM_viewMode[viewIndex].car.addr = gSVM_mem.pCar[viewIndex].addr;
		gSVM_viewMode[viewIndex].car.rect.x = 0;
		gSVM_viewMode[viewIndex].car.rect.y = 0;
		gSVM_viewMode[viewIndex].car.rect.width = 300;
		gSVM_viewMode[viewIndex].car.rect.height = 300;
		gSVM_viewMode[viewIndex].car.maskColor = 0;

		gSVM_viewMode[viewIndex].compression.isEnable = TRUE;
		gSVM_viewMode[viewIndex].compression.ratio = eSVM_COMPRESSION_RATIO_HALF;

		for ( camIndex = 0; camIndex < eSVM_CHANNEL_MAX; camIndex++ )
		{
			gSVM_viewMode[viewIndex].roi.channel[camIndex].startYLine = 0;
			gSVM_viewMode[viewIndex].roi.channel[camIndex].endYLine = inputHeight - 1;
			gSVM_viewMode[viewIndex].roi.isEnable[camIndex] = FALSE;
		}

		for ( camIndex = 0; camIndex < eSVM_CHANNEL_MAX; camIndex++ )
		{
			for ( i = 0; i < eSVM_BOUNDARYAEAR_MAX; i++ )
			{
				gSVM_viewMode[viewIndex].boundary.channel[camIndex].area[i].startYLine = inputHeight * i / 4;
				gSVM_viewMode[viewIndex].boundary.channel[camIndex].area[i].scale = eSVM_BOUNDARYSCALE_ONE + i;
			}
			gSVM_viewMode[viewIndex].boundary.isEnable[camIndex] = FALSE;
		}

		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			gSVM_viewMode[viewIndex].section[i].rect.x = 0;
			gSVM_viewMode[viewIndex].section[i].rect.y = 0;
			gSVM_viewMode[viewIndex].section[i].rect.width = outputWidth ;
			gSVM_viewMode[viewIndex].section[i].rect.height = outputHeight;
			gSVM_viewMode[viewIndex].section[i].isEnable = FALSE;
		}
	}

	return eSUCCESS;
}

/*
 * function				:	SVM_ycbcrTorgb

 * param y				:	input y
 * param cb				:	input Cb
 * param cr				:	input Cr
 * param *r				:	output pointer of red color
 * param *g				:	output pointer of green color
 * param *b				:	output pointer of blue color

 * return				:	void

 * brief
	YCbCr color to RGB color.
*/
static void SVM_ycbcrTorgb(uint8 y, uint8 cb, uint8 cr, uint8 *r, uint8 *g, uint8 *b)
{
	uint8 coefCb4G, coefCr4G;
	uint16 coefCr4R, coefCb4B;
	bool isWith128 = utilGetBit(SVM_COLOR_COEF_REG->r2y4y, 24);

	coefCb4G = utilGetBits(SVM_COLOR_COEF_REG->y2r4g, 0, 8);
	coefCr4G = utilGetBits(SVM_COLOR_COEF_REG->y2r4g, 8, 8);
	coefCr4R = utilGetBits(SVM_COLOR_COEF_REG->y2r4rnb, 0, 10);
	coefCb4B = utilGetBits(SVM_COLOR_COEF_REG->y2r4rnb, 16, 10);

	if ( isWith128 )
	{
		*r = (uint8)(y + (coefCr4R / 256.f) * (cr - 128));
		*g = (uint8)(y - (coefCb4G / 256.f) * (cb - 128) - (coefCr4G / 256.f) * (cr - 128));
		*b = (uint8)(y + (coefCb4B / 256.f) * (cb - 128));
	}
	else
	{
		*r = (uint8)(y + (coefCr4R / 256.f) * cr);
		*g = (uint8)(y - (coefCb4G / 256.f) * cb - (coefCr4G / 256.f) * cr);
		*b = (uint8)(y + (coefCb4B / 256.f) * cb);
	}
}

/*
 * function				:	SVM_rgbToycbcr

 * param r				:	input red color
 * param g				:	input green color
 * param b				:	input blue color
 * param *y				:	output pointer of y
 * param *cb			:	output pointer of Cb
 * param *cr			:	output pointer of Cr

 * return				:	void

 * brief
	RGB color to YCbCr color.
*/
static void SVM_rgbToycbcr(uint8 r, uint8 g, uint8 b, uint8 *y, uint8 *cb, uint8 *cr)
{
	uint8 coefR4Y, coefG4Y, coefB4Y;
	uint8 coefR4Cb, coefG4Cb, coefB4Cb;
	uint8 coefR4Cr, coefG4Cr, coefB4Cr;
	bool isWith128 = utilGetBit(SVM_COLOR_COEF_REG->r2y4y, 24);

	coefR4Y = utilGetBits(SVM_COLOR_COEF_REG->r2y4y, 0, 8);
	coefG4Y = utilGetBits(SVM_COLOR_COEF_REG->r2y4y, 8, 8);
	coefB4Y = utilGetBits(SVM_COLOR_COEF_REG->r2y4y, 16, 8);

	coefR4Cb = utilGetBits(SVM_COLOR_COEF_REG->r2y4cb, 0, 8);
	coefG4Cb = utilGetBits(SVM_COLOR_COEF_REG->r2y4cb, 8, 8);
	coefB4Cb = utilGetBits(SVM_COLOR_COEF_REG->r2y4cb, 16, 8);

	coefR4Cr = utilGetBits(SVM_COLOR_COEF_REG->r2y4cr, 0, 8);
	coefG4Cr = utilGetBits(SVM_COLOR_COEF_REG->r2y4cr, 8, 8);
	coefB4Cr = utilGetBits(SVM_COLOR_COEF_REG->r2y4cr, 16, 8);

	if ( isWith128 )
	{
		*y = (uint8)(((coefR4Y / 256.f) * r) + ((coefG4Y / 256.f) * g) + ((coefB4Y / 256.f) * b));
		*cb = (uint8)(((-coefR4Cb / 256.f) * r) + ((-coefG4Cb / 256.f) * g) + ((coefB4Cb / 256.f) * b) + 128);
		*cr = (uint8)(((coefR4Cr / 256.f) * r) + ((-coefG4Cr / 256.f) * g) + ((-coefB4Cr / 256.f) * b) + 128);
	}
	else
	{
		*y = (uint8)(((coefR4Y / 256.f) * r) + ((coefG4Y / 256.f) * g) + ((coefB4Y / 256.f) * b));
		*cb = (uint8)(((-coefR4Cb / 256.f) * r) + ((-coefG4Cb / 256.f) * g) + ((coefB4Cb / 256.f) * b));
		*cr = (uint8)(((coefR4Cr / 256.f) * r) + ((-coefG4Cr / 256.f) * g) + ((-coefB4Cr / 256.f) * b));
	}
}

/*======================================================================================================================
 = Export function
======================================================================================================================*/

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setOutputMode

 * param mode				:	output mode (ref. enum SVM_OUTPUTMODE_E)

 * return					:	error code

 * brief
	Sets the outputmode of the SVM.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setOutputMode(uint8 IN mode)
{
	int8 ret = eSUCCESS;
	uint8 inBurst = 0x0, outBurst = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( mode >= eSVM_OUTPUTMODE_MAX )
		return eERROR_INVALIED_ARGUMENT;

	inBurst = utilGetBits(SVM_CTRL_REG->wmain, 4, 4);
	outBurst = utilGetBits(SVM_CTRL_REG->rmain, 4, 4);

	switch (mode)
	{
	case eSVM_OUTPUTMODE_BYPASS_FRONT:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x0 << 8) | (0x0 << 9) | (0x0 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x0 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x1);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x0);
		break;
	case eSVM_OUTPUTMODE_BYPASS_LEFT:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x0 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x0 << 9) | (0x0 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x0 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x4);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x1);
		break;
	case eSVM_OUTPUTMODE_BYPASS_RIGHT:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x0 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x0 << 8) | (0x1 << 9) | (0x0 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x0 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0xC);
		SVM_CTRL_REG->outMode =  utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x1);
		break;
	case eSVM_OUTPUTMODE_BYPASS_BACK:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x0 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x0 << 8) | (0x0 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x0 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x3);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x0);
		break;
	case eSVM_OUTPUTMODE_BLEND_HALF_ONLY:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x82);
		break;
	case eSVM_OUTPUTMODE_ALPHA:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x8A);
		break;
	case eSVM_OUTPUTMODE_LUT_ALPHA:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0x8E);
		break;
	case eSVM_OUTPUTMODE_LUT_ALPHA_BC:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0xDE);
		break;
	case eSVM_OUTPUTMODE_LUT_ALPHA_BC_COEF:
		if ( gSVM_isInput )
			SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((inBurst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
		if ( gSVM_isOutput )
			SVM_CTRL_REG->rmain = (0x1 << 0) | ((outBurst & utilBitMask(4)) << 4) | (0x1 << 8);
		SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 0, 4, 0x0);
		SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 0, 8, 0xFE);
		break;
	}

	return ret;
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
int8 APIENTRY SVMAPI_setChannelFilpping(uint8 IN channel, bool IN isHorizontal, bool IN isVertical)
{
	uint8 pos = 0x0;
	uint8 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( channel >= eSVM_CHANNEL_MAX )
		return eERROR_INVALIED_ARGUMENT;

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

	SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, pos, 2, value);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateCoefOfBC

 * param coefBC				:	coefficient information of brightness control

 * return					:	error code

 * brief
	Update coefficient of brightness control.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_updateCoefOfBC(pSVM_BCCOEF_T IN coefBC)
{
	int i = 0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( coefBC == NULL )
		return eERROR_INVALIED_ARGUMENT;

	for ( i = 0; i < eSVM_COLOR_MAX; i++ )
	{
		SVM_COEF_REG->coef1[i] = (coefBC->coef1[i][eSVM_SUBPART_FL]) | (coefBC->coef1[i][eSVM_SUBPART_FR] << 8) | (coefBC->coef1[i][eSVM_SUBPART_BR] << 16) | (coefBC->coef1[i][eSVM_SUBPART_BL] << 24);
		SVM_COEF_REG->coef3[i] = (coefBC->coef3[i][eSVM_SUBPART_FL]) | (coefBC->coef3[i][eSVM_SUBPART_FR] << 8) | (coefBC->coef3[i][eSVM_SUBPART_BR] << 16) | (coefBC->coef3[i][eSVM_SUBPART_BL] << 24);
	}

	for ( i = 0; i < eSVM_COLOR_MAX; i++ )
	{
		SVM_COEF_REG->coef2n4[i] = (coefBC->coef2[i][eSVM_FB_PART_F]) | (coefBC->coef2[i][eSVM_FB_PART_B] << 8) | (coefBC->coef4[i][eSVM_LR_PART_L] << 16) | (coefBC->coef4[i][eSVM_LR_PART_R] << 24);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_updateRatioOfBC

 * param ratio				:	ratio of brightness control

 * return					:	void

 * brief
	Update ratio of brightness control.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_updateRatioOfBC(uint8 IN ratio)
{
	SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 8, 8, ratio);
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
int8 APIENTRY SVMAPI_updateFBLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size)
{
	uint32 page0Addr = 0x0, page1Addr = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size > gSVM_mem.pFBLUT[gSVM_viewMode[viewMode].pageNum4FBLUT][viewMode].size )
		return eERROR_INVALIED_ARGUMENT;

	page0Addr = gSVM_mem.pFBLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewMode].addr;
	page1Addr = gSVM_mem.pFBLUT[eSVM_PAGEFLIPPING_NUMBER_1][viewMode].addr;

	if ( gSVM_viewMode[viewMode].pageNum4FBLUT == eSVM_PAGEFLIPPING_NUMBER_0 )		// page0 -> page1
	{
		memcpy((void*)page1Addr, lutBin, size);		// TBD
		gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_FB] = page1Addr;
		gSVM_mem.pFBLUT[eSVM_PAGEFLIPPING_NUMBER_1][viewMode].size = size;
	}
	else																			// page1 -> page0
	{
		memcpy((void*)page0Addr, lutBin, size);		// TBD
		gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_FB] = page0Addr;
		gSVM_mem.pFBLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewMode].size = size;
	}

	if ( gSVM_curViewMode == viewMode )
	{
		uint32 viewModeAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_FB];
		if ( gSVM_isFBLUTAddr4Morphing )
		{
			SVM_CTRL_REG->fbLutAddr[1] = viewModeAddr;
		}
		else
		{
			SVM_CTRL_REG->fbLutAddr[0] = viewModeAddr;
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
int8 APIENTRY SVMAPI_updateLRLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size)
{
	uint32 page0Addr = 0x0, page1Addr = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size > gSVM_mem.pLRLUT[gSVM_viewMode[viewMode].pageNum4FBLUT][viewMode].size )
		return eERROR_INVALIED_ARGUMENT;

	page0Addr = gSVM_mem.pLRLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewMode].addr;
	page1Addr = gSVM_mem.pLRLUT[eSVM_PAGEFLIPPING_NUMBER_1][viewMode].addr;

	if ( gSVM_viewMode[viewMode].pageNum4LRLUT == eSVM_PAGEFLIPPING_NUMBER_0 )		// page0 -> page1
	{
		memcpy((void*)page1Addr, lutBin, size);		// TBD
		gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_LR] = page1Addr;
		gSVM_mem.pLRLUT[eSVM_PAGEFLIPPING_NUMBER_1][viewMode].size = size;
	}
	else																			// page1 -> page0
	{
		memcpy((void*)page0Addr, lutBin, size);		// TBD
		gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_LR] = page0Addr;
		gSVM_mem.pLRLUT[eSVM_PAGEFLIPPING_NUMBER_0][viewMode].size = size;
	}

	if ( gSVM_curViewMode == viewMode )
	{
		uint32 viewModeAddr = gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_LR];
		if ( gSVM_isLRLUTAddr4Morphing )
		{
			SVM_CTRL_REG->lrLutAddr[1] = viewModeAddr;
		}
		else
		{
			SVM_CTRL_REG->lrLutAddr[0] = viewModeAddr;
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
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_updateBCLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( lutBin == NULL || viewMode >= gSVM_viewModeCount || size > gSVM_mem.pBCLUT[viewMode].size )
		return eERROR_INVALIED_ARGUMENT;

	memcpy((void*)gSVM_mem.pBCLUT[viewMode].addr, lutBin, size);		// TBD

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBGColor

 * param r					:	red color
 * param g					:	green color
 * param b					:	blue color

 * return					:	void

 * brief
	Sets the background color of the SVM.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setBGColor(uint8 IN r, uint8 IN g, uint8 IN b)
{
	uint8 y, cb, cr;

	SVM_rgbToycbcr(r, g, b, &y, &cb, &cr);
	SVM_CTRL_REG->bgColor = (cr) || (cb << 8) || (y <<16);
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setMaskColor

 * param r					:	red color
 * param g					:	green color
 * param b					:	blue color

 * return					:	void

 * brief
	Sets the maskcolor of the SVM.
	Maskcolor is used in an invalid lut areas.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setMaskColor(uint8 IN r, uint8 IN g, uint8 IN b)
{
	uint8 y, cb, cr;

	SVM_rgbToycbcr(r, g, b, &y, &cb, &cr);
	SVM_CTRL_REG->maskColor = (cr) || (cb << 8) || (y <<16);
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCoefColor

 * param coefColor			:	information of color coefficient

 * return					:	void

 * brief
	Sets the color coefficient of the SVM.
	yuv to rgb or rgb to yuv
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_setCoefColor(pSVM_COLORCOEF_T IN coefColor)
{
	SVM_COLOR_COEF_REG->r2y4y = (coefColor->r2y.r4y) | (coefColor->r2y.g4y << 8) | (coefColor->r2y.b4y << 16) | (coefColor->isWith128 << 24);
	SVM_COLOR_COEF_REG->r2y4cb = (coefColor->r2y.r4cb) | (coefColor->r2y.g4cb << 8) | (coefColor->r2y.b4cb << 16);
	SVM_COLOR_COEF_REG->r2y4cr = (coefColor->r2y.r4cr) | (coefColor->r2y.g4cr << 8) | (coefColor->r2y.b4cr << 16);

	SVM_COLOR_COEF_REG->y2r4g = (coefColor->y2r.cb4g) | (coefColor->y2r.cr4g << 8);
	SVM_COLOR_COEF_REG->y2r4rnb = (coefColor->y2r.cr4r & utilBitMask(9)) | ((coefColor->y2r.cb4b & utilBitMask(9)) << 16);
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCar

 * param viewMode			:	number of viewmode
 * param rect				:	position for drawing cars
 * param maskColorR			:	red of maskColor
 * param maskColorG			:	green of maskColor
 * param maskColorB			:	blue of maskColor

 * return					:	error code

 * brief
	Sets the car model of the SVM.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setCar(uint8 IN viewMode, SVM_RECT_T IN rect, uint8 IN maskColorR, uint8 IN maskColorG, uint8 IN maskColorB)
{
	uint8 maskColorY, maskColorCb, maskColorCr;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].car.rect = rect;

	SVM_rgbToycbcr(maskColorR, maskColorR, maskColorR, &maskColorY, &maskColorCb, &maskColorCr);
	gSVM_viewMode[viewMode].car.maskColor = (maskColorCr) || (maskColorCb << 8) || (maskColorY <<16);

	if ( gSVM_curViewMode == viewMode )
	{
		SVM_CTRL_REG->carCtrl = gSVM_viewMode[viewMode].car.maskColor & utilBitMask(24);
		SVM_CTRL_REG->carX = (rect.x & utilBitMask(11)) | (((rect.width - 1) & utilBitMask(9)) << 16);
		SVM_CTRL_REG->carY = (rect.y & utilBitMask(11)) | (((rect.height - 1) & utilBitMask(9)) << 16);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setCarEable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the car object.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setCarEnable(uint8 IN viewMode, bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].car.isEnable = isEnable;
	if ( gSVM_curViewMode == viewMode )
		SVM_CTRL_REG->carY = utilPutBit(SVM_CTRL_REG->carY, 31, isEnable);

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
int8 APIENTRY SVMAPI_setCompression(uint8 IN viewMode, uint8 IN ratio)
{
	int i = 0;
	uint32 pos = 0x0;
	uint32 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount || ratio > eSVM_COMPRESSION_RATIO_QUARTER )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].compression.ratio = ratio;

	if ( gSVM_curViewMode == viewMode )
	{
		value = SVM_CTRL_REG->compression;

		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			pos = (i * 0x2) + 0x1;
			value = utilPutBit(value, pos, ratio);
		}

		SVM_CTRL_REG->compression = value;
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
int8 APIENTRY SVMAPI_setCompressionEnable(uint8 IN viewMode, bool IN isEnable)
{
	int i = 0;
	uint32 pos = 0x0;
	uint32 value = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount )
		return eERROR_INVALIED_ARGUMENT;

	if ( gSVM_curViewMode == viewMode )
	{
		value = SVM_CTRL_REG->compression;

		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			pos = i * 0x2;
			value = utilPutBit(value, pos, isEnable);
		}

		SVM_CTRL_REG->compression = value;
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
int8 APIENTRY SVMAPI_setROI(uint8 IN viewMode, uint8 IN channel, SVM_CHANNEL_ROI_T IN roi)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount || channel >= eSVM_CHANNEL_MAX ||
		 roi.startYLine >= gSVM_inputSize.height || roi.endYLine >= gSVM_inputSize.height || roi.startYLine >= roi.endYLine )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].roi.channel[channel].startYLine = roi.startYLine;
	gSVM_viewMode[viewMode].roi.channel[channel].endYLine = roi.endYLine;

	if ( gSVM_curViewMode == viewMode && gSVM_viewMode[viewMode].roi.isEnable[channel] )
		SVM_CTRL_REG->roi[channel] = (roi.startYLine & utilBitMask(11)) | ((roi.endYLine & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->roi[channel], 31) << 31);

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
int8 APIENTRY SVMAPI_setROIEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( viewMode >= gSVM_viewModeCount || channel >= eSVM_CHANNEL_MAX )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].roi.isEnable[channel] = isEnable;

	if ( gSVM_curViewMode == viewMode )
		SVM_CTRL_REG->roi[channel] = utilPutBit(SVM_CTRL_REG->roi[channel], 31, isEnable);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBoundary

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
int8 APIENTRY SVMAPI_setBoundary(uint8 IN viewMode, uint8 IN channel, SVM_CHANNEL_BOUNDARY_T IN chBoundary)
{
	int i = 0;
	uint8 pos = 0x0;
	uint16 startY[4];

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

 	if ( viewMode >= gSVM_viewModeCount || channel >= eSVM_CHANNEL_MAX )
		return eERROR_INVALIED_ARGUMENT;

	for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
	{
		if ( chBoundary.area[i].scale >= eSVM_BOUNDARYSCALE_MAX )
			return eERROR_INVALIED_ARGUMENT;
	}

	gSVM_viewMode[viewMode].boundary.channel[channel] = chBoundary;

	if ( gSVM_curViewMode == viewMode && gSVM_viewMode[viewMode].boundary.isEnable[channel] )
	{
		/* The register set value must be a multiple of 4.*/
		for ( i = 1; i < 4; i++ )
			startY[i] = chBoundary.area[i].startYLine / 4;

		SVM_CTRL_REG->boundary[channel] = ((startY[1] & utilBitMask(9)) << 0) |
										  ((startY[2] & utilBitMask(9)) << 9) |
										  ((startY[3] & utilBitMask(9)) << 18) |
										  (utilGetBit(SVM_CTRL_REG->boundary[channel], 31) << 31);

		pos = channel * 0x8;
		SVM_CTRL_REG->boundaryScale = utilPutBits(SVM_CTRL_REG->boundaryScale, pos, 8,
												(chBoundary.area[0].scale) | (chBoundary.area[1].scale << 2) | (chBoundary.area[2].scale << 4) | (chBoundary.area[3].scale << 6));
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setBoundaryEnable

 * param viewMode			:	number of viewmode
 * param channel			:	number of camera (ref. enum SVMCTRL_CHANNEL_E)
 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the boundary function.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setBoundaryEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

 	if ( viewMode >= gSVM_viewModeCount || channel >= eSVM_CHANNEL_MAX )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].boundary.isEnable[channel] = isEnable;

	if ( gSVM_curViewMode == viewMode )
		SVM_CTRL_REG->boundary[channel] = utilPutBit(SVM_CTRL_REG->boundary[channel], 31, isEnable);

	return eSUCCESS;
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
int8 APIENTRY SVMAPI_setSection(uint8 IN viewMode, uint8 IN sectionNumber, SVM_RECT_T IN sectionRect)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

 	if ( viewMode >= gSVM_viewModeCount || sectionNumber >= eSVM_SECTION_NUMBER_MAX )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].section[sectionNumber].rect = sectionRect;

	if ( gSVM_curViewMode == viewMode )
	{
		switch (sectionNumber)
		{
		case eSVM_SECTION_NUMBER_0:
			SVM_CTRL_REG->section0X = (sectionRect.x & utilBitMask(11)) | (((sectionRect.width - 1) & utilBitMask(11)) << 16);
			SVM_CTRL_REG->section0Y = (sectionRect.y & utilBitMask(11)) | (((sectionRect.height - 1) & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->section0Y, 31) << 31);
			break;
		case eSVM_SECTION_NUMBER_1:
			SVM_CTRL_REG->section1X = (sectionRect.x & utilBitMask(11)) | (((sectionRect.width - 1) & utilBitMask(11)) << 16);
			SVM_CTRL_REG->section1Y = (sectionRect.y & utilBitMask(11)) | (((sectionRect.height - 1) & utilBitMask(11)) << 16) | (utilGetBit(SVM_CTRL_REG->section1Y, 31) << 31);
			break;
		}
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setSectionEnable

 * param viewMode			:	number of viewmode
 * param sectionNumber		:	number of section for output
 * param section			:	information of section

 * return					:	error code

 * brief
	Enable or disable of the section function.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setSectionEnable(uint8 IN viewMode, uint8 IN sectionNumber, bool IN isEnable)
{
	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

 	if ( viewMode >= gSVM_viewModeCount || sectionNumber >= eSVM_SECTION_NUMBER_MAX )
		return eERROR_INVALIED_ARGUMENT;

	gSVM_viewMode[viewMode].section[sectionNumber].isEnable = isEnable;
	if ( gSVM_curViewMode == viewMode )
	{
		switch (sectionNumber)
		{
		case eSVM_SECTION_NUMBER_0:
			SVM_CTRL_REG->section0Y = utilPutBit(SVM_CTRL_REG->section0Y, 31, isEnable);
			break;
		case eSVM_SECTION_NUMBER_1:
			SVM_CTRL_REG->section1Y = utilPutBit(SVM_CTRL_REG->section1Y, 31, isEnable);
			break;
		}
	}

	return eSUCCESS;
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
int8 APIENTRY SVMAPI_setViewMode(uint8 IN viewMode, bool IN isFBMorphing, bool IN isLRMorphing)
{
	int i = 0;
	uint32 lutAddrViewmodeFB = 0x0, lutAddrViewmodeLR = 0x0;

	if ( gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( gSVM_curViewMode == viewMode )
		return eSUCCESS;

	if ( viewMode >= gSVM_viewModeCount )
		return eERROR_INVALIED_ARGUMENT;

	if ( gSVM_curViewMode != viewMode )
	{
		gSVM_curViewMode = viewMode;

		/* Compression */
		SVMAPI_setCompression(viewMode, gSVM_viewMode[viewMode].compression.ratio);
		SVMAPI_setCompressionEnable(viewMode, gSVM_viewMode[viewMode].compression.isEnable);

		/* ROI */
		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			SVMAPI_setROI(viewMode, i, gSVM_viewMode[viewMode].roi.channel[i]);
			SVMAPI_setROIEnable(viewMode, i, gSVM_viewMode[viewMode].roi.isEnable[i]);
		}

		/* boundary */
		for ( i = 0; i < eSVM_CHANNEL_MAX; i++ )
		{
			SVMAPI_setBoundary(viewMode, i, gSVM_viewMode[viewMode].boundary.channel[i]);
			SVMAPI_setBoundaryEnable(viewMode, i, gSVM_viewMode[viewMode].boundary.isEnable[i]);
		}

		/* section */
		for ( i = 0; i < eSVM_SECTION_NUMBER_MAX; i++ )
		{
			SVMAPI_setSection(viewMode, i, gSVM_viewMode[viewMode].section[i].rect);
			SVMAPI_setSectionEnable(viewMode, i, gSVM_viewMode[viewMode].section[i].isEnable);
		}

		/* FB LUT & LR LUT & Morphing */
		lutAddrViewmodeFB = gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_FB];
		lutAddrViewmodeLR = gSVM_viewMode[viewMode].lutAddr[eSVM_MAINPART_LR];
		if ( isFBMorphing )
		{
			if ( gSVM_isFBLUTAddr4Morphing )
			{
				SVM_CTRL_REG->fbLutAddr[0] = lutAddrViewmodeFB;
				gSVM_isFBLUTAddr4Morphing = FALSE;
			}
			else
			{
				SVM_CTRL_REG->fbLutAddr[1] = lutAddrViewmodeFB;
				gSVM_isFBLUTAddr4Morphing = TRUE;
			}
		}
		else
		{
			if ( gSVM_isFBLUTAddr4Morphing )
			{
				SVM_CTRL_REG->fbLutAddr[1] = lutAddrViewmodeFB;
				gSVM_isFBLUTAddr4Morphing = TRUE;
			}
			else
			{
				SVM_CTRL_REG->fbLutAddr[0] = lutAddrViewmodeFB;
				gSVM_isFBLUTAddr4Morphing = FALSE;
			}
		}

#if (0)		// TBD
		if ( isLRMorphing )
		{
			if ( gSVM_isLRLUTAddr4Morphing )
			{
				SVM_CTRL_REG->lrLutAddr[0] = lutAddrViewmodeLR;
				gSVM_isLRLUTAddr4Morphing = FALSE;
			}
			else
			{
				SVM_CTRL_REG->lrLutAddr[1] = lutAddrViewmodeLR;
				gSVM_isLRLUTAddr4Morphing = TRUE;
			}
		}
		else
		{
			if ( gSVM_isLRLUTAddr4Morphing )
			{
				SVM_CTRL_REG->lrLutAddr[1] = lutAddrViewmodeLR;
				gSVM_isLRLUTAddr4Morphing = TRUE;
			}
			else
			{
				SVM_CTRL_REG->lrLutAddr[0] = lutAddrViewmodeLR;
				gSVM_isLRLUTAddr4Morphing = FALSE;
			}
		}
#else
		SVM_CTRL_REG->lrLutAddr[0] = lutAddrViewmodeLR;
		gSVM_isLRLUTAddr4Morphing = FALSE;
#endif

		/* LUT of brightness control */
		SVM_CTRL_REG->bcLutAddr = gSVM_viewMode[viewMode].bclutAddr;

		if ( isFBMorphing )
		{
			bool isDecrement = !gSVM_isFBLUTAddr4Morphing;
			uint8 speed = 0x80;
			uint8 ratio = 0x0;
			if ( isDecrement )
				ratio = 0xFF;
			else
				ratio = 0x0;
			SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 16, 8, ratio);
			SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 16, 5, (TRUE) | ((speed & utilBitMask(3)) << 1) || (isDecrement << 4));
		}

#if (0)		// TBD
		if ( isLRMorphing )
		{
			bool isDecrement = !gSVM_isLRLUTAddr4Morphing;
			uint8 speed = 0x80;
			uint8 ratio = 0x0;
			if ( isDecrement )
				ratio = 0xFF;
			else
				ratio = 0x0;
			SVM_CTRL_REG->chCtrl = utilPutBits(SVM_CTRL_REG->chCtrl, 24, 8, ratio);
			//SVM_CTRL_REG->outMode = utilPutBits(SVM_CTRL_REG->outMode, 16, 5, (TRUE) | ((speed & utilBitMask(3)) << 1) || (isDecrement << 4));
		}
#endif

		/* car object */
		SVM_CTRL_REG->carAddr = gSVM_viewMode[viewMode].car.addr;
		SVM_CTRL_REG->carCtrl = utilPutBits(SVM_CTRL_REG->carCtrl, 0, 24, gSVM_viewMode[viewMode].car.maskColor);
		SVM_CTRL_REG->carX = (gSVM_viewMode[viewMode].car.rect.x & utilBitMask(11)) | ((gSVM_viewMode[viewMode].car.rect.width & utilBitMask(9)) << 16);
		SVM_CTRL_REG->carY = (gSVM_viewMode[viewMode].car.rect.y & utilBitMask(11)) | ((gSVM_viewMode[viewMode].car.rect.height & utilBitMask(9)) << 16) |
							 (gSVM_viewMode[viewMode].car.isEnable << 31);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_getStatistics

 * param stats				:	porinter of statistics structur

 * return					:	void

 * brief
	Gets statistics value from the output of SVM.
***************************************************************************************************************************************************************/
void APIENTRY SVMAPI_getStatistics(pSVM_STATISTICS_T OUT stats)
{
	int i = 0, j = 0, k = 0;
	for ( i = 0; i < eSVM_MAINPART_MAX; i++ )
	{
		for ( j = 0; j < eSVM_SUBPART_MAX; j++ )
		{
			for ( k = 0; k < eSVM_COLOR_MAX; k++ )
				stats->value[i][j][eSVM_COLOR_R] = SVM_STAT_REG->stat[i][j][k] & utilBitMask(26);
		}
	}
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setInputEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the input data.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setInputEnable(bool IN isEnable)
{
	uint8 burst = 0x0;

	if ( isEnable && gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	burst = utilGetBits(SVM_CTRL_REG->wmain, 4, 4);

	if ( isEnable )
	{
		gSVM_isInput = TRUE;
		SVM_CTRL_REG->wmain = (0x1 << 0) | (0x1 << 1) | ((burst & utilBitMask(4)) << 4) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10);
	}
	else
	{
		gSVM_isInput = FALSE;
		SVM_CTRL_REG->wmain = (burst & utilBitMask(4)) << 4;
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setOutputEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the output data.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setOutputEnable(bool IN isEnable)
{
	uint8 burst = 0x0;

	if ( isEnable && gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	burst = utilGetBits(SVM_CTRL_REG->rmain, 4, 4);

	if ( isEnable )
	{
		gSVM_isOutput = TRUE;
		SVM_CTRL_REG->rmain = (0x1 << 0) | ((burst & utilBitMask(4)) << 4) | (0x1 << 8);
	}
	else
	{
		gSVM_isOutput = FALSE;
		SVM_CTRL_REG->rmain = (burst & utilBitMask(4)) << 4;
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_setEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the SVM. If set to disable, it is front bypass.
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_setSVMEnable(bool IN isEnable)
{
	if ( isEnable && gSVM_isInit == FALSE )
		return eERROR_SVM_NOT_INITIALIZE;

	if ( gSVM_curViewMode < 0 )
		return eERROR_SVM_VIEWMODE_NOT_SELECTED;

	SVM_CTRL_REG->enable = utilPutBit(SVM_CTRL_REG->enable, 0, isEnable);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	SVMAPI_initialize

 * param inputWidth			:	input width of camera
 * param inputHeight		:	input height of camera
 * param outputWidth		:	output width of svm
 * param outputWidth		:	output height of svm
 * param samplingCount		: 	distance for down-sampling
 * param viewModeCnt		:	count of viewmode

 * return					:	Error code

 * brief
	Initializes the SVM(Surround View Monitor).
***************************************************************************************************************************************************************/
int8 APIENTRY SVMAPI_initialize(uint16 IN inputWidth, uint16 IN inputHeight, uint16 IN outputWidth, uint16 IN outputHeight, uint8 IN samplingCount, uint8 IN viewModeCount)
{
	int8 ret = eSUCCESS;
	uint16 outLineDaly = 0, pipLineDelay = 0;
	uint16 hblank = 0, vblank = 0;
	uint16 holdTimeNoScale = 0, holdTimeOneOverTwoScale = 0, holdTimeOneOverFourScale = 0, holdTimeOneOverEightScale = 0;

	if ( gSVM_isInit ) goto End;

	if ( viewModeCount == 0 )
	{
		ret = eERROR_SVM_VIEWMODE_COUNT_ZERO;
		goto End;
	}

	gSVM_inputSize.width = inputWidth;
	gSVM_inputSize.height = inputHeight;
	gSVM_outputSize.width = outputWidth;
	gSVM_outputSize.height = outputHeight;

	ret = SVM_setResolution(inputWidth, inputHeight, outputWidth, outputHeight, samplingCount);
	if ( ret != eSUCCESS ) goto End;

	ret = SVM_initMemory(SVM_MAX_WIDTH, SVM_MAX_HEIGHT, samplingCount, viewModeCount);
	if ( ret != eSUCCESS ) goto End;

	ret = SVM_initViewMode(inputWidth, inputHeight, outputWidth, outputHeight);
	if ( ret != eSUCCESS ) goto End;

	/* burst size of write main */
	SVM_CTRL_REG->wmain = utilPutBits(SVM_CTRL_REG->wmain, 4, 4, 0x7);

	/* burst size of read main */
	SVM_CTRL_REG->rmain = utilPutBits(SVM_CTRL_REG->rmain, 4, 4, 0x4);

	/* burst size of car */
	SVM_CTRL_REG->carCtrl = utilPutBits(SVM_CTRL_REG->carCtrl, 24, 7, 64);

	/* wrapping line */
	SVM_CTRL_REG->wline0 = (gSVM_mem.wrappingLine & utilBitMask(13)) || ((gSVM_mem.wrappingLine & utilBitMask(13)) << 16);
	SVM_CTRL_REG->wline1 = (gSVM_mem.wrappingLine & utilBitMask(13)) || ((gSVM_mem.wrappingLine & utilBitMask(13)) << 16);

	/* sync	*/
	outLineDaly = 0;
	pipLineDelay = 19;
	SVM_CTRL_REG->syncCtrl = (outLineDaly & utilBitMask(16)) | ((pipLineDelay & utilBitMask(8)) << 16);

	/* output frame rate : 74.25MHz, 30fps */
	hblank = 2020;
	vblank = 30;
	SVM_CTRL_REG->blank = (hblank & utilBitMask(16)) | ((vblank & utilBitMask(16)) << 16);

	/* invalid lut area to valid lut area*/
	SVM_CTRL_REG->outMode = utilPutBit(SVM_CTRL_REG->outMode, 7, TRUE);

	/* write hold time for scale down case (boundary) */
	holdTimeNoScale = 300;
	holdTimeOneOverTwoScale = 600;
	holdTimeOneOverFourScale = 1200;
	holdTimeOneOverEightScale = 1500;
	SVM_CTRL_REG->sclDownHoldCnt[0] = (holdTimeNoScale & utilBitMask(11)) || ((holdTimeOneOverTwoScale & utilBitMask(11)) << 16);
	SVM_CTRL_REG->sclDownHoldCnt[1] = (holdTimeOneOverFourScale & utilBitMask(11)) || ((holdTimeOneOverEightScale & utilBitMask(11)) << 16);

	gSVM_viewModeCount = viewModeCount;
	gSVM_isInit = TRUE;

End:
	return ret;
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
	SVM_clearMemory();

	if ( gSVM_viewMode != NULL )
	{
		free(gSVM_viewMode);
		gSVM_viewMode = NULL;
	}

	gSVM_viewModeCount = 0;
	gSVM_curViewMode = 0;
	gSVM_isInit = FALSE;
}
