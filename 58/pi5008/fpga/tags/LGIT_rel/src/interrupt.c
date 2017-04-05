#include <nds32_intrinsic.h>
#include "intr.h"
#include "system.h"

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


	/* Edge-trigger */
	__nds32__mtsr_isb(0xFFFFFFFF, NDS32_SR_INT_TRIGGER);
	
	/* Check IVIC numbers (IVB.NIVIC) */
	reg = __nds32__mfsr(NDS32_SR_IVB);
	if ((reg & 0x0E) != 0) {	// 32IVIC
		/* enable HW# (timer, dma) */
		__nds32__mtsr((IC_IRQ_9 | IC_IRQ_7), NDS32_SR_INT_MASK2);

		/* Interrupt pending register, write 1 to clear */
		__nds32__mtsr(0xFFFFFFFF, NDS32_SR_INT_PEND2);
	} 
}

