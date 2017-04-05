#ifndef _PI5008_SPI_H
#define _PI5008_SPI_H

#include "system.h"
#include "type.h"


//#define	SPI_CPHA	0x01			/* clock phase */
//#define	SPI_CPOL	0x02			/* clock polarity */

#define SPI_MEMCTRL_READ_SINGLE		0x0
#define SPI_MEMCTRL_READ_FAST		0x1
#define SPI_MEMCTRL_READ_DUAL		0x2
#define SPI_MEMCTRL_READ_DUAL_IO	0xA
#define SPI_MEMCTRL_READ_QUAD		0x4
#define SPI_MEMCTRL_READ_QUAD_IO	0xc

#define SPI_MEMCTRL_MODE_STATE_NONE	0x0
#define SPI_MEMCTRL_MODE_STATE_HIZ	0x1
#define SPI_MEMCTRL_MODE_STATE_VAL	0x2

#define SPI_MEMCTRL_MODE_SIZE_0		0x0
#define SPI_MEMCTRL_MODE_SIZE_4		0x4
#define SPI_MEMCTRL_MODE_SIZE_8		0x8

#define SPI_MEMCTRL_DUMMY_SIZE_0	0x0
#define SPI_MEMCTRL_DUMMY_SIZE_2	0x2
#define SPI_MEMCTRL_DUMMY_SIZE_4	0x4
#define SPI_MEMCTRL_DUMMY_SIZE_8	0x8
#define SPI_MEMCTRL_DUMMY_SIZE_16	0x10



typedef struct tagSPI_REG_T
{
	vuint32 id;								/* 0x000 		- ID and revision register */
	vuint32 rsv0[3];						/* 0x004 ~ 0x00c	- reserved */
	vuint32 trans_fmt;						/* 0x010 		- Transfer format register */
	vuint32 direct_io;						/* 0x014 		- Direct IO control register */
	vuint32 rsv1[2];						/* 0x018 ~ 0x01c	- reserved */
	vuint32 trans_ctrl;						/* 0x020 		- Transfer control register */
	vuint32 cmd;							/* 0x024 		- Command register */
	vuint32 addr;							/* 0x028 		- Address register */
	vuint32 data;							/* 0x02c 		- Data register */
	vuint32 ctrl;							/* 0x030 		- Control register */
	vuint32 status;							/* 0x034 		- Status register */
	vuint32 irq_enable;						/* 0x038 		- Interrupt enable register */
	vuint32 irq_status;						/* 0x03c 		- Interrupt status register */
	vuint32 timing;							/* 0x040 		- Interface timing register */
	vuint32 rsv2[3];						/* 0x044 ~ 0x04c	- reserved */
	vuint32 mem_ctrl;						/* 0x050 		- Memory access control register */
	vuint32 rsv3[3];						/* 0x054 ~ 0x05c	- reserved */
	vuint32 slave_status;					/* 0x060 		- Slave status register */
	vuint32 slave_data_cnt;					/* 0x064 		- Slave data count register */
	vuint32 rsv4[5];						/* 0x068 ~ 0x078	- reserved */
	vuint32 config;							/* 0x07c 		- Configuration register */
}SPI_REG_T;


typedef struct tagSPI_MEM_REG_T
{
	vuint32 buffer[4];						/* 0x000~0x00c 	- Buffer */
	vuint32 ctrl;							/* 0x010		 	- control register */
	vuint32 divider;						/* 0x014		 	- divider register */
	vuint32 rx_delay;						/* 0x018		 	- delay rx clock register */
	vuint32 fr_cmd;							/* 0x01c		 	- Read command register */
	vuint32 fr_info;						/* 0x020		 	- Info register */

}SPI_MEM_REG_T;


typedef struct tagSPI_MEM_FR_INFO_T
{
	uint32 dummy_size		: 7;
	uint32 rsv1				: 1;
	uint32 mode_size		: 4;
	uint32 mode_out_state	: 4;
	uint32 mode_value		: 8;
	uint32 read_type		: 4;
	uint32 rsv2				: 4;
}SPI_MEM_FR_INFO;

#define SPI0_CTRL_REG			((SPI_REG_T*)			(SPI0_BASE_ADDR))
#if defined(SYSTEM_BUS_DW)
#define SPI1_CTRL_REG			((SPI_REG_T*)			(SPI1_BASE_ADDR))
#define SPI2_CTRL_REG			((SPI_REG_T*)			(SPI2_BASE_ADDR))
#define SPI3_CTRL_REG			((SPI_REG_T*)			(SPI3_BASE_ADDR))
#define SPI4_CTRL_REG			((SPI_REG_T*)			(SPI4_BASE_ADDR))
#endif

#define SPI_MEM_CTRL_REG		((SPI_MEM_REG_T*)		(QSPI_BASE_ADDR))

int32 spi_initialize(uint32 slave, uint32 freq, uint32 clk_mode, uint32 wordlen);
int32 spi_tx(const uint8 *dout, uint32 size);
int32 spi_xfer(const uint8 *dout, uint8 *din, uint32 size);

void spi_memctrl_set(uint32 div, uint8 read_cmd, SPI_MEM_FR_INFO fr_info);



#endif
