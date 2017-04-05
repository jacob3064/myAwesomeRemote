/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
// Config the features of startup demo programs.

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Please put the defines shared by makefile projects and AndeSight projects

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// GCC < 4.8.2 (Before BSP400 gcc version)
// Check the GCC version for toolchain builtin macro compatible issue.
#if GCC_VERSION < 40802

#ifdef NDS32_BASELINE_V3
#define __NDS32_ISA_V3__	NDS32_BASELINE_V3
#endif

#ifdef NDS32_BASELINE_V3M
#define __NDS32_ISA_V3M__	NDS32_BASELINE_V3M
#endif

#endif

// Chkeck whether the following intrinsic functions are supported by toolchain.
// Some intrinsic functions are not supported by old toolchain and need to be wrapped.
#ifndef __ASSEMBLER__

#include <nds32_intrinsic.h>

#ifndef __nds32__mtsr_isb
#define __nds32__mtsr_isb(val, srname)  \
do {__nds32__mtsr(val, srname);__nds32__isb();} while(0)
#endif

#ifndef __nds32__mtsr_dsb
#define __nds32__mtsr_dsb(val, srname)	\
do {__nds32__mtsr(val, srname);__nds32__dsb();} while(0)
#endif

#endif

#ifndef	CFG_MAKEFILE

// The defines are only used by AndeSight projects

//----------------------------------------------------------------------------------------------------

// Users can configure the defines in this area
// to match different environment setting

// Platform select : AG101P 
//#define	CFG_AG101P		// platform is AG101P
#define CFG_PIXELPLUS_FPGA

// Support ZOL select			// do ZOL supporting when CPU supports ZOL
#define	CFG_HWZOL

// Build mode select
// The BUILD_MODE can be specified to BUILD_BURN/BUILD_LOAD only.
#define BUILD_MODE	BUILD_LOAD


//----------------------------------------------------------------------------------------------------
// The followings are predefined settings
// Please do not modify them

#define	BUILD_LOAD	1		// The program is loaded by GDB or eBIOS
#define	BUILD_BURN	2		// The program is burned to the flash, but run in RAM
							// demo-ls2 use BURN mode

#ifndef __NDS32_EXT_EX9__
	// This toolchain cannot support EX9
	#define CONFIG_NO_NDS32_EXT_EX9
#endif

#if BUILD_MODE == BUILD_BURN
	// Burn mode
	#define CFG_BURN
	#define CFG_LLINIT              // do low level init
	#define CFG_REMAP       // do remap
#elif BUILD_MODE == BUILD_LOAD
	// Load mode
	#define CFG_LOAD
#else
	#error "Unsupport mode"
#endif

#ifdef __NDS32_ISA_V3M__
	// v3m toolchain only support 16MB
	#undef CFG_16MB
	#define CFG_16MB
#endif

#endif // CFG_MAKEFILE

#endif // __CONFIG_H__
