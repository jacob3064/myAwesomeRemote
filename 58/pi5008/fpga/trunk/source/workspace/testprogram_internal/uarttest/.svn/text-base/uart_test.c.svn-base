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

#include "intr.h"
#include "system.h"
#include "uart.h"
#include "uart_test.h"
#include "debug.h"
#include "timer.h"
#include "sys_api.h"
#include "utils.h"

#define DEFAULT_BAUDRATE	(eBAUDRATE_115200)
#define DEFAULT_DATABIT		(eDATA_BIT_8)
#define DEFAULT_STOPBIT		(eSTOP_BIT_1)
#define DEFAULT_PARITY		(ePARITY_NONE)

#define DEFAULT_RXLVL		(eRXLVL_NOT_EMPTY)
#define DEFAULT_TXLVL		(eTXLVL_NOT_FULL)

static void _delay( uint32 count )
{
	while(count-- > 0){
		asm volatile ("nop");
	}
}

#if defined(USE_VERIFICATION)
void uart_isr(int32 num)
{
	uint32 msk = (1 << num);
	UART_REG_T *pUart = (UART_REG_T *)(UART0_CTRL_REG);
	uint8 reg;
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	reg = pUart->iir & 0xF;

	switch(reg)
	{
		case 0b0100:
			//printf("rx ready!(%x)\n", reg);
			uarttest_rx_int(0);
			break;
			
		case 0b0010:
			//printf("tx empty!(%x)\n", reg);
			uarttest_tx_int(0);
			break;
	}
	
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}
#endif

static void uarttest_init(uint8 ch, uint32 baudrate, uint32 databit, uint32 stopbit, uint32 parity)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint16 	div = 0;
	
	div = PCLK / (16 * baudrate);

	// Set DLAB to 1
	pUart->lcr |= (1<<7);

	// Set OSCR (default value)
	pUart->oscr = 16;

	// Set DLL for baudrate
	pUart->dll = ((div >> 0) & 0xff);
	pUart->dlm = ((div >> 8) & 0xff);

	// Set DLAB to 0
	pUart->lcr &= ~(1<<7);

	// LCR: data bit, stop bit, parity
	pUart->lcr = (databit | stopbit | parity);
}

static void uarttest_init_rxfifo (uint8 ch, uint8 level)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	
	// IER: Interrupt enable register.
	pUart->ier |= (1<<0);
	
	// FCR: Set rx trigger level & Reset rx
	pUart->fcr |= (level<<6) | (1<<1);
}

static void uarttest_init_txfifo (uint8 ch, uint8 level)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	
	// IER: Interrupt enable register.
	pUart->ier |= (1<<1);
	
	// FCR: Set tx trigger level & Reset tx
	pUart->fcr |= (level<<4) | (1<<2);
}

static void uarttest_fifo_enable (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);

	// FCR: Enable FIFO
	pUart->fcr |= (1<<0);

	// Init & Enable interrupt
	initIntr();
	GIE_ENABLE();
}

void uarttest_reset_reg (uint8 ch)
{
	uint32 i, err;
	vuint32 *pUart = (vuint32 *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint32 addr;
	
	uint32 resetValue[] = {
		0x02011002, 0x00000000, 0x00000000, 0x00000000,		// 0x0000
		0x00000000, 0x00000010, 0x00000000, 0x00000000,		// 0x0010
		0x00000000, 0x00000000, 0x00000001, 0x00000000,		// 0x0020
		0x00000000, 0x00000060, 0x00000000, 0x00000000,		// 0x0030
	};
	uint32 rdReg[] = {
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,		// 0x0000
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,		// 0x0010
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,		// 0x0020
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,		// 0x0030
	};

	memcpy(&rdReg, (uint32*)pUart, sizeof(resetValue));

	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);

	printf("Reset Value Read Test Start (%d)\n", ch);

	err = 0;
	addr = pUart;
	for(i=0 ; i<sizeof(resetValue)/sizeof(uint32) ; i++)
	{
		if(rdReg[i] != resetValue[i])
		{
			printf("addr(0x%08X) resetValue(%08X) regValue(%08X)\n", addr+(i*4), resetValue[i], rdReg[i]);
			err = 1;
		}
	}

	if(err)
		printf(">>> Result Fail!\n");
	else
		printf(">>> Result Success!\n");
	printf("Reset Value Read Test End (%d)\n", ch);
}

void uarttest_loopback (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint32 data;
	uint8 start_data = 48;	// '0' : 48(0x30)
	uint8 end_data = 90;	// 'Z' : 90(0x5A)
	uint8 reg;
	uint8 buf[100] = {0};
	uint8 err;

	
	//uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
	
	pUart->lcr |= (1<<7);
	pUart->lcr &= ~(1<<7);
	_delay(0x10000);
	
	
	// enable loopback mode //
	pUart->mcr |= (1<<4);

	for(data=start_data ; data<=end_data ; data++) //ascii code(0 ~ z)
	{
		do {
			reg = (pUart->lsr >> 5) & 0x1; // tx hold reg empty
		} while(!reg);
		
		pUart->thr = data;	//tx data

		do {
			reg = pUart->lsr & 0x1;	// data ready
		} while(!reg);

		buf[data] = pUart->rbr & 0xFF;	// rx data
 	}

	// disable loopback mode //
	pUart->mcr = 0;

	printf("loopback test start (%d)\n", ch);
	err = 0;
 	for(data=start_data ; data<=end_data ; data++) //ascii code(0 ~ z)
	{
		if(buf[data] != data)
		{
			printf("Fail!! (%c, %c)\n", buf[data], data);
			err = 1;
		}
	}

	if(err)
		printf(">>> Reault Fail!\n");
	else
		printf(">>> Reault Success\n");
		
	printf("loopback test end (%d)\n", ch);
}

void uarttest_baudrate (uint8 ch)
{
	uint8 i;
	uint32 baudrate;

#if 1
	{
		baudrate = 115200;
#else
	for(i=0 ; i<9 ; i++)
	{
		_delay(0x500000);
		
		switch(i)
		{
			case 0:		baudrate = 2400;	break;
			case 1:		baudrate = 4800;	break;
			case 2:		baudrate = 9600;	break;
			case 3:		baudrate = 19200;	break;
			case 4:		baudrate = 38400;	break;
			case 5:		baudrate = 57600;	break;
			case 6:		baudrate = 115200;	break;
			case 7:		baudrate = 230400;	break;
			case 8:		baudrate = 460800;	break;
		}
#endif
		uarttest_init(ch, baudrate, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
		printf("baudrate test => ch(%d), baudrate(%dbps), databit(8), stopbit(1), parity(no)\n", ch, baudrate);

	}
}

void uarttest_databit (uint8 ch)
{
	uint8 i;
	uint32 data;

#if 1
	{
		data = 3;
#else
	for(i=0 ; i<4 ; i++)
	{
		_delay(0x500000);
		
		switch(i)
		{
			case 0:		data = 0;	break;
			case 1:		data = 1;	break;
			case 2:		data = 2;	break;
			case 3:		data = 3;	break;
		}
#endif
		uarttest_init(ch, DEFAULT_BAUDRATE, data, DEFAULT_STOPBIT, DEFAULT_PARITY);
		printf("databit test => ch(%d), baudrate(115200bps), databit(%d), stopbit(1), parity(no)\n", ch, (data+5));
	}
}

void uarttest_stopbit (uint8 ch)
{
	uint8 i;
	uint32 stop;

#if 1
	{
		stop = 0x0;
#else
	for(i=0 ; i<2 ; i++)
	{
		_delay(0x500000);
		
		switch(i)
		{
			case 0:		stop = 0x0;	break;
			case 1:		stop = 0x4;	break;
		}
#endif
		uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, stop, DEFAULT_PARITY);
		printf("stopbit test => ch(%d), baudrate(115200bps), databit(8), stopbit(%d), parity(no)\n", ch, (stop==0) ? (stop+1) : (stop-2));
	}
}

void uarttest_parity (uint8 ch)
{
	uint8 i;
	uint32 parity;

#if 1
	{
		parity = 0x0;
#else
	for(i=0 ; i<3 ; i++)
	{
		_delay(0x500000);
		
		switch(i)
		{
			case 0:		parity = 0x0;	break;
			case 1:		parity = 0x8;	break;
			case 2:		parity = 0x18;	break;
		}
#endif
		uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, parity);
		printf("parity test => ch(%d), baudrate(115200bps), databit(8), stopbit(1), parity(0x%x)\n", ch, parity);
	}
}

void uarttest_tx_n_rx (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint8 reg;
	uint8 data;
	uint8 start_data = 48;	//ASCII (0 ~ Z) 
	uint8 end_data = 90;
	uint32 cnt;
	uint32 size;
	uint8 err;
	uint8 buf[1024] = {0};
	
//	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);

	size = 1024;	// 1K

	// transmit
	data = start_data;	
	for(cnt=0 ; cnt<size ; cnt++)
	{
		do {
			reg = (pUart->lsr >> 5) & 0x1; // tx hold reg empty
		} while(!reg);
		pUart->thr = data;	//tx data

		data++;
		if(data > end_data)
			data = start_data;
	}

	// receive
	for(cnt=0 ; cnt<size ; cnt++)
	{
		do {
			reg = pUart->lsr & 0x1;	// data ready
		} while(!reg);
		buf[cnt] = pUart->rbr & 0xFF;	// rx data
	}

	// verify
	err = 0;
	data = start_data;
	for(cnt=0 ; cnt<size ; cnt++)
	{
		if(buf[cnt] != data)
			err = 1;

		data++;
		if(data > end_data)
			data = start_data;
	}

	if(err)
		printf(">>> Result Fail!\n");
	else
		printf(">>> Result Success!\n");

	printf("%dbyte receive test End\n", size);
}

//#define USE_TRG_TEST
void uarttest_rx_trigger (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	
	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
	uarttest_init_rxfifo(ch, DEFAULT_RXLVL);
	uarttest_fifo_enable(ch);
	
	printf("rx trigger level test!\n");

	// rx trigger level
	//   0 : eRXLVL_NOT_EMPTY
	//   1 : eRXLVL_MORE_THAN_3
	//   2 : eRXLVL_MORE_THAN_7
	//   3 : eRXLVL_MORE_THAN_13
	pUart->fcr = (0<<6) | (1<<1);
	printf(">> not empty!\n");
	_delay(0x300000);
	
	pUart->fcr = (1<<6) | (1<<1);
	printf(">> more than 3!\n");
	_delay(0x300000);
	
	pUart->fcr = (2<<6) | (1<<1);
	printf(">> more than 7!\n");
	_delay(0x300000);
	
	pUart->fcr = (3<<6) | (1<<1);
	printf(">> more than 13!\n");
	_delay(0x300000);
}

uint8 trfifo[1024] = {0};
uint32 txIdx = 0, rxIdx = 0;
uint32 txcnt = 0;
void uarttest_tx_trigger (uint8 ch)
{
	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
	printf("rx trigger level test omission\n");
	
#if 0
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint32 i;
	uint8 data;
	uint8 start_data = 48;	//ASCII (0 ~ Z) 
	uint8 end_data = 90;
	uint8 buffer[100] = {0};
	uint32 cnt;
	uint32 size;

	data = start_data;
	size = 1024;
	for(cnt=0 ; cnt<size ; cnt++)
	{
		trfifo[cnt] = data;
		
		data++;
		if(data > end_data)
			data = start_data;
	}
	rxIdx = size;

	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
	
	pUart->ier = (1<<1);
	// tx trigger level
	//   0 : eTXLVL_NOT_FULL
	//   1 : eTXLVL_LESS_THAN_12
	//   2 : eTXLVL_LESS_THAN_8
	//   3 : eTXLVL_LESS_THAN_4
	pUart->fcr = (0<<4) | (1<<0) | (1<<2);
	
	initIntr();
	GIE_ENABLE();
	
	if(utilGetBits(pUart->lsr, 5, 1))
	{
		int i;
		for(i=0 ; i<16 ; i++)
			pUart->thr = trfifo[txIdx++];
	}
	
	while(1);
#endif
}

void uarttest_rx_n_tx_fifo (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint8 reg;
	uint8 err;

	uarttest_init(ch, DEFAULT_BAUDRATE, DEFAULT_DATABIT, DEFAULT_STOPBIT, DEFAULT_PARITY);
//	uarttest_init_fifo(ch, DEFAULT_RXLVL, DEFAULT_TXLVL);
	uarttest_init_rxfifo(ch, DEFAULT_RXLVL);
	uarttest_init_txfifo(ch, DEFAULT_TXLVL);
	uarttest_fifo_enable(ch);
}

#if defined(USE_TRG_TEST)
void uarttest_rx_int (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint8 buf[32] = {0};
	uint8 cnt = 0, i;

	while(utilGetBits(pUart->lsr, 0, 1))
	{
		buf[cnt++] = utilGetBits(pUart->rbr, 0, 8);
	}

	printf("rx trigger fifo test : %d byte\n", cnt);
	for(i=0 ; i<cnt ; i++)
		printf("%c", buf[i]);
	printf("\n");
}

void uarttest_tx_int (uint8 ch)
{
#if 0
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint8 reg;
	int i;

	txcnt++;

	if(utilGetBits(pUart->lsr, 5, 1))
	{
		if(rxIdx <= txIdx)
			return;
		
		for(i=0 ; i<16 ; i++)
			pUart->thr = trfifo[txIdx++];
	}
#endif
}

#else

void uarttest_rx_int (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint32 i;
	uint8 reg;

	for(i=0 ; i<10000 ; i++)
	{
		if(utilGetBit(pUart->lsr, 0))
		{
			trfifo[rxIdx++] = utilGetBits(pUart->rbr, 0, 8);
		}
	}

	if( (rxIdx > txIdx) && (utilGetBits(pUart->lsr, 5, 1)) )
		pUart->thr = trfifo[txIdx++];
}

void uarttest_tx_int (uint8 ch)
{
	UART_REG_T *pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	uint32 i;
	
	for(i=0 ; i<16 ; i++)
	{
		if(utilGetBits(pUart->lsr, 5, 1))
		{
			if(rxIdx <= txIdx)
				return;
			pUart->thr = trfifo[txIdx++];
		}
	}
}
#endif

