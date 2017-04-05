#ifndef _PI5008_DMA_H
#define _PI5008_DMA_H

#include "system.h"
#include "type.h"

#define DMA_MAX_CHANNEL 	(8)

#define DMA_BSIZE_1            	(0)  // Burst size = 1
#define DMA_BSIZE_2             (1)  // Burst size = 2
#define DMA_BSIZE_4             (2)  // Burst size = 4
#define DMA_BSIZE_8             (3)  // Burst size = 8
#define DMA_BSIZE_16            (4)  // Burst size = 16
#define DMA_BSIZE_32            (5)  // Burst size = 32
#define DMA_BSIZE_64            (6)  // Burst size = 64
#define DMA_BSIZE_128           (7)  // Burst size = 128

#define DMA_WIDTH_BYTE          (0)  // Width = 1 byte
#define DMA_WIDTH_HALFWORD      (1)  // Width = 2 bytes
#define DMA_WIDTH_WORD          (2)  // Width = 4 bytes

#define DMA_EVENT_DONE			(0)
#define DMA_EVENT_ABORT			(1)
#define DMA_EVENT_ERROR			(2)

typedef void (*DMA_CALLBACK) (uint8 ch, uint32 event);

typedef struct tagDMA_CH_REG_T
{
	vuint32 ctrl;							/* 0x044			- Channel0 control register */
	vuint32 src;							/* 0x048			- Channel0 source address register */
	vuint32 dst;							/* 0x04c			- Channel0 destination address register */
	vuint32 size;							/* 0x050			- Channel0 control register */
	vuint32 llp;							/* 0x054			- Channel0 control register */

}DMA_CH_REG_T;

typedef struct tagDMA_REG_T
{
	vuint32 id;								/* 0x000 		- ID and revision register */
	vuint32 rsv0[3];						/* 0x004 ~ 0x00c	- reserved */
	vuint32 cfg;							/* 0x010			- Configuration register */
	vuint32 rsv1[3];						/* 0x014 ~ 0x01c	- reserved */
	vuint32 ctrl;							/* 0x020			- Control register */
	vuint32 rsv2[3];						/* 0x024 ~ 0x02c	- reserved */
	vuint32 irq_status;						/* 0x030			- Interrupt status register */
	vuint32 ch_enable;						/* 0x034			- Channel enable register - RO */
	vuint32 rsv3[2];						/* 0x038 ~ 0x03c	- reserved */
	vuint32 ch_abort;						/* 0x040			- Channel abort register - WO */

	DMA_CH_REG_T ch_reg[DMA_MAX_CHANNEL];	/* 0x044 ~ 0x0E0	- Channel register */

}DMA_REG_T;

typedef struct{
	uint8 priority;			// 0: low, 1: high priority
	uint8 src_burst_size;	// 0: 1, 1: 2, 2: 4, 3: 8, ... 7: 128 transfer ( 2^x )
	uint8 src_width;		// 0: byte, 1: half, 2: word transfer
	uint8 dst_width;		// 0: byte, 1: half, 2: word transfer
	uint8 src_mode;			// 0: normal, 1: handshake mode
	uint8 dst_mode;			// 0: normal, 1: handshake mode
	uint8 src_addr_ctrl;	// 0: increment, 1: decrement, 2: fixed address
	uint8 dst_addr_ctrl;	// 0: increment, 1: decrement, 2: fixed address
	uint8 src_req_sel;		// source DMA request select
	uint8 dst_req_sel;		// source DMA request select
	uint8 int_abort_mask;	// 0: allow the abort irq to be trigered, 1: disable the abort irq
	uint8 int_err_mask;		// 0: allow the error irq to be trigered, 1: disable the error irq
	uint8 int_tcm_mask;		// 0: allow the terminal count irq to be trigered, 1: disable the terminal count irq
	uint8 rsv[3];
	
}DMA_CTL;

typedef struct{
	uint32 ctrl;
	uint32 src_addr;
	uint32 dst_addr;
	uint32 trasfer_size;
	uint32 llp_pointer;
}LLP_DESC;


#define DMA_CTRL_REG			((DMA_REG_T*)			(DMA_BASE_ADDR + 0x0000))


void dma_sw_reset(void);
void dma_set_callback(DMA_CALLBACK callback);
int32 dma_set_config(int32 ch, uint32 src_addr, uint32 dst_addr, uint32 size, DMA_CTL *pCtrl, LLP_DESC *pLLP );
int32 dma_set_enable(int32 ch, int32 en);
int32 dma_abort(int32 ch);
int32 dma_get_irqstatus(void);
int32 dma_get_done(int32 ch);


#endif

