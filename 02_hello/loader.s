.section .text.loader
.code16

.globl startup
startup:
.byte 0xea
.word start, 0x7c0

start:
	/* turn off irqs and set forward direction */
	cli
	cld

	/* init segments */
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	xorl %esp, %esp

	movw $hello, %si
	callw print

	std				/* change the direction flag */
	movw $hello_end - 1, %si	/* point to the 'd' from the 'world' */
	callw print

loop:
	hlt
	jmp loop

print:
	movb $0x0e, %ah
	movw $0x0007, %bx
0:
	lodsb
	testb %al, %al
	jz 0f
	int $0x10		/* call BIOS to show this char */
	jmp 0b
0:
	ret

.section .data.loader
	.byte 0 /* for reverse */
hello:
	.ascii "Hello world"
hello_end:
	.byte 0 /* for forward */
