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

#ifndef __SVM_API_H__
#define __SVM_API_H__

#include "type.h"


/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/
#define SVM_USE_LUT						(0xFF)
#define SVM_UNUSE_LUT					(0xFE)
#define SVM_USE_CAR						(SVM_USE_LUT)
#define SVM_UNUSE_CAR					(SVM_UNUSE_LUT)

#define SVM_APPLY_ALL_VIEWMODE			(0xFF)

typedef void (*SVM_CALLBACK) (void);

/***************************************************************************************************************************************************************
 * Enumeration
***************************************************************************************************************************************************************/
enum tagSVM_CHANNEL_E
{
	eSVM_CHANNEL_FRONT = 0,												// cam0
	eSVM_CHANNEL_LEFT,													// cam1
	eSVM_CHANNEL_RIGHT,													// cam2
	eSVM_CHANNEL_BACK,													// cam3
	eSVM_CHANNEL_MAX,
};

enum tagSVM_BURST_LENGTH_E
{
	eSVM_BURST_LENGTH_8	= 4,
	eSVM_BURST_LENGTH_16,
	eSVM_BURST_LENGTH_32,
	eSVM_BURST_LENGTH_64,
	eSVM_BURST_LENGTH_128,
	eSVM_BURST_LENGTH_MAX,
};

enum tagSVM_OUTPUT_PART_E
{
	eSVM_OUTPUT_PART_FB = 0,
	eSVM_OUTPUT_PART_LR,
	eSVM_OUTPUT_PART_MAX,
};

enum tagSVM_FB_PART_E
{
	eSVM_FB_PART_F = 0,
	eSVM_FB_PART_B,
	eSVM_FB_PART_MAX,
};

enum tagSVM_LR_PART_E
{
	eSVM_LR_PART_L,
	eSVM_LR_PART_R,
	eSVM_LR_PART_MAX,
};

enum tagSVM_SUBPART_E
{
	eSVM_SUBPART_FL = 0,
	eSVM_SUBPART_FR,
	eSVM_SUBPART_BR,
	eSVM_SUBPART_BL,
	eSVM_SUBPART_MAX,
};

enum tagSVM_BOUNDARYAEAR_E
{
	eSVM_BOUNDARYAEAR_0 = 0,
	eSVM_BOUNDARYAEAR_1,
	eSVM_BOUNDARYAEAR_2,
	eSVM_BOUNDARYAEAR_3,
	eSVM_BOUNDARYAEAR_MAX,
};

enum tagSVM_BOUNDARYSCALE_E
{
	eSVM_BOUNDARYSCALE_ONE = 0,											// not scale
	eSVM_BOUNDARYSCALE_ONE_OVER_TWO,									// 1/2 scale
	eSVM_BOUNDARYSCALE_ONE_OVER_FOUR,									// 1/4 scale
	eSVM_BOUNDARYSCALE_ONE_OVER_EIGHT,									// 1/8 scale
	eSVM_BOUNDARYSCALE_MAX,
};

enum tagSVM_COMPRESSION_RATIO_E
{
	eSVM_COMPRESSION_RATIO_HALF = 0,									// 1/2 compression
	eSVM_COMPRESSION_RATIO_QUARTER,										// 1/4 compression
	eSVM_COMPRESSION_MAX,
};

enum tagSVM_COLOR_E
{
	eSVM_COLOR_R	= 0,
	eSVM_COLOR_G,
	eSVM_COLOR_B,
	eSVM_COLOR_MAX,
};

enum tagSVM_SECTION_NUMBER_E
{
	eSVM_SECTION_NUMBER_0	= 0,
	eSVM_SECTION_NUMBER_1,
	eSVM_SECTION_NUMBER_MAX,
};

enum tagSVM_EDGE_MODE_E
{
	eSVM_EDGE_MODE_LUT = 0,
	eSVM_EDGE_MODE_FIXED_GAIN,
	eSVM_EDGE_MODE_MAX,
};

enum tagSVM_OUTPUTMODE_E
{
	eSVM_OUTPUTMODE_BYPASS_FRONT = 0,									// only front
	eSVM_OUTPUTMODE_BYPASS_LEFT,										// only left
	eSVM_OUTPUTMODE_BYPASS_RIGHT,										// only right
	eSVM_OUTPUTMODE_BYPASS_BACK,										// only back
	eSVM_OUTPUTMODE_LUT_HALF_BLEND,										// 4channel + FB&LR LUT + 1/2 blending
	eSVM_OUTPUTMODE_LUT_BCLUT_ONLY_ALPHA,								// 4channel + FB&LR LUT + only alpha of Brightness control LUT
	eSVM_OUTPUTMODE_LUT_BCLUT,											// 4channel + FB&LR LUT + Brightness control LUT
	eSVM_OUTPUTMODE_MAX,
};

enum tagSVM_PAGEFLIPPING_NUMBER_E
{
	eSVM_PAGEFLIPPING_NUMBER_0 = 0,
	eSVM_PAGEFLIPPING_NUMBER_1,
	eSVM_PAGEFLIPPING_NUMBER_MAX,
};


/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef struct tagSVM_RECT_T
{
	uint16 x;
	uint16 y;
	uint16 width;
	uint16 height;
} SVM_RECT_T, *pSVM_RECT_T;

typedef struct tagSVM_COLORCOEFR2Y_T									// coefficient for RGB to YCbCr
{
	uint8 r4y;
	uint8 g4y;
	uint8 b4y;

	uint8 r4cb;
	uint8 g4cb;
	uint8 b4cb;

	uint8 r4cr;
	uint8 g4cr;
	uint8 b4cr;
} SVM_COLORCOEF_R2Y_T, *pSVM_COLORCOEF_R2Y_T;

typedef struct tagSVM_COLORCOEF_Y2R_T									// coefficient for YCbCr to RGB
{
	uint16 cr4r;
	uint8 cr4g;

	uint8 cb4g;
	uint16 cb4b;
} SVM_COLORCOEF_Y2R_T, *pSVM_COLORCOEF_Y2R;

typedef struct tagSVM_COLORCOEF_T
{
	bool isWith128;

	SVM_COLORCOEF_R2Y_T r2y;
	SVM_COLORCOEF_Y2R_T y2r;
} SVM_COLORCOEF_T, *pSVM_COLORCOEF_T;

typedef struct tagSVM_BCCOEF_T											// coefficient for brightness control
{
	uint8 coef1[eSVM_COLOR_MAX][eSVM_SUBPART_MAX];
	uint8 coef2[eSVM_COLOR_MAX][eSVM_FB_PART_MAX];
	uint8 coef3[eSVM_COLOR_MAX][eSVM_SUBPART_MAX];
	uint8 coef4[eSVM_COLOR_MAX][eSVM_LR_PART_MAX];
} SVM_BCCOEF_T, *pSVM_BCCOEF_T;

typedef struct tagSVM_RGB_STATISTICS_T									// structur for RGB statistics
{
	uint32 value[eSVM_OUTPUT_PART_MAX][eSVM_SUBPART_MAX][eSVM_COLOR_MAX];
} SVM_RGB_STATISTICS_T, *pSVM_RGB_STATISTICS_T;

typedef struct tagSVM_ROI_AREA_T
{
	uint16 startYLine;
	uint16 endYLine;
} SVM_ROI_AREA_T, *pSVM_ROI_AREA_T;

typedef struct tagSVM_BOUNDARY_RANGE_T
{
	uint16 startYLine;													// value must be a multiple of 4.
	uint8 scale;														// ref. enum SVM_BOUNDARYSCALE_E
} SVM_BOUNDARY_RANGE_T, *pSVM_BOUNDARY_RANGE;

typedef struct tagSVM_BOUNDARY_T
{
	SVM_BOUNDARY_RANGE_T area[eSVM_BOUNDARYAEAR_MAX];					// area[0].startYLine not used. area[0].startYline is 0.
} SVM_BOUNDARY_T, *pSVM_BOUNDARY;


/***************************************************************************************************************************************************************
 * Function
***************************************************************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
* input control
************************************************************************************/
APICALL int32 APIENTRY SVMAPI_setChannelInputAddr(uint8 IN channel, uint32 IN addr);
APICALL int32 APIENTRY SVMAPI_getChannelInputAddr(uint32 OUT *channel0Addr, uint32 OUT *channel1Addr, uint32 OUT *channel2Addr, uint32 OUT *channel3Addr);

APICALL int32 APIENTRY SVMAPI_setChannelFilpping(uint8 IN channel, bool IN isHorizontal, bool IN isVertical);

APICALL int32 APIENTRY SVMAPI_setCompression(uint8 IN viewMode, uint8 IN ratio);
APICALL int32 APIENTRY SVMAPI_setCompressionEnable(uint8 IN viewMode, bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setROI(uint8 IN viewMode, uint8 IN channel, SVM_ROI_AREA_T IN roi);
APICALL int32 APIENTRY SVMAPI_setROIEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setHorizontalScale(uint8 IN viewMode, uint8 IN channel, SVM_BOUNDARY_T IN chBoundary);
APICALL void APIENTRY SVMAPI_setHoldTime(uint16 IN noScale, uint16 IN oneOverTwoScale, uint16 IN oneOverFourScale, uint16 IN oneOverEightScale);
APICALL int32 APIENTRY SVMAPI_setVerticalHalfScale(uint8 IN viewMode, uint8 IN channel, bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setInputBurstLength(uint8 IN burstLength);

APICALL void APIENTRY SVMAPI_setInputTestModeEnable(bool IN isEnable);

APICALL void APIENTRY SVMAPI_setInputWrappingInitEnable(bool IN isEnable);
APICALL int32 APIENTRY SVMAPI_setInputEnable(bool IN isFrontEnable, bool IN isLeftEnable, bool IN isRightEnable, bool IN isBackEnable);

/************************************************************************************
* svm control
************************************************************************************/
APICALL void APIENTRY SVMAPI_setSync(uint8 IN pipLineDelay, uint16 IN outLineDaly);

APICALL void APIENTRY SVMAPI_setBGColor(uint8 IN y, uint8 IN cb, uint8 IN cr);
APICALL void APIENTRY SVMAPI_setMaskColor(uint8 IN y, uint8 IN cbcr);
APICALL void APIENTRY SVMAPI_setColorCoef(pSVM_COLORCOEF_T IN coefColor);

APICALL int32 APIENTRY SVMAPI_setCar(uint8 IN viewMode, SVM_RECT_T IN rect, uint8 IN maskColorY, uint8 IN maskColorCb, uint8 IN maskColorCr);
APICALL int32 APIENTRY SVMAPI_setCarReadBurstLength(uint8 IN burstLength);
APICALL int32 APIENTRY SVMAPI_updateCar(uint8 IN viewMode, uint16* IN carImage, uint32 IN size);
APICALL int32 APIENTRY SVMAPI_setCarAddr(uint8 IN viewMode, uint32 IN addr);
APICALL int32 APIENTRY SVMAPI_getCarAddr(uint8 IN viewMode, uint32 OUT *addr);
APICALL int32 APIENTRY SVMAPI_setCarEnable(uint8 IN viewMode, bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setEdgeEnhancementMode(uint8 IN mode);
APICALL int32 APIENTRY SVMAPI_setEdgeEnhancementFixedGain(float IN gain);
APICALL int32 APIENTRY SVMAPI_setEdgeEnhancementEnable(bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setDynamicblendingCoef(uint8 coef1, uint8 coef3, uint8 coef6, uint8 coef8);
APICALL int32 APIENTRY SVMAPI_setDynamicblendingEnable(bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setMorphingSpeed(uint8 IN speed);

APICALL int32 APIENTRY SVMAPI_updateBCCoef(pSVM_BCCOEF_T IN coefBC);
APICALL void APIENTRY SVMAPI_updateBCRatio(uint8 IN ratio);

APICALL uint8 APIENTRY SVMAPI_getPageNum4Lut(uint8 IN viewMode, uint8 IN outputPartNum);

APICALL int32 APIENTRY SVMAPI_updateFBLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size);
APICALL int32 APIENTRY SVMAPI_updateLRLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size);
APICALL int32 APIENTRY SVMAPI_updateBCLUT(uint8 IN viewMode, uint32* IN lutBin, uint32 IN size);

APICALL int32 APIENTRY SVMAPI_setFBLUTAddr(uint8 IN viewMode, uint32 IN pageflipping0Addr, uint32 IN pageflipping1Addr);
APICALL int32 APIENTRY SVMAPI_setLRLUTAddr(uint8 IN viewMode, uint32 IN pageflipping0Addr, uint32 IN pageflipping1Addr);
APICALL int32 APIENTRY SVMAPI_setBCLUTAddr(uint8 IN viewMode, uint32 IN addr);
APICALL int32 APIENTRY SVMAPI_getFBLUTAddr(uint8 IN viewMode, uint32 OUT *pageflipping0Addr, uint32 OUT *pageflipping1Addr);
APICALL int32 APIENTRY SVMAPI_getLRLUTAddr(uint8 IN viewMode, uint32 OUT *pageflipping0Addr, uint32 OUT *pageflipping1Addr);
APICALL int32 APIENTRY SVMAPI_getBCLUTAddr(uint8 IN viewMode, uint32 OUT *addr);

APICALL void APIENTRY SVMAPI_getRGBStatistics(pSVM_RGB_STATISTICS_T OUT stats);

APICALL int32 APIENTRY SVMAPI_setSVMEnable(bool IN isEnable);

/************************************************************************************
* output control
************************************************************************************/
APICALL void APIENTRY SVMAPI_setBlank(uint16 IN hBlank, uint16 IN vBlank);

APICALL int32 APIENTRY SVMAPI_setSection(uint8 IN viewMode, uint8 IN sectionNumber, SVM_RECT_T IN sectionRect);
APICALL int32 APIENTRY SVMAPI_setSectionEnable(uint8 IN viewMode, uint8 IN sectionNumber, bool IN isEnable);

APICALL int32 APIENTRY SVMAPI_setOutputBurstLength(uint8 IN burstLength);

APICALL void APIENTRY SVMAPI_setInvalidLUTEnable(uint8 IN isEnable);

APICALL int32 APIENTRY SVMAPI_setOutputEnable(bool IN isFBEnable, bool IN isLREnable);

/************************************************************************************
* callback function (ISR)
************************************************************************************/
APICALL void APIENTRY SVMAPI_setCBOutputVsync(SVM_CALLBACK IN callback);

/************************************************************************************
* view mode control
************************************************************************************/
APICALL int32 APIENTRY SVMAPI_setViewMode(uint8 IN viewMode, bool IN isFBMorphing, bool IN isLRMorphing);

APICALL int32 APIENTRY SVMAPI_createViewMode(uint8 IN setFBLUT, uint8 IN setLRLUT, uint8 IN setBCLUT, uint8 IN setCarImage, uint8 IN outputMode, uint8 OUT *viewMode);
APICALL void APIENTRY SVMAPI_deleteAllViewMode(void);

APICALL int32 APIENTRY SVMAPI_initialize(uint16 IN inputWidth, uint16 IN inputHeight, uint16 IN outputWidth, uint16 IN outputHeight, uint8 IN samplingCount, float IN inputFrameCount);
APICALL void APIENTRY SVMAPI_deInitialize(void);

#ifdef __cplusplus
}
#endif

#endif //__SVM_API_H__