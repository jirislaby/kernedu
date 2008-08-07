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

	FAULT_HANDLER div_by_zero, 1		/* 0 */
	FAULT_HANDLER nmi, 1			/* 2 */
	FAULT_HANDLER breakpoint, 1		/* 3 */
	FAULT_HANDLER overflow, 1		/* 4 */
	FAULT_HANDLER bound_exceeded, 1		/* 5 */
	FAULT_HANDLER inval_opcode, 1		/* 6 */
	FAULT_HANDLER double_fault, 0		/* 8 */
	FAULT_HANDLER segment_np, 0		/* 11 */
	FAULT_HANDLER general_protection, 0	/* 13 */

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
