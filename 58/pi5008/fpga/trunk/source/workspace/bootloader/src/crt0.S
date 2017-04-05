##==============================================================================
##
##  crt0.S
##
##  nds32 startup code
##
##==============================================================================
##
## Copyright (c) 2012-2015 Andes Technology Corporation
## All rights reserved.
##

#include <nds32_init.inc>
#include "config.h"
#include "intr.h"

##------------------------------------------------------------------------------
	.macro vector name
	.align 2
	j OS_Trap_\name
	.endm

	.section .nds32_init, "ax"
!========================================================================
! Vector table
!========================================================================
	.align 2
exception_vector:
	j _start                    !  (0) Trap Reset
	vector TLB_Fill             !  (1) Trap TLB fill
	vector PTE_Not_Present      !  (2) Trap PTE not present
	vector TLB_Misc             !  (3) Trap TLB misc
	vector TLB_VLPT_Miss        !  (4) Trap TLB VLPT miss
	vector Machine_Error        !  (5) Trap Machine error
	vector Debug_Related        !  (6) Trap Debug related
	vector General_Exception    !  (7) Trap General exception
	vector Syscall              !  (8) Syscall

	.align  2
exception_vector_end:


	.section .text
OS_Trap_TLB_Fill:
OS_Trap_PTE_Not_Present:
OS_Trap_TLB_Misc:
OS_Trap_TLB_VLPT_Miss:
OS_Trap_Machine_Error:
OS_Trap_Debug_Related:
OS_Trap_General_Exception:

1:  b   1b
OS_Trap_Syscall:
	SYSCALL_SAVE_ALL
	bal syscall_handler
	SYSCALL_RESTORE_ALL
	iret
		
##------------------------------------------------------------------------------
## Startup code

	.global _start
	.func _start
	.type _start, @function

	.align 2
_start:
	!************************** Begin of do-not-modify **************************
	! Please don't modify this code
	! Initialize the registers used by the compiler

	nds32_init		! NDS32 startup initial macro in <nds32_init.inc>

#ifdef CFG_DEBUG
	mfsr $r0, $MSC_CFG

#ifndef CONFIG_NO_NDS32_EXT_EX9
	li  $r1, (1 << 24)      ! EIT
	and $r2, $r0, $r1
1:	beqz $r2, 1b
#endif

#ifdef __NDS32_EXT_IFC__
	li  $r1, (1 << 19)      ! IFC
	and $r2, $r0, $r1
1:	beqz $r2, 1b
#endif

#endif //#ifdef CFG_DEBUG

	!*************************** End of do-not-modify ***************************

	bal __init
	bal bootloader_main
	
	jr	$r0
	
1:  b   1b

	.size _start, .-_start

	.end
