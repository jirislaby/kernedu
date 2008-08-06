.section .text

.globl div_by_zero
div_by_zero:
	pushl $0
	pushl $do_div_by_zero
	jmp irq_handler

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

.section .data
