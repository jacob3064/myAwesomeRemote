/**
 * \file
 *
 * \brief	VPU(Video Processing Unit) APIs
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

#ifndef __VPU_API_H__
#define __VPU_API_H__

#include "type.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/


/***************************************************************************************************************************************************************
 * Enumeration
***************************************************************************************************************************************************************/
enum tagVPU_CHANNEL_E
{
	eVPU_CHANNEL_FRONT = 0,
	eVPU_CHANNEL_LEFT,
	eVPU_CHANNEL_RIGHT,
	eVPU_CHANNEL_BACK,
	eVPU_CHANNEL_SVM,
	eVPU_CHANNEL_OUTPUT,
	eVPU_CHANNEL_MAX,
};

enum tagVPU_FRAME_E
{
	eVPU_FRAME_1ST = 0,
	eVPU_FRAME_2ND,
	eVPU_FRAME_MAX,
};

/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef struct tagVPU_SIZE_T
{
	uint16 width;
	uint16 height;
} VPU_SIZE_T, *pVPU_SIZE_T;

typedef struct tagVPU_RECT_T
{
	uint16 x;
	uint16 y;
	uint16 width;
	uint16 height;
} VPU_RECT_T, *pVPU_RECT_T;

typedef struct tagVPU_FAST_PARAM_T
{
	uint8 number;
	uint8 threshold;
} VPU_FAST_PARAM_T, *pVPU_FAST_PARAM_T;

typedef struct tagVPU_BRIEF_LUT_VALUE_T
{
	uint8 x1;
	uint8 y1;
	uint8 x2;
	uint8 y2;
} VPU_BRIEF_LUT_VALUE_T, *pVPU_BRIEF_LUT_VALUE_T;

typedef struct tagVPU_BRIEF_LUT_T
{
	VPU_BRIEF_LUT_VALUE_T lut[128];
} VPU_BRIEF_LUT_T, *pVPU_BRIEF_LUT_T;

typedef struct tagVPU_FAST_RESULT_POS_T
{
	uint16 x;
	uint16 y;
} VPU_FAST_RESULT_POS_T, *pVPU_FAST_RESULT_POS_T;

typedef struct tagVPU_BRIEF_RESULT_DESC_T
{
	uint32 descriptor[4];
} VPU_BRIEF_RESULT_DESC_T, *pVPU_BRIEF_RESULT_DESC_T;

/***************************************************************************************************************************************************************
 * Function
***************************************************************************************************************************************************************/
APICALL uint8 APIENTRY VPUAPI_FB_initialize (VPU_SIZE_T IN inputSize, uint16 IN limitFeatureCount);
APICALL void APIENTRY VPUAPI_FB_deInitialize (void);

APICALL uint8 APIENTRY VPUAPI_FB_setChannel (uint8 IN channel);
APICALL uint8 APIENTRY VPUAPI_FB_setScale (VPU_SIZE_T IN scaleSize);
APICALL uint8 APIENTRY VPUAPI_FB_setScaleEnable (bool IN isEnable);
APICALL uint8 APIENTRY VPUAPI_FB_setROI (VPU_RECT_T IN roi);
APICALL uint8 APIENTRY VPUAPI_FB_setROIEnable (bool IN isEnable);
APICALL uint8 APIENTRY VPUAPI_FB_setZone (uint8 IN zoneNumber, bool IN isEnable, VPU_RECT_T IN zone);

APICALL uint8 APIENTRY VPUAPI_FAST_setParam (VPU_FAST_PARAM_T IN fastParam);
APICALL uint8 APIENTRY VPUAPI_BRIEF_setLut (pVPU_BRIEF_LUT_T IN briefLut);
APICALL uint8 APIENTRY VPUAPI_BRIEF_getLut (pVPU_BRIEF_LUT_T OUT briefLut);

APICALL uint8 APIENTRY VPUAPI_FB_start (uint8 IN frameInterval, uint16* OUT featureNumOf1st, uint16* OUT featureNumof2nd);

APICALL pVPU_FAST_RESULT_POS_T APIENTRY VPUAPI_FAST_getResult (uint8 IN frameNumber);
APICALL pVPU_BRIEF_RESULT_DESC_T APIENTRY VPUAPI_BRIEF_getResult (uint8 IN frameNumber);

APICALL uint8* APIENTRY VPUAPI_getHammingDistance (pVPU_BRIEF_RESULT_DESC_T IN desc1, uint32 IN descCount1, pVPU_BRIEF_RESULT_DESC_T IN desc2, uint32 IN descCount2);
APICALL uint8* APIENTRY VPUAPI_getHammingDistance_mem (pVPU_BRIEF_RESULT_DESC_T IN desc1, pVPU_BRIEF_RESULT_DESC_T IN desc2, uint32 IN descCount);
APICALL uint8 APIENTRY VPUAPI_FB_DMA_copy (uint32* IN src, uint32* OUT dst, uint16 IN width, uint16 IN height);


#ifdef __cplusplus
}
#endif

#endif //__VPU_H__
