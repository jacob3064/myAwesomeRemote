/* system.h */
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//#define PCLK	74250000
#define PCLK	37125000	

#define UART0_BAUD_RATE	38400

//==================================================
#define SPI_MEM_BASE_ADDR		(0x10000000)
#define DRAM_MEM_BASE_ADDR		(0x20000000)

#define SCU_BASE_ADDR			(0xf0000000)
#define MISC_BASE_ADDR			(0xf0100000)

#define GPIO0_BASE_ADDR			(0xf0200000)
#define GPIO1_BASE_ADDR			(0xf0201000)

#define WDT_BASE_ADDR			(0xf0400000)
#define DDRC_BASE_ADDR			(0xf0500000)
#define ISP_BASE_ADDR			(0xf0600000)
#define ISP_SRAM_BASE_ADDR		(0xf0700000)
#define SVM_BASE_ADDR			(0xf0800000)

#define DU_BASE_ADDR			(0xf0900000)
#define ADC_BASE_ADDR			(0xf0a00000)
#define QSPI_BASE_ADDR			(0xf0b00000)	//qspi memory controller

#define TIMER0_BASE_ADDR		(0xf0c00000)

#define UART0_BASE_ADDR			(0xf0d00000)
#define UART1_BASE_ADDR			(0xf0d01000)

#define MIPI_BASE_ADDR			(0xf0e00000)

#define I2CS_BASE_ADDR			(0xf1000000)	//for picaso
#define I2CM0_BASE_ADDR			(0xf1100000)	//sensor
#define I2CM1_BASE_ADDR			(0xf1101000)

#define SPI0_BASE_ADDR			(0xf1200000)
#define SPI1_BASE_ADDR			(0xf1201000)

#define DMA_BASE_ADDR			(0xf2000000)
#define VPU_BASE_ADDR			(0xf1400000)

//===================================================

/* Low-level port I/O */
#define GetRegValue(reg)		(*((volatile unsigned int *)(reg)))
#define SetRegValue(reg, data)	((*((volatile unsigned int *)(reg))) = (unsigned int)(data))
#define BITCLR(reg, mask)		((*((volatile unsigned int *)(reg))) &= ~(unsigned int)(mask))
#define BITSET(reg, mask)		((*((volatile unsigned int *)(reg))) |= (unsigned int)(mask))


typedef void (*handler)(void);




#endif//__SYSTEM_H__
