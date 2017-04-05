#include "system.h"
#include "i2c.h"


/**********************************
- for picasso, slave function
**********************************/
void i2cs_init(void)
{
	// i2c slave mode set, slave id = 0x34
	SetRegValue(rI2CS_FIFO_FLUSH, I2CS_FIFO_FLUSH);	// fifo flush
	SetRegValue(rI2CS_I2C_MODE, 0x46);				// fifo full intr en, fifo threshold 3, slave
	SetRegValue(rI2CS_SLV_ADDR, (unsigned char)(I2CS_SLAVE_ADDR<<1));
	// i2c slave BUS Master set
	SetRegValue(rI2CS_AHB_MASTER, 0x1);				// rd endian = 00, wr endian = 00, msb first = 0, addr_pos = 00, ahb master enable = 1)
}

/**********************************
- i2cm0/i2cm1, master function
**********************************/
void i2cm_init(unsigned int ch, unsigned char dev_addr)
{
	BITSET(rI2CM_CMD(ch), CMD_RESET);

	// 74.25Mhz/2/2=18.5625Mhz
	// (ex. APB-clk:40MHZ)
	//   standard mode : T_SUDAT=4, T_SP=2, T_HDDAT=6, T_SCLRatio=0, T_SCLHi=182
	//   fast mode : T_SUDAT=0, T_SP=2, T_HDDAT=6, T_SCLRatio=1, T_SCLHi=23
	SetRegValue(rI2CM_SETUP(ch), (0<<24) | (2<<21) | (6<<16) | (1<<13) | (23<<4) | DMA_DISABLE | MASTER_MODE | ADDRESSING_7BIT | I2C_ENABLE);
	SetRegValue(rI2CM_ADDR(ch), dev_addr);
}

void i2cm_write(unsigned int ch, unsigned char addr, unsigned char data)
{
	while(GetRegValue(rI2CM_STATUS(ch) & STAT_BUSY));	// check busy
//	BITSET(rI2CM_CMD(ch), CMD_RESET);					// reset

	BITSET(rI2CM_CTRL(ch), PHASE_START | PHASE_ADDR | PHASE_DATA | PHASE_STOP | PHASE_DIR_MST_TRS | 2);
	BITSET(rI2CM_INTEN(ch), INT_CMPL);	// INT_FIFO_EMPTY(ISR)

	SetRegValue(rI2CM_DATA(ch), addr);
	SetRegValue(rI2CM_DATA(ch), data);
	
	BITSET(rI2CM_CMD(ch), CMD_TRANSACTION);				// issue transaction
	while(GetRegValue(rI2CM_STATUS(ch) & STAT_CMPL));	// check complete
}

void i2cm_read(unsigned int ch, unsigned char addr, unsigned char *data, unsigned char count)
{
	// transmit address
	while(GetRegValue(rI2CM_STATUS(ch) & STAT_BUSY));	// check busy
//	BITSET(rI2CM_CMD(ch), CMD_RESET);					// reset

	BITSET(rI2CM_CTRL(ch), PHASE_START | PHASE_ADDR | PHASE_DATA | PHASE_STOP | PHASE_DIR_MST_TRS | 1);
	BITSET(rI2CM_INTEN(ch), INT_CMPL);	// INT_FIFO_EMPTY(ISR)

	SetRegValue(rI2CM_DATA(ch), addr);

	BITSET(rI2CM_CMD(ch), CMD_TRANSACTION);				// issue transaction
	while(GetRegValue(rI2CM_STATUS(ch) & STAT_CMPL));	// check complete

	// receive data
	BITSET(rI2CM_CTRL(ch), PHASE_START | PHASE_ADDR | PHASE_DATA | PHASE_STOP | PHASE_DIR_MST_RCV | count);
	BITSET(rI2CM_INTEN(ch), INT_CMPL);	// INT_FIFO_FULL(ISR)
	
	BITSET(rI2CM_CMD(ch), CMD_TRANSACTION);				// issue transaction
	do {
		*data++ = GetRegValue(rI2CM_DATA(ch));
	} while(GetRegValue(rI2CM_STATUS(ch) & STAT_CMPL));	// check complete
}

