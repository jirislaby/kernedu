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
	callw print
	popw %ax

	callw get_cursor_line

	lgdtl gdtr_contents
	movl %cr0, %eax
	orl $1, %eax
	movl %eax, %cr0
	
	ljmpl $8, $protected	/* code segment (1*8) */
protected:
.code32
	movw $16, %ax		/* data segment (2*8) */
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movl $4096, %esp

	xorw %ax, %ax		/* null segment (0*8)*/
	movw %ax, %fs
	movw %ax, %gs

	calll main		/* a C function */
loop:
	hlt
	jmp loop

.code16
print:
	movw 2(%esp), %si
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

get_cursor_line:
	movb $0x03, %ah
	xorw %bx, %bx
	int $0x10
	movb %dh, cursor_line
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
