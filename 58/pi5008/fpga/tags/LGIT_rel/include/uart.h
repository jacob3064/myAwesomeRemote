/* uart.h */
#ifndef __UART_H__
#define __UART_H__

#include "type.h"
#include "system.h"

#define UART_MAX_DEV	2

#define UART_PNT_CH		(0)
#define UART_PRT_CH		(1)

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

#define UART0_CTRL_REG			((UART_REG_T*)			(UART0_BASE_ADDR + 0x0000U))
#define UART1_CTRL_REG			((UART_REG_T*)			(UART0_BASE_ADDR + 0x1000U))


// FIFO Control Register //
#define FIFO_ENABLE		((unsigned int)1<<0)
#define RXFIFO_RESET	((unsigned int)1<<1)
#define TXFIFO_RESET	((unsigned int)1<<2)

// Line Control Register //
#define DLAB_ENABLE		((unsigned int)1<<7)

#define LENGTH_5BITS	((unsigned int)0<<0)
#define LENGTH_6BITS	((unsigned int)1<<0)
#define LENGTH_7BITS	((unsigned int)2<<0)
#define LENGTH_8BITS	((unsigned int)3<<0)

#define STOPBIT_1		((unsigned int)0<<2)
#define STOPBIT_2		((unsigned int)1<<2)

#define PARITY_NO		(((unsigned int)0<<5) | ((unsigned int)0<<4) | ((unsigned int)0<<3))
#define PARITY_ODD		(((unsigned int)0<<5) | ((unsigned int)0<<4) | ((unsigned int)1<<3))
#define PARITY_EVEN		(((unsigned int)0<<5) | ((unsigned int)1<<4) | ((unsigned int)1<<3))
#define PARITY_STICK	(((unsigned int)1<<5) | ((unsigned int)1<<4) | ((unsigned int)1<<3))


//void uart_init(int ch, int baudrate);
void uart_init(int ch, int baudrate, int databit, int stopbit, int parity);
void outbyte(int ch, char c);
char inbyte(int ch);



#endif // __UART_H__
