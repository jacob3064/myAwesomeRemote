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

#include "debug.h"
#include "system.h"
#include "sys_api.h"
#include "interrupt.h"
#include "timer.h"



/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_38400, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE, NULL);
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
static uint32 timer_on = 0;
static uint32 timer_cnt[16];

static uint32 gIRQ_TEST_CNT = 100;
static uint8 gTIMER_DEV = 0;

static void ontimer(int32 ch)
{
	//printf("ontimer ch%d\n", ch);

	if(timer_cnt[ch]++ >= gIRQ_TEST_CNT){
		timer_on &= (~(1<<ch));
		timer_kill(gTIMER_DEV, ch);
		printf("timer kill. ch: %d\n", ch);
	}
}

static void timer_test_irq(TIMER_CFG *cfg, uint32 reload_val)
{
	int32 i;
	
	printf("=============================================\n");
	printf("Timer test\n");
	printf("=============================================\n");
	printf("Reload: 0x%x, Config - [0]: 0x%x, [1]: 0x%x, [2]: 0x%x, [3]: 0x%x\n", 
					reload_val, cfg->timer_mode[0], cfg->timer_mode[1], cfg->timer_mode[2], cfg->timer_mode[3]);
	
	timer_init(gTIMER_DEV, cfg, ontimer );

	timer_on = 0;
	memset(&timer_cnt, 0, sizeof(timer_cnt));

	for(i=0;i<cfg->n_timer_ch;i++){
		if(timer_set(gTIMER_DEV, i, reload_val, 1) < 0){
			printf("timer set fail\n");
			while(1);
		}else{
			timer_on |= (1<<i);
			printf("timer set. ch: %d, load cnt: 0x%x\n", i, reload_val);
		}
	}
	
	while(timer_on);

	printf("Timer test done\n");

}

static void timer_test_freerun(TIMER_CFG *cfg, uint32 reload_val)
{
	int32 i;
	uint32 curr_cnt;
	uint32 prev_cnt;
	uint32 mode_backup;
	
	printf("=============================================\n");
	printf("Timer test free run\n");
	printf("=============================================\n");
	printf("Reload: 0x%x, Config - [0]: 0x%x, [1]: 0x%x, [2]: 0x%x, [3]: 0x%x\n", 
					reload_val, cfg->timer_mode[0], cfg->timer_mode[1], cfg->timer_mode[2], cfg->timer_mode[3]);


	mode_backup = cfg->timer_mode[0];
	cfg->timer_mode[0] = PIT_MODE_TIMER32;
	timer_init(gTIMER_DEV, cfg, ontimer );

	timer_on = 0;
	memset(&timer_cnt, 0, sizeof(timer_cnt));
	gIRQ_TEST_CNT = 3;
	if(timer_set(gTIMER_DEV, 0, PCLK, 1) < 0){
		printf("timer set fail\n");
		while(1);
	}else{
		timer_on = 1;
	}
	printf("wait 4 second...\n");
	while(timer_on);



	cfg->timer_mode[0] = mode_backup;
	timer_init(gTIMER_DEV, cfg, ontimer );

	for(i=0;i<cfg->n_timer_ch;i++){
		if(timer_set(gTIMER_DEV, i, reload_val, 0) < 0){
			printf("timer set fail\n");
			while(1);
		}else{
			printf("timer set. ch: %d, load cnt: 0x%x\n", i, reload_val);
		}
	}

	prev_cnt = timer_get_cnt(gTIMER_DEV, 0);
	while(1){
		curr_cnt = timer_get_cnt(gTIMER_DEV, 0);
		if( curr_cnt > prev_cnt || (curr_cnt + 0x1000) < prev_cnt){
			for(i=0;i<cfg->n_timer_ch;i++){
				printf("timer[%d]: 0x%x\n", i, timer_get_cnt(gTIMER_DEV, i));
			}

			prev_cnt = curr_cnt;
		}
	}


}

static void pwm_test(TIMER_CFG *cfg, uint32 high_period, uint32 low_period)
{
	int32 i;

	printf("=============================================\n");
	printf("PWM test\n");
	printf("=============================================\n");
	printf("high period: 0x%x, low period: 0x%x, Config - [0]: 0x%x, [1]: 0x%x, [2]: 0x%x, [3]: 0x%x\n", 
					high_period, low_period, cfg->timer_mode[0], cfg->timer_mode[1], cfg->timer_mode[2], cfg->timer_mode[3]);
	
	timer_init(gTIMER_DEV, cfg, NULL );
  
	for(i=0;i<cfg->n_pwm_ch;i++){
		if(pwm_set(gTIMER_DEV, i, high_period, low_period) < 0){
			printf("pwm set fail\n");
			while(1);
		}else{
 			printf("pwm set. ch: %d, high period: 0x%x, low period: 0x%x\n", i, high_period, low_period);
		}
	}
 
 
}


int main()
{

	TIMER_CFG cfg;
	
	system_initialize();
	print_init();
	MAIN_PRINT("Timer test begin\n");
	gen_swi();

	
	cfg.timer_mode[0] = PIT_MODE_TIMER32;
	cfg.timer_mode[1] = PIT_MODE_TIMER16;
	cfg.timer_mode[2] = PIT_MODE_PWM_TIMER16_MIX;
	cfg.timer_mode[3] = PIT_MODE_TIMER32;
	timer_test_irq(&cfg, 0xffff);


	cfg.timer_mode[0] = PIT_MODE_TIMER32;
	cfg.timer_mode[1] = PIT_MODE_TIMER32;
	cfg.timer_mode[2] = PIT_MODE_TIMER32;
	cfg.timer_mode[3] = PIT_MODE_TIMER32;
	timer_test_freerun(&cfg, PCLK*4);

#if 0
	cfg.timer_mode[0] = PIT_MODE_PWM;
	cfg.timer_mode[1] = PIT_MODE_TIMER32;
	cfg.timer_mode[2] = PIT_MODE_TIMER32;
	cfg.timer_mode[3] = PIT_MODE_TIMER32;
	pwm_test(&cfg, 0xff, 0x2ff);
#endif	

	while(1);

	return 1;
}
