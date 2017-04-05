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

#include "svm_test_LUT4FrontBack.h"
#include "svm_test_LUT4LeftRight.h"
#include "svm_test_LUT4BC.h"
#include "svm_test_carImage.h"
#include "svm_register.h"

// Address for SVM test
#define SVM_EXAMPLE_BASE_ADDR				(0x24000000)

#define SVM_EXAMPLE_CAM0_ADDR				(SVM_EXAMPLE_BASE_ADDR + 0x00000000)
#define SVM_EXAMPLE_CAM1_ADDR				(SVM_EXAMPLE_BASE_ADDR + 0x01000000)
#define SVM_EXAMPLE_CAM2_ADDR				(SVM_EXAMPLE_BASE_ADDR + 0x02000000)
#define SVM_EXAMPLE_CAM3_ADDR				(SVM_EXAMPLE_BASE_ADDR + 0x03000000)

#define SVM_EXAMPLE_LUT4FRONTBACK_ADDR		(SVM_EXAMPLE_BASE_ADDR + 0x04000000)		// size : 58968 byte ( (161+1) * 91) : Add 1 for make it even
#define SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR		(SVM_EXAMPLE_BASE_ADDR + 0x04010000)		// size : 58968 byte
#define SVM_EXAMPLE_LUT4BC_ADDR				(SVM_EXAMPLE_BASE_ADDR + 0x04020000)		// size : 58968 byte
#define SVM_EXAMPLE_CARIMAGE_ADDR			(SVM_EXAMPLE_BASE_ADDR + 0x04030000)		// size : 109440 byte

enum tagSVM_TEST_E
{
	eSVM_TEST_TOPVIEW = 0,
	eSVM_TEST_BYPASS_FRONT,
	eSVM_TEST_BYPASS_LEFT,
	eSVM_TEST_BYPASS_RIGHT,
	eSVM_TEST_BYPASS_BACK,
};

static void svm_test(uint8 mode)
{
	int i = 0;
	uint32 *p;

	MAIN_PRINT("SVM : Start svm_test\n");

	/********************************************************************************
	* Disable SVM
	********************************************************************************/
	SVM_CTRL_REG->wmain = 0x00000000;
	SVM_CTRL_REG->rmain = 0x00000000;
	SVM_CTRL_REG->enable = 0x00000000;


	MAIN_PRINT("SVM : LUT to DRAM\n");
	/********************************************************************************
	* LUT to DRAM
	********************************************************************************/
	// LUT4FrontBack
	p = (uint32*)SVM_EXAMPLE_LUT4FRONTBACK_ADDR;
	for ( i = 0; i < sizeof(svm_test_LUT4FrontBack) / 4; i++ )
	{
		*(p+i) = svm_test_LUT4FrontBack[i];
	}

	// LUT4LeftRight
	p = (uint32*)SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR;
	for ( i = 0; i < sizeof(svm_test_LUT4LeftRight) / 4; i++ )
	{
		*(p+i) = svm_test_LUT4LeftRight[i];
	}

	// LUT4BC
	p = (uint32*)SVM_EXAMPLE_LUT4BC_ADDR;
	for ( i = 0; i < sizeof(svm_test_LUT4BC) / 4; i++ )
	{
		*(p+i) = svm_test_LUT4BC[i];
	}

	// car image
	p = (uint32*)SVM_EXAMPLE_CARIMAGE_ADDR;
	for ( i = 0; i < sizeof(svm_test_carImage) / 4; i++ )
	{
		*(p+i) = svm_test_carImage[i];
	}

	/********************************************************************************
	* LUT to DRAM (check)
	********************************************************************************/
	{
		uint32 errCnt[4] = {0,};

		// LUT4FrontBack
		p = (uint32*)SVM_EXAMPLE_LUT4FRONTBACK_ADDR;
		for ( i = 0; i < sizeof(svm_test_LUT4FrontBack) / 4; i++ )
		{
			if ( *(p+i) != svm_test_LUT4FrontBack[i] )
				errCnt[0]++;
		}

		// LUT4LeftRight
		p = (uint32*)SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR;
		for ( i = 0; i < sizeof(svm_test_LUT4LeftRight) / 4; i++ )
		{
			if ( *(p+i) != svm_test_LUT4LeftRight[i] )
				errCnt[1]++;
		}

		// LUT4BC
		p = (uint32*)SVM_EXAMPLE_LUT4BC_ADDR;
		for ( i = 0; i < sizeof(svm_test_LUT4BC) / 4; i++ )
		{
			if ( *(p+i) != svm_test_LUT4BC[i] )
				errCnt[2]++;
		}

		// car image
		p = (uint32*)SVM_EXAMPLE_CARIMAGE_ADDR;
		for ( i = 0; i < sizeof(svm_test_carImage) / 4; i++ )
		{
			if ( *(p+i) != svm_test_carImage[i] )
				errCnt[3]++;
		}

		if ( errCnt[0] != 0 )
			MAIN_PRINT("[LUT to DRAM] LUT4FrontBack : total (%d), failed(%d)\n", (int32)(sizeof(svm_test_LUT4FrontBack) / 4), (int32)errCnt[0]);
		if ( errCnt[1] != 0 )
			MAIN_PRINT("[LUT to DRAM] LUT4LeftRight : total (%d), failed(%d)\n", (int32)(sizeof(svm_test_LUT4LeftRight) / 4), (int32)errCnt[1]);
		if ( errCnt[2] != 0 )
			MAIN_PRINT("[LUT to DRAM] LUT4BC : total (%d), failed(%d)\n", (int32)(sizeof(svm_test_LUT4BC) / 4), (int32)errCnt[2]);
		if ( errCnt[3] != 0 )
			MAIN_PRINT("[LUT to DRAM] CarImage : total (%d), failed(%d)\n", (int32)(sizeof(svm_test_carImage) / 4), (int32)errCnt[3]);
	}

	/********************************************************************************
	* Set register for top view
	********************************************************************************/
	SVM_CTRL_REG->syncCtrl			= 0x80130000;					// [15:0] OUT_LINE_DELAY : 0
																	// [23:16] PIP_LINE_DELAY : 19
	SVM_CTRL_REG->inputSize			= 0x02d00500;					// [10:0] SVM_HWIDTH : 1280
																	// [26:16] SVM_VHEIGHT : 720
	SVM_CTRL_REG->outputSize		= 0x02d00500;					// [10:0] SVM_HWIDTH_OUT : 1280
																	// [26:16] SVM_VHEIGHT_OUT : 720
	SVM_CTRL_REG->blank				= 0x003c09e4;					// [15:0] SVM_HBLANK : 2532
																	// [31:16] SVM_VBLANK : 60
	SVM_CTRL_REG->wline0			= 0x08700870;					// [0:12] C0_WMAIN_WLINE : 2160
																	// [16:28] C1_WMAIN_WLINE : 2160
	SVM_CTRL_REG->wline1			= 0x08700870;					// [0:12] C2_WMAIN_WLINE : 2160
																	// [16:28] C3_WMAIN_WLINE : 1800
	SVM_CTRL_REG->camWmainAddr[0]	= SVM_EXAMPLE_CAM0_ADDR;		// 5529600 byte = 1280 x 720 x 2 x 3
	SVM_CTRL_REG->camWmainAddr[1]	= SVM_EXAMPLE_CAM1_ADDR;
	SVM_CTRL_REG->camWmainAddr[2]	= SVM_EXAMPLE_CAM2_ADDR;
	SVM_CTRL_REG->camWmainAddr[3]	= SVM_EXAMPLE_CAM3_ADDR;

	SVM_CTRL_REG->fbLutAddr[0]		= SVM_EXAMPLE_LUT4FRONTBACK_ADDR;
	SVM_CTRL_REG->fbLutAddr[1]		= SVM_EXAMPLE_LUT4FRONTBACK_ADDR;
	SVM_CTRL_REG->lrLutAddr[0]		= SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR;
	SVM_CTRL_REG->lrLutAddr[1]		= SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR;

	if ( mode == eSVM_TEST_BYPASS_FRONT )
		SVM_CTRL_REG->chCtrl		= 0x00000001;
	else if ( mode == eSVM_TEST_BYPASS_LEFT )
		SVM_CTRL_REG->chCtrl		= 0x00000004;
	else if ( mode == eSVM_TEST_BYPASS_RIGHT )
		SVM_CTRL_REG->chCtrl		= 0x0000000C;
	else if ( mode == eSVM_TEST_BYPASS_BACK )
		SVM_CTRL_REG->chCtrl		= 0x00000003;
	else
		SVM_CTRL_REG->chCtrl		= 0x00000000;
																	// [0] FB_bypss_en
																	// [1] FB_bypass_sel
																	// [2] LR_bypass_en
																	// [3] LR_bypass_sel
																	// [8] hmirror_en
																	// [9] vmirror_en
																	// [10] hmirror_en1
																	// [11] vmirror_en1
																	// [12] hmirror_en2
																	// [13] vmirror_en2
																	// [14] hmirror_en3
																	// [15] vmirror_en3
																	// [23:16] lut_ratio. lut ratio for F/B LUT
																	// [31:24] lut_ratio2. lut ratio for L/R LUT
	SVM_CTRL_REG->samplingCnt		= 0x00005aa1;					// [7:0] TXCNT : 161
																	// [15:8] TYCNT_1 : 90
																	// [23:16] SOFFSET : 0
																	// [31:24] TOFFSET : 0
	SVM_CTRL_REG->samplingDistance	= 0x10000007;					// [7:0] DSTW_1 : 7
																	// [31:16] INCR : 4096
	SVM_CTRL_REG->sizeRatio			= 0x01000100;					// [9:0] hscl_main_org. (width/width_out) * 2^8 : 256
																	// [25:16] vscl_main_org. (height/height_out) * 2^8 : 256
	SVM_CTRL_REG->sclDownHoldCnt[0]	= 0x0258012c;					// [0] [10:0] no_scale_down_hold_cnt : 300
																	// [0] [26:16] 1/2_scale_down_hold_cnt : 600
	SVM_CTRL_REG->sclDownHoldCnt[1]	= 0x05dc04b0;					// [1] [10:0] 1/4_scale_down_hold_cnt : 1200
																	// [1] [26:16] 1/8_scale_down_hold_cnt : 1500
	SVM_CTRL_REG->carAddr			= SVM_EXAMPLE_CARIMAGE_ADDR;
	SVM_CTRL_REG->carX				= 0x009703f2;					// [10:0] CAR_XST : 1010
																	// [24:16] CAR_WID_M1 : 151
	SVM_CTRL_REG->carY				= 0x816700ac;					// [10:0] CAR_YST : 172
																	// [24:16] CAR_HEI_M1 : 359
																	// [31] CAR_BLEND_EN : 1
	SVM_CTRL_REG->carCtrl			= 0x40ff554c;					// [23:0] CAR_MASK_COLOR : y 0xff, cb 0x55, cr 0x4c
																	// [30:24] CAR_BURST_CTRL : 64
	SVM_CTRL_REG->maskColor			= 0x00000000;					// [23:0] MASK_COLOR
	SVM_CTRL_REG->bgColor			= 0x00008080;					// [23:0] BG_COLOR : y 0, cb 128, cr 128 -> black
	SVM_CTRL_REG->boundary[0]		= 0x81f49632;					// [8:0] cam#_boundary0 : 50
																	// [17:9] cam#_boundary1 : 75
																	// [26:18] cam#_boundary2 : 125
																	// [31] cam#_vscl_down : 1
	SVM_CTRL_REG->boundary[1]		= 0x81f49632;
	SVM_CTRL_REG->boundary[2]		= 0x81f49632;
	SVM_CTRL_REG->boundary[3]		= 0x81f49632;
	if ( mode == eSVM_TEST_TOPVIEW )
		SVM_CTRL_REG->boundaryScale		= 0x55555555;
	else
		SVM_CTRL_REG->boundaryScale		= 0x00000000;
																	// [1:0] cam0_range0_scale : 1 -> 1/2 sclae
																	// [3:2] cam0_range1_scale : 1
																	// [5:4] cam0_range2_scale : 1
																	// [7:6] cam0_range3_scale : 1
																	// [9:8] cam1_range0_scale : 1
																	// [11:10] cam1_range1_scale : 1
																	// [13:12] cam1_range2_scale : 1
																	// [15:14] cam1_range3_scale : 1
																	// [17:16] cam2_range0_scale : 1
																	// [19:18] cam2_range1_scale : 1
																	// [21:20] cam2_range2_scale : 1
																	// [23:22] cam2_range3_scale : 1
																	// [25:24] cam3_range0_scale : 1
																	// [27:26] cam3_range1_scale : 1
																	// [29:28] cam3_range2_scale : 1
																	// [31:30] cam3_range3_scale : 1

	if ( mode == eSVM_TEST_BYPASS_FRONT || mode == eSVM_TEST_BYPASS_BACK )
		SVM_CTRL_REG->outMode		= 0x00000000;
	else if ( mode == eSVM_TEST_BYPASS_LEFT || mode == eSVM_TEST_BYPASS_RIGHT )
		SVM_CTRL_REG->outMode		= 0x00000001;
	else
		SVM_CTRL_REG->outMode		= 0x0040ff7f;
																	// [1:0] out_mode : 2
																	// [2] lut_mode : 1
																	// [3] blend_en : 1
																	// [4] alpha_mode : 1
																	// [5] bc_mode : 0
																	// [6] bc_en : 1
																	// [7] invalid_gen : 0
																	// [15:8] BC_ratio : 0xff
																	// [16] auto_morp_en : 0
																	// [19:17] morp_speed : 0
																	// [20] morp_dir : 0
																	// [22] dynamic_blend_en : 1
																	// [23] edge_en : 0
																	// [24] gain_mode : 0
																	// [25:30] fixed_gain : 0
	SVM_CTRL_REG->bcLutAddr			= SVM_EXAMPLE_LUT4BC_ADDR;
	SVM_CTRL_REG->compression		= 0x00000055;					// [0] C0_COMPRESS_EN : 1
																	// [1] C0_COMPRESS_RATIO : 0
																	// [2] C1_COMPRESS_EN : 1
																	// [3] C1_COMPRESS_RATIO : 0
																	// [4] C2_COMPRESS_EN : 1
																	// [5] C2_COMPRESS_RATIO : 0
																	// [6] C3_COMPRESS_EN : 1
																	// [7] C3_COMPRESS_RATIO : 0
	SVM_CTRL_REG->section0X			= 0x03670000;					// [10:0] SECT0_XST : 0
																	// [26:16] SECT0_WID_M1 : 871
	if ( mode == eSVM_TEST_TOPVIEW )
		SVM_CTRL_REG->section0Y		= 0x824e0000;
	else
		SVM_CTRL_REG->section0Y		= 0x024e0000;
																	// [10:0] SECT0_YST : 0
																	// [26:16] SECT0_HEI_M1 : 590
																	// [31] SECT0_EN : 0 or 1
	SVM_CTRL_REG->section1X			= 0x018f0370;					// [10:0] SECT1_XST : 880
																	// [26:16] SECT1_WID_M1 : 399
	if ( mode == eSVM_TEST_TOPVIEW )
		SVM_CTRL_REG->section1Y		= 0x82cf0000;
	else
		SVM_CTRL_REG->section1Y		= 0x02cf0000;
																	// [10:0] SECT1_YST : 0
																	// [26:16] SECT1_HEI_M1 : 719
																	// [31] SECT1_EN : 0 or 1
	SVM_CTRL_REG->roi[0]			= 0x00000000;
	SVM_CTRL_REG->roi[1]			= 0x00000000;
	SVM_CTRL_REG->roi[2]			= 0x00000000;
	SVM_CTRL_REG->roi[3]			= 0x00000000;

	SVM_DYNAMIC_BLEND_REG->var		= 0xffffffff;

	if ( mode == eSVM_TEST_BYPASS_FRONT )
		SVM_CTRL_REG->wmain			= 0x00000073;
	else if ( mode == eSVM_TEST_BYPASS_LEFT )
		SVM_CTRL_REG->wmain			= 0x00000172;
	else if ( mode == eSVM_TEST_BYPASS_RIGHT )
		SVM_CTRL_REG->wmain			= 0x00000272;
	else if ( mode == eSVM_TEST_BYPASS_BACK )
		SVM_CTRL_REG->wmain			= 0x00000472;
	else
		SVM_CTRL_REG->wmain			= 0x00000773;
																	// [0] WMAIN_EN : 1
																	// [1] WMAIN_WRAP_EN : 1
																	// [2] WMAIN_WRAP_INIT_EN : 0
																	// [7:4] WMAIN_BURST_CTRL : 7 - 64
																	// [8] WMAIN_EN1 : 1
																	// [9] WMAIN_EN2 : 1
																	// [10] WMAIN_EN3 : 1
	if ( mode == eSVM_TEST_BYPASS_FRONT || mode == eSVM_TEST_BYPASS_BACK )
		SVM_CTRL_REG->rmain			= 0x00000140;
	else if ( mode == eSVM_TEST_BYPASS_LEFT || mode == eSVM_TEST_BYPASS_RIGHT )
		SVM_CTRL_REG->rmain			= 0x00000041;
	else
		SVM_CTRL_REG->rmain			= 0x00000141;
																	// [0] RMAIN_EN : 1
																	// [1] RMAIN_READ_MODE : 0
																	// [7:4] RMAIN_BURST_CTRL : 4 - 8
																	// [8] RMAIN_EN1 : 1
	SVM_CTRL_REG->enable			= 0x00000007;					// [0] SVM_EN : 1
																	// [1] READ_SYNC_MOD : 1


	/********************************************************************************
	* Set register for top view (check)
	********************************************************************************/
	{
		uint32 errCnt = 0;
		uint32 totalCnt = 0;

		totalCnt++;
		if ( SVM_CTRL_REG->syncCtrl	 != 0x80130000 )
		{
			MAIN_PRINT("syncCtrl failed : 0x%08x -> 0x%08x\n", 0x80130000, SVM_CTRL_REG->syncCtrl);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->inputSize != 0x02d00500 )
		{
			MAIN_PRINT("inputSize failed : 0x%08x -> 0x%08x\n", 0x02d00500, SVM_CTRL_REG->inputSize);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->blank != 0x003c09e4 )
		{
			MAIN_PRINT("blank failed : 0x%08x -> 0x%08x\n", 0x003c09e4, SVM_CTRL_REG->blank);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->wline0 != 0x08700870 )
		{
			MAIN_PRINT("wline0 failed : 0x%08x -> 0x%08x\n", 0x07800780, SVM_CTRL_REG->wline0);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->wline1 != 0x08700870 )
		{
			MAIN_PRINT("wline1 failed : 0x%08x -> 0x%08x\n", 0x07800780, SVM_CTRL_REG->wline1);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->camWmainAddr[0]	!= SVM_EXAMPLE_CAM0_ADDR )
		{
			MAIN_PRINT("camWmainAddr[0] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_CAM0_ADDR, SVM_CTRL_REG->camWmainAddr[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->camWmainAddr[1]	!= SVM_EXAMPLE_CAM1_ADDR )
		{
			MAIN_PRINT("camWmainAddr[1] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_CAM1_ADDR, SVM_CTRL_REG->camWmainAddr[1]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->camWmainAddr[2]	!= SVM_EXAMPLE_CAM2_ADDR )
		{
			MAIN_PRINT("camWmainAddr[2] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_CAM2_ADDR, SVM_CTRL_REG->camWmainAddr[2]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->camWmainAddr[3]	!= SVM_EXAMPLE_CAM3_ADDR )
		{
			MAIN_PRINT("camWmainAddr[3] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_CAM3_ADDR, SVM_CTRL_REG->camWmainAddr[3]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->fbLutAddr[0] != SVM_EXAMPLE_LUT4FRONTBACK_ADDR )
		{
			MAIN_PRINT("fbLutAddr[0] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_LUT4FRONTBACK_ADDR, SVM_CTRL_REG->fbLutAddr[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->fbLutAddr[1]	 != SVM_EXAMPLE_LUT4FRONTBACK_ADDR )
		{
			MAIN_PRINT("fbLutAddr[1] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_LUT4FRONTBACK_ADDR, SVM_CTRL_REG->fbLutAddr[1]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->lrLutAddr[0] != SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR )
		{
			MAIN_PRINT("lrLutAddr[1] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR, SVM_CTRL_REG->lrLutAddr[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->lrLutAddr[1] != SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR )
		{
			MAIN_PRINT("lrLutAddr[1] failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_LUT4LEFTRIGHT_ADDR, SVM_CTRL_REG->lrLutAddr[1]);
			errCnt++;
		}

		totalCnt++;
		switch (mode)
		{
		case eSVM_TEST_BYPASS_FRONT:
			if ( SVM_CTRL_REG->chCtrl != 0x00000001 )
			{
				MAIN_PRINT("chCtrl failed : 0x%08x -> 0x%08x\n", 0x00000001, SVM_CTRL_REG->chCtrl);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_LEFT:
			if ( SVM_CTRL_REG->chCtrl != 0x00000004 )
			{
				MAIN_PRINT("chCtrl failed : 0x%08x -> 0x%08x\n", 0x00000004, SVM_CTRL_REG->chCtrl);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_RIGHT:
			if ( SVM_CTRL_REG->chCtrl != 0x0000000C )
			{
				MAIN_PRINT("chCtrl failed : 0x%08x -> 0x%08x\n", 0x0000000C, SVM_CTRL_REG->chCtrl);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_BACK:
			if ( SVM_CTRL_REG->chCtrl != 0x00000003 )
			{
				MAIN_PRINT("chCtrl failed : 0x%08x -> 0x%08x\n", 0x00000003, SVM_CTRL_REG->chCtrl);
				errCnt++;
			}
			break;
		default:
			if ( SVM_CTRL_REG->chCtrl != 0x00000000 )
			{
				MAIN_PRINT("chCtrl failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->chCtrl);
				errCnt++;
			}
			break;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->samplingCnt != 0x00005aa1 )
		{
			MAIN_PRINT("samplingCnt failed : 0x%08x -> 0x%08x\n", 0x00005aa1, SVM_CTRL_REG->samplingCnt);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->samplingDistance != 0x10000007 )
		{
			MAIN_PRINT("samplingDistance failed : 0x%08x -> 0x%08x\n", 0x10000007, SVM_CTRL_REG->samplingDistance);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->sizeRatio != 0x01000100 )
		{
			MAIN_PRINT("sizeRatio failed : 0x%08x -> 0x%08x\n", 0x01000100, SVM_CTRL_REG->sizeRatio);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->sclDownHoldCnt[0] != 0x0258012c )
		{
			MAIN_PRINT("sclDownHoldCnt[0] failed : 0x%08x -> 0x%08x\n", 0x0258012c, SVM_CTRL_REG->sclDownHoldCnt[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->sclDownHoldCnt[1] != 0x05dc04b0 )
		{
			MAIN_PRINT("sclDownHoldCnt[1] failed : 0x%08x -> 0x%08x\n", 0x05dc04b0, SVM_CTRL_REG->sclDownHoldCnt[1]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->carAddr != SVM_EXAMPLE_CARIMAGE_ADDR )
		{
			MAIN_PRINT("carAddr failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_CARIMAGE_ADDR, SVM_CTRL_REG->carAddr);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->carX	!= 0x009703f2 )
		{
			MAIN_PRINT("carX failed : 0x%08x -> 0x%08x\n", 0x009703f2, SVM_CTRL_REG->carX);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->carY != 0x816700ac )
		{
			MAIN_PRINT("carY failed : 0x%08x -> 0x%08x\n", 0x816700ac, SVM_CTRL_REG->carY);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->carCtrl != 0x40ff554c )
		{
			MAIN_PRINT("carCtrl failed : 0x%08x -> 0x%08x\n", 0x40ff554c, SVM_CTRL_REG->carCtrl);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->maskColor != 0x00000000 )
		{
			MAIN_PRINT("maskColor failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->maskColor);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->bgColor != 0x00008080 )
		{
			MAIN_PRINT("bgColor failed : 0x%08x -> 0x%08x\n", 0x00008080, SVM_CTRL_REG->bgColor);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->boundary[0] != 0x81f49632 )
		{
			MAIN_PRINT("boundary[0] failed : 0x%08x -> 0x%08x\n", 0x81f49632, SVM_CTRL_REG->boundary[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->boundary[1] != 0x81f49632 )
		{
			MAIN_PRINT("boundary[1] failed : 0x%08x -> 0x%08x\n", 0x81f49632, SVM_CTRL_REG->boundary[1]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->boundary[2] != 0x81f49632 )
		{
			MAIN_PRINT("boundary[2] failed : 0x%08x -> 0x%08x\n", 0x81f49632, SVM_CTRL_REG->boundary[2]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->boundary[3] != 0x81f49632 )
		{
			MAIN_PRINT("boundary[3] failed : 0x%08x -> 0x%08x\n", 0x81f49632, SVM_CTRL_REG->boundary[3]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->boundaryScale != 0x55555555 )
		{
			MAIN_PRINT("boundaryScale failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->boundaryScale);
			errCnt++;
		}

		totalCnt++;
		if ( mode == eSVM_TEST_BYPASS_FRONT || mode == eSVM_TEST_BYPASS_BACK )
		{
			if ( SVM_CTRL_REG->outMode	!= 0x00000000 )
			{
				MAIN_PRINT("outMode failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->outMode);
				errCnt++;
			}
		}
		else if ( mode == eSVM_TEST_BYPASS_LEFT || mode == eSVM_TEST_BYPASS_RIGHT )
		{
			if ( SVM_CTRL_REG->outMode	!= 0x00000001 )
			{
				MAIN_PRINT("outMode failed : 0x%08x -> 0x%08x\n", 0x00000001, SVM_CTRL_REG->outMode);
				errCnt++;
			}
		}
		else
		{
			if ( SVM_CTRL_REG->outMode	!= 0x0040ff7f && SVM_CTRL_REG->outMode	!= 0x0020ff7f )
			{
				MAIN_PRINT("outMode failed : 0x%08x -> 0x%08x\n", 0x0040ff7f, SVM_CTRL_REG->outMode);
				errCnt++;
			}
		}

		totalCnt++;
		if ( SVM_CTRL_REG->bcLutAddr != SVM_EXAMPLE_LUT4BC_ADDR )
		{
			MAIN_PRINT("bcLutAddr failed : 0x%08x -> 0x%08x\n", SVM_EXAMPLE_LUT4BC_ADDR, SVM_CTRL_REG->bcLutAddr);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->compression != 0x00000055 )
		{
			MAIN_PRINT("compression failed : 0x%08x -> 0x%08x\n", 0x00000055, SVM_CTRL_REG->compression);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->section0X != 0x03670000 )
		{
			MAIN_PRINT("section0X failed : 0x%08x -> 0x%08x\n", 0x03670000, SVM_CTRL_REG->section0X);
			errCnt++;
		}

		totalCnt++;
		if ( mode == eSVM_TEST_TOPVIEW )
		{
			if ( SVM_CTRL_REG->section0Y != 0x824e0000 )
			{
				MAIN_PRINT("section0Y failed : 0x%08x -> 0x%08x\n", 0x824e0000, SVM_CTRL_REG->section0Y);
				errCnt++;
			}
		}
		else
		{
			if ( SVM_CTRL_REG->section0Y != 0x024e0000 )
			{
				MAIN_PRINT("section0Y failed : 0x%08x -> 0x%08x\n", 0x024e0000, SVM_CTRL_REG->section0Y);
				errCnt++;
			}
		}

		totalCnt++;
		if ( SVM_CTRL_REG->section1X != 0x018f0370 )
		{
			MAIN_PRINT("section1X failed : 0x%08x -> 0x%08x\n", 0x018f0370, SVM_CTRL_REG->section1X);
			errCnt++;
		}

		totalCnt++;
		if ( mode == eSVM_TEST_TOPVIEW )
		{
			if ( SVM_CTRL_REG->section1Y != 0x82cf0000 )
			{
				MAIN_PRINT("section1Y failed : 0x%08x -> 0x%08x\n", 0x82cf0000, SVM_CTRL_REG->section1Y);
				errCnt++;
			}
		}
		else
		{
			if ( SVM_CTRL_REG->section1Y != 0x02cf0000 )
			{
				MAIN_PRINT("section1Y failed : 0x%08x -> 0x%08x\n", 0x02cf0000, SVM_CTRL_REG->section1Y);
				errCnt++;
			}
		}

		totalCnt++;
		if ( SVM_CTRL_REG->roi[0] != 0x00000000 )
		{
			MAIN_PRINT("roi[0] failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->roi[0]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->roi[1] != 0x00000000 )
		{
			MAIN_PRINT("roi[1] failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->roi[1]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->roi[2] != 0x00000000 )
		{
			MAIN_PRINT("roi[2] failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->roi[2]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_CTRL_REG->roi[3] != 0x00000000 )
		{
			MAIN_PRINT("roi[3] failed : 0x%08x -> 0x%08x\n", 0x00000000, SVM_CTRL_REG->roi[3]);
			errCnt++;
		}

		totalCnt++;
		if ( SVM_DYNAMIC_BLEND_REG->var != 0xffffffff )
		{
			MAIN_PRINT("dynamic blend failed : 0x%08x -> 0x%08x\n", 0xffffffff, SVM_DYNAMIC_BLEND_REG->var);
			errCnt++;
		}

		totalCnt++;
		switch (mode)
		{
		case eSVM_TEST_BYPASS_FRONT:
			if ( SVM_CTRL_REG->wmain != 0x00000073 )
			{
				MAIN_PRINT("wmain failed : 0x%08x -> 0x%08x\n", 0x00000073, SVM_CTRL_REG->wmain);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_LEFT:
			if ( SVM_CTRL_REG->wmain!= 0x00000172 )
			{
				MAIN_PRINT("wmain failed : 0x%08x -> 0x%08x\n", 0x00000172, SVM_CTRL_REG->wmain);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_RIGHT:
			if ( SVM_CTRL_REG->wmain != 0x00000272 )
			{
				MAIN_PRINT("wmain failed : 0x%08x -> 0x%08x\n", 0x00000272, SVM_CTRL_REG->wmain);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_BACK:
			if ( SVM_CTRL_REG->wmain != 0x00000472 )
			{
				MAIN_PRINT("wmain failed : 0x%08x -> 0x%08x\n", 0x00000472, SVM_CTRL_REG->wmain);
				errCnt++;
			}
			break;
		default:
			if ( SVM_CTRL_REG->wmain != 0x00000773 )
			{
				MAIN_PRINT("wmain failed : 0x%08x -> 0x%08x\n", 0x00000773, SVM_CTRL_REG->wmain);
				errCnt++;
			}
			break;
		}

		totalCnt++;
		switch (mode)
		{
		case eSVM_TEST_BYPASS_FRONT:
		case eSVM_TEST_BYPASS_BACK:
			if ( SVM_CTRL_REG->rmain != 0x00000140 )
			{
				MAIN_PRINT("rmain failed : 0x%08x -> 0x%08x\n", 0x00000140, SVM_CTRL_REG->rmain);
				errCnt++;
			}
			break;
		case eSVM_TEST_BYPASS_LEFT:
		case eSVM_TEST_BYPASS_RIGHT:
			if ( SVM_CTRL_REG->rmain != 0x00000041 )
			{
				MAIN_PRINT("rmain failed : 0x%08x -> 0x%08x\n", 0x00000041, SVM_CTRL_REG->rmain);
				errCnt++;
			}
			break;
		default:
			if ( SVM_CTRL_REG->rmain != 0x00000141 )
			{
				MAIN_PRINT("rmain failed : 0x%08x -> 0x%08x\n", 0x00000141, SVM_CTRL_REG->rmain);
				errCnt++;
			}
			break;
		}


		totalCnt++;
		if ( SVM_CTRL_REG->enable != 0x00000007 )
		{
			MAIN_PRINT("enable failed : 0x%08x -> 0x%08x\n", 0x00000007, SVM_CTRL_REG->enable);
			errCnt++;
		}

		if ( errCnt != 0 )
			MAIN_PRINT("Set register for top view failed: total (%d), failed(%d)\n", totalCnt++, errCnt);
	}

	switch (mode)
	{
	case eSVM_TEST_BYPASS_FRONT:
		MAIN_PRINT("SVM : Start front bypass\n");
		break;
	case eSVM_TEST_BYPASS_LEFT:
		MAIN_PRINT("SVM : Start left bypass\n");
		break;
	case eSVM_TEST_BYPASS_RIGHT:
		MAIN_PRINT("SVM : Start right bypass\n");
		break;
	case eSVM_TEST_BYPASS_BACK:
		MAIN_PRINT("SVM : Start back bypass\n");
		break;
	default:
		MAIN_PRINT("SVM : Start top view\n");
		break;
	}
}

/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_38400, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE, NULL);
	SYSAPI_DU_initialize();
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
int main()
{
	system_initialize();

	MAIN_PRINT("main begin\n");

	svm_test(eSVM_TEST_TOPVIEW);

	while(1);

	return 1;
}
