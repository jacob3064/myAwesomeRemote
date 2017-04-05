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

#define DMA_EXAMPLE_SRC_ADDR		0x20100000
#define DMA_EXAMPLE_DST_ADDR		0x20200000
#define DMA_EXAMPLE_SIZE			0x10000

static void dma_isr_handler(uint8 ch, uint32 event)
{
	MAIN_PRINT("dma isr. ch: %d event: %d\n", ch, event);
}
static void dma_test(void)
{
	uint32 *srcPA = (uint32 *)DMA_EXAMPLE_SRC_ADDR;
	uint32 *dstPA = (uint32 *)DMA_EXAMPLE_DST_ADDR;
	uint32 *srcVA;
	uint32 *dstVA;
	int32 dmaSize = DMA_EXAMPLE_SIZE;
	int32 i,j;
	int32 ret;

	SYSAPI_DMA_initialize(dma_isr_handler);

	j=0;
	// Non-cacheable region write -> cacheable region read
	MAIN_PRINT("===================================================\n");
	MAIN_PRINT("Non-cacheable region write -> Cacheable region read\n");
	srcVA = (uint32 *)ADDR_NON_CACHEABLE((uint32)srcPA);
	dstVA = (uint32 *)ADDR_CACHEABLE((uint32)srcPA);

	MAIN_PRINT("write addr: 0x%x, read addr: 0x%x, size: 0x%x\n", (uint32)srcVA, (uint32)dstVA, dmaSize);
	for(i=0;i<(dmaSize >> 2);i++){
		*(srcVA+i) = i+j;
	}
	SYSAPI_CACHE_invalidate(dstVA,dmaSize);
	for(i=0;i<(dmaSize >> 2);i++){
		if(*(dstVA+i) != i+j){
			MAIN_PRINT("read fail %d, (0x%x, 0x%x)\n", i, *(dstVA+i), i+j);
			break;
		}

	}
	if(i == (dmaSize >> 2)){
		MAIN_PRINT("success\n");
	}

	j++;
	// Cacheable region write -> dma -> Cacheable region read
	MAIN_PRINT("===================================================\n");
	MAIN_PRINT("Cacheable region write -> dma -> Cacheable region read\n");
	srcVA = (uint32 *)ADDR_CACHEABLE((uint32)srcPA);
	dstVA = (uint32 *)ADDR_CACHEABLE((uint32)dstPA);

	MAIN_PRINT("src addr: 0x%x, dst addr: 0x%x, size: 0x%x\n", (uint32)srcVA, (uint32)dstVA, dmaSize);

	for(i=0;i<(dmaSize >> 2);i++){
		*(srcVA+i) = i+j;
	}

	SYSAPI_CACHE_writeback(srcVA, dmaSize);
	if((ret = SYSAPI_DMA_m2m_word(0, srcPA, dstPA, dmaSize, TRUE)) != eSUCCESS){
		MAIN_PRINT("DMA Fail(%d)\n", ret);
	}else{
		SYSAPI_CACHE_invalidate(dstVA, dmaSize);

		for(i=0;i<(dmaSize >> 2);i++){
			if(*(srcVA+i) != *(dstVA+i)){
				MAIN_PRINT("dma fail %d, (0x%x, 0x%x)\n", i, *(srcVA+i), *(dstVA+i));
				break;
			}
		}
		if(i == (dmaSize >> 2)){
			MAIN_PRINT("success\n");
		}
	}

	j++;
	// Non-cacheable region write -> dma -> cacheable region read
	MAIN_PRINT("===================================================\n");
	MAIN_PRINT("Non-cacheable region write -> dma -> Cacheable region read\n");
	srcVA = (uint32 *)ADDR_NON_CACHEABLE((uint32)srcPA);
	dstVA = (uint32 *)ADDR_CACHEABLE((uint32)dstPA);

	MAIN_PRINT("src addr: 0x%x, dst addr: 0x%x, size: 0x%x\n", (uint32)srcVA, (uint32)dstVA, dmaSize);

	for(i=0;i<(dmaSize >> 2);i++){
		*(srcVA+i) = i+j;
	}
	if((ret = SYSAPI_DMA_m2m_word(0, srcPA, dstPA, dmaSize, TRUE)) != eSUCCESS){
		MAIN_PRINT("DMA Fail(%d)\n", ret);
	}else{

		SYSAPI_CACHE_invalidate(dstVA, dmaSize);
		for(i=0;i<(dmaSize >> 2);i++){
			if(*(srcVA+i) != *(dstVA+i)){
				MAIN_PRINT("dma fail %d, (0x%x, 0x%x)\n", i, *(srcVA+i), *(dstVA+i));
				break;
			}
		}
		if(i == (dmaSize >> 2)){
			MAIN_PRINT("success\n");
		}
	}
	j++;
	// Non-cacheable region write -> dma -> non-cacheable region read
	MAIN_PRINT("===================================================\n");
	MAIN_PRINT("Non-cacheable region write -> dma -> Non-cacheable region read\n");
	srcVA = (uint32 *)ADDR_NON_CACHEABLE((uint32)srcPA);
	dstVA = (uint32 *)ADDR_NON_CACHEABLE((uint32)dstPA);

	MAIN_PRINT("src addr: 0x%x, dst addr: 0x%x, size: 0x%x\n", (uint32)srcVA, (uint32)dstVA, dmaSize);

	for(i=0;i<(dmaSize >> 2);i++){
		*(srcVA+i) = i+j;
	}
	if((ret = SYSAPI_DMA_m2m_word(0, srcPA, dstPA, dmaSize, FALSE)) != eSUCCESS){
		MAIN_PRINT("DMA Fail(%d)\n", ret);
	}else{
		while((ret = SYSAPI_DMA_check_result(0)) == eERROR_SYS_DMA_CHANNEL_IN_USE);
		for(i=0;i<(dmaSize >> 2);i++){
			if(*(srcVA+i) != *(dstVA+i)){
				MAIN_PRINT("dma fail %d, (0x%x, 0x%x)\n", i, *(srcVA+i), *(dstVA+i));
				break;
			}
		}
		if(i == (dmaSize >> 2)){
			MAIN_PRINT("success\n");
		}
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
	print_init();
	MAIN_PRINT("main begin\n");

	dma_test();

	while(1);

	return 1;
}
