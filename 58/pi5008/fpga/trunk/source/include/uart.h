/* uart.h */
#ifndef __UART_H__
#define __UART_H__

#include "type.h"
#include "system.h"

//#define USE_VERIFICATION	/// TEMPORARY MACRO FOR VERIFICATION

#define UART_MAX_DEV	2

#define UART_PNT_CH		(0)
#define UART_PRT_CH		(1)

typedef void (*UART_CALLBACK) (uint8 ch, uint32 event);

typedef struct tagUART_REG_T
{
	vuint32 id;					/* 0x000 		- id */
	vuint32 rsv[3];				/* 0x004 ~ 0x00c	- reserved */
	vuint32 cfg;				/* 0x010			- HW configure regisster */	
	vuint32 oscr;				/* 0x014			- Over sample control register */	
	vuint32 rsv1[2];			/* 0x018 ~ 0x01c	- reserved */
	union {
		vuint32 rbr;			/* 0x020			- Receiver Buffer Register for Read */
		vuint32 thr;			/* 0x020 		- Transmitter Holding Register for Write */
		vuint32 dll;			/* 0x020 		- Divisor latch LSB */
	};
	union {
		vuint32 ier;			/* 0x024 		- Interrupt enable register */
		vuint32 dlm;			/* 0x024 		- Divisor latch MSB */
	};
	union {
		vuint32 iir;			/* 0x028 		- Interrupt identification register */
		vuint32 fcr;			/* 0x028 		- FIFO control register */
	};
	vuint32 lcr;				/* 0x02c 		- Line control register */
	vuint32 mcr;				/* 0x030 		- Modem control register */
	vuint32 lsr;				/* 0x034 		- Line status register */
	vuint32 msr;				/* 0x038 		- Modem status register */
	vuint32 scr;				/* 0x03c 		- Scratch Register */
}UART_REG_T;

#define UART0_CTRL_REG			((UART_REG_T*)		(UART0_BASE_ADDR + 0x0000U))
#if defined(SYSTEM_BUS_DW)
#define UART1_CTRL_REG			((UART_REG_T*)		(UART1_BASE_ADDR + 0x0000U))
#define UART_IRQ_STATUS_REG		((vuint32*)			(UART_IRQ_STATUS_ADDR + 0x0000U))
#endif


extern void uart_init(uint8 ch, uint32 baudrate, uint32 databit, uint32 stopbit, uint32 parity);
extern void uart_init_rxfifo (uint8 ch, uint8 level);
extern void uart_init_txfifo (uint8 ch, uint8 level);
extern void uart_fifo_enable (uint8 ch);
extern void uart_set_callback (UART_CALLBACK callback);
extern void uart_outbyte(uint8 ch, uint8 c);
extern uint8 uart_inbyte(uint8 ch);
extern uint32 uart_get_id_revision(int32 ch);
extern uint8 uart_get_rxReady (uint8 ch);
extern uint8 uart_get_rxData (uint8 ch);
extern uint8 uart_get_txEmpty (uint8 ch);
extern void uart_set_txData (uint8 ch, uint8 data);


#endif // __UART_H__
