/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <nds32_intrinsic.h>

#include "debug.h"
#include "system.h"
#include "du.h"
#include "sys_api.h"
#include "intr.h"
#include "vpu_api.h"
#include "vpu_test.h"

//#define USE_EX_SCALE
//#define USE_EX_ROI
//#define USE_EX_ZONE
//#define USE_EX_BRIEF_LUT
#define USE_EX_FB
#define USE_FB_PRINT
//#define USE_EX_REG_HAMMING
#define USE_EX_MEM_HAMMING
#define USE_HD_PRINT
void vpu_test (void)
{
	typedef struct {
		uint16 number;
		pVPU_FAST_RESULT_POS_T fast;
		pVPU_BRIEF_RESULT_DESC_T brief;
	} fb;

	fb feature[eVPU_FRAME_MAX];
	uint8* hamming;
	uint16 briefNum1, briefNum2;
	uint8 limitErr = 0;	
	VPU_SIZE_T inputSize;
	uint16 limitFeatureCount;

	VPU_PRINT("VPU example start\n");

	// initialize //
	inputSize.width = 1280;
	inputSize.height = 720;
	limitFeatureCount = 10240;
	VPUAPI_FB_initialize(inputSize, limitFeatureCount);
	VPUAPI_FB_setChannel(eVPU_CHANNEL_FRONT); // 4:SVM

	// scale //
#if defined(USE_EX_SCALE)
	{
	VPU_SIZE_T scaleSize;
	scaleSize.width = 640;
	scaleSize.height = 360;
	VPUAPI_FB_setScale(scaleSize);
	VPUAPI_FB_setScaleEnable(TRUE);
	}
#else
	VPUAPI_FB_setScaleEnable(FALSE);
#endif

	// roi //
	// notice! : x, y of roi reassign 0, 0
#if defined(USE_EX_ROI)
	{
	VPU_RECT_T roi;
	roi.x = 640;
	roi.y = 360;
	roi.width = 640;
	roi.height = 360;
	VPUAPI_FB_setROI(roi);
	VPUAPI_FB_setROIEnable(TRUE);
	}
#else
	VPUAPI_FB_setROIEnable(FALSE);
#endif

	// zone //
#if defined(USE_EX_ZONE)
	{
	VPU_RECT_T zone;
	zone.x = 0;
	zone.y = 0;
	zone.width = 300;
	zone.height = 720;
	VPUAPI_FB_setZone(0, TRUE, zone);
	zone.x = 980;
	zone.y = 0;
	zone.width = 300;
	zone.height = 720;
	VPUAPI_FB_setZone(1, TRUE, zone);
	zone.x = 0;
	zone.y = 360;
	zone.width = 1280;
	zone.height = 360;
	VPUAPI_FB_setZone(2, TRUE, zone);
	VPUAPI_FB_setZone(3, FALSE, zone);
	}
#endif

	// fast param setting //
	{
	VPU_FAST_PARAM_T fastParam;
	fastParam.number = 9;
	fastParam.threshold = 72;
	VPUAPI_FAST_setParam(fastParam);
	}

	// Get&Set brief LUT //
#if defined(USE_EX_BRIEF_LUT)
	{
	VPU_BRIEF_LUT_T briefLut;
	VPUAPI_BRIEF_getLut(&briefLut);
	VPUAPI_BRIEF_setLut(&briefLut);
	}
#endif

	// fast & brief start //
#if defined(USE_EX_FB)
	limitErr = VPUAPI_FB_start(0, &feature[eVPU_FRAME_1ST].number, &feature[eVPU_FRAME_2ND].number);
	feature[eVPU_FRAME_1ST].fast = VPUAPI_FAST_getResult(eVPU_FRAME_1ST);
	feature[eVPU_FRAME_1ST].brief = VPUAPI_BRIEF_getResult(eVPU_FRAME_1ST);
	feature[eVPU_FRAME_2ND].fast = VPUAPI_FAST_getResult(eVPU_FRAME_2ND);
	feature[eVPU_FRAME_2ND].brief = VPUAPI_BRIEF_getResult(eVPU_FRAME_2ND);

	if (limitErr == eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_1ST_FRAME)
	{
		briefNum1 = limitFeatureCount;
		briefNum2 = feature[eVPU_FRAME_2ND].number;
	}
	else if (limitErr == eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_2ND_FRAME)
	{
		briefNum1 = feature[eVPU_FRAME_1ST].number;;
		briefNum2 = limitFeatureCount;
	}
	else if (limitErr == eERROR_VPU_TOO_MAANY_LIMIT_FEATURE_COUNT_BOTH_FRAME)
	{
		briefNum1 = limitFeatureCount;
		briefNum2 = limitFeatureCount;
	}
	else
	{
		briefNum1 = feature[eVPU_FRAME_1ST].number;
		briefNum2 = feature[eVPU_FRAME_2ND].number;
	}
#endif

	// hamming distance start //
#if defined(USE_EX_REG_HAMMING)
	hamming = VPUAPI_getHammingDistance(feature[eVPU_FRAME_1ST].brief, briefNum1, feature[eVPU_FRAME_2ND].brief, briefNum2);
#elif defined(USE_EX_MEM_HAMMING)
	hamming = VPUAPI_getHammingDistance_mem(feature[eVPU_FRAME_1ST].brief, feature[eVPU_FRAME_2ND].brief, (briefNum1<=briefNum2)?briefNum1:briefNum2);
#endif

	{
	uint32 i, j, frm;
	vuint32 *pFast, *pBrief;
	uint16 *num;

#if defined(USE_FB_PRINT)
	for(frm=0 ; frm<eVPU_FRAME_MAX ; frm++)
	{
		printf("\n---fast & breif---\n");

		if(frm == eVPU_FRAME_1ST)
		{
			printf("feature1 num : %d, limit count : %d\n", feature[eVPU_FRAME_1ST].number, limitFeatureCount);
			num = &briefNum1;
		}
		else
		{
			printf("feature2 num ; %d, limit count : %d\n", feature[eVPU_FRAME_2ND].number, limitFeatureCount);
			num = &briefNum2;
		}

		//print_vpu_reg();
		//print_feature_point((uint32 *)feature[frm].fast, (int32)feature[frm].number);
		
		pFast = (vuint32 *)feature[frm].fast;
		pBrief = (vuint32 *)feature[frm].brief;
		for(i=0 ; i<*num ; i++)
		{
			// H->L
			printf("%d. ( %d,\t%d) = %08x%08x%08x%08x\n",
				i+1,
				(pFast[i]>>16)&0xFFFF, pFast[i]&0xFFFF, pBrief[3], pBrief[2], pBrief[1], pBrief[0]);

			pBrief += 4;

		}
	}
#endif
#if defined(USE_HD_PRINT)
	#if defined(USE_EX_REG_HAMMING)
	printf("\n---reg hamming distance---\n");
	for(i=0 ; i<briefNum1 ; i++)
	{
		for(j=0 ; j<briefNum2 ; j++)
		{
			printf("%02X ", hamming[j]);
		}
		hamming += (((briefNum2 + 7) >> 3) << 3);
		printf("\n");
	}
	#elif defined(USE_EX_MEM_HAMMING)
	printf("\n---mem hamming distance---\n");
	for(i=0 ; i<((briefNum1<=briefNum2)?briefNum1:briefNum2) ; i++)
	{
		if( ((i%16) == 0) && (i!=0) )
		{
			printf("\n");
		}
		
		printf("%02X ", hamming[i]);
	}
	#endif
	printf("\n");
#endif // USE_HD_PRINT
	}

	VPU_PRINT("VPU example end\n");
}

void vpu_test_copy (void)
{
	uint32 *src, *dst;
	
	VPU_PRINT("VPU dma copy example start\n");

	src = (uint32 *)0x20100000;
	dst = (uint32 *)0x20300000;
	VPUAPI_FB_DMA_copy(src, dst, 1280, 720);

	VPU_PRINT("VPU dma copy example end\n");
}


/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_115200, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE, NULL);
	SYSAPI_DU_initialize();
}

#define USE_REG_HD
/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
int main()
{
	system_initialize();
	print_init();
	
	MAIN_PRINT("main begin\n");

	//-------------------------->>
	// example using API for user.
//	vpu_test_copy();
	while(1){
		vpu_test();
		_delay(0x8000000);
	}

	//--------------------------<<
	//-------------------------->>
	// test for verification. (to do delete)
	//vputest_scenario();
	//vputest_reset_reg();
	//vputest_fast_brief_dma();
	//vputest_hamming_1reg_mem_dma();
	//vputest_hamming_reg_mem_dma();
	//vputest_hamming_mem_mem_dma();
	//vputest_copy_dma();
	//vputest_copy_2d_dma();
	//vputest_brief_lut();
	//vputest_fast_roi_dma();
	//vputest_fast_zone_dma();
	//vputest_fast_scale_dma();
	//vputest_scenario_loop();
	//--------------------------<<
	
	MAIN_PRINT("main end\n");

	while(1);

	return 1;
}
