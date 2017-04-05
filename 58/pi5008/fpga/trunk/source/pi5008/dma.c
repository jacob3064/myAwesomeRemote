#include <stdio.h>
#include <nds32_intrinsic.h>
#include "intr.h"
#include "dma.h"
#include "debug.h"
#include "utils.h"

static DMA_REG_T *gpDMA = DMA_CTRL_REG;
static DMA_CALLBACK gDMACallback = NULL;

void dma_isr(int32 num)
{
	uint32 msk = (1 << num);
	uint32 reg;
	int i;
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	reg = gpDMA->irq_status;

	for(i=0;i<DMA_MAX_CHANNEL;i++){
		if(reg & (1<<i)){				// ERROR
			//DMA_PRINT("dma isr error: 0x%x\n", reg&0xff);
			if(gDMACallback)gDMACallback(i, DMA_EVENT_ERROR);
		}else if(reg & (1<<(i+8))){		// ABORT
			//DMA_PRINT("dma isr abort: 0x%x\n", (reg>>8)&0xff); 
			if(gDMACallback)gDMACallback(i, DMA_EVENT_ABORT);
		}else if(reg & (1<<(i+16))){	// DONE
			//DMA_PRINT("dma isr done: 0x%x\n", (reg>>16)&0xff);
			if(gDMACallback)gDMACallback(i, DMA_EVENT_DONE);
		}
	}

	gpDMA->irq_status = reg;
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
	
}


void dma_sw_reset(void)
{
	DMA_PRINT("dma cfg: 0x%x\n", gpDMA->cfg);
	gpDMA->ctrl = 1;
}

void dma_set_callback(DMA_CALLBACK callback)
{
	gDMACallback = callback;
}

int dma_set_config(int32 ch, uint32 src_addr, uint32 dst_addr, uint32 size, DMA_CTL *pCtrl, LLP_DESC *pLLP )
{
	uint32 reg = 0;
	int32 ret = 1;

	DMA_PRINT("dma set config. ch: %d, src: 0x%x, dst: 0x%x, size: %d\n", ch, (uint32)src_addr, (uint32)dst_addr, size);
	reg = ((pCtrl->priority & 1)<<29) |
			((pCtrl->src_burst_size & 7)<<22) |
			((pCtrl->src_width & 3)<<20) |
			((pCtrl->dst_width & 3)<<18) |
			((pCtrl->src_mode & 1)<<17) |
			((pCtrl->dst_mode & 1)<<16) |
			((pCtrl->src_addr_ctrl & 3)<<14) |
			((pCtrl->dst_addr_ctrl & 3)<<12) |
			((pCtrl->src_req_sel & 0xf)<<8) |
			((pCtrl->dst_req_sel & 0xf)<<4) |
			((pCtrl->int_abort_mask & 1)<<3) |
			((pCtrl->int_err_mask & 1)<<2) |
			((pCtrl->int_tcm_mask & 1)<<1);

	gpDMA->ch_reg[ch].ctrl = reg;		// set chn control register
	gpDMA->ch_reg[ch].src = src_addr;	// set source address	
	gpDMA->ch_reg[ch].dst = dst_addr;	// set destination address
	gpDMA->ch_reg[ch].size = size;		// set transfer size

	if(pLLP){
		gpDMA->ch_reg[ch].llp = (vuint32)pLLP;
	}

	DMA_PRINT("ch%d ctrl: 0x%x\n", ch, gpDMA->ch_reg[ch].ctrl);

	return ret;
}

int32 dma_set_enable(int32 ch, int32 en)
{
	int32 ret = 1;

	if(en)
		gpDMA->ch_reg[ch].ctrl = utilSetBit( gpDMA->ch_reg[ch].ctrl, 0 );
	else
		gpDMA->ch_reg[ch].ctrl = utilClearBit( gpDMA->ch_reg[ch].ctrl, 0 );
	

	return ret;
}

int32 dma_abort(int32 ch)
{
	int32 ret = 1;
	
	gpDMA->ch_abort = utilSetBit(gpDMA->ch_abort, ch);

	return ret;
}

int32 dma_get_irqstatus(void)
{
	return gpDMA->irq_status;
}

int32 dma_get_done(int32 ch)
{
	int32 ret = 0;

	if(utilGetBit(gpDMA->ch_reg[ch].ctrl, 0) == 0)ret = 1;

	return ret;
	
}

