.section .text.loader
.code16

.globl startup
startup:
.byte 0xea
.word start, 0x7c0

start:
cli
cld

movw %cs, %ax
movw %ax, %ds
movw %ax, %es
movw %ax, %ss

xorl %esp, %esp

call update_state

# read next sector(s) from the disk
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

calll clear_bss # init would preload some values, we need to do it here
call update_state
calll init

call update_state

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
