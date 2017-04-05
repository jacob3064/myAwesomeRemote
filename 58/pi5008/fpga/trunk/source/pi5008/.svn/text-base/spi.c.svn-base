#include <stdio.h>
#include <nds32_intrinsic.h>
#include "utils.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "debug.h"


#define TRANSFER_ISR				0


#define SPI_TCR_WR           	(0x0 << 24)
#define SPI_TCR_WONLY          	(0x1 << 24)
#define SPI_TCR_RONLY          	(0x2 << 24)

#if defined(SYSTEM_BUS_DW)
#define MAX_SPI_DEV_NUM				5
#else
#define MAX_SPI_DEV_NUM				1
#endif


#if defined(TRANSFER_ISR)
typedef struct tagSPI_RESOURCE_T{
	uint32 *pTxBuf;
	uint32 *pRxBuf;
	uint32 TxSize;
	uint32 RxSize;
}SPI_RESOURCE_T;

#endif

static SPI_REG_T *gpSPI[] = {
		SPI0_CTRL_REG,
#if defined(SYSTEM_BUS_DW)
		SPI1_CTRL_REG,
		SPI2_CTRL_REG,
		SPI3_CTRL_REG,
		SPI4_CTRL_REG,
#endif		
};

static SPI_MEM_REG_T *gpSPIMem = SPI_MEM_CTRL_REG;

static SPI_CALLBACK gSPICallback[MAX_SPI_DEV_NUM];

static uint32 gDMARequestTx[MAX_SPI_DEV_NUM] = {
		SPI0_DMA_TX_REQ,
#if defined(SYSTEM_BUS_DW)
		SPI1_DMA_TX_REQ,
		SPI2_DMA_TX_REQ,
		SPI3_DMA_TX_REQ,
		SPI4_DMA_TX_REQ,
#endif
};
static uint32 gDMARequestRx[MAX_SPI_DEV_NUM] = {
		SPI0_DMA_RX_REQ,
#if defined(SYSTEM_BUS_DW)
		SPI1_DMA_RX_REQ,
		SPI2_DMA_RX_REQ,
		SPI3_DMA_RX_REQ,
		SPI4_DMA_RX_REQ,
#endif
};

static vuint32 *gMISC_CS_PINMUX = (vuint32 *)(MISC_BASE_ADDR + 0xf8);
static uint8 gCS_Bit[MAX_SPI_DEV_NUM] = {3, 0, 1, 2, };
static uint8 gCS_GPIOCh[MAX_SPI_DEV_NUM] = {6, 3, 4, 5, };

//void spi0_isr(sint32 num)
ISR(spi_isr, num)
{
	uint32 msk = (1 << num);
	uint32 reg;
	sint32 i;
	sint32 dev = 0;
#if defined(SYSTEM_BUS_DW)
	vuint32 *irq_reg = (vuint32 *)SPI_IRQ_STATUS_ADDR;	// need not to clear
#endif
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

#if defined(SYSTEM_BUS_DW)
	for(i=0;i<MAX_SPI_DEV_NUM;i++){
		if((*irq_reg>>i)&1){
			dev = i;
#endif

	reg = gpSPI[dev]->irq_status;
	printf("spi[%d] isr: 0x%x\n", dev, reg);
	if(gSPICallback[dev])gSPICallback[dev](reg);
	gpSPI[dev]->irq_status = reg;

#if defined(SYSTEM_BUS_DW)
		}
	}
#endif

	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);

}

sint32 spi_initialize(sint32 ch, uint32 slave, uint32 freq, uint32 clk_mode, uint32 wordlen, SPI_CALLBACK callback)
{
	sint32 ret = 0;
	uint32 div;
	
	gpSPI[ch]->ctrl = 7;	// Tx/Rx FIFO reset, SPI reset

	while(gpSPI[ch]->ctrl & 7);
	
	gpSPI[ch]->trans_fmt = ( (((wordlen-1)&0x1f) << 8) | ((slave & 1)<<2) | (clk_mode&3) );
	//gpSPI[ch]->trans_ctrl = ((data_mode&3) << 22);
	gpSPI[ch]->ctrl = ( (2 << 16) |	(2 << 8) );	// Tx/Rx FIFO Threshold

	div = (PCLK / (freq * 2)) - 1;
	gpSPI[ch]->timing = utilPutBits(gpSPI[ch]->timing, 0, 8, div);

	printf("[SPI%d Ctrl Registers]\n", ch);
	print_hexw((uint32 *)gpSPI[ch], 0x80);

	gSPICallback[ch] = callback;

    /* Register SPI ISR */
    //SYSAPI_OS_register_isr(IRQ_SPI_VECTOR, spi_isr, old);
	//SYSAPI_OS_register_isr(IRQ_SPI_MEMC_VECTOR, spi_isr, old);
	
	return ret;
}


sint32 spi_tx(sint32 ch, const uint8 *dout, uint32 size)
{
	sint32 ret = 0;

	sint32 len_tx;
	sint32 n_bytes;
	uint32 data;

	//while(gpSPI[ch]->status & 1);
	
	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_WONLY | (((size-1)&0x1ff)<<12));

	gpSPI[ch]->cmd = 0;	// start transfer
	
	for(len_tx=0; len_tx < size;){
		//printf("len tx: %d, size: %d, status: 0x%x\n", len_tx, size, gpSPI[ch]->status);
		if(len_tx < size && !(utilGetBit(gpSPI[ch]->status,23))){	// Tx not full
			memcpy(&data, dout, n_bytes);
			gpSPI[ch]->data = data;
			dout += n_bytes;
			len_tx += n_bytes;
		}
	}

	while(gpSPI[ch]->status & 1);

	return ret;

}

sint32 spi_rx(sint32 ch, uint8 *din, uint32 size)
{
	sint32 ret = 0;

	sint32 len_rx;
	vuint32 val;
	sint32 n_bytes;
	uint32 data;

	//while(gpSPI[ch]->status & 1);
	
	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_RONLY | (((size-1)&0x1ff)<<0));

	gpSPI[ch]->cmd = 0;	// start transfer
	
	for(len_rx=0; len_rx < size;){
		//printf("len rx: %d, size: %d, status: 0x%x\n", len_rx, size, gpSPI[ch]->status);
		if(!utilGetBit(gpSPI[ch]->status, 14)){	// Rx not empty
			val = gpSPI[ch]->data;
			data = (uint8)val;
			memcpy(din, &data, n_bytes);
			din += n_bytes;
			len_rx += n_bytes;
		}

	}

	while(gpSPI[ch]->status & 1);

	return ret;
}


sint32 spi_xfer(sint32 ch, const uint8 *dout, uint8 *din, uint32 size)
{
	sint32 ret = 0;

	sint32 len_tx, len_rx;
	vuint32 val;
	sint32 n_bytes;
	uint32 data;

	//while(gpSPI[ch]->status & 1);
	
	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_WR | (((size-1)&0x1ff)<<12) | (((size-1)&0x1ff)<<0));

	gpSPI[ch]->cmd = 0;	// start transfer
	
	for(len_tx=0, len_rx=0; len_rx < size;){
		//printf("len tx: %d, len rx: %d, size: %d, status: 0x%x\n", len_tx, len_rx, size, gpSPI[ch]->status);
		if(len_tx < size && !(utilGetBit(gpSPI[ch]->status,23))){	// Tx not full
			if(dout){
				memcpy(&data, dout, n_bytes);
				gpSPI[ch]->data = data;
				dout += n_bytes;
			}else{
				gpSPI[ch]->data = 0;
			}
			len_tx += n_bytes;
		}

		if(!utilGetBit(gpSPI[ch]->status, 14)){	// Rx not empty
			val = gpSPI[ch]->data;
			if (din){
				data = (uint8)val;
				memcpy(din, &data, n_bytes);
				din += n_bytes;
			}
			len_rx += n_bytes;
		}

	}

	while(gpSPI[ch]->status & 1);

	return ret;
}


#if 0
sint32 spi_tx_dma(sint32 ch, const uint8 *dout, uint32 size, sint32 dma_ch)
{
	sint32 ret = 0;
	sint32 n_bytes;
	DMA_CONFIG cfg;

	if(gDMARequestTx[ch] == (uint32)(-1))return -1;

	//while(gpSPI[ch]->status & 1);

	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_WONLY | (((size-1)&0x1ff)<<12));

	memset(&cfg.ctrl, 0, sizeof(DMA_CTL));
	cfg.ctrl.priority = 0;
	cfg.ctrl.src_burst_size = DMA_BSIZE_1;
	cfg.ctrl.src_width = DMA_WIDTH_BYTE;
	cfg.ctrl.dst_width = DMA_WIDTH_BYTE;
	cfg.ctrl.src_mode = 0;		// normal
	cfg.ctrl.dst_mode = 1;		// handshake
	cfg.ctrl.src_addr_ctrl = 0;	// increment
	cfg.ctrl.dst_addr_ctrl = 2;	// fixed
	cfg.ctrl.src_req_sel = 0;
	cfg.ctrl.dst_req_sel = gDMARequestTx[ch];	// spi request ??????
	cfg.ctrl.int_abort_mask = 0;
	cfg.ctrl.int_err_mask = 0;
	cfg.ctrl.int_tcm_mask = 0;
	cfg.src_addr = (uint32)(dout);
	cfg.dst_addr = (uint32)(&gpSPI[ch]->data);
	cfg.trasfer_size = (size >> cfg.ctrl.src_width);
	cfg.llp_pointer = 0;

	dma_set_config(dma_ch, &cfg);
	dma_set_enable(dma_ch, 1);

	gpSPI[ch]->irq_enable = SPI_INTR_TRANSFER_END;
	gpSPI[ch]->cmd = 0;	// start transfer

	return ret;

}

sint32 spi_rx_dma(sint32 ch, uint8 *din, uint32 size, sint32 dma_ch)
{
	sint32 ret = 0;

	sint32 n_bytes;
	DMA_CONFIG cfg;

	if(gDMARequestRx[ch] == (uint32)(-1))return -1;

	//while(gpSPI[ch]->status & 1);

	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_RONLY | (((size-1)&0x1ff)<<0));

	memset(&cfg.ctrl, 0, sizeof(DMA_CTL));
	cfg.ctrl.priority = 0;
	cfg.ctrl.src_burst_size = DMA_BSIZE_1;
	cfg.ctrl.src_width = DMA_WIDTH_BYTE;
	cfg.ctrl.dst_width = DMA_WIDTH_BYTE;
	cfg.ctrl.src_mode = 1;		// handshake
	cfg.ctrl.dst_mode = 0;		// normal
	cfg.ctrl.src_addr_ctrl = 2;	// fixed
	cfg.ctrl.dst_addr_ctrl = 0;	// increment
	cfg.ctrl.src_req_sel = gDMARequestRx[ch];
	cfg.ctrl.dst_req_sel = 0;
	cfg.ctrl.int_abort_mask = 0;
	cfg.ctrl.int_err_mask = 0;
	cfg.ctrl.int_tcm_mask = 0;
	cfg.src_addr = (uint32)(&gpSPI[ch]->data);
	cfg.dst_addr = (uint32)(din);
	cfg.trasfer_size = (size >> cfg.ctrl.src_width);
	cfg.llp_pointer = 0;

	dma_set_config(dma_ch, &cfg);
	dma_set_enable(dma_ch, 1);

	gpSPI[ch]->irq_enable = SPI_INTR_TRANSFER_END;
	gpSPI[ch]->cmd = 0;	// start transfer


	return ret;
}

sint32 spi_xfer_dma(sint32 ch, const uint8 *dout, uint8 *din, uint32 size, sint32 dma_tx_ch, sint32 dma_rx_ch)
{
	sint32 ret = 0;

	sint32 n_bytes;
	DMA_CONFIG cfg_tx;
	DMA_CONFIG cfg_rx;

	if((gDMARequestRx[ch] == (uint32)(-1)) || (gDMARequestTx[ch] == (uint32)(-1)))return -1;

	//while(gpSPI[ch]->status & 1);

	n_bytes = ( utilGetBits(gpSPI[ch]->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI[ch]->trans_ctrl = (SPI_TCR_WR | (((size-1)&0x1ff)<<12) | (((size-1)&0x1ff)<<0));

	memset(&cfg_tx.ctrl, 0, sizeof(DMA_CTL));
	cfg_tx.ctrl.priority = 0;
	cfg_tx.ctrl.src_burst_size = DMA_BSIZE_1;
	cfg_tx.ctrl.src_width = DMA_WIDTH_BYTE;
	cfg_tx.ctrl.dst_width = DMA_WIDTH_BYTE;
	cfg_tx.ctrl.src_mode = 0;		// normal
	cfg_tx.ctrl.dst_mode = 1;		// handshake
	cfg_tx.ctrl.src_addr_ctrl = 0;	// increment
	cfg_tx.ctrl.dst_addr_ctrl = 2;	// fixed
	cfg_tx.ctrl.src_req_sel = 0;
	cfg_tx.ctrl.dst_req_sel = gDMARequestTx[ch];
	cfg_tx.ctrl.int_abort_mask = 0;
	cfg_tx.ctrl.int_err_mask = 0;
	cfg_tx.ctrl.int_tcm_mask = 0;
	cfg_tx.src_addr = (uint32)(dout);
	cfg_tx.dst_addr = (uint32)(&gpSPI[ch]->data);
	cfg_tx.trasfer_size = (size >> cfg_tx.ctrl.src_width);
	cfg_tx.llp_pointer = 0;

	memset(&cfg_rx.ctrl, 0, sizeof(DMA_CTL));
	cfg_rx.ctrl.priority = 0;
	cfg_rx.ctrl.src_burst_size = DMA_BSIZE_1;
	cfg_rx.ctrl.src_width = DMA_WIDTH_BYTE;
	cfg_rx.ctrl.dst_width = DMA_WIDTH_BYTE;
	cfg_rx.ctrl.src_mode = 1;		// handshake
	cfg_rx.ctrl.dst_mode = 0;		// normal
	cfg_rx.ctrl.src_addr_ctrl = 2;	// fixed
	cfg_rx.ctrl.dst_addr_ctrl = 0;	// increment
	cfg_rx.ctrl.src_req_sel = gDMARequestRx[ch];
	cfg_rx.ctrl.dst_req_sel = 0;
	cfg_rx.ctrl.int_abort_mask = 0;
	cfg_rx.ctrl.int_err_mask = 0;
	cfg_rx.ctrl.int_tcm_mask = 0;
	cfg_rx.src_addr = (uint32)(&gpSPI[ch]->data);
	cfg_rx.dst_addr = (uint32)(din);
	cfg_rx.trasfer_size = (size >> cfg_rx.ctrl.src_width);
	cfg_rx.llp_pointer = 0;


	dma_set_config(dma_tx_ch, &cfg_tx);
	dma_set_config(dma_rx_ch, &cfg_rx);
	dma_set_enable(dma_tx_ch, 1);
	dma_set_enable(dma_rx_ch, 1);

	gpSPI[ch]->irq_enable = SPI_INTR_TRANSFER_END;
	gpSPI[ch]->cmd = 0;	// start transfer


	return ret;
}
#endif

sint32 spi_direct_cs_enable(sint32 ch, uint8 en)
{
	if(en){
		gpio_set_dir(1, gCS_GPIOCh[ch], GPIO_DIR_OUT, 1);
		*gMISC_CS_PINMUX |= (1 << gCS_Bit[ch]);

	}else{
		*gMISC_CS_PINMUX &= (~(1 << gCS_Bit[ch]));

	}

	return 1;
}

sint32 spi_cs_activate(sint32 ch)
{
	gpio_set_value(1, gCS_GPIOCh[ch], 0);

	return 1;
}

sint32 spi_cs_deactivate(sint32 ch)
{
	gpio_set_value(1, gCS_GPIOCh[ch], 1);

	return 1;
}


void spi_memctrl_set(uint32 div, uint8 read_cmd, SPI_MEM_FR_INFO fr_info)
{
	uint32 *ptr;
	
	gpSPIMem->divider = div;
	gpSPIMem->fr_cmd = read_cmd;

	ptr = (uint32 *)&fr_info;
	gpSPIMem->fr_info = *ptr;
	//memcpy(&gpSPIMem->fr_info, &fr_info, 4);

	//printf("[SPI Mem Ctrl Registers]\n");
	//print_hexw((uint32 *)gpSPIMem, 0x24);
}

