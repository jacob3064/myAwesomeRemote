#ifndef _PI5008_SPI_FLASH_H
#define _PI5008_SPI_FLASH_H

#include "type.h"


#define FLASH_PAGE_SIZE				(256)
#define FLASH_ERASE_SECTOR			(4*1024)
#define FLASH_ERASE_BLOCK_32K		(32*1024)
#define FLASH_ERASE_BLOCK_64K		(64*1024)


int32 flash_init(uint32 freq, uint32 clk_mode, uint8 quad_io);
int32 flash_read_id(uint8 *id);
int32 flash_read_status(void);
int32 flash_write_status(uint8 status);
int32 flash_write_enable(uint8 enable);
int32 flash_read(uint8 *buf, uint32 addr, int32 size);
int32 flash_write(uint8 *buf, uint32 addr, int32 size);
int32 flash_erase(uint32 addr, int32 len, int32 erase_size);


#endif

