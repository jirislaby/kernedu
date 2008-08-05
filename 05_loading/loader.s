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

	call update_state

	/* read next sector(s) from the disk */
	movw $__os_end, %ax
	subw $__os_start, %ax	# code length
	addw $511, %ax		# rounding up to 512 boundary
	shrw $9, %ax		# / 512 (bytes/sector)
	pushw %ax		# count ----------+
	movb $0x02, %ah		# read op	  |
	movw $__os_start, %bx	# dest		  |
	movw $0x0002, %cx	# cyl[10]:sec[6]  |
	movw $0x0080, %dx	# head:drive	  |
	int $0x13		# BIOS read	  |
	popw %bx		# stored count <--+
	xorb %bh, %bh
	cmpw %bx, %ax		# ret:read
	jne loop

	call update_state

	cmpl $0xdedaeda0, __os_start_magic
	jne loop

	call update_state

	cmpl $0xdedaedaf, __os_end_magic
	jne loop

	call update_state

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
	movl $(10*1024*1024), %esp	/* stack at 10M */

	xorw %ax, %ax		/* null segment (0*8) */
	movw %ax, %fs
	movw %ax, %gs

	/* no suprises! main would preload some values, we need to do it here */
	call clear_bss
	call main

	/* just in case ... */
	call halt
.code16

loop:
	hlt
	jmp loop

# print state, starting with 'A'
update_state:
movb $0x0e, %ah
movb state, %al
movw $0x0007, %bx
int $0x10
incb state
ret

.section .data.loader
state:
	.byte 'A'
gdtr_contents:
	.short 8 * 3 - 1 /* size * (null + code + data) - 1 */
	.long gdt + 0x7c00
	.align 8
gdt:
	.quad 0 /* null */
	.quad 0x00cf9a007c00ffff /* code */
	.quad 0x00cf92007c00ffff /* data */
