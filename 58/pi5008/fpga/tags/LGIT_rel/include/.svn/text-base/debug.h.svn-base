#ifndef _PI5008_DEBUG_H
#define _PI5008_DEBUG_H
#include <stdio.h>
#include "type.h"

//#define DU_DEBUG
//#define MENU_DEBUG
//#define PVI_DEBUG
#define MAIN_DEBUG
//#define TIMER_DEBUG
//#define DMA_DEBUG
#define VPU_DEBUG

#ifdef DU_DEBUG
#define DU_PRINT(x...)		{ printf("%s(%d): ", "DU", __LINE__); \
								printf(x); }
#else
#define DU_PRINT(x...)	(void)(0)
#endif

#ifdef MENU_DEBUG
#define MENU_PRINT(x...)	{ printf("%s(%d): ", "MENU", __LINE__); \
								printf(x); }
#else
#define MENU_PRINT(x...)	(void)(0)
#endif

#ifdef PVI_DEBUG
#define PVI_PRINT(x...)		{ printf("%s(%d): ", "PVI", __LINE__); \
								printf(x); }
#else
#define PVI_PRINT(x...)	(void)(0)
#endif

#ifdef MAIN_DEBUG
#define MAIN_PRINT(x...)	{ printf("%s(%d): ", "MAIN", __LINE__); \
								printf(x); }
#else
#define MAIN_PRINT(x...)	(void)(0)
#endif

#ifdef TIMER_DEBUG
#define TIMER_PRINT(x...)	{ printf("%s(%d): ", __FILE__, __LINE__); \
								printf(x); }
#else
#define TIMER_PRINT(x...)	(void)(0)
#endif

#ifdef DMA_DEBUG
#define DMA_PRINT(x...)	{ printf("%s(%d): ", __FILE__, __LINE__); \
								printf(x); }
#else
#define DMA_PRINT(x...)	(void)(0)
#endif

#define _dbg(fmt, arg...) { printf("[%s:%d] "fmt, __FILE__, __LINE__, ## arg); }

#ifdef VPU_DEBUG
#define VPU_PRINT(x...)	{ printf("%s(%d): ", "VPU", __LINE__); \
								printf(x); }
#else
#define VPU_PRINT(x...)	(void)(0)
#endif


void print_hex(uint8 *buf, uint32 size);
void print_hexw(uint32 *buf, uint32 size);


#endif
