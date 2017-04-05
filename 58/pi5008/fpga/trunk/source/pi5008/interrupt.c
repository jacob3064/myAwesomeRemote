#include <nds32_intrinsic.h>
#include "intr.h"
#include "system.h"
#include "type.h"
#include "debug.h"

inline void GIE_ENABLE()
{
	__nds32__setgie_en();
}

inline void GIE_DISABLE()
{
	__nds32__setgie_dis();
	__nds32__dsb();
}

void initIntr()
{
	unsigned int reg;

	/* Check IVIC numbers (IVB.NIVIC) */
	reg = __nds32__mfsr(NDS32_SR_IVB);
	if ((reg & 0x0E) != 0) {	// 32IVIC
		/* enable HW# (timer, dma) */

#if defined(MODIFIED_IRQ_NUM)
		__nds32__mtsr((IC_IRQ_SW | IC_IRQ_DMA | IC_IRQ_TIMER | IC_IRQ_SPI | IC_IRQ_GPIO | IC_IRQ_UART ), NDS32_SR_INT_MASK2);
#else
		__nds32__mtsr((IC_IRQ_4 | IC_IRQ_7 | IC_IRQ_9 | IC_IRQ_18 | IC_IRQ_12 ), NDS32_SR_INT_MASK2);
#endif
		/* Interrupt pending register, write 1 to clear */
		__nds32__mtsr(0xFFFFFFFF, NDS32_SR_INT_PEND2);
	}
}

void sw_isr(uint32 num)
{
	uint32 msk = (1 << num);
	
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_INT_MASK2) & ~msk, NDS32_SR_INT_MASK2);
	__nds32__mtsr(msk, NDS32_SR_INT_PEND2);												// clear int pending
	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_PEND) & (~0x10000), NDS32_SR_INT_PEND);		// clear swi

	
	printf("SW Interrupt\n");

	__nds32__mtsr(__nds32__mfsr(NDS32_SR_INT_MASK2) | msk, NDS32_SR_INT_MASK2);

}

void gen_swi(void)
{
	unsigned int int_pend;

	int_pend = __nds32__mfsr(NDS32_SR_INT_PEND);
	int_pend |= 0x10000;
	__nds32__mtsr_dsb(int_pend, NDS32_SR_INT_PEND);

}

