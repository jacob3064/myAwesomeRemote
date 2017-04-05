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

#ifndef __SVM_H__
#define __SVM_H__

#include "type.h"


/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/
#define SVM_MIN_WIDTH					(320)
#define SVM_MIN_HEIGHT					(240)
#define SVM_MAX_WIDTH					(1920)
#define SVM_MAX_HEIGHT					(1080)

/***************************************************************************************************************************************************************
 * Enumeration
***************************************************************************************************************************************************************/
enum tagSVM_CHANNEL_E
{
	eSVM_CHANNEL_CAM0_FRONT = 0,
	eSVM_CHANNEL_CAM1_LEFT,
	eSVM_CHANNEL_CAM2_RIGHT,
	eSVM_CHANNEL_CAM3_BACK,
	eSVM_CHANNEL_MAX,
};

enum tagSVM_BYPASS_E
{
	eSVM_BYPASS_FB = 0,
	eSVM_BYPASS_LR,
	eSVM_BYPASS_BLEND,
	eSVM_BYPASS_MAX,
};

enum tagSVM_PART_E
{
	eSVM_PART_F = 0,
	eSVM_PART_B,
	eSVM_PART_L,
	eSVM_PART_R,
	eSVM_PART_MAX,
};

enum tagSVM_MAINPART_E
{
	eSVM_MAINPART_FB = 0,
	eSVM_MAINPART_LR,
	eSVM_MAINPART_MAX,
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
	eSVM_BOUNDARYSCALE_ONE = 0,								// not scale
	eSVM_BOUNDARYSCALE_ONE_OVER_TWO,						// 1/2 scale
	eSVM_BOUNDARYSCALE_ONE_OVER_FOUR,						// 1/4 scale
	eSVM_BOUNDARYSCALE_ONE_OVER_EIGHT,						// 1/8 scale
	eSVM_BOUNDARYSCALE_MAX,
};

enum tagSVM_COMPRESSION_RATIO_E
{
	eSVM_COMPRESSION_RATIO_HALF = 0,						// 1/2 compression
	eSVM_COMPRESSION_RATIO_QUARTER,							// 1/4 compression
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

enum tagSVM_OUTPUTMODE_E
{
	eSVM_OUTPUTMODE_BYPASS_FRONT = 0,						// only front
	eSVM_OUTPUTMODE_BYPASS_LEFT,							// only left
	eSVM_OUTPUTMODE_BYPASS_RIGHT,							// only right
	eSVM_OUTPUTMODE_BYPASS_BACK,							// only back
	eSVM_OUTPUTMODE_BLEND_HALF_ONLY,						// 4channel + 1/2 blending
	eSVM_OUTPUTMODE_ALPHA,									// 4channel + alpha of HW + not used lut4BC
	eSVM_OUTPUTMODE_LUT_ALPHA,								// 4channel + alpha of lut4BC
	eSVM_OUTPUTMODE_LUT_ALPHA_BC,							// 4channel + alpha of lut4BC + BC area of lut4BC + coef of HW
	eSVM_OUTPUTMODE_LUT_ALPHA_BC_COEF,						// 4channel + alpha of lut4BC + BC area of lut4BC + coef of SW(Input register value)
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

typedef struct tagSVM_COLORCOEFR2Y_T						// coefficient for RGB to YCbCr
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

typedef struct tagSVM_COLORCOEF_Y2R_T						// coefficient for YCbCr to RGB
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

typedef struct tagSVM_BCCOEF_T								// coefficient for brightness control
{
	uint8 coef1[eSVM_COLOR_MAX][eSVM_SUBPART_MAX];
	uint8 coef2[eSVM_COLOR_MAX][eSVM_FB_PART_MAX];
	uint8 coef3[eSVM_COLOR_MAX][eSVM_SUBPART_MAX];
	uint8 coef4[eSVM_COLOR_MAX][eSVM_LR_PART_MAX];
} SVM_BCCOEF_T, *pSVM_BCCOEF_T;

typedef struct tagSVM_STATISTICS_T							// structur for statistics
{
	uint32 value[eSVM_MAINPART_MAX][eSVM_SUBPART_MAX][eSVM_COLOR_MAX];
} SVM_STATISTICS_T, *pSVM_STATISTICS_T;

typedef struct tagSVM_CAR_T
{
	uint32 addr;											// dram address of car image
	SVM_RECT_T rect;										// rectangle area for car output
	uint32 maskColor;										// maskcolor for car output
	bool isEnable;											// enable or disable for car output
} SVM_CAR_T, *pSVM_CAR_T;

typedef struct tagSVM_COMPRESSION_T
{
	bool isEnable;
	uint8 ratio;											// ref. enum SVM_COMPRESSION_RATIO_E
} SVM_COMPRESSION_T, *pSVM_COMPRESSION;

typedef struct tagSVM_CHANNEL_ROI_T
{
	uint16 startYLine;
	uint16 endYLine;
} SVM_CHANNEL_ROI_T, *pSVM_CHANNEL_ROI_T;

typedef struct tagSVM_ROI_T
{
	bool isEnable[eSVM_CHANNEL_MAX];
	SVM_CHANNEL_ROI_T channel[eSVM_CHANNEL_MAX];
} SVM_ROI_T, *pSVM_ROI_T;

typedef struct tagSVM_BOUNDARY_RANGE_T
{
	uint16 startYLine;										// value must be a multiple of 4.
	uint8 scale;											// ref. enum SVM_BOUNDARYSCALE_E
} SVM_BOUNDARY_RANGE_T, *pSVM_BOUNDARY_RANGE;

typedef struct tagSVM_CHANNEL_BOUNDARY_T
{
	SVM_BOUNDARY_RANGE_T area[eSVM_BOUNDARYAEAR_MAX];		// area[0].startYLine not used. area[0].startYline is 0.
} SVM_CHANNEL_BOUNDARY_T, *pSVM_CHANNEL_BOUNDARY;

typedef struct tagSVM_BOUNDARY_T
{
	bool isEnable[eSVM_CHANNEL_MAX];
	SVM_CHANNEL_BOUNDARY_T channel[eSVM_CHANNEL_MAX];
} SVM_BOUNDARY_T, *pSVM_BOUNDARY_T;

typedef struct tagSVM_SECTION_T
{
	bool isEnable;											// enable or disable
	SVM_RECT_T rect;										// output area
} SVM_SECTION_T, *pSVM_SECTION_T;

typedef struct tagSVM_VIEWMODE_T
{
	uint8 pageNum4FBLUT;									// page flipping number of FB LUT (ref. enum SVM_PAGEFLIPPING_NUMBER_E)
	uint8 pageNum4LRLUT;									// page flipping number of LR LUT (ref. enum SVM_PAGEFLIPPING_NUMBER_E)

	uint32 lutAddr[eSVM_MAINPART_MAX];						// lut address for fb or lr
	uint32 bclutAddr;										// lut address for brightness control

	SVM_CAR_T car;											// structure for car model
	SVM_COMPRESSION_T compression;							// structure for compression. Compression is used to reduce MBW(Memory Band Width).
	SVM_ROI_T roi;											// structure for ROI. ROI is used to reduce MBW(Memory Band Width).
	SVM_BOUNDARY_T boundary;								// structure for boundary. Boundary is used to reduce MBW(Memory Band Width).
	SVM_SECTION_T section[eSVM_SECTION_NUMBER_MAX];			// structure for section. Section is used to reduce MBW(Memory Band Width).
} SVM_VIEWMODE_T, *pSVM_VIEWMODE_T;

/***************************************************************************************************************************************************************
 * Function
***************************************************************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

APICALL int8 APIENTRY SVMAPI_setOutputMode(uint8 IN mode);

APICALL int8 APIENTRY SVMAPI_setChannelFilpping(uint8 IN channel, bool IN isHorizontal, bool IN isVertical);


APICALL int8 APIENTRY SVMAPI_updateCoefOfBC(pSVM_BCCOEF_T IN coefBC);
APICALL void APIENTRY SVMAPI_updateRatioOfBC(uint8 IN ratio);
APICALL int8 APIENTRY SVMAPI_updateFBLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size);
APICALL int8 APIENTRY SVMAPI_updateLRLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size);
APICALL int8 APIENTRY SVMAPI_updateBCLUT(uint8 IN viewMode, uint8* IN lutBin, uint16 IN size);


APICALL void APIENTRY SVMAPI_setBGColor(uint8 IN r, uint8 IN g, uint8 IN b);
APICALL void APIENTRY SVMAPI_setMaskColor(uint8 IN r, uint8 IN g, uint8 IN b);
APICALL void APIENTRY SVMAPI_setCoefColor(pSVM_COLORCOEF_T IN coefColor);

APICALL int8 APIENTRY SVMAPI_setCar(uint8 IN viewMode, SVM_RECT_T IN rect, uint8 IN maskColorR, uint8 IN maskColorG, uint8 IN maskColorB);
APICALL int8 APIENTRY SVMAPI_setCarEnable(uint8 IN viewMode, bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_setCompression(uint8 IN viewMode, uint8 IN ratio);
APICALL int8 APIENTRY SVMAPI_setCompressionEnable(uint8 IN viewMode, bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_setROI(uint8 IN viewMode, uint8 IN channel, SVM_CHANNEL_ROI_T IN roi);
APICALL int8 APIENTRY SVMAPI_setROIEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_setBoundary(uint8 IN viewMode, uint8 IN channel, SVM_CHANNEL_BOUNDARY_T IN chBoundary);
APICALL int8 APIENTRY SVMAPI_setBoundaryEnable(uint8 IN viewMode, uint8 IN channel, bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_setSection(uint8 IN viewMode, uint8 IN sectionNumber, SVM_RECT_T IN sectionRect);
APICALL int8 APIENTRY SVMAPI_setSectionEnable(uint8 IN viewMode, uint8 IN sectionNumber, bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_setViewMode(uint8 IN viewMode, bool IN isFBMorphing, bool IN isLRMorphing);

APICALL void APIENTRY SVMAPI_getStatistics(pSVM_STATISTICS_T OUT stats);

APICALL int8 APIENTRY SVMAPI_setInputEnable(bool IN isEnable);
APICALL int8 APIENTRY SVMAPI_setOutputEnable(bool IN isEnable);
APICALL int8 APIENTRY SVMAPI_setSVMEnable(bool IN isEnable);

APICALL int8 APIENTRY SVMAPI_initialize(uint16 IN inputWidth, uint16 IN inputHeight, uint16 IN outputWidth, uint16 IN outputHeight, uint8 IN samplingCount, uint8 IN viewModeCount);
APICALL void APIENTRY SVMAPI_deInitialize(void);


#ifdef __cplusplus
}
#endif

#endif //__SVM_H__
