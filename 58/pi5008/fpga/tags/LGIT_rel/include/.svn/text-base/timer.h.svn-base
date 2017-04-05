#ifndef _PI5008_TIMER_H
#define _PI5008_TIMER_H

#include "system.h"
#include "type.h"

typedef struct tagTIMER_CH_REG_T
{
	vuint32 ctrl;							/* 0x020 		- Channel control register */
	vuint32 reload;							/* 0x024 		- Channel reload register */
	vuint32 counter;						/* 0x028 		- Channel counter register */
	vuint32 rsv;							/* 0x02c 		- Reserved */
}TIMER_CH_REG_T;

typedef struct tagTIMER_REG_T
{
	vuint32 id;								/* 0x000 		- ID and revision register */
	vuint32 rsv0[3];						/* 0x004 ~ 0x00c	- reserved */
	vuint32 cfg;							/* 0x010 		- Configuration register */
	vuint32 irq_enable;						/* 0x014 		- Interrupt enable register */
	vuint32 irq_status;						/* 0x018 		- Interrupt status register */
	vuint32 ch_enable;						/* 0x01c 		- Channel enable register */

	TIMER_CH_REG_T ch_reg[4];				/* 0x020 ~ 0x05c	- Channel register */
}TIMER_REG_T;

#define TIMER0_CTRL_REG			((TIMER_REG_T*)			(TIMER0_BASE_ADDR + 0x0000U))


void timer_set(int ch, unsigned int loadcnt);
void timer_kill(int ch);
unsigned int timer_get_cnt(int ch);


#endif
