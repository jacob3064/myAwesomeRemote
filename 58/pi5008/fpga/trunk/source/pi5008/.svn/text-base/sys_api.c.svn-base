#include <stdio.h>
#include <string.h>
#include "sys_api.h"
#include "type.h"
#include "error.h"
#include "system.h"
#include "cache.h"
#include "interrupt.h"
#include "uart.h"
#include "dma.h"
#include "debug.h"

typedef struct tagSYSDMA_STATUS_T
{
	bool isDone;
	bool isAbort;
	bool isError;
}SYSDMA_STATUS_T, *pSYSDMA_STATUS_T;

typedef struct tagSYSDMA_DATA_T
{
	SYSDMA_STATUS_T status[SYS_DMA_MAX_CHANNEL];
	SYSDMA_CALLBACK callback;
}SYSDMA_DATA_T, *pSYSDMA_DATA_T;

static SYSDMA_DATA_T gSYSDMAData;
//static SYSDMA_CALLBACK gDMACallback = NULL;

/***************************************************************************************************************************************************************
 * Local functions
***************************************************************************************************************************************************************/
static void DMA_isr_hadler(uint8 IN ch, uint32 IN event)
{
	uint32 eventResult;

	if(event == DMA_EVENT_DONE){
		eventResult = eDMA_EVENT_DONE;
		gSYSDMAData.status[ch].isDone = TRUE;
	}else if(event == DMA_EVENT_ABORT){
		eventResult = eDMA_EVENT_ABORT;
		gSYSDMAData.status[ch].isAbort = TRUE;
	}else{
		eventResult = eDMA_EVENT_ERROR;
		gSYSDMAData.status[ch].isError = TRUE;
	}

	if(gSYSDMAData.callback)
		gSYSDMAData.callback(ch, eventResult);
}


/***************************************************************************************************************************************************************
 * API functions
***************************************************************************************************************************************************************/
int32 APIENTRY SYSAPI_initialize(void)
{
	cache_enable();
	initIntr();
	GIE_ENABLE();

	SVMAPI_load();

	SetRegValue(0xF01000EC, 0x20);		// BTO Inversion : sync_bt_sel
	SetRegValue(0xF01000E4, 0x1);		// BT_OUT_MUX : DU
	SetRegValue(0xF01000F4, 0x1);		// vsync tie 

	return eSUCCESS;
}


int32 APIENTRY SYSAPI_UART_initialize(uint8 IN ch, int32 IN baudRate, int32 IN dataBit, int32 IN stopBit, int32 IN parity, SYSDMA_CALLBACK IN callback)
{
	uart_init(ch, baudRate, dataBit, stopBit, parity);
	if(callback)
	{
		uart_init_rxfifo(ch, eRXLVL_NOT_EMPTY);
		uart_init_txfifo(ch, eTXLVL_NOT_FULL);
		uart_fifo_enable(ch);
		uart_set_callback(callback);
	}

	return eSUCCESS;
}

void APIENTRY SYSAPI_DMA_initialize(SYSDMA_CALLBACK IN callback)
{
	dma_sw_reset();
	dma_set_callback(DMA_isr_hadler);
	memset(&gSYSDMAData, 0, sizeof(gSYSDMAData));
	gSYSDMAData.callback = callback;
}
int32 APIENTRY SYSAPI_DMA_m2m_word(uint8 IN ch, uint32 IN *pPhySrc, uint32 IN *pPhyDst, int32 IN size, bool isBlock)
{
	int32 ret = eSUCCESS;
	DMA_CTL ctrl;

	if(ch >= SYS_DMA_MAX_CHANNEL)return eERROR_SYS_DMA_INVALID_CHANNEL;
	if(!dma_get_done(ch))return eERROR_SYS_DMA_CHANNEL_IN_USE;

	memset(&gSYSDMAData.status[ch], 0, sizeof(gSYSDMAData.status[ch]));

	ctrl.priority = 0;
	ctrl.src_burst_size = DMA_BSIZE_32;
	ctrl.src_width = DMA_WIDTH_WORD;
	ctrl.dst_width = DMA_WIDTH_WORD;
	ctrl.src_mode = 0;
	ctrl.dst_mode = 0;
	ctrl.src_addr_ctrl = 0;	// increment
	ctrl.dst_addr_ctrl = 0;	// increment
	ctrl.src_req_sel = 0;
	ctrl.dst_req_sel = 0;
	ctrl.int_abort_mask = 0;
	ctrl.int_err_mask = 0;
	ctrl.int_tcm_mask = 0;

	dma_set_config(ch, (unsigned int)pPhySrc, (unsigned int)pPhyDst, (size >> ctrl.src_width), &ctrl, NULL );
	dma_set_enable(ch, 1);

	if(isBlock){
		while(!dma_get_done(ch));

		if(gSYSDMAData.status[ch].isDone)ret = eSUCCESS;
		else if(gSYSDMAData.status[ch].isAbort)ret = eERROR_SYS_DMA_ABORT;
		else if(gSYSDMAData.status[ch].isError)ret = eERROR_SYS_DMA_ERROR;
	}

	return ret;
}

int32 APIENTRY SYSAPI_DMA_abort(uint8 IN ch)
{
	int32 ret = eSUCCESS;

	if(ch >= SYS_DMA_MAX_CHANNEL)return eERROR_SYS_DMA_INVALID_CHANNEL;

	dma_abort(ch);

	return ret;
}

int32 APIENTRY SYSAPI_DMA_check_result(uint8 IN ch)
{
	int32 ret = eSUCCESS;

	if(ch >= SYS_DMA_MAX_CHANNEL)return eERROR_SYS_DMA_INVALID_CHANNEL;

	if(!dma_get_done(ch))ret = eERROR_SYS_DMA_CHANNEL_IN_USE;
	else{
		if(gSYSDMAData.status[ch].isDone)ret = eSUCCESS;
		else if(gSYSDMAData.status[ch].isAbort)ret = eERROR_SYS_DMA_ABORT;
		else if(gSYSDMAData.status[ch].isError)ret = eERROR_SYS_DMA_ERROR;
	}

	return ret;
}

void APIENTRY SYSAPI_CACHE_writeback(uint32 IN *pAddr, int32 IN size)
{
	cache_wb_range(pAddr, size);
}

void APIENTRY SYSAPI_CACHE_invalidate(uint32 IN *pAddr, int32 IN size)
{
	cache_inv_range(pAddr, size);
}

void APIENTRY SYSAPI_DU_initialize (void)
{
	du_init();
}