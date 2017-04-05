/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include <nds32_intrinsic.h>
#include "config.h"

#ifndef VECTOR_BASE
#define VECTOR_BASE	0x20000000
#endif

/* It will use Default_Handler if you don't have one */
#pragma weak tlb_exception_handler   = Default_Handler
#pragma weak error_exception_handler = Default_Handler
#pragma weak syscall_handler         = Default_Handler
#pragma weak HW0_ISR   = Default_Handler
#pragma weak HW1_ISR   = Default_Handler
#pragma weak HW2_ISR   = Default_Handler
#pragma weak HW3_ISR   = Default_Handler
#pragma weak HW4_ISR   = Default_Handler
#pragma weak HW5_ISR   = Default_Handler
#pragma weak HW6_ISR   = Default_Handler
#pragma weak HW7_ISR   = Default_Handler
#pragma weak HW8_ISR   = Default_Handler
#pragma weak HW9_ISR   = Default_Handler
#pragma weak HW10_ISR  = Default_Handler
#pragma weak HW11_ISR  = Default_Handler
#pragma weak HW12_ISR  = Default_Handler
#pragma weak HW13_ISR  = Default_Handler
#pragma weak HW14_ISR  = Default_Handler
#pragma weak HW15_ISR  = Default_Handler
#pragma weak HW16_ISR  = Default_Handler
#pragma weak HW17_ISR  = Default_Handler
#pragma weak HW18_ISR  = Default_Handler
#pragma weak HW19_ISR  = Default_Handler
#pragma weak HW20_ISR  = Default_Handler
#pragma weak HW21_ISR  = Default_Handler
#pragma weak HW22_ISR  = Default_Handler
#pragma weak HW23_ISR  = Default_Handler
#pragma weak HW24_ISR  = Default_Handler
#pragma weak HW25_ISR  = Default_Handler
#pragma weak HW26_ISR  = Default_Handler
#pragma weak HW27_ISR  = Default_Handler
#pragma weak HW28_ISR  = Default_Handler
#pragma weak HW29_ISR  = Default_Handler
#pragma weak HW30_ISR  = Default_Handler
#pragma weak HW31_ISR  = Default_Handler
#pragma weak SW0_ISR   = Default_Handler

//extern int uart_puts(const char *);

__attribute__((unused))
static void Default_Handler()
{
	//uart_puts("Default Handler");
	while (1) ;
}

#pragma weak __soc_init = __null_function

void __null_function()
{;
}

void __c_init()
{
/* Use compiler builtin memcpy and memset */
#define MEMCPY(des, src, n) __builtin_memcpy ((des), (src), (n))
#define MEMSET(s, c, n) __builtin_memset ((s), (c), (n))

	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;
	extern char __bss_start;
	extern char _end;
	int size;

	/* Copy data section from ROM to RAM*/
	size = &_edata - &__data_start;
	MEMCPY(&__data_start, &__data_lmastart, size);

	/* Clear bss section */
	size = &_end - &__bss_start;
	MEMSET(&__bss_start, 0, size);
	return;
}

void __cpu_init()
{
	unsigned int tmp;

	/* turn on BTB */
	tmp = 0x0;
	__nds32__mtsr(tmp, NDS32_SR_MISC_CTL);

	/* disable all hardware interrupts */
	__nds32__mtsr(0x0, NDS32_SR_INT_MASK);
	/* INT_MASK2 only exists when IVB.IVIC_VER=1 */
	if (__nds32__mfsr(NDS32_SR_IVB) & (0x1 << 11))
		__nds32__mtsr(0x0, NDS32_SR_INT_MASK2);

	/* Set IVB */
	/* Default: IVIC, vector size: 4 bytes, base: VECTOR_BASE */
	tmp = VECTOR_BASE;
#if defined(USE_C_EXT) && defined(__NDS32_ISA_V3M__)
	/* Use C extension (USE_C_EXT in CLFAGS) insetad of
	 * assembly version, and v3m toolchain is used.
	 */
	tmp |= (1 << 14);	// set vector size: 16 bytes
#endif
#ifdef CFG_EVIC
	tmp |= (1 << 13);	// set EVIC
#endif
	__nds32__mtsr(tmp, NDS32_SR_IVB);

	/* Set PSW */
	/* set PSW.INTL to 0 and PSW.CPL to 7 to allow any priority */
	tmp = __nds32__mfsr(NDS32_SR_PSW);
	tmp &= 0xfffffff9;
	tmp |= 0x70008;
#ifdef CFG_HWZOL
	/* Enable PSW AEN */
	tmp = tmp | 0x2000;
#endif
	__nds32__mtsr_dsb(tmp, NDS32_SR_PSW);

	/* Check interrupt priority programmable*
	* IVB.PROG_PRI_LVL
	*      0: Fixed priority       -- no exist ir18 1r19
	*      1: Programmable priority
	*/
	if (__nds32__mfsr(NDS32_SR_IVB) & 0x01) {
		/* Set PPL2FIX_EN to 0 to enable Programmable
	 	* Priority Level */
		__nds32__mtsr(0x0, NDS32_SR_INT_CTRL);
		/* Check IVIC numbers (IVB.NIVIC) */
	}

	return;
}


void __init()
{
/*----------------------------------------------------------
   !!  Users should NOT add any code before this comment  !!
------------------------------------------------------------*/
	__cpu_init();
	__c_init();     //copy data section, clean bss

	/* Double check ZOL supporting */
	/*
	 * Check whether the CPU configured with ZOL supported.
	 * The MSC_CFG.MSC_EXT($cr4) indicates MSC_CFG2 register exist
	 * and MSC_CFG2($cr7) bit 5 indicates ZOL supporting.
	 */
#ifdef CFG_HWZOL
	if (!((__nds32__mfsr(NDS32_SR_MSC_CFG) & (3 << 30))
	    && (__nds32__mfsr(NDS32_SR_MSC_CFG2) & (1 << 5)))) {
		/* CPU doesn't support ZOL, but build with ZOL supporting. */
		//uart_puts("CPU doesn't support ZOL, but build with ZOL supporting !!\n");
		while(1);
	}
#else
	if ((__nds32__mfsr(NDS32_SR_MSC_CFG) & (3 << 30))
	    && (__nds32__mfsr(NDS32_SR_MSC_CFG2) & (1 << 5))) {
		/* CPU supports ZOL, but build without ZOL supporting. */
		//uart_puts("CPU supports ZOL, but build without ZOL supporting !!\n");
	}
#endif
}

