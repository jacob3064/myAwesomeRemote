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

#if defined(SYSTEM_BUS_DW)
#define MAX_PIT_DEV_NUM		2
#else
#define MAX_PIT_DEV_NUM		1
#endif
#define MAX_TIMER_NUM		16
#define MAX_PWM_NUM			4

typedef struct tagPIT_CHANNEL_INFO_T
{
	uint32 idx;				// timer ch
	uint32 sub_idx;			// timer sub ch
	uint32 bit_pos;			// bit position of reload value for timer or pwm low period
	uint32 nbits;			// reload value bits
}PIT_CHANNEL_INFO;


static TIMER_REG_T *gpTimer[] = {
	TIMER0_CTRL_REG,
#if defined(SYSTEM_BUS_DW)
	TIMER1_CTRL_REG,
#endif
};

/*
static PIT_CHANNEL_INFO gTimerCh[MAX_PIT_DEV_NUM][MAX_TIMER_NUM];
static PIT_CHANNEL_INFO gPWMCh[MAX_PIT_DEV_NUM][MAX_PWM_NUM];
static uint32 gnTimer[MAX_PIT_DEV_NUM];
static uint32 gnPWM[MAX_PIT_DEV_NUM];

static TIMER_CALLBACK gTIMERCallback[MAX_PIT_DEV_NUM];
*/
typedef struct tagTIMER_CONTEXT_T
{
	PIT_CHANNEL_INFO TimerCh[MAX_TIMER_NUM];
	PIT_CHANNEL_INFO PWMCh[MAX_PWM_NUM];
	uint32 nTimer;
	uint32 nPWM;
	
	TIMER_CALLBACK TIMERCallback;
}TIMER_CONTEXT;

static TIMER_CONTEXT gCTX[MAX_PIT_DEV_NUM];

static void show_timer_info(uint8 dev)
{
	int i;

	printf("dev: %d, timer ch: %d, pwm ch: %d\n", dev, gCTX[dev].nTimer, gCTX[dev].nPWM);
	for(i=0;i<gCTX[dev].nTimer;i++){
		printf("[dev: %d, ch%d]Timer idx: %d, sub: %d, bits: %d\n", dev, i, gCTX[dev].TimerCh[i].idx, gCTX[dev].TimerCh[i].sub_idx, gCTX[dev].TimerCh[i].nbits);
	}

	
	for(i=0;i<gCTX[dev].nPWM;i++){
		printf("[dev: %d, ch%d]PWM idx: %d, sub: %d, bits: %d\n", dev, i, gCTX[dev].PWMCh[i].idx, gCTX[dev].PWMCh[i].sub_idx, gCTX[dev].PWMCh[i].nbits);
	}
}
int32 timer_init(uint8 dev, TIMER_CFG *cfg, TIMER_CALLBACK timer_callback )
{
	int32 i;
	int32 timer_ch;
	int32 pwm_ch;

	gpTimer[dev]->ch_enable = 0;
	gpTimer[dev]->irq_enable = 0;
	
	timer_ch = gCTX[dev].nTimer = 0;
	pwm_ch = gCTX[dev].nPWM = 0;

	for(i=0;i<4;i++){
		if(cfg->timer_mode[i] == PIT_MODE_TIMER32){
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 0;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 0;
			gCTX[dev].TimerCh[timer_ch].nbits = 32;
			timer_ch++;
		}else if(cfg->timer_mode[i] == PIT_MODE_TIMER16){
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 0;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 0;
			gCTX[dev].TimerCh[timer_ch].nbits = 16;
			timer_ch++;
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 1;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 16;
			gCTX[dev].TimerCh[timer_ch].nbits = 16;
			timer_ch++;
		}else if(cfg->timer_mode[i] == PIT_MODE_TIMER8){
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 0;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 0;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 1;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 8;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 2;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 16;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 3;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 24;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;
		}else if(cfg->timer_mode[i] == PIT_MODE_PWM){
			gCTX[dev].PWMCh[pwm_ch].idx = i;
			gCTX[dev].PWMCh[pwm_ch].sub_idx = 3;
			gCTX[dev].PWMCh[pwm_ch].bit_pos = 0;
			gCTX[dev].PWMCh[pwm_ch].nbits = 16;
			pwm_ch++;
		}else if(cfg->timer_mode[i] == PIT_MODE_PWM_TIMER16_MIX){
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 0;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 0;
			gCTX[dev].TimerCh[timer_ch].nbits = 16;
			timer_ch++;

			gCTX[dev].PWMCh[pwm_ch].idx = i;
			gCTX[dev].PWMCh[pwm_ch].sub_idx = 3;
			gCTX[dev].PWMCh[pwm_ch].bit_pos = 16;
			gCTX[dev].PWMCh[pwm_ch].nbits = 8;
			pwm_ch++;
		}else if(cfg->timer_mode[i] == PIT_MODE_PWM_TIMER8_MIX){
			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 0;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 0;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;

			gCTX[dev].TimerCh[timer_ch].idx = i;
			gCTX[dev].TimerCh[timer_ch].sub_idx = 1;
			gCTX[dev].TimerCh[timer_ch].bit_pos = 8;
			gCTX[dev].TimerCh[timer_ch].nbits = 8;
			timer_ch++;

			gCTX[dev].PWMCh[pwm_ch].idx = i;
			gCTX[dev].PWMCh[pwm_ch].sub_idx = 3;
			gCTX[dev].PWMCh[pwm_ch].bit_pos = 16;
			gCTX[dev].PWMCh[pwm_ch].nbits = 8;
			pwm_ch++;
		}

		gpTimer[dev]->ch_reg[i].ctrl = (((cfg->pwm_init_val&1)<<4) | (1<<3) | cfg->timer_mode[i]);
		
	}
	gCTX[dev].nTimer = timer_ch;
	gCTX[dev].nPWM = pwm_ch;
	cfg->n_timer_ch = gCTX[dev].nTimer;
	cfg->n_pwm_ch = gCTX[dev].nPWM;

	gCTX[dev].TIMERCallback = timer_callback;
 
	if(gCTX[dev].nPWM >= MAX_PWM_NUM){
		printf("Error! Too many pwm initialized\n");
		return -1;
	}
	if(gCTX[dev].nTimer >= MAX_TIMER_NUM){
		printf("Error! Too many timer initialized\n");
		return -1;
	}

	show_timer_info(dev);

	return 1;
}


int32 timer_set(uint8 dev, int32 ch, uint32 loadcnt, uint8 irq_en)
{

	if(ch >= gCTX[dev].nTimer){
		printf("ch over max. dev: %d, ch: %d, max: %d\n", dev, ch, gCTX[dev].nTimer);
		return -1;
	}
	if(loadcnt > (utilPower2(gCTX[dev].TimerCh[ch].nbits)-1)){
		printf("load cnt over max. dev: %d, ch: %d, loadcnt: %d, nbits: %d, max: 0x%x\n", dev, ch, loadcnt, gCTX[dev].TimerCh[ch].nbits, (uint32)(utilPower2(gCTX[dev].TimerCh[ch].nbits)-1));
		return -1;
	}

	gpTimer[dev]->ch_enable = utilClearBit(gpTimer[dev]->ch_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);					// timer disable
	gpTimer[dev]->ch_reg[gCTX[dev].TimerCh[ch].idx].reload = 
		utilPutBits(gpTimer[dev]->ch_reg[gCTX[dev].TimerCh[ch].idx].reload, gCTX[dev].TimerCh[ch].bit_pos, gCTX[dev].TimerCh[ch].nbits, loadcnt);		// timer reload
	gpTimer[dev]->ch_enable = utilSetBit(gpTimer[dev]->ch_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);						// timer enable
	if(irq_en)
		gpTimer[dev]->irq_enable = utilSetBit(gpTimer[dev]->irq_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);				// irq enable
	else
		gpTimer[dev]->irq_enable = utilClearBit(gpTimer[dev]->irq_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);				// irq disable

	
	//printf("timer set. ch: %d, enable: 0x%x, reload: 0x%x, irqen: 0x%x\n", ch, gpTimer->ch_enable, gpTimer->ch_reg[gTimerCh[ch].idx].reload, gpTimer->irq_enable);
	//printf("idx: %d, sub: %d, pos: %d\n", gTimerCh[ch].idx, gTimerCh[ch].sub_idx, pos); 


	return 1;
}


int32 timer_kill(uint8 dev, int32 ch)
{

	if(ch >= gCTX[dev].nTimer)return -1;
	
	gpTimer[dev]->ch_enable = utilClearBit(gpTimer[dev]->ch_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);					// timer enable
	gpTimer[dev]->irq_enable = utilClearBit(gpTimer[dev]->irq_enable, gCTX[dev].TimerCh[ch].idx*4 + gCTX[dev].TimerCh[ch].sub_idx);					// irq enable

	return 1;
}

int32 pwm_set(uint8 dev, int32 ch, uint32 high_period, uint32 low_period )
{
	if(ch >= gCTX[dev].nPWM){
		printf("ch over max. dev: %d, ch: %d, max: %d\n", dev, ch, gCTX[dev].nPWM);
		return -1;
	}
	if(utilMax(high_period, low_period) > (utilPower2(gCTX[dev].PWMCh[ch].nbits)-1)){
		printf("period over max. dev: %d, ch: %d, high period: %d, low period: %d, nbits: %d, max: 0x%x\n", 
						dev, ch, high_period, low_period, gCTX[dev].PWMCh[ch].nbits, (uint32)(utilPower2(gCTX[dev].PWMCh[ch].nbits)-1));
		return -1;
	}

	gpTimer[dev]->ch_enable = utilClearBit(gpTimer[dev]->ch_enable, gCTX[dev].PWMCh[ch].idx*4 + gCTX[dev].PWMCh[ch].sub_idx);									// pwm disable
	gpTimer[dev]->ch_reg[gCTX[dev].PWMCh[ch].idx].reload = 
		utilPutBits(gpTimer[dev]->ch_reg[gCTX[dev].PWMCh[ch].idx].reload, gCTX[dev].PWMCh[ch].bit_pos, gCTX[dev].PWMCh[ch].nbits, low_period);						// pwm low reload
	gpTimer[dev]->ch_reg[gCTX[dev].PWMCh[ch].idx].reload = 
		utilPutBits(gpTimer[dev]->ch_reg[gCTX[dev].PWMCh[ch].idx].reload, gCTX[dev].PWMCh[ch].bit_pos + gCTX[dev].PWMCh[ch].nbits, gCTX[dev].PWMCh[ch].nbits, high_period);	// pwm high reload

	gpTimer[dev]->ch_enable = utilSetBit(gpTimer[dev]->ch_enable, gCTX[dev].PWMCh[ch].idx*4 + gCTX[dev].PWMCh[ch].sub_idx); 									// pwm enable
 
	return 1;
}

int32 pwm_kill(uint8 dev, int32 ch)
{

	if(ch >= gCTX[dev].nPWM)return -1;
	
	gpTimer[dev]->ch_enable = utilClearBit(gpTimer[dev]->ch_enable, gCTX[dev].PWMCh[ch].idx*4 + gCTX[dev].PWMCh[ch].sub_idx);					// timer enable
 
	return 1;
}

uint32 timer_get_cnt(uint8 dev, int32 ch)
{

	if(ch >= gCTX[dev].nTimer)return 0;

	return gpTimer[dev]->ch_reg[gCTX[dev].TimerCh[ch].idx].counter;
}

void timer_isr(uint32 num)
{
	uint32 msk = (1 << num);
	uint32 reg;
	int32 i,j;
	uint8 dev = 0;
#if defined(SYSTEM_BUS_DW)	
	vuint32 *irq_reg = (vuint32 *)TIMER_IRQ_STATUS_ADDR;	// need not to clear
#endif	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);


	for(j=0;j<MAX_PIT_DEV_NUM;j++){
#if defined(SYSTEM_BUS_DW)
		if((*irq_reg>>j)&1)dev = j;
#endif
		reg = gpTimer[dev]->irq_status;
		//printf("timer. reg: 0x%x\n", reg);
		for(i=0;i<gCTX[dev].nTimer;i++){
			if(reg & (1<<(gCTX[dev].TimerCh[i].idx*4 + gCTX[dev].TimerCh[i].sub_idx))){
				//printf("ISR. Timer ch: %d\n", i);
				if(gCTX[dev].TIMERCallback)gCTX[dev].TIMERCallback(i);
			}
		}
		gpTimer[dev]->irq_status = reg;
	}	
    __nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}


