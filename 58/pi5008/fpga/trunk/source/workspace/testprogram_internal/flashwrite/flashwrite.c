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
#include "spi.h"
#include "intr.h"
#include "spi_flash.h"
#include "interrupt.h"

#define SPI_FLASH_SIGN	0xABCD1234

typedef struct tagFLASH_HDR_T
{
	unsigned int sign;

	unsigned int ver;
	unsigned int flash_addr;
	unsigned int load_addr;	
	unsigned int size;		
	
	unsigned char rsv[44];

}FLASH_HDR_T;


#define SPI_FLASH_SIZE				( 4*1024*1024 )
#define SPI_FLASH_TEST_BUF			( DRAM_MEM_BASE_ADDR + 0x3000000 )
#define SPI_FLASH_TEST_BUF_SIZE		( 1*1024*1024 )	

#define DRAM_BOOTLOADER_BIN_ADDR	0x21000000
#define DRAM_FIRMWARE_BIN_ADDR		0x21010000

#define FLASH_HDR_SIZE						0x40
#define BOOT_LOADER_FLASH_HEADER_OFFSET		0x0
#define BOOT_LOADER_FLASH_OFFSET			(BOOT_LOADER_FLASH_HEADER_OFFSET + FLASH_HDR_SIZE)
#define BOOT_LOADER_RAM_ADDRESS				0x11000
#define BOOT_LOADER_SIZE					(0x1000 - FLASH_HDR_SIZE)

#define FIRMWARE_FLASH_HEADER_OFFSET		0x4000
#define FIRMWARE_FLASH_OFFSET				(FIRMWARE_FLASH_HEADER_OFFSET + FLASH_HDR_SIZE)
#define FIRMWARE_DRAM_ADDRESS				0x20000000
#define FIRMWARE_SIZE						(0x10000 - FLASH_HDR_SIZE)


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


static void flash_bl_write(void)
{
	FLASH_HDR_T fhdr;

	fhdr.sign = SPI_FLASH_SIGN;
	
	fhdr.ver = 0;
	fhdr.flash_addr = BOOT_LOADER_FLASH_OFFSET;		// boot loader flash offset
	fhdr.load_addr = BOOT_LOADER_RAM_ADDRESS;		// boot loader ram load address
	fhdr.size = BOOT_LOADER_SIZE;					// boot loader size

	flash_init(PCLK/4, 0, 0);

	printf("flash erasing...\n");
	flash_erase(BOOT_LOADER_FLASH_HEADER_OFFSET, BOOT_LOADER_SIZE + FLASH_HDR_SIZE, FLASH_ERASE_SECTOR);

	printf("bootloader writing at 0x%x, size: 0x%x\n", BOOT_LOADER_FLASH_HEADER_OFFSET, fhdr.size + sizeof(fhdr));
	flash_write((uint8 *)&fhdr, BOOT_LOADER_FLASH_HEADER_OFFSET, sizeof(fhdr));
	flash_write((uint8 *)DRAM_BOOTLOADER_BIN_ADDR, fhdr.flash_addr, fhdr.size);

	// check valid
	if(memcmp((uint8 *)&fhdr, (uint8 *)(SPI_MEM_BASE_ADDR + BOOT_LOADER_FLASH_HEADER_OFFSET), sizeof(fhdr)) != 0){
		printf("bootloader header writing fail.\n");
	}else{
		printf("bootloader header writing OK.\n");
	}
	
	if(memcmp((uint8 *)DRAM_BOOTLOADER_BIN_ADDR, (uint8 *)(SPI_MEM_BASE_ADDR+fhdr.flash_addr), fhdr.size) != 0){
		printf("bootloader writing fail\n");
	}else{
		printf("bootloader writing OK\n");
	}


}

static void flash_fw_write(void)
{
	FLASH_HDR_T fhdr;
	
	fhdr.sign = SPI_FLASH_SIGN;
	
	fhdr.ver = 0;
	fhdr.flash_addr = FIRMWARE_FLASH_OFFSET;
	fhdr.load_addr = FIRMWARE_DRAM_ADDRESS;
	fhdr.size = FIRMWARE_SIZE;

	flash_init(PCLK/4, 0, 0);

	printf("flash erasing...\n");
	flash_erase(FIRMWARE_FLASH_HEADER_OFFSET, FIRMWARE_SIZE + FLASH_HDR_SIZE, FLASH_ERASE_SECTOR);

	printf("firmware writing at 0x%x, size: 0x%x\n", FIRMWARE_FLASH_HEADER_OFFSET, fhdr.size + sizeof(fhdr));
	flash_write((uint8 *)&fhdr, FIRMWARE_FLASH_HEADER_OFFSET, sizeof(fhdr));
	flash_write((uint8 *)DRAM_FIRMWARE_BIN_ADDR, fhdr.flash_addr, fhdr.size);

	// check valid
	if(memcmp((uint8 *)&fhdr, (uint8 *)(SPI_MEM_BASE_ADDR + FIRMWARE_FLASH_HEADER_OFFSET), sizeof(fhdr)) != 0){
		printf("firmware header writing fail.\n");
	}else{
		printf("firmware header writing OK.\n");
	}
	
	if(memcmp((uint8 *)DRAM_FIRMWARE_BIN_ADDR, (uint8 *)(SPI_MEM_BASE_ADDR+fhdr.flash_addr), fhdr.size) != 0){
		printf("firmware writing fail\n");
	}else{
		printf("firmware writing OK\n");
	}


}

int main()
{
	FLASH_HDR_T fhdr;

	system_initialize();
	print_init();

	flash_bl_write();
	flash_fw_write();

	printf("Finish.\n");
	
	while(1);

	return 1;
}
