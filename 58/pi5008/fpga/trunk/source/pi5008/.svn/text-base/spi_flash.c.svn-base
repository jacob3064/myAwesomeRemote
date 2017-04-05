#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "spi_flash.h"


#define SPI_CTRL_FLASH_CH		0


#define SPI_FLASH_MANF_MXIC		1


#define CMD_READ_ID					0x9f

#define CMD_ERASE_4K				0x20
#define CMD_ERASE_32K				0x52
#define CMD_ERASE_64K				0xd8

#define CMD_WRITE_STATUS			0x01
#define CMD_PAGE_PROGRAM			0x02
#define CMD_WRITE_DISABLE			0x04
#define CMD_READ_STATUS				0x05
#define CMD_WRITE_ENABLE			0x06
#define CMD_PAGE_PROGRAM_QUAD_IO	0x38

#define CMD_READ_SLOW				0x03
#define CMD_READ_FAST				0x0b
#define CMD_READ_DUAL_IO			0xbb
#define CMD_READ_QUAD_IO			0xeb


//static uint8 *gTxBuf = (uint8 *)0x21000000;
//static uint8 *gRxBuf = (uint8 *)0x22000000;

static uint8 gTxBuf[FLASH_PAGE_SIZE + 8];
static uint8 gRxBuf[FLASH_PAGE_SIZE + 8];
static int32 flash_cmd_wait_ready(uint32 timeout)
{
	int32 status;
	
	do{
		status = flash_read_status();
		//printf("status: 0x%x\n", status);
	}while(status & 1);

	return 1;

}



int32 flash_init(uint32 freq, uint32 clk_mode, uint8 quad_io)
{
	uint8 id[8];
	int32 status;
	
	spi_initialize(SPI_CTRL_FLASH_CH, 0, freq, clk_mode, 8, NULL);
	flash_read_id(id);
	printf("flash id: 0x%x, 0x%x, 0x%x, 0x%x\n", id[0], id[1], id[2], id[3]);


#if defined(SPI_FLASH_MANF_MXIC)
	flash_write_enable(1);
	do{
		status = flash_read_status();
	}while(!(status & 2));

	if(quad_io)
		status |= (1<<6);
	else
		status &= (~(1<<6));
	
	flash_write_status(status);
	flash_cmd_wait_ready(1);
	
	flash_write_enable(0);
#endif	

	return 0;
}

int32 flash_read_id(uint8 *id)
{
	uint8 cmd[4];
	uint8 buf[4];
	int32 size;

	size = 4;
	memset(cmd, 0, sizeof(cmd));
	cmd[0] = CMD_READ_ID;
	

	spi_xfer(SPI_CTRL_FLASH_CH, cmd, buf, size);

	memcpy(id, buf, 4);

	return 0;	
}

int32 flash_read_status(void)
{
	uint8 cmd[2];
	uint8 status[2];
	
	cmd[0] = CMD_READ_STATUS;
	
	spi_xfer(SPI_CTRL_FLASH_CH, cmd, status, 2);

	return (uint32)status[1];

}

int32 flash_write_status(uint8 status)
{
	uint8 cmd[2];
	
	cmd[0] = CMD_WRITE_STATUS;
	cmd[1] = status;
	
	//spi_xfer(SPI_CTRL_FLASH_CH, cmd, NULL, 2);
	spi_tx(SPI_CTRL_FLASH_CH, cmd, 2);

	return 0;

}


int32 flash_page_read(uint8 *buf, uint32 addr, int32 size)
{
	uint8 cmd[5];

	cmd[0] = CMD_READ_FAST;
	cmd[1] = ((addr>>16)&0xff);
	cmd[2] = ((addr>>8)&0xff);
	cmd[3] = ((addr>>0)&0xff);	
	cmd[4] = 0; // dummy

	memcpy(gTxBuf, cmd, 5);
	spi_xfer(SPI_CTRL_FLASH_CH, gTxBuf, gRxBuf, size + 5);

	memcpy(buf, &gRxBuf[5], size);

	return 0;

}

int32 flash_read(uint8 *buf, uint32 addr, int32 size)
{
	uint32 read_unit;
	uint32 read_size;

	read_size = 0;
	read_unit = ((size > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : size);


	while(read_size < size){

		//printf("[%s]write size: %d, written: %d, total size: %d\n",__FUNCTION__, write_unit, written_size, size);	

		read_unit = (((size - read_size) > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : (size - read_size));
		if(read_unit > (FLASH_PAGE_SIZE - (addr % FLASH_PAGE_SIZE)))read_unit = (FLASH_PAGE_SIZE - (addr % FLASH_PAGE_SIZE));
		flash_page_read(buf, addr, read_unit);
		buf += read_unit;
		addr += read_unit;
		read_size += read_unit;
		
	}

	return 0;	
}

int32 flash_write_enable(uint8 enable)
{
	uint8 cmd;

	if(enable)cmd = CMD_WRITE_ENABLE;
	else cmd = CMD_WRITE_DISABLE;
	
	//spi_xfer(SPI_CTRL_FLASH_CH, &cmd, NULL, 1);
	spi_tx(SPI_CTRL_FLASH_CH, &cmd, 1);

	return 0;

}
int32 flash_page_program(uint8 *buf, uint32 addr, int32 size)
{
	uint8 cmd[4];

	cmd[0] = CMD_PAGE_PROGRAM;
	cmd[1] = ((addr>>16)&0xff);
	cmd[2] = ((addr>>8)&0xff);
	cmd[3] = ((addr>>0)&0xff);	

	memcpy(gTxBuf, cmd, 4);
	memcpy(&gTxBuf[4], buf, size);
	size += 4;

	flash_write_enable(1);
	
	//spi_xfer(SPI_CTRL_FLASH_CH, gTxBuf, NULL, size);
	spi_tx(SPI_CTRL_FLASH_CH, gTxBuf, size);
	
	flash_cmd_wait_ready(1);
	
	flash_write_enable(0);

	return 0;

}

int32 flash_write(uint8 *buf, uint32 addr, int32 size)
{
	uint32 write_unit;
	uint32 written_size;

	written_size = 0;
	write_unit = ((size > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : size);


	while(written_size < size){

		//printf("[%s]write size: %d, written: %d, total size: %d\n",__FUNCTION__, write_unit, written_size, size);	

		write_unit = (((size - written_size) > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : (size - written_size));
		if(write_unit > (FLASH_PAGE_SIZE - (addr % FLASH_PAGE_SIZE)))write_unit = (FLASH_PAGE_SIZE - (addr % FLASH_PAGE_SIZE));
		flash_page_program(buf, addr, write_unit);
		buf += write_unit;
		addr += write_unit;
		written_size += write_unit;
		
	}

	return 0;
}

int32 flash_erase(uint32 addr, int32 len, int32 erase_size)
{
	uint8 cmd[4];

	if(erase_size == FLASH_ERASE_SECTOR)
		cmd[0] = CMD_ERASE_4K;
	else if(erase_size == FLASH_ERASE_BLOCK_32K)
		cmd[0] = CMD_ERASE_32K;
	else if(erase_size == FLASH_ERASE_BLOCK_64K)
		cmd[0] = CMD_ERASE_64K;
	else
		return -1;

	if(addr % erase_size || len % erase_size){
		printf("Flash Error!!! Erase address/length not multiple of erase size\n");
		return -1;
	}
	

	while(len > 0){
		//printf("[%s]addr: 0x%x, erase size: %d\n",__FUNCTION__, addr, erase_size);
		cmd[1] = ((addr>>16)&0xff);
		cmd[2] = ((addr>>8)&0xff);
		cmd[3] = ((addr>>0)&0xff);	

		flash_write_enable(1);
		
		//spi_xfer(SPI_CTRL_FLASH_CH, cmd, NULL, 4);
		spi_tx(SPI_CTRL_FLASH_CH, cmd, 4);
		
		flash_cmd_wait_ready(1);

		flash_write_enable(0);

		len -= erase_size;
		addr += erase_size;
	}
	return 0;	
}

