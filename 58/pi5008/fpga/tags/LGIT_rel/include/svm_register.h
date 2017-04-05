/**
 * \file
 *
 * \brief	SVM Register Map
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

#ifndef __SVMREGISTER_H__
#define __SVMREGISTER_H__

#include "type.h"
#include "system.h"

/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef struct tagSVM_REG_T
{
	vuint32 enable;						//0x000	// [0] SVM_EN
												// [1] READ_SYNC_MOD
	vuint32 syncCtrl;					//0x004	// [15:0] OUT_LINE_DELAY
												// [23:16] PIP_LINE_DELAY
	vuint32 inputSize;					//0x008	// [10:0] SVM_HWIDTH
												// [26:16] SVM_VHEIGHT
	vuint32 outputSize;					//0x00c	// [10:0] SVM_HWIDTH_OUT
												// [26:16] SVM_VHEIGHT_OUT
	vuint32 blank;						//0x010	// [15:0] SVM_HBLANK
												// [31:16] SVM_VBLANK
	vuint32 wmain;						//0x014	// [0] WMAIN_EN : Front
												// [1] WMAIN_WRAP_EN
												// [2] WMAIN_WRAP_INIT_EN
												// [7:4] WMAIN_BURST_CTRL
												// [8] WMAIN_EN1 : Left
												// [9] WMAIN_EN2 : Right
												// [10] WMAIN_EN3 : Back
	vuint32 wline0;						//0x018	// [0:12] C0_WMAIN_WLINE
												// [16:28] C1_WMAIN_WLINE
	vuint32 wline1;						//0x01c	// [0:12] C2_WMAIN_WLINE
												// [16:28] C3_WMAIN_WLINE
	vuint32 wmainState;					//0x020	// [0] WMAIN_OVERFLOW
												// [1] WMAIN_FRAME_DONE
												// [28:16] WMAIN_CURRENT_LINE
	vuint32 wmainCurAddr;				//0x024	// [31:0] WMAIN_CURRENT_ADDR
	vuint32 rmain;						//0x028	// [0] RMAIN_EN : Front/Back
												// [1] RMAIN_READ_MODE
												// [7:4] RMAIN_BURST_CTRL
												// [8] RMAIN_EN1 : Left/Right
	vuint32 rmainState;					//0x02c	// [0] RMAIN_UNDERFLOW
												// [1] RMAIN_FRMAE_DONE
	vuint32 camWmainAddr[4];			//0x030	// [31:0] CAM#_WMAIN_START_ADDR
	vuint32 fbLutAddr[2];				//0x040	// [31:0] LUT0_START_ADDR#
	vuint32 lrLutAddr[2];				//0x048	// [31:0] LUT1_START_ADDR#
	vuint32 chCtrl;						//0x050	// [0] FB_bypss_en
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
	vuint32 samplingCnt;				//0x054	// [7:0] TXCNT
												// [15:8] TYCNT_1
												// [23:16] SOFFSET
												// [31:24] TOFFSET
	vuint32 samplingDistance;			//0x058	// [7:0] DSTW_1
												// [31:16] INCR
	vuint32 sizeRatio;					//0x05c	// [9:0] hscl_main_org. (width/width_out) * 2^8
												// [25:16] vscl_main_org. (height/height_out) * 2^8
	vuint32 sclDownHoldCnt[2];			//0x060	// [0] [10:0] no_scale_down_hold_cnt
												// [0] [26:16] 1/2_scale_down_hold_cnt
												// [1] [10:0] 1/4_scale_down_hold_cnt
												// [1] [26:16] 1/8_scale_down_hold_cnt
	vuint32 carAddr;					//0x068	// [31:0] CAR_START_ADDR
	vuint32 carX;						//0x06c	// [10:0] CAR_XST
												// [24:16] CAR_WID_M1
	vuint32 carY;						//0x070	// [10:0] CAR_YST
												// [24:16] CAR_HEI_M1
												// [31] CAR_BLEND_EN
	vuint32 carCtrl;					//0x074	// [23:0] CAR_MASK_COLOR
												// [30:24] CAR_BURST_CTRL
	vuint32 maskColor;					//0x078	// [23:0] MASK_COLOR
	vuint32 bgColor;					//0x07c	// [23:0] BG_COLOR
	vuint32 section0X;					//0x080	// [10:0] SECT0_XST
												// [26:16] SECT0_WID_M1
	vuint32 section0Y;					//0x084	// [10:0] SECT0_YST
												// [26:16] SECT0_HEI_M1
												// [31] SECT0_EN
	vuint32 section1X;					//0x088	// [10:0] SECT1_XST
												// [26:16] SECT1_WID_M1
	vuint32 section1Y;					//0x08c	// [10:0] SECT1_YST
												// [26:16] SECT1_HEI_M1
												// [31] SECT1_EN
	vuint32 roi[4];						//0x090	// [10:0] C#_START_LINE
												// [26:16] C#_END_LINE
												// [31] C#_AREA_EN
	vuint32 boundary[4];				//0x0a0	// [8:0] cam#_boundary0
												// [17:9] cam#_boundary1
												// [26:18] cam#_boundary2
												// [31] cam#_vscl_down
	vuint32 boundaryScale;				//0x0b0	// [1:0] cam0_range0_scale
												// [3:2] cam0_range1_scale
												// [5:4] cam0_range2_scale
												// [7:6] cam0_range3_scale
												// [9:8] cam1_range0_scale
												// [11:10] cam1_range1_scale
												// [13:12] cam1_range2_scale
												// [15:14] cam1_range3_scale
												// [17:16] cam2_range0_scale
												// [19:18] cam2_range1_scale
												// [21:20] cam2_range2_scale
												// [23:22] cam2_range3_scale
												// [25:24] cam3_range0_scale
												// [27:26] cam3_range1_scale
												// [29:28] cam3_range2_scale
												// [31:30] cam3_range3_scale
	vuint32 outMode;					//0x0b4	// [1:0] out_mode
												// [2] lut_mode
												// [3] blend_en
												// [4] alpha_mode
												// [5] bc_mode
												// [6] bc_en
												// [7] invalid_gen
												// [15:8] BC_ratio
												// [16] auto_morp_en
												// [19:17] morp_speed
												// [20] morp_dir
												// [24] gain_mode
												// [25:30] fixed_gain
	vuint32 bcLutAddr;					//0x0b8	// [31:0] LUT4BC_START_ADDR
	vuint32 compression;				//0x0bc	// [0] C0_COMPRESS_EN
												// [1] C0_COMPRESS_RATIO
												// [2] C1_COMPRESS_EN
												// [3] C1_COMPRESS_RATIO
												// [4] C2_COMPRESS_EN
												// [5] C2_COMPRESS_RATIO
												// [6] C3_COMPRESS_EN
												// [7] C3_COMPRESS_RATIO
	vuint32 mbwIntr;					//0x0c0	// [2:0] fill_cnt
												// [4] FB_mbw_intr
												// [5] LR_mbw_intr
}SVM_REG_T;

typedef union tagSVMSTAT_REG_U
{
	vuint32 stat[2][4][3];				// [FB, LR][FL,FR, BR, BL][R, G, B]

	struct
	{	vuint32 flStatOfFB[3];			//0x000	// [0] [25:0] FL_R0
												// [1] [25:0] FL_G0
												// [2] [25:0] FL_B0
		vuint32 frStatOfFB[3];			//0x00c	// [0] [25:0] FR_R0
												// [1] [25:0] FR_G0
												// [2] [25:0] FR_B0
		vuint32 brStatOfFB[3];			//0x018	// [0] [25:0] BR_R0
												// [1] [25:0] BR_G0
												// [2] [25:0] BR_B0
		vuint32 blStatOfFB[3];			//0x024	// [0] [25:0] BL_R0
												// [1] [25:0] BL_G0
												// [2] [25:0] BL_B0
		vuint32 flStatOfLR[3];			//0x030	// [0] [25:0] FL_R1
												// [1] [25:0] FL_G1
												// [2] [25:0] FL_B1
		vuint32 frStatOfLR[3];			//0x03c	// [0] [25:0] FR_R1
												// [1] [25:0] FR_G1
												// [2] [25:0] FR_B1
		vuint32 brStatOfLR[3];			//0x048	// [0] [25:0] BR_R1
												// [1] [25:0] BR_G1
												// [2] [25:0] BR_B1
		vuint32 blStatOfLR[3];			//0x054	// [0] [25:0] BL_R1
												// [1] [25:0] BL_G1
												// [2] [25:0] BL_B1
	} fld;
} SVMSTAT_REG_U;

typedef struct tagSVMCOEF_REG_T
{
	vuint32 coef1[3];					//0x000	// [0] [7:0] COEF1_FL4R
												// [0] [15:8] COEF1_FR4R
												// [0] [23:16] COEF1_BR4R
												// [0] [31:24] COEF1_BL4R
												// [1] [7:0] COEF1_FL4G
												// [1] [15:8] COEF1_FR4G
												// [1] [23:16] COEF1_BR4G
												// [1] [31:24] COEF1_BL4G
												// [2] [7:0] COEF1_FL4B
												// [2] [15:8] COEF1_FR4B
												// [2] [23:16] COEF1_BR4B
												// [2] [31:24] COEF1_BL4B
	vuint32 coef3[3];					//0x00c	// [0] [7:0] COEF3_FL4R
												// [0] [15:8] COEF3_FR4R
												// [0] [23:16] COEF3_BR4R
												// [0] [31:24] COEF3_BL4R
												// [1] [7:0] COEF3_FL4G
												// [1] [15:8] COEF3_FR4G
												// [1] [23:16] COEF3_BR4G
												// [1] [31:24] COEF3_BL4G
												// [2] [7:0] COEF3_FL4B
												// [2] [15:8] COEF3_FR4B
												// [2] [23:16] COEF3_BR4B
												// [2] [31:24] COEF3_BL4B
	vuint32 coef2n4[3];					//0x018	// [0] [7:0] COEF2_F4R
												// [0] [15:8] COEF2_B4R
												// [0] [23:16] COEF4_L4R
												// [0] [31:24] COEF4_R4R
												// [1] [7:0] COEF2_F4G
												// [1] [15:8] COEF2_B4G
												// [1] [23:16] COEF4_L4G
												// [1] [31:24] COEF4_R4G
												// [2] [7:0] COEF2_F4B
												// [2] [15:8] COEF2_B4B
												// [2] [23:16] COEF4_L4B
												// [2] [31:24] COEF4_R4B
} SVMCOEF_REG_T;

typedef struct tagSVMCOLORCOEF_REG_T
{
	vuint32 r2y4y;						//0x000	// [7:0] coefr4y
												// [15:8] coefg4y
												// [23:16] coefb4y
												// [24] with128
	vuint32 r2y4cb;						//0x004	// [7:0] coefr4cb
												// [15:8] coefg4cb
												// [23:16] coefb4cb
	vuint32 r2y4cr;						//0x008	// [7:0] coefr4cr
												// [15:8] coefg4cr
												// [23:16] coefb4cr
	vuint32 y2r4g;						//0x00c	// [7:0] coefcb4g
												// [15:8] coefcr4g
	vuint32 y2r4rnb;					//0x010	// [9:0] coefcr4r
												// [25:16] coefcb4b
} SVMCOLORCOEF_REG_T;

typedef struct tagSVM_DYNAMIC_BLEND_REG_U
{
	vuint32		var;

	struct
	{
		vuint32	coef1	:	8,
				coef3	:	8,
				coef6	:	8,
				coef8	:	8;
	}fld;
} SVM_DYNAMIC_BLEND_REG_U;

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/

#define SVM_CTRL_REG			((SVM_REG_T*)				(SVM_BASE_ADDR + 0x0000))
#define SVM_STAT_REG			((SVMSTAT_REG_U*)			(SVM_BASE_ADDR + 0x0100))
#define SVM_COEF_REG			((SVMCOEF_REG_T*)			(SVM_BASE_ADDR + 0x0160))
#define SVM_COLOR_COEF_REG		((SVMCOLORCOEF_REG_T*)		(SVM_BASE_ADDR + 0x0184))
#define SVM_DYNAMIC_BLEND_REG	((SVM_DYNAMIC_BLEND_REG_U*)	(SVM_BASE_ADDR + 0x0198))

#endif //__SVMREGISTER_H__
