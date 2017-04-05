#ifndef _PI5008_TIMER_H
#define _PI5008_TIMER_H

#include "system.h"
#include "type.h"


#define PIT_MODE_TIMER32				1
#define PIT_MODE_TIMER16				2
#define PIT_MODE_TIMER8					3
#define PIT_MODE_PWM					4
#define PIT_MODE_PWM_TIMER16_MIX		6
#define PIT_MODE_PWM_TIMER8_MIX			7

typedef void (*TIMER_CALLBACK) (int32 ch);
 
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

#define TIMER0_CTRL_REG			((TIMER_REG_T*)			(TIMER0_BASE_ADDR))
#if defined(SYSTEM_BUS_DW)
#define TIMER1_CTRL_REG			((TIMER_REG_T*)			(TIMER1_BASE_ADDR))
#endif

typedef struct tagTIMER_CFG_T
{
	uint32 timer_mode[4];	// IN
	uint32 pwm_init_val;	// IN
	uint32 n_timer_ch;		// OUT
	uint32 n_pwm_ch;		// OUT
}TIMER_CFG;

int32 timer_init(uint8 dev, TIMER_CFG *cfg, TIMER_CALLBACK timer_callback);
int32 timer_set(uint8 dev, int32 ch, uint32 loadcnt, uint8 irq_en);
int32 timer_kill(uint8 dev, int32 ch);
uint32 timer_get_cnt(uint8 dev, int32 ch);
int32 pwm_set(uint8 dev, int32 ch, uint32 high_period, uint32 low_period);
int32 pwm_kill(uint8 dev, int32 ch);



#endif
