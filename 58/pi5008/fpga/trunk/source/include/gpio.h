#ifndef _PI5008_GPIO_H
#define _PI5008_GPIO_H

#include "system.h"
#include "type.h"


typedef struct tagGPIO_REG_T
{
	vuint32 id;								/* 0x000 		- ID and revision register */
	vuint32 rsv0[3];						/* 0x004 ~ 0x00c	- reserved */
	vuint32 cfg;							/* 0x010			- Configuration register */
	vuint32 rsv1[3];						/* 0x014 ~ 0x01c	- reserved */
	vuint32 din;							/* 0x020			- Channel data in register */
	vuint32 dout;							/* 0x024			- Channel data out register */
	vuint32 dir;							/* 0x028			- Channel direction register */
	vuint32 dout_clr;						/* 0x02c			- Channel data out clear register */
	vuint32 dout_set;						/* 0x030			- Channel data out set register */
	vuint32 rsv2[3];						/* 0x034 ~ 0x03c	- reserved */
	vuint32 pull_enable;					/* 0x040			- Channel pull enable register */
	vuint32 pull_type;						/* 0x044			- Channel pull type register */
	vuint32 rsv3[2];						/* 0x048 ~ 0x04c	- reserved */
	vuint32 irq_enable;						/* 0x050			- Interrupt enable register */
	vuint32 irq_mode0;						/* 0x054			- Interrupt mode0 register */
	vuint32 irq_mode1;						/* 0x058			- Interrupt mode1 register */
	vuint32 irq_mode2;						/* 0x05c			- Interrupt mode2 register */
	vuint32 irq_mode3;						/* 0x060			- Interrupt mode3 register */
	vuint32 irq_status;						/* 0x064			- Interrupt status register */
	vuint32 rsv4[2];						/* 0x068 ~ 0x06c	- reserved */
	vuint32 debounce_en;					/* 0x070			- Debounce enable register*/
	vuint32 debounce_ctrl;					/* 0x074			- Debounce control register */
	vuint32 rsv5[2];						/* 0x078 ~ 0x07c	- reserved */
}GPIO_REG_T;

#define GPIO0_CTRL_REG			((GPIO_REG_T*)			(GPIO0_BASE_ADDR))
#if defined(SYSTEM_BUS_DW)
#define GPIO1_CTRL_REG			((GPIO_REG_T*)			(GPIO1_BASE_ADDR))
#endif


// GPIO Direction
#define GPIO_DIR_IN		0
#define GPIO_DIR_OUT	1

typedef void (*GPIO_CALLBACK) (int32 ch);


void gpio_init(int32 dev, GPIO_CALLBACK gpio_callback);
void gpio_set_dir(int32 dev, int32 pin, int32 dir, int32 out_val);
int32 gpio_get_dir(int32 dev, int32 pin);
void gpio_set_irq_en(int32 dev, int32 pin, int32 enable);
int32 gpio_get_irq_en(int32 dev, int32 pin);
int32 gpio_set_value(int32 dev, int32 pin, int32 value);
int32 gpio_get_out_value(int32 dev, int32 pin, int32 value);
int32 gpio_get_value(int32 dev, int32 pin);

#endif
