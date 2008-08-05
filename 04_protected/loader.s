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

	/* get y cursor position from the BIOS and put it into cursor_line */
	movb $0x03, %ah
	xorw %bx, %bx
	int $0x10
	movb %dh, cursor_line

	/* set cursor position to [0,0] */
	movb $0x02, %ah
	xorb %bh, %bh
	xorw %dx, %dx
	int $0x10

	/* protected mode */
	lgdtl gdtr_contents
	movl %cr0, %eax
	orl $1, %eax		/* 1 is protected bit */
	movl %eax, %cr0
	
	ljmpl $8, $0f		/* code segment (1*8), update IP */
0:
.code32
	movw $16, %ax		/* data segment (2*8) */
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movl $(2*1024*1024), %esp	/* stack at 2M */

	xorw %ax, %ax		/* null segment (0*8) */
	movw %ax, %fs
	movw %ax, %gs

	calll main		/* a C function */
loop:
	hlt
	jmp loop

.code16
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
hello_text:
	.asciz "Hello world from assembly\n\r"
.globl cursor_line
cursor_line:
	.short 0
gdtr_contents:
	.short 8 * 3 - 1 /* size * (null + code + data) - 1 */
	.long gdt + 0x7c00
	.align 8
gdt:
	.quad 0 /* null */
	.quad 0x00cf9a007c00ffff /* code */
	.quad 0x00cf92007c00ffff /* data */
