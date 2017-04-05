/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <nds32_intrinsic.h>

//#include "debug.h"
#include "system.h"
#include "sys_api.h"
#include "gpio.h"
#include "intr.h"

uint32 gGPIO_DEV = 0;

/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_115200, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE);
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
static void __delay ( uint32 count )
{
	while(count-- > 0){
		asm volatile ("nop");
	}

}

static void gpio_callback_dev0(int ch)
{
	printf("gpio callback dev0. ch: %d, val: %d\n", ch, gpio_get_value(0, ch));
	printf("gpio callback dev0. ch: %d, val: %d\n", ch, gpio_get_value(0, ch));
	printf("gpio callback dev0. ch: %d, val: %d\n", ch, gpio_get_value(0, ch));
	printf("gpio callback dev0. ch: %d, val: %d\n", ch, gpio_get_value(0, ch));
	printf("gpio callback dev0. ch: %d, val: %d\n\n", ch, gpio_get_value(0, ch));
}
static void gpio_callback_dev1(int ch)
{
	printf("gpio callback dev1. ch: %d, val: %d\n", ch, gpio_get_value(1, ch));
}


static void LED_TEST(void)
{
	int32 cnt = 0;
	int32 val = 1;
	int32 i;

	printf("LED TEST BEGIN\n");

	for(i=0;i<8;i++){
		gpio_set_dir( 0, i, GPIO_DIR_OUT, 0);
	}

	while(++cnt < 10){
		val ^= 1;
		for(i=0;i<8;i++){
			gpio_set_value(0, i, val);
			__delay(0x200000);
		}
		
	}

	printf("LED TEST FINISHED\n");

}

static void SW_KEY_IN_TEST(void)
{
	int32 i;

	printf("Waiting for gpin...\n");
	
	for(i=0;i<16;i++){
		gpio_set_dir( 0, i, GPIO_DIR_IN, 0);
		gpio_set_irq_en( 0, i, 1);
	}

}

static void MICOM_IN_TEST(void)
{
	int32 i;

	printf("Waiting for micom gpin...\n");
	
	for(i=0;i<16;i++){
		gpio_set_dir( 1, i, GPIO_DIR_IN, 0);
		gpio_set_irq_en( 1, i, 1);
	}

	while(1){
		printf("gpin: 0x%x\n", gpio_get_value(1, 0));
		__delay(0x800000);
		
	}
}

static void MICOM_OUT_TEST(void)
{
	int32 i;
	int32 val = 0;
	
	for(i=0;i<16;i++){
		gpio_set_dir( 1, i, GPIO_DIR_OUT, 0);
		gpio_set_irq_en( 1, i, 1);
	}

	while(1){
		for(i=0;i<16;i++){
			gpio_set_value( 1, i, val);
		}	
		__delay(0x800000);

		val ^= 1;
	}

}

int main()
{
	system_initialize();
	print_init();
	printf("main begin\n");

	gpio_init(0, gpio_callback_dev0);
	gpio_init(1, gpio_callback_dev1);

	//LED_TEST();
	//MICOM_IN_TEST();
	//MICOM_OUT_TEST();
	SW_KEY_IN_TEST();
	
	while(1);

	return 1;
}
