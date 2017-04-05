/* i2c.h */
#ifndef __I2C_H__
#define __I2C_H__


/*-----------------------------------------------
	I2CS Register 
-----------------------------------------------*/
#define rI2CS_I2C_MODE			(I2CS_BASE_ADDR + 0x40)
#define rI2CS_SLV_ADDR			(I2CS_BASE_ADDR + 0x44)
#define rI2CS_FIFO_FLUSH		(I2CS_BASE_ADDR + 0x48)
#define rI2CS_INT_CLR			(I2CS_BASE_ADDR + 0x4c)
#define rI2CS_AHB_MASTER		(I2CS_BASE_ADDR + 0x50)
#define rI2CS_AHB_RDATA			(I2CS_BASE_ADDR + 0x54)
#define rI2CS_AHB_SLAVE_EN		(I2CS_BASE_ADDR + 0x58)
#define rI2CS_SLV_WDATA			(I2CS_BASE_ADDR + 0x60)
#define rI2CS_SLV_RDATA			(I2CS_BASE_ADDR + 0x60)

#define rI2CS_DBG_STATE			(I2CS_BASE_ADDR + 0x80)
#define rI2CS_DBG_ADDR			(I2CS_BASE_ADDR + 0x84)
#define rI2CS_DBG_RDATA			(I2CS_BASE_ADDR + 0x88)
#define rI2CS_DBG_WDATA			(I2CS_BASE_ADDR + 0x8c)

//-------------------------------------------------------------
#define I2CS_SLAVE_ADDR		(0x1A)
#define I2CS_FIFO_FLUSH		(0x1)


/*-----------------------------------------------
	I2CM Register 
-----------------------------------------------*/
#define rI2CM_START(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x00)
#define rI2CM_STOP(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x04)
#define rI2CM_WDATA(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x08)
#define rI2CM_CYCLE(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x0c)
#define rI2CM_SR(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x10)
#define rI2CM_RDATA(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x14)

#define rI2CM_INTEN(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x14)
#define rI2CM_STATUS(ch)	(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x18)
#define rI2CM_ADDR(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x1C)
#define rI2CM_DATA(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x20)
#define rI2CM_CTRL(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x24)
#define rI2CM_CMD(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x28)
#define rI2CM_SETUP(ch)		(I2CM0_BASE_ADDR + ((unsigned int)(ch)*0x00001000U) + 0x2C)

// Interrupt Enable Register(RW)
#define INT_CMPL			((unsigned int)1<<9)
#define INT_BYTE_RECV		((unsigned int)1<<8)
#define INT_BYTE_TRANS		((unsigned int)1<<7)
#define INT_START			((unsigned int)1<<6)
#define INT_STOP			((unsigned int)1<<5)
#define INT_ARB_LOSE		((unsigned int)1<<4)
#define INT_ADDR_HIT		((unsigned int)1<<3)
#define INT_FIFO_HALF		((unsigned int)1<<2)
#define INT_FIFO_FULL		((unsigned int)1<<1)
#define INT_FIFO_EMPTY		((unsigned int)1<<0)

// Status Register //
#define STAT_LINE_SDA		((unsigned int)1<<14)
#define STAT_LINE_SCL		((unsigned int)1<<13)
#define STAT_GEN_CALL		((unsigned int)1<<12)
#define STAT_BUSY			((unsigned int)1<<11)
#define STAT_ACK			((unsigned int)1<<10)
#define STAT_CMPL			((unsigned int)1<<9)
#define STAT_BYTE_RCV		((unsigned int)1<<8)
#define STAT_BYTE_TRANS		((unsigned int)1<<7)
#define STAT_START			((unsigned int)1<<6)
#define STAT_STOP			((unsigned int)1<<5)
#define STAT_ARB_LOSE		((unsigned int)1<<4)
#define STAT_ADDR_HIT		((unsigned int)1<<3)
#define STAT_FIFO_HALF		((unsigned int)1<<2)
#define STAT_FIFO_FULL		((unsigned int)1<<1)
#define STAT_FIFO_EMPTY		((unsigned int)1<<0)

// Control Register //
#define PHASE_START			((unsigned int)1<<12)
#define PHASE_ADDR			((unsigned int)1<<11)
#define PHASE_DATA			((unsigned int)1<<10)
#define PHASE_STOP			((unsigned int)1<<9)
#define PHASE_DIR_MST_TRS	((unsigned int)0<<8)
#define PHASE_DIR_MST_RCV	((unsigned int)1<<8)
#define PHASE_DIR_SLV_TRS	((unsigned int)1<<8)
#define PHASE_DIR_SLV_RCV	((unsigned int)0<<8)

// Command Register //
#define CMD_NO_ACTION		((unsigned int)0x0)
#define CMD_TRANSACTION		((unsigned int)0x1)
#define CMD_RSP_ACK			((unsigned int)0x2)
#define CMD_RSP_NACK		((unsigned int)0x3)
#define CMD_CLEAR			((unsigned int)0x4)
#define CMD_RESET			((unsigned int)0x5)

// Setup Register //
#define DMA_DISABLE			((unsigned int)0<<3)
#define DMA_ENABLE			((unsigned int)1<<3)

#define SLAVER_MODE			((unsigned int)0<<2)
#define MASTER_MODE			((unsigned int)1<<2)

#define ADDRESSING_7BIT		((unsigned int)0<<1)
#define ADDRESSING_10BIT	((unsigned int)1<<1)

#define I2C_DISABLE			((unsigned int)0<<0)
#define I2C_ENABLE			((unsigned int)1<<0)

//-------------------------------------------------------------


extern void i2cs_init(void);
extern void i2cm_init(unsigned int ch, unsigned char dev_addr);
extern void i2cm_write(unsigned int ch, unsigned char addr, unsigned char data);
extern void i2cm_read(unsigned int ch, unsigned char addr, unsigned char *data, unsigned char count);


#endif // __I2C_H__
