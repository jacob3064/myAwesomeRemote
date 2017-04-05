#include <nds32_intrinsic.h>
#include "config.h"
#include "debug.h"

#define NDS32_ISET          (0x07 << 0)
#define NDS32_IWAY          (0x07 << 3)
#define NDS32_ISIZE         (0x07 << 6)
#define NDS32_DSET          (0x07 << 0)
#define NDS32_DWAY          (0x07 << 3)
#define NDS32_DSIZE         (0x07 << 6)


void cache_enable()
{

	unsigned int cache_ctl=0, mmu_ctl, icm_cfg, dcm_cfg;

	/* Get ICache ways, sets, line size  */
	icm_cfg = __nds32__mfsr(NDS32_SR_ICM_CFG);

	if (((icm_cfg & NDS32_ISIZE) >> 6) && ((icm_cfg & NDS32_ISIZE) >> 6) <= 5){
		cache_ctl = __nds32__mfsr(NDS32_SR_CACHE_CTL);
		cache_ctl |= 0x1;

		/* Enable I Cache */
		__nds32__mtsr(cache_ctl, NDS32_SR_CACHE_CTL);

	}

	/* Get DCache ways, sets, line size  */
	dcm_cfg = __nds32__mfsr(NDS32_SR_DCM_CFG);

	if (((dcm_cfg & NDS32_DSIZE) >> 6) && ((dcm_cfg & NDS32_DSIZE) >> 6) <= 5){
		cache_ctl = __nds32__mfsr(NDS32_SR_CACHE_CTL);
		cache_ctl |= 0x2;
		/* Enable D Cache */
		__nds32__mtsr(cache_ctl, NDS32_SR_CACHE_CTL);
	}

	/* Cacheable/Write-Back for NTC0 */
	mmu_ctl = __nds32__mfsr(NDS32_SR_MMU_CTL);
	mmu_ctl &= (~(0x3<<1));
	mmu_ctl |= (0x2 << 1);

	/* Set NTC1(Non-cacheable/Non-coalesable) to PA partition 0*/
	mmu_ctl &= (~(0x3<<3));
	mmu_ctl &= (~(0x3<<13));
	
	__nds32__mtsr_isb(mmu_ctl, NDS32_SR_MMU_CTL);

	

}

void cache_disable()
{
#ifndef __NDS32_ISA_V3M__
	unsigned int dcm_cfg, cache_line, end;

	/* Check if cache available */
	if (!(__nds32__mfsr(NDS32_SR_ICM_CFG) & NDS32_ISIZE) ||
	    !(__nds32__mfsr(NDS32_SR_CACHE_CTL) & 0x3))
		return;

	/* DCache end = cache line size * cache set * cache way. */
	dcm_cfg = __nds32__mfsr(NDS32_SR_DCM_CFG);
	end = cache_line = 1 << (((dcm_cfg & NDS32_DSIZE) >> 6) + 2);
	end *= (1 << ((dcm_cfg & NDS32_DSET) + 6));
	end *= (((dcm_cfg & NDS32_DWAY) >> 3) + 1);

	/* Flush DCache */
	do {
		end -= cache_line;
		__nds32__cctlidx_wbinval(NDS32_CCTL_L1D_IX_WB, end);
		__nds32__dsb();
	} while (end > 0);

	/* Invalid DCache */
	__nds32__cctl_l1d_invalall();
	__nds32__dsb();

	/* Disable I/D Cache */
	__nds32__mtsr_dsb(__nds32__mfsr(NDS32_SR_CACHE_CTL) & ~0x3, NDS32_SR_CACHE_CTL);
#endif
}


void cache_wb_range(unsigned int *addr, int size)
{
	unsigned int dsize;

	dsize = 1 << (((__nds32__mfsr(NDS32_SR_DCM_CFG) & NDS32_DSIZE) >> 6) + 2);
	while(size > 0){
		__nds32__cctlva_wbinval_one_lvl(NDS32_CCTL_L1D_VA_WB, addr);
		__nds32__dsb();
		addr += (dsize>>2);
		size -= dsize;
	}
	
}

void cache_inv_range(unsigned int *addr, int size)
{
	unsigned int dsize;

	dsize = 1 << (((__nds32__mfsr(NDS32_SR_DCM_CFG) & NDS32_DSIZE) >> 6) + 2);

	while(size > 0){
		__nds32__cctlva_wbinval_one_lvl(NDS32_CCTL_L1D_VA_INVAL, addr);
		__nds32__dsb();
		addr += (dsize>>2);
		size -= dsize;
	}
	
}
	

