#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "spi_flash.h"



#define SPI_FLASH_MANF_MXIC		1


#define CMD_READ_ID					0x9f

#define CMD_WRITE_STATUS			0x01
#define CMD_WRITE_DISABLE			0x04
#define CMD_READ_STATUS				0x05
#define CMD_WRITE_ENABLE			0x06


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
	
	spi_initialize(0, freq, clk_mode, 8);
	//flash_read_id(id);
	//printf("flash id: 0x%x, 0x%x, 0x%x, 0x%x\n", id[0], id[1], id[2], id[3]);


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

	//flash_read_id(id);
	//printf("flash id: 0x%x, 0x%x, 0x%x, 0x%x\n", id[0], id[1], id[2], id[3]);
	
//	status = flash_read_status();
//	printf("status: 0x%x\n", status);
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
	

	spi_xfer(cmd, buf, size);

	memcpy(id, buf, 4);

	return 0;	
}

int32 flash_read_status(void)
{
	uint8 cmd[2];
	uint8 status[2];
	
	cmd[0] = CMD_READ_STATUS;
	
	spi_xfer(cmd, status, 2);

	return (uint32)status[1];

}

int32 flash_write_status(uint8 status)
{
	uint8 cmd[2];
	
	cmd[0] = CMD_WRITE_STATUS;
	cmd[1] = status;
	
	//spi_xfer(cmd, NULL, 2);
	spi_tx(cmd, 2);

	return 0;

}


int32 flash_write_enable(uint8 enable)
{
	uint8 cmd;

	if(enable)cmd = CMD_WRITE_ENABLE;
	else cmd = CMD_WRITE_DISABLE;
	
	//spi_xfer(&cmd, NULL, 1);
	spi_tx(&cmd, 1);

	return 0;

}

