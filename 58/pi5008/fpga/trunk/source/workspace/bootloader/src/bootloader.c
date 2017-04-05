#include <stdio.h>
#include <string.h>
#include "spi.h"
#include "spi_flash.h"

#define SPI_FLASH_ADDR	0x10000000
#define SPI_FLASH_SIGN	0xABCD1234

#define SPI_FLASH_FW_OFFSET		0x4000

typedef struct tagFLASH_HDR_T
{
	unsigned int sign;
	
	unsigned int ver;
	unsigned int flash_addr;	// flash address
	unsigned int load_addr;		// load address 
	unsigned int size;			// size
	
	unsigned char rsv[44];

}FLASH_HDR_T;

unsigned int bootloader_main(void)
{
	SPI_MEM_FR_INFO fr_info;
	FLASH_HDR_T fhdr;
	
	//====================================
	// set pll
	//====================================


	//====================================
	// set ddr controller
	//====================================



	//====================================
	// set spi flash quad
	//====================================
/*
	flash_init(9281250, 0, 1);

	// QUAD flash mode - MXIC
	fr_info.dummy_size = 16;
	fr_info.mode_size = 8;
	fr_info.mode_out_state = 2;
	fr_info.mode_value = 0;
	fr_info.read_type = 0xC;
	spi_memctrl_set(4, 0xEB, fr_info);
*/


	// copy firmware to dram
	memcpy(&fhdr, (unsigned int *)(SPI_FLASH_ADDR + SPI_FLASH_FW_OFFSET), sizeof(FLASH_HDR_T));
	while(fhdr.sign != SPI_FLASH_SIGN);
	memcpy((unsigned int *)fhdr.load_addr, (unsigned int *)(SPI_FLASH_ADDR + fhdr.flash_addr), fhdr.size);


	// set spi controller & mem controller to single mode
/*
	flash_init(9281250, 0, 0);
	fr_info.dummy_size = 0;
	fr_info.mode_size = 0;
	fr_info.mode_out_state = 0;
	fr_info.mode_value = 0;
	fr_info.read_type = 0;
	spi_memctrl_set(4, 0x3, fr_info);
*/
	
	// jump
	return fhdr.load_addr;
	
	
}
