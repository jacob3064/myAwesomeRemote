/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <nds32_intrinsic.h>

#include "debug.h"
#include "system.h"
#include "du.h"
#include "sys_api.h"
#include "intr.h"
#include "uart.h"
#include "uart_test.h"

uint8 buffer[1024] = {0};
uint32 rdIdx = 0;
uint32 wrIdx = 0;
static void uartex_rx_int (uint8 ch)
{
	uint32 i;

	for(i=0 ; i<10000 ; i++)
	{
		if(uart_get_rxReady(ch))
		{
			buffer[rdIdx++] = uart_get_rxData(ch);
		}
	}

	if( (rdIdx > wrIdx) && uart_get_txEmpty(ch) )
		uart_set_txData(ch, buffer[wrIdx++]);
}

static void uartex_tx_int (uint8 ch)
{
	uint32 i;

	for(i=0 ; i<16 ; i++)
	{
		if(rdIdx <= wrIdx)
			return;
		if(uart_get_txEmpty(ch))
			uart_set_txData(ch, buffer[wrIdx++]);
	}
}

static void uartex_handler(uint8 IN ch, uint32 IN event)
{
	switch(event)
	{
		case eUART_INT_RX_REG_READY:	// rx
			uartex_rx_int(ch);
			break;
		case eUART_INT_TX_REG_EMPTY:	// tx
			uartex_tx_int(ch);
			break;
	}
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
int main()
{
	print_init();

	//-------------------------->>
	// example using API for user.
#if !defined(USE_VERIFICATION)
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_115200, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE, uartex_handler);
#endif
	//--------------------------<<

	//-------------------------->>
	// test for verification.
#if defined(USE_VERIFICATION)
	{
	uint8 ch = 0;
/*
	uarttest_reset_reg(ch);
	uarttest_loopback(ch);
	uarttest_baudrate(ch);
	uarttest_databit(ch);
	uarttest_stopbit(ch);
	uarttest_parity(ch);
	uarttest_tx_n_rx(ch);
*/
//	uarttest_rx_trigger(ch);
//	uarttest_tx_trigger(ch);	// omission.
//	uarttest_rx_n_tx_fifo(ch);
	}
#endif
	//--------------------------<<

	while(1);

	return 1;
}
