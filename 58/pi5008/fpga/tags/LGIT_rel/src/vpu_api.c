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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <nds32_intrinsic.h>
#include "error.h"
#include "vpu_register.h"
#include "utils.h"
#include "vpu_api.h"
#include "intr.h"
#include "debug.h"

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/
#define BASE_ADDR_DRAM_MEM				(0x20000000U)
#define BASE_MEMORY_VPU					(BASE_ADDR_DRAM_MEM + 0x0E000000U)		// TBD

#define MAX_FEATURE_COUNTER				(65535)

/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef union tagVPU_FB_CONFIG_U
{
	uint32		var;

	struct
	{
		uint32	enable				:	1,
				dummy				:	3,
				fast_n				:	4,
				fast_th				:	8,
				ch_sel				:	3,
				score_big_or_equal	:	1,
				brief_enable		:	1,
				roi_enable			:	1,
				scl_enable			:	1,
				dma2otf_enable		:	1,
				use_posedge_vsync	:	1;
	}fld;
} VPU_FB_CONFIG_U, *pVPU_FB_CONFIG_U;

typedef struct tagVPU_ADDR_T
{
	uint32 xyforFAST[2];			// xy base address for FAST result
	uint32 descforBRIEF[2];			// descriptor base address for BRIEF result

	uint32 srcforHD;				// source base address for hamming distance
	uint32 dstforHD;				// destination base address for hamming distance
} VPU_ADDR_T, *pVPU_ADDR_T;

/***************************************************************************************************************************************************************
 * Global Variable
***************************************************************************************************************************************************************/
static bool gVPU_isInit = FALSE;
static VPU_SIZE_T gVPU_inputSize;

VPU_FB_CONFIG_U gVPU_FB_config;
VPU_ADDR_T gVPU_addr;

/*======================================================================================================================
 = Local function
======================================================================================================================*/
static void __delay( uint32 count )
{
	while(count-- > 0){
		asm volatile ("nop");
	}

}

void vpu_ctrl_isr(int num)
{
	uint32 msk = (1 << num);
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	printf("vpu0 isr[ro_error: 0x%x]\n", VPU_CTRL_REG->roError);
		
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
	
}

void vpu_dma_isr(int num)
{
	uint32 msk = (1 << num);
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	printf("vpu1 isr\n");
		
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
	
}


/*
 * function					:	VPU_calMultipleof8

 * param vale				:	input value

 * return					:	result value

 * brief
	Converts input value to a multiple of 8.
*/
static uint8 VPU_calMultipleof8(uint16 value)
{
	if ( value != 0 )
		value = value / 8;

	return (uint8)value;
}

/*
 * function					:	VPU_FB_waitForVsync

 * param frameInterval		:	interval between 1st frame and 2nd frame

 * return void				:

 * brief
	check vsync for frame interval.
*/
static void VPU_FB_waitForVsync(uint8 frameInterval)
{
	uint8 vsync = 0x1;
	uint8 count = 0;

	do
	{
		vsync = utilGetBit(VPU_CTRL_REG->roState, 9);
		if ( vsync == 0x0 ) count++;
	} while (count >= frameInterval);
}

/*
 * function					:	VPU_FB_waitForBusy

 * param void				:

 * return void				:

 * brief
	check busy state of FAST & BRIEF.
*/
static uint8 VPU_FB_waitForBusy(void)
{
	uint8 busy = 0x1;
	uint8 limit = 0x0;

	// check if busy cleared
	do
	{
		busy = utilGetBit(VPU_CTRL_REG->roState, 12);
		
		if(utilGetBit(VPU_CTRL_REG->roError, 1))
			limit = 0x1;
	} while (busy);
	
	return limit;
}

/*
 * function					:	VPU_HD_waitForBusy

 * param void				:

 * return void				:

 * brief
	check busy state of hamming distance.
*/
static void VPU_HD_waitForBusy(void)
{
    uint8 busy = 0x1;
    do
	{
        busy    = utilGetBit(VPU_DMACTRL_REG->ctrl, 0);
    } while (busy);
}

/*
 * function					:	VPU_startHD

 * param srcAddr			:	address for descriptor
 * param dstAddr			:	address for distance
 * param descCount			:	number of descriptor
 * param isMemToMem			:	only dram (TRUE) or register + drame (FALSE)

 * return void				:

 * brief
	Start hamming distance.
*/
static void VPU_startHD(uint32 srcAddr, uint32 dstAddr, uint32 descCount, bool isMemToMem)
{
	uint16 num_byte = descCount * (128/8);

	VPU_DMACTRL_REG->rdma2d_base = srcAddr;
	VPU_DMACTRL_REG->rdma2d_stride	= num_byte;
	VPU_DMACTRL_REG->rdma2d_size = (num_byte << 16) | 0x1;
	VPU_DMACTRL_REG->wdma1d_base = dstAddr;

	if (isMemToMem)
		VPU_DMACTRL_REG->ctrl = (VPU_DMACTRL_REG->ctrl & 0xffff0000) + 0xb5;
	else
		VPU_DMACTRL_REG->ctrl = (VPU_DMACTRL_REG->ctrl & 0xffff0000) + 0xb1;

	VPU_HD_waitForBusy();
}


/*======================================================================================================================
 = Export function
======================================================================================================================*/

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_initialize

 * param inputSize			:	Input video size (width, height)
 * param limitFeatureCount	:	Limit count of feature point

 * return					:	error code

 * brief
	Initializes the FAST & BRIEF.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_initialize(VPU_SIZE_T IN inputSize, uint16 IN limitFeatureCount)
{
	uint8 bwFAST, bwBREIF;
	uint32 addrPos = BASE_MEMORY_VPU;

	if ( limitFeatureCount > MAX_FEATURE_COUNTER )
		return eERROR_VPU_INVALIED_ARGUMENT_TOO_MAANY_LIMIT_FEATURE_COUNT;

	gVPU_FB_config.var = 0x0;
	gVPU_FB_config.fld.brief_enable = 0x1;
	gVPU_FB_config.fld.use_posedge_vsync = 0x0;

	// input size
	VPU_CTRL_REG->size = (inputSize.height) | (inputSize.width << 16);

	// bandwidth + limit count
	bwFAST = 0x0F;
	bwBREIF = 0x3F;
	VPU_CTRL_REG->bandwidth = (bwFAST << 0) | (bwBREIF << 8) | (limitFeatureCount << 16);

	// init address
	gVPU_addr.xyforFAST[0] = addrPos;
	addrPos += limitFeatureCount * 4;

	addrPos = ((addrPos + 7) >> 3) << 3;	// 8byte align	
	gVPU_addr.xyforFAST[1] = addrPos;
	addrPos += limitFeatureCount * 4;
	
	addrPos = ((addrPos + 7) >> 3) << 3;	// 8byte align
	gVPU_addr.descforBRIEF[0] = addrPos;
	addrPos += limitFeatureCount * 16;

	addrPos = ((addrPos + 7) >> 3) << 3;	// 8byte align
	gVPU_addr.descforBRIEF[1] = addrPos;
	addrPos += limitFeatureCount * 16;
	
	addrPos = ((addrPos + 7) >> 3) << 3;	// 8byte align
	gVPU_addr.srcforHD = addrPos;

	gVPU_inputSize = inputSize;
	gVPU_isInit = TRUE;

	VPU_PRINT("fast1 : 0x%08X,\tfast2 : 0x%08X,\tbrief1 : 0x%08X,\tbrief2 : 0x%08X\n", 
				gVPU_addr.xyforFAST[0], gVPU_addr.xyforFAST[1], gVPU_addr.descforBRIEF[0], gVPU_addr.descforBRIEF[1]);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_deInitialize

 * param 					:

 * return					:

 * brief
	Deinitializes the FAST & BRIEF.
***************************************************************************************************************************************************************/
void APIENTRY VPUAPI_FB_deInitialize(void)
{
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setChannel

 * param channel			:	channel number

 * return					:	error code

 * brief
	Sets the channel number. (ref. enum VPU_CHANNEL_E)
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setChannel(uint8 IN channel)
{
	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	if ( channel >= eVPU_CHANNEL_MAX )
		return eERROR_INVALIED_ARGUMENT;

	gVPU_FB_config.fld.ch_sel = channel;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setScale

 * param scale				:	scale size (width, height)

 * return					:	error code

 * brief
	Sets the scale size.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setScale(VPU_SIZE_T IN scaleSize)
{
	uint16 vRatio, hRatio;

	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	vRatio = (256 * gVPU_inputSize.height) / scaleSize.height;
	hRatio = (256 * gVPU_inputSize.width) / scaleSize.width;

	VPU_CTRL_REG->sclSize = (scaleSize.height) | (scaleSize.width << 16);
	VPU_CTRL_REG->sclRatio = (vRatio) | (hRatio << 16);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setScaleEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the scale.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setScaleEnable(bool IN isEnable)
{
	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	gVPU_FB_config.fld.scl_enable = isEnable;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setROI

 * param roi				:	roi aear (x, y, width, height)

 * return					:	error code

 * brief
	Sets the ROI area.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setROI(VPU_RECT_T IN roi)
{
	uint16 startX, startY, endX, endY;

	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	startX = roi.x;
	startY = roi.y;
	endX = (roi.x + roi.width) - 1;
	endY = (roi.y + roi.height) - 1;

	if ( (endX > gVPU_inputSize.width) || (endY > gVPU_inputSize.height) )
		return eERROR_INVALIED_ARGUMENT;

	VPU_CTRL_REG->roiStart = (startY) | (startX << 16);
	VPU_CTRL_REG->roiEnd = (endY) | (endX << 16);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setROIEnable

 * param isEnable			:	enable or disable

 * return					:	error code

 * brief
	Enable or disable of the ROI.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setROIEnable(bool IN isEnable)
{
	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	gVPU_FB_config.fld.roi_enable = isEnable;

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_setZone

 * param zoneNumber			:	zone number ( 0 ~3 )
 * param isEnable			:	enable or disable
 * param zone				:	rectagle area of zone. if isEnable is disable, it is not used and the value is 0xffffffff.

 * return					:	error code

 * brief
	Enable or disable of the ROI.
	The register set value must be a multiple of 8.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_setZone(uint8 IN zoneNumber, bool IN isEnable, VPU_RECT_T IN zone)
{
	uint8 startX, startY, endX, endY;			// multiple of 8

	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	if ( zoneNumber > 3 )
		return eERROR_INVALIED_ARGUMENT;

	if ( isEnable )
	{
		startX = VPU_calMultipleof8(zone.x);
		startY = VPU_calMultipleof8(zone.y);
		endX = VPU_calMultipleof8((zone.x + zone.width) - 1);
		endY = VPU_calMultipleof8((zone.y + zone.height) - 1);

		VPU_CTRL_REG->fast_zone[zoneNumber] = (startX << 0) | (endX << 8) | (startY << 16) | (endY << 24);
	}
	else
	{
		VPU_CTRL_REG->fast_zone[zoneNumber] = 0xFFFFFFFF;
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FAST_setParam

 * param fastParam			:	struct of FAST parameter

 * return					:	error code

 * brief
	Sets the FAST parameter. (ref. struct VPU_FAST_PARAM_T)
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FAST_setParam(VPU_FAST_PARAM_T IN fastParam)
{
	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	gVPU_FB_config.fld.fast_n = fastParam.number & utilBitMask(4);
	gVPU_FB_config.fld.fast_th = fastParam.threshold & utilBitMask(8);

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_BRIEF_setLut

 * param briefLut			:	array of BRIEF LUT.

 * return					:	error code

 * brief
	Update BRIEF LUT.
	The counter of BRIEF LUT array must be 128.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_BRIEF_setLut(pVPU_BRIEF_LUT_T IN briefLut)
{
	int i = 0;
	uint32 value = 0x0;

	if ( briefLut == NULL )
		return eERROR_INVALIED_ARGUMENT;

	for ( i = 0; i < 128; i++ )
	{
		value = ((briefLut->lut[i].y1 & utilBitMask(2)) << 0 ) | ((briefLut->lut[i].x1 & utilBitMask(2)) << 4 ) |
				((briefLut->lut[i].y2 & utilBitMask(2)) << 12) | ((briefLut->lut[i].x2 & utilBitMask(2)) << 16) |
				(i << 24);

		VPU_CTRL_REG->lut = value | (1 << 31);
        VPU_CTRL_REG->lut = value | (0 << 31);
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_BRIEF_getLut

 * param briefLut			:	array of BRIEF LUT.

 * return					:	error code

 * brief
	Get BRIEF LUT.
	The counter of BRIEF LUT array must be 128.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_BRIEF_getLut(pVPU_BRIEF_LUT_T OUT briefLut)
{
	int i = 0;

	if ( briefLut == NULL )
		return eERROR_INVALIED_ARGUMENT;

	for ( i = 0; i < 128; i++ )
	{
		VPU_CTRL_REG->lut = (VPU_CTRL_REG->lut & 0x00FFFFFF) | (i<<24) | (0<<31);
		briefLut->lut[i].y1 = utilGetBits(VPU_CTRL_REG->roLut, 0, 2);
		briefLut->lut[i].x1 = utilGetBits(VPU_CTRL_REG->roLut, 4, 2);
		briefLut->lut[i].y2 = utilGetBits(VPU_CTRL_REG->roLut, 12, 2);
		briefLut->lut[i].x2 = utilGetBits(VPU_CTRL_REG->roLut, 16, 2);
	}

	return eSUCCESS;
}


/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FB_start

 * param frameInterval		:	interval between 1st frame and 2nd frame
 * param featureNumOf1st	:	featrue number of 1st frame
 * param featureNumOf2nd	:	feature number of 2nd frame

 * return					:	error code

 * brief
 	Start FAST & BRIEF.
***************************************************************************************************************************************************************/
uint8 APIENTRY VPUAPI_FB_start(uint8 IN frameInterval, uint16* OUT featureNumOf1st, uint16* OUT featureNumOf2nd)
{
	uint32 i = 0;
	uint16 featureNum[eVPU_FRAME_MAX] = {0};
	uint8 frameErr[eVPU_FRAME_MAX] = {0};

	if ( gVPU_isInit == FALSE )
		return eERROR_VPU_NOT_INITIALIZE;

	if ( frameInterval == 0 )
	{
		for ( i = 0; i < eVPU_FRAME_MAX; i++ )
		{
			VPU_CTRL_REG->xyBase = gVPU_addr.xyforFAST[i];
			VPU_CTRL_REG->descBase = gVPU_addr.descforBRIEF[i];

			gVPU_FB_config.fld.enable = 0x0;
			VPU_CTRL_REG->config = gVPU_FB_config.var;				// FAST & BREIF config
			gVPU_FB_config.fld.enable = 0x1;
			VPU_CTRL_REG->config = gVPU_FB_config.var;				// FAST & BREIF enable
		}

		for ( i = 0; i < eVPU_FRAME_MAX; i++ )
		{
			frameErr[i] = VPU_FB_waitForBusy();
			featureNum[i] = (uint16)VPU_CTRL_REG->roNumScore;
		}
	}
	else
	{
		for ( i = 0; i < eVPU_FRAME_MAX; i++ )
		{
			VPU_CTRL_REG->xyBase = gVPU_addr.xyforFAST[i];
			VPU_CTRL_REG->descBase = gVPU_addr.descforBRIEF[i];

			gVPU_FB_config.fld.enable = 0x1;
			VPU_CTRL_REG->config = gVPU_FB_config.var;				// FAST & BREIF config and enable
			__delay(0x10000);
			frameErr[i] = VPU_FB_waitForBusy();
			featureNum[i] = (uint16)VPU_CTRL_REG->roNumScore;

			VPU_FB_waitForVsync(frameInterval);
		}
	}

	*featureNumOf1st = featureNum[eVPU_FRAME_1ST];
	*featureNumOf2nd = featureNum[eVPU_FRAME_2ND];
	if ( frameErr[eVPU_FRAME_1ST] && frameErr[eVPU_FRAME_2ND] )
	{
		return eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_BOTH_FRAME;
	}
	else if ( frameErr[eVPU_FRAME_1ST] )
	{
		return eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_1ST_FRAME;
	}
	else if ( frameErr[eVPU_FRAME_2ND] )
	{
		return eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_2ND_FRAME;
	}

	return eSUCCESS;
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_FAST_getResult

 * param frameNumber		:	frame number ( 1st or 2nd = 0 or 1 )

 * return					:	array of feature point or null

 * brief
	Gets feature point of FAST.
***************************************************************************************************************************************************************/
pVPU_FAST_RESULT_POS_T APIENTRY VPUAPI_FAST_getResult(uint8 IN frameNumber)
{
	if ( frameNumber >= eVPU_FRAME_MAX )
		return NULL;

	return (pVPU_FAST_RESULT_POS_T)gVPU_addr.xyforFAST[frameNumber];
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_BRIEF_getResult

 * param frameNumber		:	frame number ( 1st or 2nd = 0 or 1 )

 * return					:	array of descriptor or null

 * brief
	Gets descriptor of BRIEF.
***************************************************************************************************************************************************************/
pVPU_BRIEF_RESULT_DESC_T APIENTRY VPUAPI_BRIEF_getResult(uint8 IN frameNumber)
{
	if ( frameNumber >= eVPU_FRAME_MAX )
		return NULL;

	return (pVPU_BRIEF_RESULT_DESC_T)gVPU_addr.descforBRIEF[frameNumber];
}

/***************************************************************************************************************************************************************
 * function					:	VPUAPI_getHammingDistance

 * param srcDesc			:	array of source descriptor
 * param srcDescCount		:	array count of source descriptor
 * param dstDesc			:	array of destination descriptor
 * param dstDescCount		:	array count of destination descriptor

 * return					:	array of hamming distance or null

 * brief
	Gets value of hamming distance.
	srcDesc[0] ---------- dstDesc[0]
						dstDesc[1]
						.........
						dstDesc[N]
	srcDesc[1] ---------- dstDesc[0]
						dstDesc[1]
						.........
						dstDesc[N]
	...............
	srcDesc[N] --------- dstDesc[0]
						dstDesc[1]
						.........
						dstDesc[N]
***************************************************************************************************************************************************************/
uint8* APIENTRY VPUAPI_getHammingDistance(pVPU_BRIEF_RESULT_DESC_T IN desc1, uint32 IN descCount1, pVPU_BRIEF_RESULT_DESC_T IN desc2, uint32 IN descCount2)
{
	uint32 i = 0, j = 0;
	uint8 *p_dstforHD;

	if ( desc1 == NULL || descCount1 == 0 || desc2 == NULL || descCount2 == 0 )
		return NULL;

	gVPU_addr.dstforHD = ((gVPU_addr.srcforHD + 7) >> 3) << 3;		// 8byte align
	VPU_PRINT("dstforHD : 0x%08X\n", gVPU_addr.dstforHD);

	p_dstforHD = (uint8 *)gVPU_addr.dstforHD;
	for ( i = 0; i < descCount1; i++ )
	{
		for ( j = 0; j < 4; j++ )
			VPU_HAMMING_REG->desc[j] = desc1[i].descriptor[j];

		VPU_startHD((uint32)desc2, (uint32)p_dstforHD, descCount2, FALSE);
		p_dstforHD += descCount1;
	}
	
	return (uint8*)gVPU_addr.dstforHD;
}

uint8* APIENTRY VPUAPI_getHammingDistance_mem(pVPU_BRIEF_RESULT_DESC_T IN desc1, pVPU_BRIEF_RESULT_DESC_T IN desc2, uint32 IN descCount)
{
	// TBD.
	
	uint32 i = 0;
	uint8 *p_srcforHD;
	
	if ( desc1 == NULL || desc2 == NULL || descCount == 0 )
		return NULL;

	gVPU_addr.srcforHD = (sizeof(VPU_BRIEF_RESULT_DESC_T) * descCount) * 2;
	gVPU_addr.dstforHD  = ((gVPU_addr.srcforHD + 7) >> 3) << 3;		// 8byte align
	VPU_PRINT("srcforHD : 0x%08X,\tdstforHD : 0x%08X\n", gVPU_addr.srcforHD, gVPU_addr.dstforHD);

	p_srcforHD = (uint8 *)gVPU_addr.srcforHD;
	for ( i = 0; i < descCount; i++ )
	{
		*p_srcforHD++ = desc1[i].descriptor[0];
		*p_srcforHD++ = desc1[i].descriptor[1];
		*p_srcforHD++ = desc1[i].descriptor[2];
		*p_srcforHD++ = desc1[i].descriptor[3];
		*p_srcforHD++ = desc2[i].descriptor[0];
		*p_srcforHD++ = desc2[i].descriptor[1];
		*p_srcforHD++ = desc2[i].descriptor[2];
		*p_srcforHD++ = desc2[i].descriptor[3];
	}
	VPU_startHD(gVPU_addr.srcforHD, gVPU_addr.dstforHD, descCount, TRUE);
	
	return (uint8*)gVPU_addr.dstforHD;
}

