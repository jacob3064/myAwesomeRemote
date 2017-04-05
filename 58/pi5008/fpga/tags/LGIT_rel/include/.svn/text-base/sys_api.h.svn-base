#ifndef __SYS_API_H__
#define __SYS_API_H__

#include "type.h"
#include "error.h"

#define SYS_DMA_MAX_CHANNEL		(8)

#define ADDR_CACHEABLE(va) 		( va  & (~(0x3<<30)) )
#define ADDR_NON_CACHEABLE(va) 	( va  | (0x1<<30) )

typedef void (*SYSDMA_CALLBACK) (uint8 ch, uint32 event);

enum tagUART_BAUDRATE_E
{
	eBAUDRATE_2400 = 2400,
	eBAUDRATE_4800 = 4800,
	eBAUDRATE_9600 = 9600,
	eBAUDRATE_19200 = 19200,
	eBAUDRATE_38400 = 38400,
	eBAUDRATE_57600 = 57600,
	eBAUDRATE_115200 = 115200,
	eBAUDRATE_230400 = 230400,
	eBAUDRATE_460800 = 460800,
	eBAUDRATE_921600 = 921600,
};

enum tagUART_DATABIT_E
{
	eDATA_BIT_5 = 0,
	eDATA_BIT_6 = 1,
	eDATA_BIT_7 = 2,
	eDATA_BIT_8 = 3,
};

enum tagUART_PARITY_E
{
	ePARITY_NONE = 0x0,
	ePARITY_ODD = 0x8,
	ePARITY_EVEN = 0x18,
};

enum tagUART_STOP_BIT_E
{
	eSTOP_BIT_1 = 0x0,
	eSTOP_BIT_2 = 0x4,
};


enum tagDMA_EVENT_RESULT_E
{
	eDMA_EVENT_DONE = 0, 
	eDMA_EVENT_ABORT,
	eDMA_EVENT_ERROR,
};


int32 APIENTRY SYSAPI_initialize(void);
int32 APIENTRY SYSAPI_UART_initialize(uint8 IN ch, int32 IN baudRate, int32 IN dataBit, int32 IN stopBit, int32 IN parity);
void APIENTRY SYSAPI_DMA_initialize(SYSDMA_CALLBACK IN callback);
int32 APIENTRY SYSAPI_DMA_m2m_word(uint8 IN ch, uint32 IN *pPhySrc, uint32 IN *pPhyDst, int32 IN size, bool isBlock);
int32 APIENTRY SYSAPI_DMA_check_result(uint8 IN ch);
int32 APIENTRY SYSAPI_DMA_abort(uint8 IN ch);
void APIENTRY SYSAPI_CACHE_writeback(uint32 IN *pAddr, int32 IN size);
void APIENTRY SYSAPI_CACHE_invalidate(uint32 IN *pAddr, int32 IN size);
void APIENTRY SYSAPI_DU_initialize (void);

#endif

