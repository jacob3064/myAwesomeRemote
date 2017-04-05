/**
 * \file
 *
 * \brief	VPU(Video Processing Unit) Register Map
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

#ifndef __VPUREGISTER_H__
#define __VPUREGISTER_H__

#include "type.h"
#include "system.h"

/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/
typedef struct tagVPUDMAMONITOR_REG_T
{
	vuint32 ctrl;					// [0] enable_ch0, enable bus monitor for ch0 
									// [1] enable_ch1, enable bus monitor for ch1 
									// [2] enable_ch2, enable bus monitor for ch2 
									// [3] enable_ch3, enable bus monitor for ch3 
									// [4] clear_ch0, clear bus monitor counter for ch0, auto-cleared and read as 0 always 
									// [5] clear_ch1, clear bus monitor counter for ch1, auto-cleared and read as 0 always 
									// [6] clear_ch2, clear bus monitor counter for ch2, auto-cleared and read as 0 always 
									// [7] clear_ch3, clear bus monitor counter for ch3, auto-cleared and read as 0 always 
 	vuint32 ch0_total;				// [31:0] ch0_total, total number of cycle after cleared and enabled 
	vuint32 ch0_word;				// [31:0] ch0_word, total number of bus word after cleared and enabled 
	vuint32 ch1_total;				// [31:0] ch1_total, total number of cycle after cleared and enabled 
 	vuint32 ch1_word;				// [31:0] ch1_word, total number of bus word after cleared and enabled 
 	vuint32 ch2_total;				// [31:0] ch2_total, total number of cycle after cleared and enabled 
	vuint32 ch2_word;				// [31:0] ch2_word, total number of bus word after cleared and enabled 
 	vuint32 ch3_total;				// [31:0] ch3_total, total number of cycle after cleared and enabled 
	vuint32 ch3_word;				// [31:0] ch3_word, total number of bus word after cleared and enabled 
} VPUDMAMONITOR_REG_T;

typedef struct tagVPUDMACTRL_REG_T
{
	vuint32 ctrl;					// [0] start, busy (auto clear)
									// [1] irq_done, manual clear
									// [3:2] function_mode_minor
									// [6:4] function_mode
									// 0: 2d-dma
									// 1: projection
									// 2: rle_encode
									// 3: hamming_distance
									// 4. register control
									// 7: dma2otf
									// [7] use_wdma1d_otf
									// [15:8] reserved
									// [23:16] bw_dma
									// [31:24] bw_vpu
	vuint32 rdma2d_base;			// [31:0] rdma2d_base
	vuint32 rdma2d_stride;			// [15:0] rdma2d_stride
	vuint32 rdma2d_size;			// [15:0] rdma2d_height,
									// [31:16] rdma2d_width, 
	vuint32 wdma2d_base;			// [31:0] wdma2d_base
	vuint32 wdma2d_stride;			// [15:0] wdma2d_stride
	vuint32 wdma2d_size;			// [15:0] wdma2d_height,
									// [31:16] wdma2d_width 
	vuint32 wdma1d_base;			// [31:0] wdma1d_base
	vuint32 wdma1d_size;			// [15:0] wdma1d_num_word
	vuint32 version;				// [31:0] YYYYMMDD	
} VPUDMACTRL_REG_T;

typedef struct tagVPUDMAHAMMING_REG_T
{
	vuint32 desc[4];
} VPUDMAHAMMING_REG_T;

typedef struct tagVPUOTFMONITOR_REG_T
{
	vuint32 ctrl;					// [0] enable_ch0,	enable bus monitor for ch0 
									// [1] enable_ch1,  enable bus monitor for ch1 
									// [2] enable_ch2, 	enable bus monitor for ch2 
									// [3] enable_ch3, 	enable bus monitor for ch3 
									// [4] clear_ch0, 	clear bus monitor counter for ch0, auto-cleared and read as 0 always 
									// [5] clear_ch1 	clear bus monitor counter for ch1, auto-cleared and read as 0 always 
									// [6] clear_ch2 	clear bus monitor counter for ch2, auto-cleared and read as 0 always 
									// [7] clear_ch3 	clear bus monitor counter for ch3, auto-cleared and read as 0 always 
	vuint32 ch0_total;				// [31:0] ch0_total, total number of cycle after cleared and enabled 
	vuint32 ch0_word;				// [31:0] ch0_word, total number of bus word after cleared and enabled 
	vuint32 ch1_total;				// [31:0] ch1_total, total number of cycle after cleared and enabled 
	vuint32 ch1_word;				// [31:0] ch1_word, total number of bus word after cleared and enabled 
	vuint32 ch2_total;				// [31:0] ch2_total, total number of cycle after cleared and enabled 
	vuint32 ch2_word;				// [31:0] ch2_word, total number of bus word after cleared and enabled
	vuint32 ch3_total;				// [31:0] ch3_total, total number of cycle after cleared and enabled 
	vuint32 ch3_word;				// [31:0] ch3_word, total number of bus word after cleared and enabled 
} VPUOTFMONITOR_REG_T;

typedef struct tagVPUCTRL_REG_T
{
	vuint32 config;					// [0] enable, only valid on negedge of vsync
									// [7:4] fast_n
									// [15:8] fast_th
									// [18:16] ch_sel, 0~3 : isp, 4: svm, 5: bto
									// [19] score_big_or_equal, 0: big_only
									// [20] : brief_enable
									// [21] : roi_enable
									// [22] : scl_enable
									// [23] : dma2otf_enable	(was dma_enable)
									// [24] : use_posedge_vsync
									// [25] : otf2dma_enable 	(experimental) 
									// [26] : use_sync_roi 		(experimental) 
									// [27] : reserved
									// [28] : manual_flush 		(experimental) 
									// [29] : auto_flush		(experimental) 
									// [31:30] reserved
	vuint32 size;					// [15:0] height
									// [31:16] width
	vuint32 bandwidth;				// [7:0] bw_fast
									// [15:8] bw_brief
									// [31:16] counter_limit, interrupt asserted when the featur point is over the counter_limit
	vuint32 roiStart;				// [15:0] roi_start_y
									// [31:16] roi_start_x
	vuint32 roiEnd;					// [15:0] roi_end_y
									// [31:16] roi_end_x
	vuint32 xyBase;					// [31:0] xy_base (x,y) 32-bit unit, padded
	vuint32 descBase;				// [31:0] desc_base, 128-bit unit
	vuint32 lut;					// [1:0] p0y, 0~3
									// [5:4] p0x, 0~3
									// [13:12] p1y, 0~3
									// [17:16] p1x, 0~3
									// [30:24] addr (0 ~ 128)
									// [31] write_enable
	vuint32 sclSize;				// [15:0] scl_height
									// [31:16] scl_width
	vuint32 sclRatio;				// [15:0] scl_ver_ratio
									// [31:16] scl_hor_ratio
	vuint32 _reserved_28;
	vuint32 _reserved_2c;
	vuint32 fast_zone[4];			// [31:24] eoy
									// [23:16] soy
									// [15:8] eox
									// [7:0] sox
	vuint32 roLut;					// [1:0] p0y, 0~3
									// [5:4] p0x, 0~3
									// [13:12] p1y, 0~3
									// [17:16] p1x, 0~3
	vuint32 roState;				// [7:0] pix_data
									// [8] pix_hsync
									// [9] pix_vsync
									// [12] frame_busy
	vuint32 roNumScorePrev;			// [15:0] number of feature point detected from the previous frame
	vuint32 roFrameCounter;			// [31:0] frame_counter
	vuint32 roError;				// [0] irq_done
									// [1] irq_count_limit
									// [2] irq_from_fast_ored
									// [3] irq_pixel_blocked
									// [4] irq_from_scl
									// [5] irq_from_bus_request_pending (debugging purpose only) 
									// [7:6] flag_not_fully_flushed (debugging purpose only) 
									// [8] safe_enabled_sync (debugging purpose only) 
	vuint32 roIrqFast;				// [31:0] irq_from_fast
	vuint32 roNumScore;				// [15:0] number of feature point detected
	vuint32 version;				// [31:0] YYYYMMDD 
	vuint32 roActiveCnt;			// [31:16] number of VSYNC active 
									// [15:0] number of HSYNC active
	vuint32 roBlankCnt; 			// [31:16] number of VBLANK
									// [15:0] number of HBLANK 
	vuint32 roHtotalCnt;			// [15:0] number of clock from HSYNC posedge to the next 
	vuint32 roVtotalCnt;			// [31:0] number of clock from VSYNC posedge to the next
	vuint32 roActiveCntInter;		// [31:16] number of VSYNC active, internal
									// [15:0] number of HSYNC active, internal 
	vuint32 roBlankCntInter;		// [31:16] number of VBLANK, internal
									// [15:0] number of HBLANK, internal
	vuint32 roHtotalCntInter;		// [15:0] number of clock from HSYNC posedge to the next, internal 
	vuint32 roVtotalCntInter;		// [31:0] number of clock from VSYNC posedge to the next, internal 
} VPUCTRL_REG_T;

#define VPU_DMAMONITOR_REG	((VPUDMAMONITOR_REG_T*)	(VPU_BASE_ADDR + 0x00600))
#define VPU_HAMMING_REG		((VPUDMAHAMMING_REG_T*)	(VPU_BASE_ADDR + 0x00300))
#define VPU_DMACTRL_REG		((VPUDMACTRL_REG_T*)	(VPU_BASE_ADDR + 0x00000))
#define VPU_OTFMONITOR_REG	((VPUOTFMONITOR_REG_T*)	(VPU_BASE_ADDR + 0x24000))
#define VPU_CTRL_REG		((VPUCTRL_REG_T*)		(VPU_BASE_ADDR + 0x20000))

#endif //__VPUREGISTER_H__
