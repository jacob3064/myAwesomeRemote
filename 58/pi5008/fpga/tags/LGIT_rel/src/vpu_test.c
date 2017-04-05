/*
 * Copyright (c) 2012-2015 PixelPlus Technology Corporation
 * All rights reserved.
 *
 */
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <nds32_intrinsic.h>

#include "system.h"
#include "vpu_register.h"
#include "vpu_test.h"
#include "debug.h"
#include "timer.h"
#include "sys_api.h"

#define INDEX_X(pos) (((pos>>16)&0xffff)>>4)
#define INDEX_Y(pos) (((pos)&0xffff)>>4)
#define IMAGE_WIDTH		1280
#define IMAGE_HEIGHT	720

#define SRC_ADDR  		0x20100000
#define SRC_ADDR1		0x201E1000
#define FAST_ADDR		0x20300000
#define FAST_ADDR1		0x20350000
#define BRIEF_ADDR		0x20400000
#define BRIEF_ADDR1		0x20450000
#define HD_ADDR			0x20500000
#define HD_ADDR1		0x20550000
#define BRIEF_RELOAD	0x20600000

static uint32 fp_num, fp_num1;


void _delay( uint32 count )
{
	while(count-- > 0){
		asm volatile ("nop");
	}

}

static void print_vpu_reg(void)
{
	uint32 *ptr = (uint32 *)VPU_CTRL_REG;

	printf("\n[VPU CTRL REG]\n");
	print_hexw(ptr, 0x5c);

	ptr = (uint32 *)VPU_DMACTRL_REG;
	printf("\n[VPU DMA REG]\n");
	print_hexw(ptr, 0x1c);
	
	
}

void print_feature_point(uint32 *pt, int32 num)
{
	int32 x, y, i;
	int32 width = (IMAGE_WIDTH>>4);
	int32 height = (IMAGE_HEIGHT>>4);
	bool bFound = FALSE;
	
	for(y=0;y<height;y++){
		for(x=0;x<width;x++){
			bFound = FALSE;
			for(i=0;i<num;i++){
				if((INDEX_X(pt[i]) == x) && (INDEX_Y(pt[i]) == y)){
					bFound = TRUE;
					break;
				}
			}
			
			if(bFound == TRUE)printf("|*");
			else printf("| ");
		}
		printf("|\n");
	}
}

// input image(on-the-fly) -> fast & brief -> hamming distance (reg-mem)
void vputest_scenario (void)
{
	uint32 busy;
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	uint32 fast_over, fast_over1;
	uint32 *fast_addr, *fast_addr1;
	uint32 *brief_addr, *brief_addr1;

	uint32 i;
	uint32 *pAddr;

	printf("vpu_otf_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF;	// clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF;	// clear any time

	//VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x0f<<8);
	VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x3f<<8);
	VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0x0000ffff) | (10240 << 16);
	
	VPU_CTRL_REG->size = (1280 << 16) | 720;
	VPU_CTRL_REG->sclSize = (1280 << 16) | 720;
	VPU_CTRL_REG->sclRatio = (0x100 << 16) | 0x100;
	VPU_CTRL_REG->roiStart = (0 << 16) | 0;
	VPU_CTRL_REG->roiEnd = (1279 << 16) | 719;

	fast_addr = (uint32 *)FAST_ADDR;
	fast_addr1 = (uint32 *)FAST_ADDR1;
	brief_addr = (uint32 *)BRIEF_ADDR;
	brief_addr1 = (uint32 *)BRIEF_ADDR1;

	// 1st frame //
	VPU_CTRL_REG->xyBase     = (vuint32)fast_addr;
	VPU_CTRL_REG->descBase   = (vuint32)brief_addr;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) | 	// fast_n
							  (72 << 8) | 	// fast_th
							  (0 << 16) | 	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) | 	// roi_enable
							  (0 << 22) | 	// scl_enable
							  (0 << 23) | 	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	// check if busy cleared 
	busy = 1; 
	fast_over = 0;
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		if((VPU_CTRL_REG->roError >> 1) & 0x1)	fast_over = 1;
	} while (busy); 

	if(fast_over)
		fp_num = (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF;
	else
		fp_num = VPU_CTRL_REG->roNumScore;

	//print_vpu_reg();
	
	//printf("fast num : %d, limit count : %d\n", VPU_CTRL_REG->roNumScore, (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF);
#if 1
	// 2nd frame //
	VPU_CTRL_REG->xyBase     = (vuint32)fast_addr1;
	VPU_CTRL_REG->descBase   = (vuint32)brief_addr1;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) | 	// fast_n
							  (72 << 8) | 	// fast_th
							  (0 << 16) | 	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) | 	// roi_enable
							  (0 << 22) | 	// scl_enable
							  (0 << 23) | 	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	// check if busy cleared 
	busy = 1; 
	fast_over1 = 0;
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		if((VPU_CTRL_REG->roError >> 1) & 0x1)	fast_over1 = 1;
	} while (busy); 

	if(fast_over1)
		fp_num1 = (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF;
	else
		fp_num1 = VPU_CTRL_REG->roNumScore;

	//print_vpu_reg();
	//fp_num2 = VPU_CTRL_REG->roNumScore;
	//printf("fast num : %d, limit count : %d\n", VPU_CTRL_REG->roNumScore, (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF);

	{

		SYSAPI_CACHE_invalidate(fast_addr, fp_num*4);
		SYSAPI_CACHE_invalidate(brief_addr, fp_num*16);

		SYSAPI_CACHE_invalidate(fast_addr1, fp_num1*4);
		SYSAPI_CACHE_invalidate(brief_addr1, fp_num1*16);

		printf("\n---fast & breif---\n");
		pAddr = brief_addr;
		for(i=0 ; i<fp_num ; i++)
		{
			// H->L
			printf("( %d,\t%d) = %08x%08x%08x%08x\n",
				(fast_addr[i]>>16)&0xFFFF, fast_addr[i]&0xFFFF, pAddr[3], pAddr[2], pAddr[1], pAddr[0]);

			pAddr += 4;
		}
		print_feature_point(fast_addr, fp_num);

		printf("\n---fast & breif---\n");
		pAddr = brief_addr1;
		for(i=0 ; i<fp_num1 ; i++)
		{
			// H->L
			printf("( %d,\t%d) = %08x%08x%08x%08x\n",
				(fast_addr1[i]>>16)&0xFFFF, fast_addr1[i]&0xFFFF, pAddr[3], pAddr[2], pAddr[1], pAddr[0]);

			pAddr += 4;
		}
		print_feature_point(fast_addr1, fp_num1);
	}
#endif
	printf("vpu_otf_test_end\n");
}

// load image -> dma -> fast & brief
void vputest_fast_brief_dma (void)
{
	uint32 busy;
	uint32 fast_over, fast_over1;
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	uint32 *src_addr, *src_addr1;
	uint32 *fast_addr, *fast_addr1;
	uint32 *brief_addr, *brief_addr1;

	uint32 i;
	uint32 num;

	printf("vpu_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF;	// clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF;	// clear any time

	src_addr = (uint32 *)SRC_ADDR;
	src_addr1 = (uint32 *)SRC_ADDR1;
	fast_addr = (uint32 *)FAST_ADDR;
	fast_addr1 = (uint32 *)FAST_ADDR1;
	brief_addr = (uint32 *)BRIEF_ADDR;
	brief_addr1 = (uint32 *)BRIEF_ADDR1;

	//VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x0f<<8);
	VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x3f<<8);
	VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0x0000ffff) | (10240 << 16);
	VPU_CTRL_REG->size = (1280 << 16) | 720;
	VPU_CTRL_REG->sclSize = (1280 << 16) | 720;
	VPU_CTRL_REG->sclRatio = (0x100 << 16) | 0x100;
	VPU_CTRL_REG->roiStart = (0 << 16) | 0;
	VPU_CTRL_REG->roiEnd = (1279 << 16) | 719;

	// 1st frame //
	VPU_CTRL_REG->xyBase     = (vuint32)fast_addr;
	VPU_CTRL_REG->descBase   = (vuint32)brief_addr;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) | 	// fast_n
							  (72 << 8) | 	// fast_th
							  (2 << 16) | 	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) | 	// roi_enable
							  (0 << 22) | 	// scl_enable
							  (1 << 23) | 	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	VPU_DMACTRL_REG->rdma2d_base 	= (vuint32)src_addr; 
	VPU_DMACTRL_REG->rdma2d_stride	= 1280; 
	VPU_DMACTRL_REG->rdma2d_size 	= (1280 << 16) | 720; 
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xf1; 

	// check if busy cleared 
	busy = 1; 
	fast_over = 0;
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		if((VPU_CTRL_REG->roError >> 1) & 0x1)	fast_over = 1;
	} while (busy); 

	print_vpu_reg();

	printf("fast num : %d, limit count : %d\n", VPU_CTRL_REG->roNumScore, (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF);

	{

		if(fast_over)
			num = (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF;
		else
			num = VPU_CTRL_REG->roNumScore;

		SYSAPI_CACHE_invalidate(fast_addr, num*4);
		SYSAPI_CACHE_invalidate(brief_addr, num*16);

		printf("\n---fast & breif---\n");
		for(i=0 ; i<num ; i++)
		{
			// H->L
			printf("( %d,\t%d) = %08x%08x%08x%08x\n",
				(fast_addr[i]>>16)&0xFFFF, fast_addr[i]&0xFFFF, brief_addr[3], brief_addr[2], brief_addr[1], brief_addr[0]);

			brief_addr += 4;

		}

		//print_feature_point(fast_addr, num);
	}

#if 1
	// 2nd frame //
	VPU_CTRL_REG->xyBase     = (vuint32)fast_addr1;
	VPU_CTRL_REG->descBase   = (vuint32)brief_addr1;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) | 	// fast_n
							  (72 << 8) | 	// fast_th
							  (2 << 16) | 	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) | 	// roi_enable
							  (0 << 22) | 	// scl_enable
							  (1 << 23) | 	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	VPU_DMACTRL_REG->rdma2d_base 	= (vuint32)src_addr1; 
	VPU_DMACTRL_REG->rdma2d_stride	= 1280; 
	VPU_DMACTRL_REG->rdma2d_size 	= (1280 << 16) | 720; 
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xf1; 

	// check if busy cleared 
	busy = 1; 
	fast_over1 = 0;
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		if((VPU_CTRL_REG->roError >> 1) & 0x1)	fast_over1 = 1;
	} while (busy); 

	print_vpu_reg();

	printf("fast num : %d, limit count : %d\n", VPU_CTRL_REG->roNumScore, (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF);

	{

		if(fast_over1)
			num = (VPU_CTRL_REG->bandwidth >> 16)&0xFFFF;
		else
			num = VPU_CTRL_REG->roNumScore;

		SYSAPI_CACHE_invalidate(fast_addr1, num*4);
		SYSAPI_CACHE_invalidate(brief_addr1, num*16);

		printf("\n---fast & breif---\n");
		for(i=0 ; i<num ; i++)
		{
			// H->L
			printf("( %d,\t%d) = %08x%08x%08x%08x\n",
				(fast_addr1[i]>>16)&0xFFFF, fast_addr1[i]&0xFFFF, brief_addr1[3], brief_addr1[2], brief_addr1[1], brief_addr1[0]);

			brief_addr1 += 4;

		}

		//print_feature_point(fast_addr1, num);
	}
#endif
	printf("vpu_test_end\n");
}

// load description -> dma -> hamming distance (1reg - mem)
void vputest_hamming_1reg_mem_dma (void)
{
	uint32 busy;
	uint32 num_byte;
	
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	printf("vpu_test_start\n");	
	
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF; // clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF; // clear any time

	VPU_HAMMING_REG->desc[0]	= 0x00000000;//0x40890958;
	VPU_HAMMING_REG->desc[1]	= 0x00000000;//0x41086e0b;
	VPU_HAMMING_REG->desc[2]	= 0x00000000;//0x61fb7f99;
	VPU_HAMMING_REG->desc[3]	= 0x00000000;//0xc3c0dbc1;

	num_byte = 48 * (128/8);

	VPU_DMACTRL_REG->rdma2d_base = (vuint32)BRIEF_ADDR;
	VPU_DMACTRL_REG->rdma2d_stride	= num_byte;
	VPU_DMACTRL_REG->rdma2d_size = (num_byte << 16) | 1;
	VPU_DMACTRL_REG->wdma1d_base = (vuint32)HD_ADDR;
	VPU_DMACTRL_REG->ctrl = 0x0f0f0000 | 0xb1;

	do {
		busy = (VPU_DMACTRL_REG->ctrl & 0x00000001);
	} while (busy); 
	
	printf("vpu_test_end\n");
}

// load description -> dma -> hamming distance (for reg - mem)
void vputest_hamming_reg_mem_dma (void)
{
	uint32 busy;
	uint32 num_byte, num;
	uint32 i;

	uint32 *desc1;
	uint32 *desc2;
	uint32 *dist;
	
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	printf("vpu_test_start\n");	
	
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF; // clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF; // clear any time

	desc1 = (uint32 *)BRIEF_ADDR;
	desc2 = (uint32 *)BRIEF_ADDR1;
	dist = (uint32 *)HD_ADDR;
	num = 48;

	//printf("desc1. 0x%08x0x%08x0x%08x0x%08x\n", desc1[3], desc1[2], desc1[1], desc1[0]); 
	//printf("desc2. 0x%08x0x%08x0x%08x0x%08x\n", desc2[3], desc2[2], desc2[1], desc2[0]); 

	for(i=0 ; i<num ; i++)
	{
		VPU_HAMMING_REG->desc[0]	= *desc1++;
		VPU_HAMMING_REG->desc[1]	= *desc1++;
		VPU_HAMMING_REG->desc[2]	= *desc1++;
		VPU_HAMMING_REG->desc[3]	= *desc1++;

		num_byte = num * (128/8);
		VPU_DMACTRL_REG->rdma2d_base = (vuint32)desc2;
		VPU_DMACTRL_REG->rdma2d_stride	= num_byte;
		VPU_DMACTRL_REG->rdma2d_size = (num_byte << 16) | 1;
		VPU_DMACTRL_REG->wdma1d_base = (vuint32)dist;
		VPU_DMACTRL_REG->ctrl = 0x0f0f0000 | 0xb1;

		do {
			busy = (VPU_DMACTRL_REG->ctrl & 0x00000001);
		} while (busy);

		
		dist += (num/4);//((48 + 7) / 8);				// 8byte align
	}
	
	printf("vpu_test_end\n");
}

// load description -> dma -> hamming distance (mem - mem)
void vputest_hamming_mem_mem_dma (void)
{
	uint32 busy;
	uint32 num;
	uint32 i;

	uint32 *desc1;
	uint32 *desc2;
	uint32 *desc;
	uint32 *dist;
	
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	printf("vpu_test_start\n");
	
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF;	// clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF;	// clear any time

	desc1 = (uint32 *)BRIEF_ADDR;
	desc2 = (uint32 *)BRIEF_ADDR1;
	desc = (uint32 *)BRIEF_RELOAD;
	dist = (uint32 *)HD_ADDR;
	num = 48;

	for(i=0 ; i<num ; i++)
	{
		*desc++ = *desc1++;
		*desc++ = *desc1++;
		*desc++ = *desc1++;
		*desc++ = *desc1++;
		*desc++ = *desc2++;
		*desc++ = *desc2++;
		*desc++ = *desc2++;
		*desc++ = *desc2++;
	}

	desc = (uint32 *)0x20102000;
	VPU_DMACTRL_REG->rdma2d_base 	= (vuint32)desc;
	VPU_DMACTRL_REG->rdma2d_stride	= 32;
	VPU_DMACTRL_REG->rdma2d_size 	= (32 << 16) | num;
	VPU_DMACTRL_REG->wdma1d_base 	= (vuint32)dist;
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xb5;

	do {
		busy = (VPU_DMACTRL_REG->ctrl & 0x00000001);
	} while (busy);

	printf("vpu_test_end\n");
}

// load description -> dma copy
void vputest_hamming_copy_dma (void)
{
	int busy=0;

	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	printf("vpu_dma_copy_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF;	// clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F;	// enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF;	// clear any time 

	VPU_DMACTRL_REG->rdma2d_base = (vuint32)SRC_ADDR;
	VPU_DMACTRL_REG->rdma2d_stride = 1280;
	VPU_DMACTRL_REG->rdma2d_size = (1280<< 16) | 720;
	VPU_DMACTRL_REG->wdma1d_base = (vuint32)SRC_ADDR1;
	VPU_DMACTRL_REG->ctrl = 0x0f0f0000 | 0x81 | (0<<2);
	//VPU_DMACTRL_REG->ctrl = 0x0F0F0081;
	do {
		busy = (VPU_DMACTRL_REG->ctrl & 0x00000001);
	} while (busy == 1);

	printf("vpu_dma_copy_test_end\n");
}

// brief lut test
void vputest_brief_lut (void)
{
	uint32 *lut;
	int32 i;
	int32 x, y;

	printf("brief lut test start\n");

	// read
	lut = (uint32 *)SRC_ADDR;
	for(i=0 ; i<128 ; i++)
	{
		VPU_CTRL_REG->lut = (VPU_CTRL_REG->lut & 0x00FFFFFF) | (i<<24) | (0<<31);
		*lut = (uint32)VPU_CTRL_REG->roLut;
		printf("%d : 0(%d, %d), 1(%d, %d)\n", i, (*lut>>4)&0x3, (*lut&0x3), (*lut>>16)&0x3, (*lut>>12)&0x3);
		lut++;
	}

	// write
	lut = (uint32 *)SRC_ADDR;
	x = 0;
	y = 3;
	for(i=0 ; i<128 ; i++)
	{
		x++;
		if(x > 3) x = 0;
		y--;
		if(y < 0) y = 3;
		VPU_CTRL_REG->lut = (y<<0) | (x<<4) | (y<<12) | (x<<16) | (i<<24) | (1<<31);
		VPU_CTRL_REG->lut = (0<<0) | (0<<4) | (0<<12) | (0<<16) | (i<<24) | (0<<31);	// need manual clear
		printf("%d : 0(%d, %d), 1(%d, %d)\n", i, x, y, x, y);
	}

	// read
	lut = (uint32 *)SRC_ADDR;
	for(i=0 ; i<128 ; i++)
	{
		VPU_CTRL_REG->lut = (VPU_CTRL_REG->lut & 0x00FFFFFF) | (i<<24) | (0<<31);
		//VPU_CTRL_REG->lut = (0<<0) | (0<<4) | (0<<12) | (0<<16) | (i<<24) | (0<<31);
		*lut = (uint32)VPU_CTRL_REG->roLut;
		printf("%d : 0(%d, %d), 1(%d, %d)\n", i, (*lut>>4)&0x3, (*lut&0x3), (*lut>>16)&0x3, (*lut>>12)&0x3);
		lut++;
	}
	
	printf("brief lut test end\n");
}

// load image -> fast roi
void vputest_fast_roi_dma (void)
{
	uint32 busy;
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	uint32 *src_addr;
	uint32 *fast_addr;
	uint32 *brief_addr;

	uint32 i;
	uint32 num;
	
	printf("roi_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF; // clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF; // clear any time

	src_addr = (uint32 *)SRC_ADDR;
	fast_addr = (uint32 *)FAST_ADDR;
	brief_addr = (uint32 *)BRIEF_ADDR;

	//VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x0f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x3f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0x0000ffff) | (256 << 16);
	VPU_CTRL_REG->size = (1280 << 16) | 720;
	VPU_CTRL_REG->sclSize = (1280 << 16) | 720;
	VPU_CTRL_REG->sclRatio = (0x100 << 16) | 0x100;
	VPU_CTRL_REG->roiStart = (320 << 16) | 180;
	VPU_CTRL_REG->roiEnd = (959 << 16) | 539;
	
	VPU_CTRL_REG->xyBase	 = (vuint32)fast_addr;
	VPU_CTRL_REG->descBase	 = (vuint32)brief_addr;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) |	// fast_n
							  (64 << 8) |	// fast_th
							  (2 << 16) |	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (1 << 21) |	// roi_enable
							  (0 << 22) |	// scl_enable
							  (1 << 23) |	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	VPU_DMACTRL_REG->rdma2d_base	= (vuint32)src_addr; 
	VPU_DMACTRL_REG->rdma2d_stride	= 1280; 
	VPU_DMACTRL_REG->rdma2d_size	= (1280 << 16) | 720; 
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xf1; 

	// check if busy cleared 
	busy = 1; 
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		//printf("busy:%d\n", busy);
	} while (busy); 

	printf("0x%08X (%d)\n", VPU_CTRL_REG->roNumScore, VPU_CTRL_REG->roNumScore);

	{

		num = VPU_CTRL_REG->roNumScore;


		printf("\n---fast & breif---\n");
		for(i=0 ; i<num ; i++)
		{
			// H->L
			printf("%d. ( %d,\t%d) = %08x%08x%08x%08x\n",
				i+1,
				(fast_addr[i]>>16)&0xFFFF, fast_addr[i]&0xFFFF, brief_addr[3], brief_addr[2], brief_addr[1], brief_addr[0]);

			brief_addr += 4;

		}
	}
	printf("roi_test_end\n");
}

// load image -> fast zone
void vputest_fast_zone_dma (void)
{
	uint32 busy;
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	uint32 *src_addr;
	uint32 *fast_addr;
	uint32 *brief_addr;

	uint32 i;
	uint32 num;
	
	printf("zone_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF; // clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF; // clear any time

	src_addr = (uint32 *)SRC_ADDR;
	fast_addr = (uint32 *)FAST_ADDR;
	brief_addr = (uint32 *)BRIEF_ADDR;

	//VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x0f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x3f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0x0000ffff) | (256 << 16);
	VPU_CTRL_REG->size = (1280 << 16) | 720;
	VPU_CTRL_REG->sclSize = (1280 << 16) | 720;
	VPU_CTRL_REG->sclRatio = (0x100 << 16) | 0x100;
	VPU_CTRL_REG->roiStart = (0 << 16) | 0;
	VPU_CTRL_REG->roiEnd = (1279 << 16) | 719;

	{
		uint8 sx, sy, ex, ey;
#if 1
		typedef struct { uint32 sx; uint32 sy; uint32 ex; uint32 ey; } point;
		point zone0 = { 0, 0, 320, 720 };
		point zone1 = { 960, 0, 1280, 720 };
		point zone2 = { 0, 0, 1280, 180 };
		point zone3 = { 0, 540, 1280, 720 };

		sx = (zone0.sx) / 8;
		sy = (zone0.sy) / 8;
		ex = (zone0.ex - 1) / 8;
		ey = (zone0.ey - 1) / 8;
		VPU_CTRL_REG->fast_zone[0] = (ey << 24) | (sy << 16) | (ex <<  8) | (sx <<  0);
		sx = (zone1.sx) / 8;
		sy = (zone1.sy) / 8;
		ex = (zone1.ex - 1) / 8;
		ey = (zone1.ey - 1) / 8;
		VPU_CTRL_REG->fast_zone[1] = (ey << 24) | (sy << 16) | (ex <<  8) | (sx <<  0);
		sx = (zone2.sx) / 8;
		sy = (zone2.sy) / 8;
		ex = (zone2.ex - 1) / 8;
		ey = (zone2.ey - 1) / 8;
		VPU_CTRL_REG->fast_zone[2] = (ey << 24) | (sy << 16) | (ex <<  8) | (sx <<  0);
		sx = (zone3.sx) / 8;
		sy = (zone3.sy) / 8;
		ex = (zone3.ex - 1) / 8;
		ey = (zone3.ey - 1) / 8;
		VPU_CTRL_REG->fast_zone[3] = (ey << 24) | (sy << 16) | (ex <<  8) | (sx <<  0);
#else
		sx = 320 / 8;
		sy = 180 / 8;
		ex = (960-1) / 8;
		ey = (540-1) / 8;
		VPU_CTRL_REG->fast_zone[0] = 0xffffffff;   // not used
		VPU_CTRL_REG->fast_zone[1] = 0xffffffff;   // not used
		VPU_CTRL_REG->fast_zone[2] = (ey << 24) | (sy << 16) | (ex <<  8) | (sx <<  0);
		VPU_CTRL_REG->fast_zone[3] = 0xffffffff;   // not used
#endif
	}
		
	VPU_CTRL_REG->xyBase	 = (vuint32)fast_addr;
	VPU_CTRL_REG->descBase	 = (vuint32)brief_addr;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) |	// fast_n
							  (64 << 8) |	// fast_th
							  (2 << 16) |	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) |	// roi_enable
							  (0 << 22) |	// scl_enable
							  (1 << 23) |	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	VPU_DMACTRL_REG->rdma2d_base	= (vuint32)src_addr; 
	VPU_DMACTRL_REG->rdma2d_stride	= 1280; 
	VPU_DMACTRL_REG->rdma2d_size	= (1280 << 16) | 720; 
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xf1; 

	// check if busy cleared 
	busy = 1; 
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		//printf("busy:%d\n", busy);
	} while (busy); 

	printf("0x%08X (%d)\n", VPU_CTRL_REG->roNumScore, VPU_CTRL_REG->roNumScore);

	{
		num = VPU_CTRL_REG->roNumScore;


		printf("\n---fast & breif---\n");
		for(i=0 ; i<num ; i++)
		{
			// H->L
			printf("%d. ( %d,\t%d) = %08x%08x%08x%08x\n",
				i+1,
				(fast_addr[i]>>16)&0xFFFF, fast_addr[i]&0xFFFF, brief_addr[3], brief_addr[2], brief_addr[1], brief_addr[0]);

			brief_addr += 4;

		}
	}
	printf("zone_test_end\n");
}

// load image -> fast scale
void vputest_fast_scale_dma (void)
{
	uint32 busy;
	uint32 V3OTF_BUS_MON = 0xF1400000 + 0x24000;
	uint32 V3DMA_BUS_MON = 0xF1400000 + 0x00600;

	uint32 *src_addr;
	uint32 *fast_addr;
	uint32 *brief_addr;

	uint32 i;
	uint32 num;
	
	printf("scale_test_start\n");

	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3OTF_BUS_MON))) = 0x000000FF; // clear any time 

	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x0000000F; // enable first 
	(*((volatile unsigned int *)(V3DMA_BUS_MON))) = 0x000000FF; // clear any time

	src_addr = (uint32 *)SRC_ADDR;
	fast_addr = (uint32 *)FAST_ADDR;
	brief_addr = (uint32 *)BRIEF_ADDR;

	//VPU_CTRL_REG->bandwidth	= (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x0f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0xffff0000) | 0x0f | (0x3f<<8);
	VPU_CTRL_REG->bandwidth = (VPU_CTRL_REG->bandwidth & 0x0000ffff) | (256 << 16);
	VPU_CTRL_REG->size = (1280 << 16) | 720;
	VPU_CTRL_REG->sclSize = (640 << 16) | 360;
	VPU_CTRL_REG->sclRatio = (0x200 << 16) | 0x200;
	
	VPU_CTRL_REG->xyBase	 = (vuint32)fast_addr;
	VPU_CTRL_REG->descBase	 = (vuint32)brief_addr;
	VPU_CTRL_REG->config	= 1 |			// start 
							  (9 << 4) |	// fast_n
							  (64 << 8) |	// fast_th
							  (2 << 16) |	// ch_sel
							  (0 << 19) |	// 0: score_big, 1:score_big_or_equal 
							  (1 << 20) |	// brief_enable 
							  (0 << 21) |	// roi_enable
							  (1 << 22) |	// scl_enable
							  (1 << 23) |	// dma_enable
							  (0 << 24);	// dma type don't care vsync

	VPU_DMACTRL_REG->rdma2d_base	= (vuint32)src_addr; 
	VPU_DMACTRL_REG->rdma2d_stride	= 1280; 
	VPU_DMACTRL_REG->rdma2d_size	= (1280 << 16) | 720; 
	VPU_DMACTRL_REG->ctrl			= 0x0f0f0000 | 0xf1; 

	// check if busy cleared 
	busy = 1; 
	do { 
		busy	= (VPU_CTRL_REG->roState >> 12) & 0x00000001;
		//printf("busy:%d\n", busy);
	} while (busy); 

	printf("0x%08X (%d)\n", VPU_CTRL_REG->roNumScore, VPU_CTRL_REG->roNumScore);

	{

		num = VPU_CTRL_REG->roNumScore;


		printf("\n---fast & breif---\n");
		for(i=0 ; i<num ; i++)
		{
			// H->L
			printf("%d. ( %d,\t%d) = %08x%08x%08x%08x\n",
				i+1,
				(fast_addr[i]>>16)&0xFFFF, fast_addr[i]&0xFFFF, brief_addr[3], brief_addr[2], brief_addr[1], brief_addr[0]);

			brief_addr += 4;

		}
	}
	printf("scale_test_end\n");
}


void vputest_scenario_loop(void)
{
	vuint32 *reg = (vuint32 *)0xf01000e8;
	uint32 *ptr;
	int i;
	int err = 0;
	
	uint32 pos[4][2] = {
			{ 320, 180 }, {959, 180}, {320, 539}, {959, 539}
	};
	uint32 desc[4][4] = {
			{ 0xf8f8f8f8, 0xf8f8f8f8, 0x0000e200, 0x6100e100 },
			{ 0x00000000, 0x00000000, 0x1c401c80, 0x1cc00000 },
			{ 0x1f1f0f1f, 0x1f1f070f, 0x03200138, 0x02280238 },
			{ 0x00000000, 0x00000000, 0x00870047, 0x00470087 },
				
	};


	*reg = 0x8b;

	printf("Test pattern test\n");
	_delay(0x1000000);
	printf("Test pattern test\n");

	while(1){
		vputest_scenario();
		vputest_hamming_reg_mem_dma();

		if(fp_num != 4 || fp_num1 != 4){
			printf("Error!!! The number of feature point is not 4(%d, %d)\n", fp_num, fp_num1);
			_delay(0x1000000);
			continue;
			
		}

		//Check 1st Frame
		// Fast
		err = 0;
		ptr = (uint32 *)FAST_ADDR;
		SYSAPI_CACHE_invalidate(ptr, 16);
		for(i=0;i<4;i++){
			if(((ptr[i]>>16) & 0xffff) != pos[i][0] || ((ptr[i]) & 0xffff) != pos[i][1]){
				printf("1st Img Error!!! Wrong Fast Pos. (%d, %d)\n", ((ptr[i]>>16) & 0xffff), ((ptr[i]) & 0xffff));
				err++;
			}
		}
		if(err == 0)
			printf("1st Img Fast Success\n");

		
		// Brief
		err = 0;
		ptr = (uint32 *)BRIEF_ADDR;
		SYSAPI_CACHE_invalidate(ptr, 16*4);
		for(i=0;i<4;i++){
			if(memcmp(ptr, desc[i], 16) != 0){
				printf("1st Img Error!!! Wrong Brief Desc. (0x%08x%08x%08x%08x)\n", ptr[3], ptr[2], ptr[1], ptr[0]);
				err++;
			}
			ptr += 4;
		}
		if(err == 0)
			printf("1st Img Brief Success\n");


		//Check 2nd Frame
		// Fast
		err = 0;
		ptr = (uint32 *)FAST_ADDR1;
		SYSAPI_CACHE_invalidate(ptr, 16);
		for(i=0;i<4;i++){
			if(((ptr[i]>>16) & 0xffff) != pos[i][0] || ((ptr[i]) & 0xffff) != pos[i][1]){
				printf("2nd Img Error!!! Wrong Fast Pos. (%d, %d)\n", ((ptr[i]>>16) & 0xffff), ((ptr[i]) & 0xffff));
				err++;
			}
		}
		
		if(err == 0)
			printf("2nd Img Fast Success\n");
		
		// Brief
		err = 0;
		ptr = (uint32 *)BRIEF_ADDR1;
		SYSAPI_CACHE_invalidate(ptr, 16*4);
		for(i=0;i<4;i++){
			if(memcmp(ptr, desc[i], 16) != 0){
				printf("2nd Img Error!!! Wrong Brief Desc. (0x%08x%08x%08x%08x)\n", ptr[3], ptr[2], ptr[1], ptr[0]);
				err++;
			}
			ptr += 4;
		}
		if(err == 0)
			printf("2nd Img Brief Success\n");

				
		// Check hamming distance
		ptr = (uint32 *)HD_ADDR;
		SYSAPI_CACHE_invalidate(ptr, 4);
		if(*ptr != 0){
			printf("Hamming distance Error!!!. 0x%x\n", *ptr);
		}else{
			printf("Hamming distance Success\n");
		}

		printf("wait\n");
		_delay(0x8000000);
		
	}	
}
