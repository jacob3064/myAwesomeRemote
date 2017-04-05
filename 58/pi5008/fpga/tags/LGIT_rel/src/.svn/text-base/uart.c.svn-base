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

#include "system.h"
#include "uart.h"
#include "utils.h"

static UART_REG_T *gpUart[] = {
	UART0_CTRL_REG,
	UART1_CTRL_REG,
};

static char GetRxReady(int ch)
{
	return utilGetBit(gpUart[ch]->lsr, 0);
}

static char GetRxHoldRegister(int ch)
{
	return utilGetBits(gpUart[ch]->rbr, 0, 8);
}


void uart_init(int ch, int baudrate, int databit, int stopbit, int parity)
{
	unsigned short 	div = 0;

	div = PCLK / (16 * baudrate);

	/* Set DLAB to 1 */
	gpUart[ch]->lcr |= DLAB_ENABLE;

	/* Set DLL for baudrate */
	gpUart[ch]->dll = ((div >> 0) & 0xff);
	gpUart[ch]->dlm = ((div >> 8) & 0xff);


	/* LCR: Length 8, 1 stop bits, no parity */
	gpUart[ch]->lcr = (databit | stopbit | parity);

	
	/* FCR: Enable FIFO, reset TX and RX. */
	gpUart[ch]->fcr = (FIFO_ENABLE | RXFIFO_RESET | TXFIFO_RESET);
}


void outbyte(int ch, char c)
{
	unsigned int reg = 0;

	do {
		reg = utilGetBits(gpUart[ch]->lsr, 5, 1);
	} while(!reg);

	gpUart[ch]->thr = c;
}

char inbyte(int ch)
{
	char reg;
	
	do{
		reg = GetRxReady(ch);
	}while(!reg);

	return GetRxHoldRegister(ch);
}

