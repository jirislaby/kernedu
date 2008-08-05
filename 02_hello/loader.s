.section .text.loader
.code16

.globl startup
startup:
	ljmp $0x7c0, $0f
0:
	/* turn off irqs and set forward direction */
	cli
	cld

	/* init segments */
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	xorl %esp, %esp

	pushw $hello_text
	calll print
	addl $2, %esp

	std			/* change the direction flag */
	pushw $(hello_end - 1)	/* point to the 'd' from the 'world' */
	calll print
	addl $2, %esp

loop:
	hlt
	jmp loop

print:
	pushw %si
	pushw %bx
	movw 8(%esp), %si
	movb $0x0e, %ah
	movw $0x0007, %bx
0:
	lodsb
	testb %al, %al
	jz 0f
	int $0x10		/* call BIOS to show this char */
	jmp 0b
0:
	popw %bx
	popw %si
	ret

.section .data.loader
	.byte 0 /* for reverse */
hello_text:
	.ascii "Hello world from assembly"
hello_end:
	.asciz "\n\r"
