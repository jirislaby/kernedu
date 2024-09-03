#include <irq.h>

.text

.macro FAULT_HANDLER name,err
.globl \name
\name:
.if \err
	pushl $0
.endif
	pushl $do_\name
	jmp irq_handler
.endm

	FAULT_HANDLER default_fault_handler, 1
	FAULT_HANDLER div_by_zero, 1		/* 0 */
	FAULT_HANDLER nmi, 1			/* 2 */
	FAULT_HANDLER breakpoint, 1		/* 3 */
	FAULT_HANDLER overflow, 1		/* 4 */
	FAULT_HANDLER bound_exceeded, 1		/* 5 */
	FAULT_HANDLER inval_opcode, 1		/* 6 */
	FAULT_HANDLER double_fault, 0		/* 8 */
	FAULT_HANDLER segment_np, 0		/* 11 */
	FAULT_HANDLER ss_fault, 0		/* 12 */
	FAULT_HANDLER general_protection, 0	/* 13 */
	FAULT_HANDLER page_fault, 0		/* 14 */
	FAULT_HANDLER alignment_check, 0	/* 17 */
	FAULT_HANDLER machine_check, 1		/* 18 */

/*
  this generates:
  push 0
  push do_irq
  jmp irq_handler
  push 1
  push do_irq
  jmp irq_handler
  . . .
  into the code as vectors_code (unused, just naming) and
  pointer to "push 0"
  pointer to "push 1"
  . . .
  to .rodata section as a vectors array (copied to idt)
*/
.section .rodata	/* vectors are in .rodata */
.align 4
.globl vectors
vectors:

.text				/* instructions in .text */
irq=IRQ_FIRST			/* start at 32 */
vectors_code:
.rept IRQ_DESCS-IRQ_FIRST	/* repeat this 224 times */
 .align 4, 0x90			/* align with nops (0x90), just in case */
1:	pushl $irq		/* number */
	pushl $do_irq		/* handler (still the same) */
	jmp irq_handler
 .previous			/* back to .rodata */
	.long 1b		/* store the pointer */
 .text				/* and switch back to .text */
 irq=irq+1
.endr
.previous	/* to .rodata */
.previous

irq_handler:
	pushal
	pushl   %ds
	pushl   %es
	pushl   %fs
	pushl   %gs
	pushl   %ss

	movl 13*4(%esp), %ecx
	pushl %esp		/* pass it as a parameter */
	calll *%ecx

	addl $8, %esp		/* discard %esp and %ss on the stack */
	popl %gs
	popl %fs
	popl %es
	popl %ds
	popal
	addl $8, %esp		/* error/irq number and handler */
	/* %cs and %ip follow on the stack, popped and used by iret: */
	iret
