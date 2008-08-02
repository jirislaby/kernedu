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

	lgdtl gdtr_contents
	movl %cr0, %eax
	orl $1, %eax
	movl %eax, %cr0
	
	ljmpl $8, $protected
protected:
.code32
	movw $16, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	xorw %ax, %ax
	movw %ax, %fs
	movw %ax, %gs

	pushl $hello_text
	calll print
	popl %eax

	calll main		/* a C function */
loop:
	hlt
	jmp loop

.globl print
print:
	pushl %esi
	pushl %ebx
	movl 8(%esp), %esi
	movb $0x0e, %ah
	movw $0x0007, %bx
0:
	lodsb
	testb %al, %al
	jz 0f
	int $0x10		/* call BIOS to show this char */
	jmp 0b
0:
	popl %ebx
	popl %esi
	ret

.section .data.loader
hello_text:
	.asciz "Hello world from assembly\n\r"
gdtr_contents:
	.short 8 * 3 - 1 /* size * (null + code + data) - 1 */
	.long gdt + 0x7c00
	.align 8
gdt:
	.quad 0 /* null */
	.quad 0x00cf9a007c00ffff /* code */
	.quad 0x00cf92007c00ffff /* data */
