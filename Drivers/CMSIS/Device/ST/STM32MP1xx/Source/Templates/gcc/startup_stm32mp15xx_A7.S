/**
  ******************************************************************************
  * @file      startup_stm32mp15xx.s
  * @author    MCD Application Team
  * @brief     STM32MP15xx Devices startup for GCC based toolchains. 
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef NO_USE_MMU
#define USE_MMU
#endif



@ --------------------
@ Mode, correspords to bits 0-5 in CPSR

#define MODE_MSK 0x1F            @ Bit mask for mode bits in CPSR

#define USR_MODE 0x10            @ User mode
#define FIQ_MODE 0x11            @ Fast Interrupt Request mode
#define IRQ_MODE 0x12            @ Interrupt Request mode
#define SVC_MODE 0x13            @ Supervisor mode
#define ABT_MODE 0x17            @ Abort mode
#define HYP_MODE 0x1A            @ Hypervisor mode
#define UND_MODE 0x1B            @ Undefined Instruction mode
#define SYS_MODE 0x1F            @ System mode
#define MON_MODE 0x16            @ Fast Interrupt Request mode

#define IRQ_FIQ_MASK 0xC0        @ IRQ FIQ MASK

#define SCTLR_V	0x2000


/* canari value is used to check stack integrity */
#define __CANARI__ 0x80000008


/* FOR TEST purpose */
/*#define DO_UNDEF*/

	.section .text
	.code 32


	.global Reset_Handler
#ifdef A7_DUAL_CORE_SMP
	.global Reset_Handler_CPU1
#endif

/* ENTRY POINT !!! */
	b Reset_Handler

/* ----------------------
 * exception table 
 * ---------------------- */
.align 7
my_exceptions:
                b _except_reset
                b _except_undef
                b _except_svc
                b _except_prefetch_abort
                b _except_data_abort
                b _except_notused
                b _except_irq
                b _except_fiq

_undef:
	b _undef

_except_reset:
	b	Reset_Handler

_except_undef:
	push {r0-r12,lr}
	bl	HardFault_Handler
1:
    b 1b

_except_svc:
	b	_except_svc

_except_prefetch_abort:
	push {r0-r12,lr}
	bl	HardFault_Handler
1:
    b 1b


_except_data_abort:
	push {r0-r12,lr}
	bl	HardFault_Handler
1:
    b 1b

_except_notused:
    b _except_notused

_except_fiq:
	b	_except_fiq

_except_irq:
	push {r0-r12,lr}
	bl  check_stack_integrity
	bl	GIC_Irq_Handler
	bl	check_stack_integrity
	pop {r0-r12,lr}
	subs	pc, lr, #4


/* ----------------------
 * cpu0 entry point
 * ---------------------- */
Reset_Handler:
	/*
	 * disable interrupts (FIQ and IRQ), also set the cpu to SVC mode,
	 */
	mrs	    r0, cpsr
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #SVC_MODE       @ Set SVC mode bits
	orr	    r0, r0, #IRQ_FIQ_MASK   @ disable FIQ and IRQ
	msr	    cpsr,r0


	/* Set V=0 in CP15 SCTLR register to allow modification of VBAR to point to vector */
	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTLR Register
	bic	r0, #SCTLR_V			@ clear CRV bit
	mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTLR Register

	/* write VBAR */
	ldr	r0, =my_exceptions
	mcr p15, 0, r0,c12,c0,0
	isb


    /* generate undef execption */
#ifdef DO_UNDEF
	mov r0,#255
	mrc p14, 1, r0, c0, c9, 7
#endif

	/* clean and invalidate all cache */
	/* because the code may be in cache */
#ifdef USE_MMU
	mov     sp, #0
	mov	r0, #0x1		/* DCCISW */
	bl	dcsw_op_all
#endif

	/* FILL BSS with ZEROS */
	bl ZeroBss

#ifdef A7_DUAL_CORE_SMP
	bl Init_Stacks_CPU1
#endif

	/* setup a stack pointer for each mode */
	mrs     r0, cpsr                @ read Original PSR value

    /* switch to irq mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #IRQ_MODE       @ Set IRQ mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__IRQ_STACK__
	mov     sp, r1                  @ End of __IRQ_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}

    /* switch to abort mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #ABT_MODE       @ Set IRQ mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__ABT_STACK__
	mov     sp, r1                  @ End of __ABT_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}

     /* switch to svc mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #SVC_MODE       @ Set SVC mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__SVC_STACK__
	mov     sp, r1                  @ End of __SVC_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}


	/* disable instruction cache #SYSTEM WORKBENCH WORKAROUND on WINDOWS*/
	bl disable_icache
	bl disable_dcache

#ifdef USE_MMU
	bl disable_mmu
	bl invalidate_tlb
	bl SetupMMU
	/*bl enable_dcache*/
#endif


#ifndef CA7_NON_SECURE
	bl enable_vfp
#endif

	/* Call the clock system intitialization function.*/
	bl  SystemInit
	/* Call static constructors */
	bl __libc_init_array


	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	main

infitine_loop:
	b infitine_loop




/* -----------------------------------------------------------------------
 * clear the bss
 *
 * check the stacks integrity
 * {r2 - r3} are scratched.
 * -----------------------------------------------------------------------
 */
ZeroBss:
	ldr r2, =__BSS_START__
	b LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	movs r3, #0
	str  r3, [r2]
	adds r2, r2, #4

LoopFillZerobss:
	ldr r3, = __BSS_END__
	cmp r2, r3
	bcc FillZerobss
	
	bx lr


/* -----------------------------------------------------------------------
 * check_stack_integrity
 *
 * check the stacks integrity
 * {r0 - r2} are scratched.
 * -----------------------------------------------------------------------
 */
check_stack_integrity:
	ldr	    r0, =__IRQ_STACK__   @ set the adress of the top __IRQ_STACK__
	ldr		r1, [r0]             @ load content of __IRQ_STACK__
	mov	    r2, #__CANARI__      @ load the original value of the canari
	cmp     R1, R2               @ R1 should contain the value of the canari
	bne     _stack_error

	ldr	    r0, =__ABT_STACK__
	ldr		r1, [r0]
	mov	    r2, #__CANARI__
	cmp     R1, R2
	bne     _stack_error

	ldr	    r0, =__SVC_STACK__
	ldr		r1, [r0]
	mov	    r2, #__CANARI__
	cmp     R1, R2
	bne     _stack_error

#ifdef A7_DUAL_CORE_SMP
	ldr	    r0, =__IRQ_STACK1__
	ldr		r1, [r0]
	mov	    r2, #__CANARI__
	cmp     R1, R2
	bne     _stack_error

	ldr	    r0, =__ABT_STACK1__
	ldr		r1, [r0]
	mov	    r2, #__CANARI__
	cmp     R1, R2
	bne     _stack_error

	ldr	    r0, =__SVC_STACK1__
	ldr		r1, [r0]
	mov	    r2, #__CANARI__
	cmp     R1, R2
	bne     _stack_error
#endif

	bx lr



/* ----------------------
 * strack error
 * 
 * jump here when the stack integrity is compromized
 * can fall here because the IRQ STACK is too small
 * ---------------------- */
_stack_error:
	push {r0-r12,lr}
	bl	HardFault_Handler


/* ----------------------
 * enable_vfp
 * 
 * check if FPU is enabled
 * Enable the FPU in Secure state
 * ---------------------- */
enable_vfp:
	/* access NSACR */
	mrc 	p15, 0, r0, c1, c0, 2
	and 	r1, r0, #(0xF << 20)
	ldr 	r3, =(0xF << 20)
	cmp 	r1, r3
	bne 	1f
	/* check vfp is enabled */
	bx lr

1:	/* CPACR    full access on copro */
	orr 	r0, r0, #(0xF << 20)
	mcr 	p15, 0, r0, c1, c0, 2

	mov 	r3, #0x40000000
	vmsr 	FPEXC, r3
	bx lr



#ifdef A7_DUAL_CORE_SMP

/* ----------------------
 * Init_Stacks_CPU1
 * 
 * set stack pointers and initialize canaris for CPU1
 * ---------------------- */
Init_Stacks_CPU1:
	/* setup a stack pointer for each mode */
	mrs     r0, cpsr                @ read Original PSR value
	/* switch to irq mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #IRQ_MODE       @ Set IRQ mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__IRQ_STACK1__
	mov     sp, r1                  @ End of __IRQ_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}

    /* switch to abort mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #ABT_MODE       @ Set IRQ mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__ABT_STACK1__
	mov     sp, r1                  @ End of __ABT_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}

     /* switch to svc mode and set stack pointer */
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #SVC_MODE       @ Set SVC mode bits
	msr     cpsr_c, r0              @ Change the mode
	ldr	    r1, =__SVC_STACK1__
	mov     sp, r1                  @ End of __SVC_STACK__
	mov     r1, #__CANARI__
	stmia   sp, {r1}

    bx lr



/* ----------------------
 * entry point for CPU1
 * 
 * ---------------------- */
Reset_Handler_CPU1:
	/*
	 * switch to SVC MODE and disable IRQ FIQ
	 */
	mrs	    r0, cpsr
	bic     r0, r0, #MODE_MSK       @ Clear the mode bits
	orr     r0, r0, #SVC_MODE       @ Set SVC mode bits
	orr	    r0, r0, #IRQ_FIQ_MASK   @ disable FIQ and IRQ
	msr	    cpsr,r0


	/* Set V=0 in CP15 SCTLR register to allow modification of VBAR to point to vector */
	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTLR Register
	bic	r0, #SCTLR_V			@ clear CRV bit
	mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTLR Register

	/* write VBAR */
	ldr	r0, =my_exceptions
	mcr p15, 0, r0,c12,c0,0
	isb

	/* clean and invalidate all cache */
	/* because the code may be in cache */
#ifdef USE_MMU
	MOV     sp, #0
	mov	r0, #0x1		/* DCCISW */
	bl	dcsw_op_all
#endif

	bl Init_Stacks_CPU1


	/* Enable FIQ/IRQ Interrupts */
	mrs	r0, cpsr
	bic r0, r0 ,#IRQ_FIQ_MASK
	msr	cpsr,r0


	/* disable instruction cache #SYSTEM WORKBENCH WORKAROUND on WINDOWS*/
	bl disable_icache
	bl disable_dcache

#ifdef USE_MMU
	bl disable_mmu
	bl invalidate_tlb
	bl SetupMMU
	/*bl enable_dcache*/
#endif

#ifndef CA7_NON_SECURE
	bl enable_vfp
#endif

	/* ---------------------------------------------
	 * Jump to main function.
	 * ---------------------------------------------
	 */
	bl	main_cpu1

infitine_loop1:
	b infitine_loop1



#endif /* A7_DUAL_CORE_SMP */
