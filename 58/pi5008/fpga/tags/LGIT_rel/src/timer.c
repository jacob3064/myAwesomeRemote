#include <stdio.h>
#include <nds32_intrinsic.h>
#include "intr.h"
#include "interrupt.h"
#include "system.h"
#include "timer.h"
#include "debug.h"
#include "intr.h"
#include "utils.h"

// down counter from reload value to 0

static TIMER_REG_T *gpTimer = TIMER0_CTRL_REG;

void timer_set(int ch, unsigned int loadcnt)
{
/*
	BITCLR(rTIMER_EN(dev), (1<<(ch*4)));				// timer disable
	SetRegValue(rTIMER_CHCTRL(dev, ch), 0x9);		// use apb clock, 32bit timer
	SetRegValue(rTIMER_CHRELOAD(dev, ch), loadcnt);		// timer reload
	BITSET(rTIMER_EN(dev), (1<<(ch*4)));				// timer enable
	BITSET(rTIMER_IRQEN(dev), (1<<(ch*4)));			// interrupt enable
*/
	gpTimer->ch_enable = utilClearBit(gpTimer->ch_enable, ch*4);			// timer disable
	gpTimer->ch_reg[ch].ctrl= 0x9;									// use apb clock, 32bit timer
	gpTimer->ch_reg[ch].reload = loadcnt;							// timer reload
	gpTimer->ch_enable = utilSetBit(gpTimer->ch_enable, ch*4);			// timer enable
	gpTimer->irq_enable = utilSetBit(gpTimer->irq_enable, ch*4);	// interrupt enable
	
	
}

void timer_kill(int ch)
{
/*
	BITCLR(rTIMER_EN(dev), (1<<(ch*4)));					// timer disable
	BITCLR(rTIMER_IRQEN(dev), (1<<(ch*4)));					// interrupt disable
*/
	gpTimer->ch_enable = utilClearBit(gpTimer->ch_enable, ch*4);			// timer disable
	gpTimer->irq_enable = utilClearBit(gpTimer->irq_enable, ch*4);	// interrupt disable
}

unsigned int timer_get_cnt(int ch)
{
//	return (unsigned int)GetRegValue(rTIMER_CHCNT(dev,ch));

	return gpTimer->ch_reg[ch].counter;
}

void timer_isr(unsigned int num)
{
	unsigned int msk = (1 << num);
	unsigned int reg;
	int i;
	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);

	printf("timer isr\n");

	//reg = GetRegValue(rTIMER_IRQST(0));
	reg = gpTimer->irq_status;
	for(i=0;i<4;i++){
		if((reg >> (i*4))&1){
			TIMER_PRINT("ISR. Timer0 ch: %d\n", i);
		}
	}
	
	//SetRegValue(rTIMER_IRQST(0), 0xffffffff);
	gpTimer->irq_status = 0xffffffff;
	printf("irq status(0x%08x): 0x%08x\n", (unsigned int)&gpTimer->irq_status, gpTimer->irq_status);
	
    __nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}

void debug_timer(void)
{
	TIMER_PRINT("timer ctrl: 0x%x\n", gpTimer->ch_reg[0].ctrl );
	TIMER_PRINT("timer reload: 0x%x\n", gpTimer->ch_reg[0].reload);
	TIMER_PRINT("timer enable: 0x%x\n", gpTimer->ch_enable);
	TIMER_PRINT("timer irqen: 0x%x\n", gpTimer->irq_enable);
	TIMER_PRINT("timer irqst: 0x%x\n", gpTimer->irq_status);
}

