#include <stdio.h>
#include "utils.h"
#include "spi.h"

#define SPI_TCR_WR           	(0x0 << 24)
#define SPI_TCR_WONLY          	(0x1 << 24)
#define SPI_TCR_RONLY          	(0x2 << 24)


static SPI_REG_T *gpSPI =	SPI0_CTRL_REG;

static SPI_MEM_REG_T *gpSPIMem = SPI_MEM_CTRL_REG;


int32 spi_initialize(uint32 slave, uint32 freq, uint32 clk_mode, uint32 wordlen)
{
	int ret = 0;
	uint32 div;
	
	gpSPI->ctrl = 7;	// Tx/Rx FIFO reset, SPI reset

	gpSPI->trans_fmt = ( (((wordlen-1)&0x1f) << 8) | ((slave & 1)<<2) | (clk_mode&3) );
	gpSPI->ctrl = ( (2 << 16) |	(2 << 8) );	// Tx/Rx FIFO Threshold

	div = (PCLK / (freq * 2)) - 1;
	gpSPI->timing = utilPutBits(gpSPI->timing, 0, 8, div);


	//printf("[SPI%d Ctrl Registers]\n", ch);

	
	return ret;
}


int32 spi_tx(const uint8 *dout, uint32 size)
{
	int32 ret = 0;

	int len_tx;
	int n_bytes;
	unsigned int data;

	while(gpSPI->status & 1);
	
	n_bytes = ( utilGetBits(gpSPI->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI->trans_ctrl = (SPI_TCR_WONLY | (((size-1)&0x1ff)<<12));

	gpSPI->cmd = 0;	// start transfer
	
	for(len_tx=0; len_tx < size;){
		//printf("len tx: %d, size: %d, status: 0x%x\n", len_tx, size, gpSPI[ch]->status);
		if(len_tx < size && !(utilGetBit(gpSPI->status,23))){	// Tx not full
			memcpy(&data, dout, n_bytes);
			gpSPI->data = data;
			dout += n_bytes;
			len_tx += n_bytes;
		}
	}

	return ret;

}


int32 spi_xfer(const uint8 *dout, uint8 *din, uint32 size)
{
	int32 ret = 0;

	int len_tx, len_rx;
	volatile unsigned int val;
	int n_bytes;
	unsigned int data;

	while(gpSPI->status & 1);
	
	n_bytes = ( utilGetBits(gpSPI->trans_fmt, 8, 5) + 1 + 7 ) / 8;
	gpSPI->trans_ctrl = (SPI_TCR_WR | (((size-1)&0x1ff)<<12) | (((size-1)&0x1ff)<<0));

	gpSPI->cmd = 0;	// start transfer
	
	for(len_tx=0, len_rx=0; len_rx < size;){
		//printf("len tx: %d, len rx: %d, size: %d, status: 0x%x\n", len_tx, len_rx, size, gpSPI[ch]->status);
		if(len_tx < size && !(utilGetBit(gpSPI->status,23))){	// Tx not full
			if(dout){
				memcpy(&data, dout, n_bytes);
				gpSPI->data = data;
				dout += n_bytes;
			}else{
				gpSPI->data = 0;
			}
			len_tx += n_bytes;
		}

		if(!utilGetBit(gpSPI->status, 14)){	// Rx not empty
			val = gpSPI->data;
			if (din){
				data = (unsigned char)val;
				memcpy(din, &data, n_bytes);
				din += n_bytes;
			}
			len_rx += n_bytes;
		}

	}

	return ret;
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
}

