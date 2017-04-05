/*
 * Copyright (c) 2012-2015 Andes Technology Corporation
 * All rights reserved.
 *
 */
#ifndef INTR_H
#define INTR_H


/* interrupt sources */
/************************************
 *	IRQ number						*
 ************************************/
#define	IRQ_0		      0  // IVIC HW0
#define	IRQ_1             1  // IVIC HW1
#define	IRQ_2			  2  // IVIC HW2
#define	IRQ_3	          3  // IVIC HW3
#define IRQ_4 		      4  // IVIC HW4
#define	IRQ_5	          5  // IVIC HW5
#define	IRQ_6             6  // IVIC HW6
#define IRQ_7		      7  // IVIC HW7
#define IRQ_8             8  // IVIC HW8
#define IRQ_9	          9  // IVIC HW9
#define IRQ_10	          10 // IVIC HW10
#define IRQ_11   	      11 // IVIC HW11
#define IRQ_12   	      12 // IVIC HW12
#define	IRQ_13		      13  // IVIC HW13
#define	IRQ_14            14  // IVIC HW14
#define	IRQ_15			  15  // IVIC HW15
#define	IRQ_16	          16  // IVIC HW16
#define IRQ_17 		      17  // IVIC HW17
#define	IRQ_18	          18  // IVIC HW18
#define	IRQ_19            19  // IVIC HW19
#define IRQ_20		      20  // IVIC HW20
#define IRQ_21            21  // IVIC HW21
#define IRQ_22	          22  // IVIC HW22
#define IRQ_23	          23  // IVIC HW23
#define IRQ_24   	      24  // IVIC HW24
#define IRQ_25   	      25  // IVIC HW25
#define	IRQ_26	          26  // IVIC HW26
#define	IRQ_27            27  // IVIC HW27
#define IRQ_28		      28  // IVIC HW28
#define IRQ_29            29  // IVIC HW29
#define IRQ_30	          30  // IVIC HW30
#define IRQ_31	          31  // IVIC HW31

#if 1

	/*	interrupt sources */
#define	IC_IRQ_RSV0		       	(1 << IRQ_0)
#define	IC_IRQ_RSV1            	(1 << IRQ_1)
#define	IC_IRQ_RSV2			   	(1 << IRQ_2)
#define	IC_IRQ_RSV3	           	(1 << IRQ_3)
#define IC_IRQ_SW	       		(1 << IRQ_4)
#define	IC_IRQ_SPIMC          	(1 << IRQ_5)
#define	IC_IRQ_SPI	           	(1 << IRQ_6)
#define IC_IRQ_DMA		       	(1 << IRQ_7)
#define IC_IRQ_WDT             	(1 << IRQ_8)
#define IC_IRQ_TIMER          	(1 << IRQ_9)
#define IC_IRQ_GPIO	           	(1 << IRQ_10)
#define IC_IRQ_I2S             	(1 << IRQ_11)
#define IC_IRQ_UART        	   	(1 << IRQ_12)
#define	IC_IRQ_I2C		       	(1 << IRQ_13)
#define	IC_IRQ_PVIRX0           (1 << IRQ_14)
#define	IC_IRQ_PVIRX1			(1 << IRQ_15)
#define	IC_IRQ_PVIRX2	        (1 << IRQ_16)
#define IC_IRQ_PVIRX3	        (1 << IRQ_17)
#define	IC_IRQ_ISP0	           	(1 << IRQ_18)
#define	IC_IRQ_ISP1	           	(1 << IRQ_19)
#define IC_IRQ_ISP2		       	(1 << IRQ_20)
#define IC_IRQ_ISP3             (1 << IRQ_21)
#define IC_IRQ_DU	           	(1 << IRQ_22)
#define IC_IRQ_SVM	           	(1 << IRQ_23)
#define IC_IRQ_VPU0             (1 << IRQ_24)
#define IC_IRQ_VPU1        	   	(1 << IRQ_25)
#define	IC_IRQ_VSYNC_CAM0		(1 << IRQ_26)
#define	IC_IRQ_VSYNC_CAM1       (1 << IRQ_27)
#define	IC_IRQ_VSYNC_CAM2		(1 << IRQ_28)
#define	IC_IRQ_VSYNC_CAM3	    (1 << IRQ_29)
#define IC_IRQ_VSYNC_SVM        (1 << IRQ_30)
#define	IC_IRQ_VSYNC_BTO        (1 << IRQ_30)

	/* ISR function name for 32IVIC/EVIC platform */
#define sw_isr					SET_HWISR(IRQ_4)
#define spi0_isr				SET_HWISR(IRQ_6)
#define dma_isr					SET_HWISR(IRQ_7)
#define timer_isr              	SET_HWISR(IRQ_9)
#define gpio_isr				SET_HWISR(IRQ_10)
#define uart_isr				SET_HWISR(IRQ_12)
#define vpu_ctrl_isr			SET_HWISR(IRQ_24)
#define vpu_dma_isr				SET_HWISR(IRQ_25)
#define SVMAPI_vsync_isr		SET_HWISR(IRQ_30)

#else
	/*	interrupt sources */
#define	IC_IRQ_0		       (1 << IRQ_0)
#define	IC_IRQ_1               (1 << IRQ_1)
#define	IC_IRQ_2			   (1 << IRQ_2)
#define	IC_IRQ_3	           (1 << IRQ_3)
#define IC_IRQ_4	           (1 << IRQ_4)
#define	IC_IRQ_5	           (1 << IRQ_5)
#define	IC_IRQ_6	           (1 << IRQ_6)
#define IC_IRQ_7		       (1 << IRQ_7)
#define IC_IRQ_8               (1 << IRQ_8)
#define IC_IRQ_9	           (1 << IRQ_9)
#define IC_IRQ_10	           (1 << IRQ_10)
#define IC_IRQ_11              (1 << IRQ_11)
#define IC_IRQ_12         	   (1 << IRQ_12)
#define	IC_IRQ_13		       (1 << IRQ_13)
#define	IC_IRQ_14              (1 << IRQ_14)
#define	IC_IRQ_15			   (1 << IRQ_15)
#define	IC_IRQ_16	           (1 << IRQ_16)
#define IC_IRQ_17	           (1 << IRQ_17)
#define	IC_IRQ_18	           (1 << IRQ_18)
#define	IC_IRQ_19	           (1 << IRQ_19)
#define IC_IRQ_20		       (1 << IRQ_20)
#define IC_IRQ_21              (1 << IRQ_21)
#define IC_IRQ_22	           (1 << IRQ_22)
#define IC_IRQ_23	           (1 << IRQ_23)
#define IC_IRQ_24              (1 << IRQ_24)
#define IC_IRQ_25         	   (1 << IRQ_25)
#define	IC_IRQ_26		       (1 << IRQ_26)
#define	IC_IRQ_27              (1 << IRQ_27)
#define	IC_IRQ_28			   (1 << IRQ_28)
#define	IC_IRQ_29	           (1 << IRQ_29)
#define IC_IRQ_30	           (1 << IRQ_30)
#define	IC_IRQ_31	           (1 << IRQ_31)

	/* ISR function name for 32IVIC/EVIC platform */
#define sw_isr					SET_HWISR(IRQ_4)
#define dma_isr					SET_HWISR(IRQ_7)
#define timer_isr              	SET_HWISR(IRQ_9)
#define uart_isr				SET_HWISR(IRQ_16)
#define spi0_isr				SET_HWISR(IRQ_18)
#define vpu_ctrl_isr			SET_HWISR(IRQ_24)
#define vpu_dma_isr				SET_HWISR(IRQ_25)
#define SVMAPI_vsync_isr		SET_HWISR(IRQ_30)

#endif

#define NDS32_HWINT_ID(hw)     NDS32_INT_H##hw
#define NDS32_HWINT(hw)        NDS32_HWINT_ID(hw)

#define HW_ISR(hw)             HW##hw##_ISR
#define SET_HWISR(hw)          HW_ISR(hw)


#ifdef __ASSEMBLER__
	.macro	hal_hw_vectors
		vector Interrupt_HW0	 //  (9) Interrupt HW0
		vector Interrupt_HW1	 // (10) Interrupt HW1
		vector Interrupt_HW2	 // (11) Interrupt HW2
		vector Interrupt_HW3	 // (12) Interrupt HW3
		vector Interrupt_HW4	 // (13) Interrupt HW4
		vector Interrupt_HW5	 // (14) Interrupt HW5
		vector Interrupt_HW6	 // (15) Interrupt HW6
		vector Interrupt_HW7	 // (16) Interrupt HW7
		vector Interrupt_HW8	 // (17) Interrupt HW8
		vector Interrupt_HW9	 // (18) Interrupt HW9 (32IVIC, TIMER0)
		vector Interrupt_HW10	// (19) Interrupt HW10
		vector Interrupt_HW11	// (20) Interrupt HW11
		vector Interrupt_HW12	// (21) Interrupt HW12
		vector Interrupt_HW13	// (22) Interrupt HW13
		vector Interrupt_HW14	// (23) Interrupt HW14
		vector Interrupt_HW15	// (24) Interrupt HW15
		vector Interrupt_HW16	// (25) Interrupt HW16
		vector Interrupt_HW17	// (26) Interrupt HW17
		vector Interrupt_HW18	// (27) Interrupt HW18
		vector Interrupt_HW19	// (28) Interrupt HW19
		vector Interrupt_HW20	// (29) Interrupt HW20
		vector Interrupt_HW21	// (30) Interrupt HW21
		vector Interrupt_HW22	// (31) Interrupt HW22
		vector Interrupt_HW23	// (32) Interrupt HW23
		vector Interrupt_HW24	// (33) Interrupt HW24
		vector Interrupt_HW25	// (34) Interrupt HW25
		vector Interrupt_HW26	// (35) Interrupt HW26
		vector Interrupt_HW27	// (36) Interrupt HW27
		vector Interrupt_HW28	// (37) Interrupt HW28
		vector Interrupt_HW29	// (38) Interrupt HW29
		vector Interrupt_HW30	// (39) Interrupt HW30
		vector Interrupt_HW31	// (40) Interrupt HW31
	.endm

	.macro	hal_hw_ISR
		Os_Trap_Interrupt_HW	IRQ_0, 0
		Os_Trap_Interrupt_HW	IRQ_1, 0
		Os_Trap_Interrupt_HW	IRQ_2, 0
		Os_Trap_Interrupt_HW	IRQ_3, 0
		Os_Trap_Interrupt_HW	IRQ_4, 0
		Os_Trap_Interrupt_HW	IRQ_5, 0
		Os_Trap_Interrupt_HW	IRQ_6, 0
		Os_Trap_Interrupt_HW	IRQ_7, 0
		Os_Trap_Interrupt_HW	IRQ_8, 0
		Os_Trap_Interrupt_HW	IRQ_9, 0
		Os_Trap_Interrupt_HW	IRQ_10, 0
		Os_Trap_Interrupt_HW	IRQ_11, 0
		Os_Trap_Interrupt_HW	IRQ_12, 0
		Os_Trap_Interrupt_HW	IRQ_13, 0
		Os_Trap_Interrupt_HW	IRQ_14, 0
		Os_Trap_Interrupt_HW	IRQ_15, 0
		Os_Trap_Interrupt_HW	IRQ_16, 0
		Os_Trap_Interrupt_HW	IRQ_17, 0
		Os_Trap_Interrupt_HW	IRQ_18, 0
		Os_Trap_Interrupt_HW	IRQ_19, 0
		Os_Trap_Interrupt_HW	IRQ_20, 0
		Os_Trap_Interrupt_HW	IRQ_21, 0
		Os_Trap_Interrupt_HW	IRQ_22, 0
		Os_Trap_Interrupt_HW	IRQ_23, 0
		Os_Trap_Interrupt_HW	IRQ_24, 0
		Os_Trap_Interrupt_HW	IRQ_25, 0
		Os_Trap_Interrupt_HW	IRQ_26, 0
		Os_Trap_Interrupt_HW	IRQ_27, 0
		Os_Trap_Interrupt_HW	IRQ_28, 0
		Os_Trap_Interrupt_HW	IRQ_29, 0
		Os_Trap_Interrupt_HW	IRQ_30, 0
		Os_Trap_Interrupt_HW	IRQ_31, 0
	.endm



	.macro gie_disable
		setgie.d
		dsb
	.endm

	.macro gie_enable
		setgie.e
	.endm

	/*
	 * push_misc/pop_misc
	 * The macros are used for saving/restoring IFC_LP and ZOL
	 * related system register.
	 */
	.macro push_misc R0="$r2", R1="$r3", R2="$r4", R3="$r5", ODD_ALIGN=0
#if defined(__NDS32_EXT_IFC__) && defined(CFG_HWZOL)
		mfusr	\R0, $IFC_LP
		mfusr	\R1, $LB
		mfusr	\R2, $LE
		mfusr	\R3, $LC
		pushm	\R0, \R3
		.if (1 == \ODD_ALIGN)
		push	\R0             // R0: Dummy
		.endif
#else
#ifdef __NDS32_EXT_IFC__
		mfusr   \R0, $IFC_LP
		.if (1 == \ODD_ALIGN)
		push	\R0
		.else
		pushm	\R0, \R1        // R1: dummy
		.endif
#endif
#ifdef CFG_HWZOL
		mfusr	\R0, $LB
		mfusr	\R1, $LE
		mfusr	\R2, $LC
		.if (1 == \ODD_ALIGN)
		pushm	\R0, \R2
		.else
		pushm	\R0, \R3        // R3: dummy
		.endif
#endif
#endif
	.endm

	.macro pop_misc R0="$r2", R1="$r3", R2="$r4", R3="$r5", ODD_ALIGN=0
#if defined(__NDS32_EXT_IFC__) && defined(CFG_HWZOL)
		.if (1 == \ODD_ALIGN)
		pop	\R0             // R0: summy
		.endif
		popm	\R0, \R3
		mtusr	\R0, $IFC_LP
		mtusr	\R1, $LB
		mtusr	\R2, $LE
		mtusr	\R3, $LC
#else
#ifdef __NDS32_EXT_IFC__
		.if (1 == \ODD_ALIGN)
		pop	\R0
		.else
		popm	\R0, \R1        // R1: dummy
		.endif
		mtusr   \R0, $IFC_LP
#endif
#ifdef CFG_HWZOL
		.if (1 == \ODD_ALIGN)
		popm	\R0, \R2
		.else
		popm	\R0, \R3        // R3: dummy
		.endif
		mtusr	\R0, $LB
		mtusr	\R1, $LE
		mtusr	\R2, $LC
#endif
#endif
	.endm

#ifdef __NDS32_ABI_2FP_PLUS__
	/* New FPU ABI */
	.macro SAVE_FPU_REGS_00
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_01
		SAVE_FPU_REGS_00
	.endm

	.macro SAVE_FPU_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd23, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd21, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd19, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd17, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd15, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd13, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd11, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm
#else
	.macro SAVE_FPU_REGS_00
		addi    $sp, $sp, -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_01
		addi    $sp, $sp, -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_02
		addi    $sp, $sp, -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm

	.macro SAVE_FPU_REGS_03
		addi    $sp, $sp, -8
		fsdi.bi $fd30, [$sp], -8
		fsdi.bi $fd28, [$sp], -8
		fsdi.bi $fd26, [$sp], -8
		fsdi.bi $fd24, [$sp], -8
		fsdi.bi $fd22, [$sp], -8
		fsdi.bi $fd20, [$sp], -8
		fsdi.bi $fd18, [$sp], -8
		fsdi.bi $fd16, [$sp], -8
		fsdi.bi $fd14, [$sp], -8
		fsdi.bi $fd12, [$sp], -8
		fsdi.bi $fd10, [$sp], -8
		fsdi.bi $fd8, [$sp], -8
		fsdi.bi $fd6, [$sp], -8
		fsdi.bi $fd4, [$sp], -8
		fsdi.bi $fd2, [$sp], -8
		fsdi.bi $fd1, [$sp], -8
		fsdi    $fd0, [$sp+0]
	.endm
#endif

	.macro push_fpu
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		SAVE_FPU_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		SAVE_FPU_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		SAVE_FPU_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		SAVE_FPU_REGS_03
#else
#endif
	.endm

#ifdef __NDS32_ABI_2FP_PLUS__
        /* New FPU ABI */
	.macro RESTORE_FPU_REGS_00
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_01
		RESTORE_FPU_REGS_00
	.endm

	.macro RESTORE_FPU_REGS_02
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_03
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd11, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd13, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd15, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd17, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd19, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd21, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd23, [$sp], 8
	.endm
#else
	.macro RESTORE_FPU_REGS_00
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_01
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_02
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
	.endm

	.macro RESTORE_FPU_REGS_03
		fldi.bi $fd0, [$sp], 8
		fldi.bi $fd1, [$sp], 8
		fldi.bi $fd2, [$sp], 8
		fldi.bi $fd4, [$sp], 8
		fldi.bi $fd6, [$sp], 8
		fldi.bi $fd8, [$sp], 8
		fldi.bi $fd10, [$sp], 8
		fldi.bi $fd12, [$sp], 8
		fldi.bi $fd14, [$sp], 8
		fldi.bi $fd16, [$sp], 8
		fldi.bi $fd18, [$sp], 8
		fldi.bi $fd20, [$sp], 8
		fldi.bi $fd22, [$sp], 8
		fldi.bi $fd24, [$sp], 8
		fldi.bi $fd26, [$sp], 8
		fldi.bi $fd28, [$sp], 8
		fldi.bi $fd30, [$sp], 8
	.endm
#endif

	.macro pop_fpu
#if defined(__NDS32_EXT_FPU_CONFIG_0__)
		RESTORE_FPU_REGS_00
#elif defined(__NDS32_EXT_FPU_CONFIG_1__)
		RESTORE_FPU_REGS_01
#elif defined(__NDS32_EXT_FPU_CONFIG_2__)
		RESTORE_FPU_REGS_02
#elif defined(__NDS32_EXT_FPU_CONFIG_3__)
		RESTORE_FPU_REGS_03
#else
#endif
	.endm

	/*
	 * The SAVE/RESTORE macros for syscall.
	 * Since we have the syscall handler to be reentrant, it is
	 * necessary to do $IPC/$IPSW saving/restoring and descending
	 * interrupt level. Because the reentrant may go beyond the
	 * HW nested interruption transition level.
	 */
	.macro SYSCALL_SAVE_ALL
		/* push caller-saved gpr and $r6 ~ $r9, except $r0 */
		pushm   $r1, $r9                    ! $r0 for return value,
		pushm   $r15,$r30                   ! $r0 ~ $r5 are arguments, and
		                                    ! $r6 ~ $r9 are clobber-list

		/* push $IFC_LP or zol($LE, $LB, $LC) with odd word align */
		push_misc "$r6","$r7","$r8","$r9",1

		/* push $IPC(+4), $IPSW */
		mfsr    $r6, $IPC
		addi    $r6, $r6, 4
		mfsr    $r7, $IPSW
		pushm   $r6, $r7                    ! save $ipc, $ipsw

		/* Descend interrupt level */
		mfsr    $r6, $PSW
#ifdef CFG_HWZOL
		/* Also enable ZOL (PSW.AEN) */
		xori    $r6, $r6, #((1 << 13) | (1 << 1))
#else
		addi    $r6, $r6, #-2
#endif
		mtsr    $r6, $PSW
		dsb

		/* push caller-saved fpu */
		push_fpu                            ! save fpu
	.endm

	.macro SYSCALL_RESTORE_ALL
		/* pop caller-saved fpu */
		pop_fpu                 ! restore fpu

		/* pop $IPC, $IPSW */
		popm    $r6, $r7        ! restore $ipc, $ipsw
		mtsr    $r6, $IPC
		mtsr    $r7, $IPSW

		/* pop $IFC_LP or zol($LE, $LB, $LC) with odd word align */
		pop_misc "$r6","$r7","$r8","$r9",1

		/* pop caller-saved gpr and $r6 ~ $r9, except $r0 */
		popm    $r15,$r30
		popm    $r1, $r9        ! $r0 for return value
	.endm

	/*
	 * The SAVE/RESTORE macros for non-nested interrupt.
	 * For non-nested interrupt, we only need to handle the GPRs
	 * and IFC_LP/ZOL related system registers. The automatically
	 * saving/restoring will perform on $PSW/$PC because of HW
	 * interruption stack level transition feature. They are
	 * stored in $IPSW/$IPC respectively.
	 */
	.macro IRQ_SAVE_ALL             !should not touch $r0 because it has been saved already
		/* push caller-saved gpr */
		pushm   $r1, $r5
		pushm   $r15,$r30

		/* push $IFC_LP or zol($LE, $LB, $LC) */
		push_misc

		/* push caller-saved fpu */
		push_fpu
	.endm

	.macro IRQ_RESTORE_ALL
		/* pop caller-saved fpu */
		pop_fpu

		/* pop $IFC_LP or zol($LE, $LB, $LC) */
		pop_misc

		/* pop caller-saved gpr */
		popm    $r15,$r30
		popm    $r1, $r5
		pop     $r0
	.endm

	/*
         * The SAVE/RESTORE macros for nested interrupt.
	 * Since the interrupt is nested and maybe go beyond
	 * the HW nested interruption transition level, it is
	 * necessary to do $IPC/$IPSW saving/restoring and
	 * descending interrupt level.
	 */
	.macro IRQ_NESTED_SAVE_ALL      !should not touch $r0 because it has been saved already
		/* push caller-saved gpr */
		pushm	$r1, $r5
		pushm	$r15, $r30

		/* push $IFC_LP or zol($LE, $LB, $LC) */
		push_misc

		/* push $IPC, $IPSW */
		mfsr    $r1, $IPC
		mfsr    $r2, $IPSW
		pushm   $r1, $r2

		/* You can use -1 if you want to
		 * descend interrupt level and enable gie or
		 * you can enable gie when you finish your top
		 * half isr. */
		/* Descend interrupt level */
		mfsr    $r1, $PSW
#ifdef CFG_HWZOL
		/* Also enable ZOL (PSW.AEN) */
		xori    $r1, $r1, #((1 << 13) | (1 << 1))
#else
		addi    $r1, $r1, #-2
#endif
		mtsr    $r1, $PSW

		/* push caller-saved fpu */
		push_fpu
	.endm

	.macro IRQ_NESTED_RESTORE_ALL
		/* pop caller-saved fpu*/
		pop_fpu

		/* pop $IPC, $IPSW*/
		popm    $r0, $r1
		gie_disable
		mtsr    $r0, $IPC
		mtsr    $r1, $IPSW

		/* pop $IFC_LP or zol($LE, $LB, $LC) */
		pop_misc

		/* pop caller-saved gpr */
		popm    $r15,$r30
		popm    $r1, $r5
		pop     $r0
	.endm
#endif  /* __ASSEMBLER__ */


#ifndef __ASSEMBLER__
inline void GIE_ENABLE();
inline void GIE_DISABLE();
void initIntr();


#endif

#endif //#ifndef INTERRUPT_H

