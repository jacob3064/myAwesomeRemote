/*----------------------------------------------------------------
//                                                              //
// Copyright (C) 2010 Authors and OPENCORES.ORG                 //
//                                                              //
// This source file may be used and distributed without         //
// restriction provided that this copyright statement is not    //
// removed from the file and that any derivative work contains  //
// the original copyright notice and the associated disclaimer. //
//                                                              //
// This source file is free software; you can redistribute it   //
// and/or modify it under the terms of the GNU Lesser General   //
// Public License as published by the Free Software Foundation; //
// either version 2.1 of the License, or (at your option) any   //
// later version.                                               //
//                                                              //
// This source is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the implied   //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      //
// PURPOSE.  See the GNU Lesser General Public License for more //
// details.                                                     //
//                                                              //
// You should have received a copy of the GNU Lesser General    //
// Public License along with this source; if not, download it   //
// from http://www.opencores.org/lgpl.shtml                     //
//                                                              //
----------------------------------------------------------------*/

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <nds32_intrinsic.h>

#include "intr.h"
#include "system.h"
#include "uart.h"
#include "debug.h"
#include "timer.h"
#include "sys_api.h"
#include "utils.h"

static UART_REG_T *gpUart[] = {
	UART0_CTRL_REG,
	UART1_CTRL_REG,
};
static UART_CALLBACK gUartCallback = NULL;

uint32 uart_get_id_revision(int32 ch)
{
    return gpUart[ch]->id;
}

uint8 uart_get_rxReady (uint8 ch)
{
	return utilGetBits(gpUart[ch]->lsr, 0, 1);
}

uint8 uart_get_rxData (uint8 ch)
{
	return utilGetBits(gpUart[ch]->rbr, 0, 8);
}

uint8 uart_get_txEmpty (uint8 ch)
{
	return utilGetBits(gpUart[ch]->lsr, 5, 1);
}

void uart_set_txData (uint8 ch, uint8 data)
{
	gpUart[ch]->thr = data;
}

#if !defined(USE_VERIFICATION)
void uart_isr(int32 num)
{
	uint32 msk = (1 << num);
	UART_REG_T *pUart;
	uint8 intrID, ch;
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	ch = 0;// ch = (UART_IRQ_STATUS_REG >> 1) & 0x1;	//	TBD.
	pUart = (UART_REG_T *)((ch) ? UART1_CTRL_REG : UART0_CTRL_REG);
	intrID = pUart->iir & 0xF;

	if(gUartCallback)
	{
		gUartCallback(0, intrID);
	}
	
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}
#endif

void uart_init(uint8 ch, uint32 baudrate, uint32 databit, uint32 stopbit, uint32 parity)
{
	uint16 	div = 0;

	div = PCLK / (16 * baudrate);

	// Set DLAB to 1
	gpUart[ch]->lcr |= (1<<7);

	// Set OSCR (default value)
	gpUart[ch]->oscr = 16;

	// Set DLL for baudrate
	gpUart[ch]->dll = ((div >> 0) & 0xff);
	gpUart[ch]->dlm = ((div >> 8) & 0xff);

	// Set DLAB to 0
	gpUart[ch]->lcr &= ~(1<<7);

	// LCR: data bit, stop bit, parity
	gpUart[ch]->lcr = (databit | stopbit | parity);
}

void uart_init_rxfifo (uint8 ch, uint8 level)
{
	// IER: Interrupt enable register.
	gpUart[ch]->ier |= (1<<0);
	
	// FCR: Set rx trigger level & Reset rx
	gpUart[ch]->fcr |= (level<<6) | (1<<1);
}

void uart_init_txfifo (uint8 ch, uint8 level)
{
	// IER: Interrupt enable register.
	gpUart[ch]->ier |= (1<<1);
	
	// FCR: Set tx trigger level & Reset tx
	gpUart[ch]->fcr |= (level<<4) | (1<<2);
}

void uart_fifo_enable (uint8 ch)
{
	// FCR: Enable FIFO
	gpUart[ch]->fcr |= (1<<0);
}


void uart_set_callback (UART_CALLBACK callback)
{
	gUartCallback = callback;
}

void uart_outbyte(uint8 ch, uint8 c)
{
	unsigned int reg = 0;
	
	do {
		reg = uart_get_txEmpty(ch);
	} while(!reg);

	uart_set_txData(ch, c);
}

uint8 uart_inbyte(uint8 ch)
{
	char reg;
	
	do{
		reg = uart_get_rxReady(ch);
	}while(!reg);

	return uart_get_rxData(ch);
}

