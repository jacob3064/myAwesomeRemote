#ifndef _PI5008_GPIO_H
#define _PI5008_GPIO_H

#include "system.h"

#define rGPIO_ID(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x00)    /* ID and revision register */
#define rGPIO_CFG(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x10)    /* Configuration register */
#define rGPIO_DIN(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x20)    /* Channel data in register */
#define rGPIO_DOUT(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x24)    /* Channel data out register */
#define rGPIO_DIR(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x28)    /* Channel direction register */
#define rGPIO_DOCLR(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x2c)    /* Channel data out clear register */
#define rGPIO_DOSET(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x30)    /* Channel data out set register */
#define rGPIO_IRQEN(ch)		(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x50)    /* Interrupt enable register */
#define rGPIO_IRQMODE0(ch)	(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x54)    /* Interrupt mode register (0 ~ 7)*/
#define rGPIO_IRQMODE1(ch)	(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x58)    /* Interrupt mode register (8 ~ 15)*/
#define rGPIO_IRQMODE2(ch)	(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x5c)    /* Interrupt mode register (16 ~ 23)*/
#define rGPIO_IRQMODE3(ch)	(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x60)    /* Interrupt mode register (24 ~ 31)*/
#define rGPIO_IRQSTATUS(ch)	(GPIO0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x64)    /* Interrupt status register */


// GPIO Direction
#define GPIO_DIR_IN		0
#define GPIO_DIR_OUT	1


void gpio_init(int port);
void gpio_set_dir(int port, int pin, int dir, int out_val);
int gpio_get_dir(int port, int pin);
void gpio_set_irq_en(int port, int pin, int enable);
int gpio_get_irq_en(int port, int pin);
int gpio_set_value(int port, int pin, int value);
int gpio_get_out_value(int port, int pin, int value);
int gpio_get_value(int port, int pin);

#endif
