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

//#define SPI_CTRL_FLASH_TEST
#define SPI_CTRL_SDIO_TEST

#define SPI_CTRL_SDIO_CH	1


#define SPI_FLASH_SIZE				( 4*1024*1024 )
#define SPI_FLASH_TEST_BUF			( DRAM_MEM_BASE_ADDR + 0x3000000 )
#define SPI_FLASH_TEST_BUF_SIZE		( 1*1024*1024 )	

#define GET_ARRAY_CNT(array)	(sizeof(array)/sizeof(array[0]))

typedef struct tagSPI_TEST_T{
	uint32 addr;
	int32 size;

}SPI_TEST_T;

typedef struct tagSPI_MEM_READ_MODE_T{
	uint32 read_mode;
	uint32 read_cmd;
	uint32 quad_mode;
	uint32 dummy_size;
	uint32 mode_size;
	uint32 mode_out_state;
}SPI_MEM_READ_MODE;

static uint32 gSPI_CTRL_FREQ;
static uint32 gSPI_CTRL_MODE;

static SPI_MEM_READ_MODE gSPI_MEM_READ_TABLE[] = {
		{SPI_MEMCTRL_READ_SINGLE, 0x03, 0, SPI_MEMCTRL_DUMMY_SIZE_0, SPI_MEMCTRL_MODE_SIZE_0, SPI_MEMCTRL_MODE_STATE_NONE},
		{SPI_MEMCTRL_READ_FAST, 0x0B, 0, SPI_MEMCTRL_DUMMY_SIZE_8, SPI_MEMCTRL_MODE_SIZE_0, SPI_MEMCTRL_MODE_STATE_NONE},
		{SPI_MEMCTRL_READ_DUAL_IO, 0xBB, 0, SPI_MEMCTRL_DUMMY_SIZE_8, SPI_MEMCTRL_MODE_SIZE_0, SPI_MEMCTRL_MODE_STATE_NONE},
		{SPI_MEMCTRL_READ_QUAD_IO, 0xEB, 1, SPI_MEMCTRL_DUMMY_SIZE_16, SPI_MEMCTRL_MODE_SIZE_8, SPI_MEMCTRL_MODE_STATE_VAL},
};

static void __delay ( uint32 count )
{
	while(count-- > 0){
		asm volatile ("nop");
	}

}

/***************************************************************************************************************************************************************
 * system initialize
***************************************************************************************************************************************************************/
static void system_initialize(void)
{
	SYSAPI_initialize();
	SYSAPI_UART_initialize(0, eBAUDRATE_115200, eDATA_BIT_8, eSTOP_BIT_1, ePARITY_NONE);
}

/***************************************************************************************************************************************************************
 * main
***************************************************************************************************************************************************************/
static void make_test_pattern(uint8 *buf, int32 size, uint32 init_val)
{
	int32 i;
	uint32 *ptr = (uint32 *)buf;
	
	for(i=0;i<(size>>2);i++){
		*(ptr+i) = (init_val + (i<<2));
	}
}

static int32 check_test_pattern(uint8 *buf, int32 size, uint32 init_val)
{
	int32 ret = 1;
	int32 i;
	uint32 *ptr = (uint32 *)buf;
	
	for(i=0;i<(size>>2);i++){
		if(*(ptr+i) != (init_val + (i<<2))){
			printf("check fail at 0x%x\n", (i<<2));
			ret = -1;
			break;
		}
	}

	return ret;
	
}

static void sequential_rw_test(void)
{
	int32 i,j;
	uint8 *buf = (uint8 *)SPI_FLASH_TEST_BUF;
	uint32 *ptr;
	SPI_MEM_FR_INFO fr_info;
	
	printf("==================================================\n");
	printf("[%s]\n",__FUNCTION__);
	printf("==================================================\n");
	
	flash_init(gSPI_CTRL_FREQ, gSPI_CTRL_MODE, 0);

	printf("[%s]flash erasing all...\n",__FUNCTION__);
	flash_erase(0, SPI_FLASH_SIZE, FLASH_ERASE_BLOCK_32K);

	for(i=0;i<SPI_FLASH_SIZE;i+=SPI_FLASH_TEST_BUF_SIZE){
		printf("[%s]writing flash at 0x%08x, 0x%08x bytes\n", __FUNCTION__, i, SPI_FLASH_TEST_BUF_SIZE);
		make_test_pattern(buf, SPI_FLASH_TEST_BUF_SIZE, i);
		flash_write(buf, i, SPI_FLASH_TEST_BUF_SIZE);
		
	}
	for(i=0;i<SPI_FLASH_SIZE;i+=SPI_FLASH_TEST_BUF_SIZE){
		printf("[%s]reading flash at 0x%08x, 0x%08x bytes...", __FUNCTION__, i, SPI_FLASH_TEST_BUF_SIZE);
		memset(buf, 0, SPI_FLASH_TEST_BUF_SIZE);
		flash_read(buf, i, SPI_FLASH_TEST_BUF_SIZE);

		if(check_test_pattern(buf, SPI_FLASH_TEST_BUF_SIZE, i) > 0){
			printf("Success\n");
		}else{
			printf("Fail\n");
		}
	}


	for(i=0;i<GET_ARRAY_CNT(gSPI_MEM_READ_TABLE);i++){
		// check spi mem controller
		printf("SPI MEM Controller Reading. (Read mode: 0x%x, Read cmd: 0x%x)...\n", gSPI_MEM_READ_TABLE[i].read_mode, gSPI_MEM_READ_TABLE[i].read_cmd);
		flash_init(gSPI_CTRL_FREQ, gSPI_CTRL_MODE, (uint8)gSPI_MEM_READ_TABLE[i].quad_mode);
		
		fr_info.dummy_size = gSPI_MEM_READ_TABLE[i].dummy_size;
		fr_info.mode_size = gSPI_MEM_READ_TABLE[i].mode_size;
		fr_info.mode_out_state = gSPI_MEM_READ_TABLE[i].mode_out_state;

		fr_info.mode_value = 0;
		fr_info.read_type = gSPI_MEM_READ_TABLE[i].read_mode;

		spi_memctrl_set(4, gSPI_MEM_READ_TABLE[i].read_cmd, fr_info);
		ptr = (uint32 *)SPI_MEM_BASE_ADDR;
		for(j=0;j<(SPI_FLASH_SIZE>>2);j++){
			if(*(ptr+j) != (j<<2)){
				printf("\nSPI Memctrl read fail(0x%x/0x%x)\n", *(ptr+j), (j<<2));
				while(1);
				break;
			}
		}
		if(j == (SPI_FLASH_SIZE>>2))
			printf("Success\n");
	}

}


static void random_rw_test(void)
{
	int32 i;
	uint8 *buf = (uint8 *)SPI_FLASH_TEST_BUF;
	SPI_TEST_T flash[4] = {
			{0x0, 0x80000},
			{0x100000, 0x40000},
			{0x2a0000, 0x10000},
			{0x310000, 0x20000}
	};
	SPI_MEM_FR_INFO fr_info;
	
	printf("==================================================\n");
	printf("[%s]\n",__FUNCTION__);
	printf("==================================================\n");

	flash_init(gSPI_CTRL_FREQ, gSPI_CTRL_MODE, 0);

	fr_info.dummy_size = 0;
	fr_info.mode_size = 0;
	fr_info.mode_out_state = 0;
	
	fr_info.mode_value = 0;
	fr_info.read_type = 0;
	
	spi_memctrl_set(4, 0x03, fr_info);


	for(i=0;i<4;i++){
		make_test_pattern(buf, flash[i].size, flash[i].addr); 	
		printf("[%s]flash erasing at 0x%08x, 0x%08x bytes\n",__FUNCTION__, flash[i].addr, flash[i].size );
		flash_erase(flash[i].addr, flash[i].size, FLASH_ERASE_BLOCK_32K);
		printf("[%s]flash writing at 0x%08x, 0x%08x bytes\n",__FUNCTION__, flash[i].addr, flash[i].size );
		flash_write(buf, flash[i].addr, flash[i].size);
		
	}

	for(i=0;i<4;i++){
		printf("[%s]reading flash at 0x%08x, 0x%08x bytes...", __FUNCTION__, flash[i].addr, flash[i].size);
		memset(buf, 0, flash[i].size);
		flash_read(buf, flash[i].addr, flash[i].size);
		if(check_test_pattern(buf, flash[i].size, flash[i].addr) > 0){
			printf("Success\n");
		}else{
			printf("Fail\n");
		}
	}
	
}

static void spi_sdio_test(void)
{
	uint8 data[4];
	uint32 i;

	//for(i=0;i<4;i++)data[i] = i;
	data[0] = 0xef;
	data[1] = 0xfe;
	data[2] = 0x77;
	data[3] = 0xee;
	
	spi_initialize(SPI_CTRL_SDIO_CH, 0, PCLK/32, 0, 8, NULL);
	spi_direct_cs_enable(1, 1);
	spi_cs_deactivate(1);
	while(1){
		spi_tx(SPI_CTRL_SDIO_CH, data, 4);
		__delay(0x1000000);
	}
	
}
int main()
{
	system_initialize();
	print_init();
	MAIN_PRINT("main begin\n");
	
	gSPI_CTRL_FREQ = PCLK/8;
	gSPI_CTRL_MODE = 0;

#if defined(SPI_CTRL_FLASH_TEST)
	sequential_rw_test();
	random_rw_test();
#elif defined(SPI_CTRL_SDIO_TEST)
	spi_sdio_test();
#endif

	printf("Finish.\n");
	while(1);

	return 1;
}
