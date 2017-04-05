#include <stdio.h>
#include <string.h>

#define SPI_FLASH_ADDR	0x10000000
#define SPI_FLASH_SIGN	0xABCD1234

// temporal flash header define

typedef struct tagFLASH_HDR_T
{
	unsigned int sign;
	
	unsigned int bl_ver;
	unsigned int bl_flash_addr;		// boot loader flash address
	unsigned int bl_load_addr;		// boot loader load address in ram
	unsigned int bl_size;			// boot loader size

	unsigned int fw_ver;
	unsigned int fw_flash_addr;		// firmware flash address
	unsigned int fw_load_addr;		// firmware load address in ram
	unsigned int fw_size;			// firmware size
	
	unsigned char rsv[28];

}FLASH_HDR_T;



static unsigned int spi_flash_boot(void)
{
	FLASH_HDR_T fhdr;

	memcpy(&fhdr, (unsigned int *)SPI_FLASH_ADDR, sizeof(FLASH_HDR_T));
	while(fhdr.sign != SPI_FLASH_SIGN);
	
	memcpy((unsigned int *)fhdr.bl_load_addr, (unsigned int *)(SPI_FLASH_ADDR + fhdr.bl_flash_addr), fhdr.bl_size);
	
	return fhdr.bl_load_addr;
	
	
}
unsigned int bootrom_main(void)
{
	unsigned int ret;
	
	ret = spi_flash_boot();
	return ret;
}
