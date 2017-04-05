#ifndef __UART_TEST_H__
#define __UART_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************************************************************************
 * Declaration
***************************************************************************************************************************************************************/


/***************************************************************************************************************************************************************
 * Enumeration
***************************************************************************************************************************************************************/

/***************************************************************************************************************************************************************
 * Structure
***************************************************************************************************************************************************************/

/***************************************************************************************************************************************************************
 * Function
***************************************************************************************************************************************************************/
extern void uarttest_reset_reg (uint8 ch);
extern void uarttest_loopback (uint8 ch);
extern void uarttest_baudrate (uint8 ch);
extern void uarttest_databit (uint8 ch);
extern void uarttest_stopbit (uint8 ch);
extern void uarttest_parity (uint8 ch);
extern void uarttest_tx_n_rx (uint8 ch);
extern void uarttest_rx_trigger (uint8 ch);
extern void uarttest_tx_trigger (uint8 ch);
extern void uarttest_rx_n_tx_fifo (uint8 ch);


#ifdef __cplusplus
}
#endif

#endif //__UART_TEST_H__
