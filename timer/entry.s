.section .text
.code16

.globl halt
halt:
cli
1:
hlt
jmp 1b

/*
  this generates:
  push 0
  jmp irq_handler
  push 1
  jmp irq_handler
  . . .
  into the code as vectors_code (unused, just naming) and
  pointer to "push 0"
  pointer to "push 1"
  . . .
  to rodata section as a vectors array (copied to idt)
*/
.section .rodata
.align 4
vectors_code:
.globl vectors
vectors:
.text
irq=0
.rept 256
 .align 4, 0x90	# align with nops (0x90)
1:	pushw $irq
	jmp irq_handler
 .previous
	.short 1b
 .text
irq=irq+1
.endr
.previous
.previous

irq_handler:
pushal
pushl   %ds
pushl   %es
pushl   %fs
pushl   %gs
pushl   %ss

movw 13*4(%esp), %ax	# the pushed irq number from the generated code above
pushw %ax		# pass it as a parameter
calll do_irq
popw %ax

addl $4, %esp	# discard %ss on the stack
popl %gs
popl %fs
popl %es
popl %ds
popal
addl $2, %esp	# finally the pushed irq number on the very bottom
# %cs and %ip follow on the stack, popped and used by iret:
iret

.section .data
