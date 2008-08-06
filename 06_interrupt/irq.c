#include <io.h>
#include <output.h>
#include <setup.h>

#include "irq.h"

struct idt_desc {
	unsigned int a;
	unsigned int b;
} __attribute__((packed)) idesc[20] __attribute__((aligned(8))) = { };

struct idt {
	unsigned short limit;
	unsigned long base;
} __attribute__((packed)) idt = { 20*8 - 1, pa(idesc) };

extern void div_by_zero(void);
void do_div_by_zero(struct pt_regs *pt)
{
	print("DIVISION BY ZERO at\n");
	print_num(pt->ip);
	halt();
}

static void do_breakpoint(void)
{
	print("BP");
	halt();
}

static void do_inval_opcode(void)
{
	print("INVAL OPCODE");
	halt();
}

static void do_general_protection(void)
{
	print("GP");
	halt();
}

static inline void set_intr(unsigned int n, void (*fn)(void))
{
	unsigned long f = (unsigned long)fn;

	/* code segment is 0x0008 */
	idesc[n].a = 0x00080000		| (f & 0xffff);
	/* 0x8000 - enable bit, 0x0e00 - interrupt descriptor type */
	idesc[n].b = (f & 0xffff0000)	| 0x8e00;
}

static void setup_handlers(void)
{
	set_intr(0, div_by_zero);
	set_intr(3, do_breakpoint);
	set_intr(6, do_inval_opcode);
	set_intr(13, do_general_protection);
}

void init_irq(void)
{
	setup_handlers();
	asm volatile("lidt %0" : : "m" (idt));
}
