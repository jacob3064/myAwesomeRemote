#include "gpio.h"
#include "system.h"

void gpio_init(int port)
{

	// set default dir out
	SetRegValue(rGPIO_DIR(port), 0xFFFFFFFF);

	// set default irq disable
	SetRegValue(rGPIO_IRQEN(port), 0x0);

	// set default irq mode - dual edge
	SetRegValue(rGPIO_IRQMODE0(port), 0x77777777);	// 0 ~ 7
	SetRegValue(rGPIO_IRQMODE1(port), 0x77777777);	// 8 ~ 15
	SetRegValue(rGPIO_IRQMODE2(port), 0x77777777);	// 16 ~ 23
	SetRegValue(rGPIO_IRQMODE3(port), 0x77777777);	// 24 ~ 31
}

void gpio_set_dir(int port, int pin, int dir, int out_val)
{
	BITSET(rGPIO_DIR(port), ((dir&1) << pin));
	if(GPIO_DIR_OUT == dir){
		BITSET(rGPIO_DOUT(port), ((out_val&1) << pin));	
	}
}

int gpio_get_dir(int port, int pin)
{
	return ((GetRegValue(rGPIO_DIR(port)) >> pin) & 1);
}

void gpio_set_irq_en(int port, int pin, int enable)
{
	if(enable)
		BITSET(rGPIO_IRQEN(port), (1<<pin));
	else
		BITCLR(rGPIO_IRQEN(port), (1<<pin));
}

int gpio_get_irq_en(int port, int pin)
{
	return ((GetRegValue(rGPIO_IRQEN(port)) >> pin) & 1);
}

int gpio_set_value(int port, int pin, int value)
{
	if(gpio_get_dir(port, pin) != GPIO_DIR_OUT)
		return -1;
	
	if(value)
		BITSET(rGPIO_DOUT(port), (1<<pin));
	else
		BITCLR(rGPIO_DOUT(port), (1<<pin));

	return 1;
		
}

int gpio_get_out_value(int port, int pin, int value)
{
	if(gpio_get_dir(port, pin) != GPIO_DIR_OUT)
		return -1;

	return ((GetRegValue(rGPIO_DOUT(port)) >> pin) & 1);

}


int gpio_get_value(int port, int pin)
{
	if(gpio_get_dir(port, pin) != GPIO_DIR_IN)
		return -1;

	return ((GetRegValue(rGPIO_DIN(port)) >> pin) & 1);
}


