#include <stdio.h>
#include <nds32_intrinsic.h>
#include "intr.h"
#include "gpio.h"
#include "system.h"
#include "utils.h"

#if defined(SYSTEM_BUS_DW)
#define MAX_GPIO_DEV_NUM		2
#else
#define MAX_GPIO_DEV_NUM		1
#endif



static GPIO_REG_T *gpGPIO[] = {
	GPIO0_CTRL_REG,
#if defined(SYSTEM_BUS_DW)		
	GPIO1_CTRL_REG,
#endif	
};
		
static GPIO_CALLBACK gGPIOCallback[MAX_GPIO_DEV_NUM];

void gpio_isr(uint32 num)
{
	uint32 msk = (1 << num);
	uint32 reg;
	int32 i,j;
	uint8 dev = 0;
#if defined(SYSTEM_BUS_DW)
	vuint32 *irq_reg = (vuint32 *)GPIO_IRQ_STATUS_ADDR;	// need not to clear
#endif	
	/* Mask and clear HW interrupt vector */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);


#if defined(SYSTEM_BUS_DW)
	for(j=0;j<MAX_GPIO_DEV_NUM;j++){
		if((*irq_reg>>j)&1){
			dev = j;
#endif
		reg = gpGPIO[dev]->irq_status;
		for(i=0;i<32;i++){
			if((reg>>i)&1){
				if(gGPIOCallback[dev])gGPIOCallback[dev](i);
			}
		}
		gpGPIO[dev]->irq_status = reg;
#if defined(SYSTEM_BUS_DW)
		}
	}	
#endif
	
    __nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);
}


void gpio_init(int32 dev, GPIO_CALLBACK gpio_callback)
{
	// set default irq disable
	gpGPIO[dev]->irq_enable = 0;

	// set default dir in
	gpGPIO[dev]->dir = 0;

	// set default irq mode - dual edge
	gpGPIO[dev]->irq_mode0 = 0x77777777;	// ch0 ~ 7
	gpGPIO[dev]->irq_mode1 = 0x77777777;	// ch8 ~ 15
	gpGPIO[dev]->irq_mode2 = 0x77777777;	// ch16 ~ 23
	gpGPIO[dev]->irq_mode3 = 0x77777777;	// ch24 ~ 31

	gGPIOCallback[dev] = gpio_callback;
}

void gpio_set_dir(int32 dev, int32 pin, int32 dir, int32 out_val)
{
	if(GPIO_DIR_OUT == dir){
		gpGPIO[dev]->dout = utilPutBit(gpGPIO[dev]->dout, pin, out_val);
	}
	gpGPIO[dev]->dir = utilPutBit(gpGPIO[dev]->dir, pin, dir);
}

int32 gpio_get_dir(int32 dev, int32 pin)
{
	return ((gpGPIO[dev]->dir >> pin ) & 1);
}

void gpio_set_irq_en(int32 dev, int32 pin, int32 enable)
{
	if(enable)
		gpGPIO[dev]->irq_enable = utilSetBit(gpGPIO[dev]->irq_enable, pin);
		
	else
		gpGPIO[dev]->irq_enable = utilClearBit(gpGPIO[dev]->irq_enable, pin);
}

int32 gpio_get_irq_en(int32 dev, int32 pin)
{
	return ((gpGPIO[dev]->irq_enable >> pin) & 1);
}

int32 gpio_set_value(int32 dev, int32 pin, int32 value)
{
	if(gpio_get_dir(dev, pin) != GPIO_DIR_OUT)
		return -1;
	
	if(value)
		gpGPIO[dev]->dout = utilSetBit(gpGPIO[dev]->dout, pin);
	else
		gpGPIO[dev]->dout = utilClearBit(gpGPIO[dev]->dout, pin);

	return 1;
		
}

int32 gpio_get_out_value(int32 dev, int32 pin, int32 value)
{
	if(gpio_get_dir(dev, pin) != GPIO_DIR_OUT)
		return -1;

	return ((gpGPIO[dev]->dout >> pin) & 1);

}


int32 gpio_get_value(int32 dev, int32 pin)
{
	if(gpio_get_dir(dev, pin) != GPIO_DIR_IN)
		return -1;

	return ((gpGPIO[dev]->din >> pin) & 1);
}


