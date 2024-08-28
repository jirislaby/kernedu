#include <io.h>
#include <irq.h>
#include <output.h>
#include <setup.h>

#define IDESCS		256

struct idt_desc {
	unsigned int a;
	unsigned int b;
} __attribute__((packed)) idesc[IDESCS] __attribute__((aligned(8))) = { };

struct idt {
	unsigned short limit;
	unsigned long base;
} __attribute__((packed)) idt = { sizeof(idesc) - 1, pa(idesc) };

static void default_handler(unsigned int irq);

static void (*irq_handlers[IDESCS - 0x20])(unsigned int) = {
	[ 0 ... IDESCS - 1 - 0x20 ] = default_handler
};

static void show_regs(struct pt_regs *pt)
{
	print_color("EIP=", 0x0c);
	print_num_color(pt->ip, 0x0c);
	print_color("\n", 0x0c);
}

extern void div_by_zero(void);
void do_div_by_zero(struct pt_regs *pt)
{
	print_color("DIVISION BY ZERO: ", 0x0c);
	show_regs(pt);
	halt();
}

extern void nmi(void);
void do_nmi(struct pt_regs *pt)
{
	print_color("NMI: ", 0x0c);
	show_regs(pt);
}

extern void overflow(void);
void do_overflow(struct pt_regs *pt)
{
	print_color("OVERFLOW: ", 0x0c);
	show_regs(pt);
}

extern void bound_exceeded(void);
void do_bound_exceeded(struct pt_regs *pt)
{
	print_color("BOUND EXCEEDED: ", 0x0c);
	show_regs(pt);
	halt();
}

extern void breakpoint(void);
void do_breakpoint(struct pt_regs *pt)
{
	print_color("BREAKPOINT: ", 0x0c);
	show_regs(pt);
}

extern void inval_opcode(void);
void do_inval_opcode(struct pt_regs *pt)
{
	print_color("INVAL OPCODE: ", 0x0c);
	show_regs(pt);
	halt();
}

extern void double_fault(void);
void do_double_fault(struct pt_regs *pt)
{
	print_color("DOUBLE FAULT\n", 0x0c);
	halt();
}

extern void segment_np(void);
void do_segment_np(struct pt_regs *pt)
{
	print_color("SEGMENT NOT PRESENT: error=", 0x0c);
	print_num_color(pt->number, 0x0c);
	show_regs(pt);
	halt();
}

extern void ss_fault(void);
void do_ss_fault(struct pt_regs *pt)
{
	print_color("STACK SEGMENT FAULT: error=", 0x0c);
	print_num_color(pt->number, 0x0c);
	show_regs(pt);
	halt();
}

extern void general_protection(void);
void do_general_protection(struct pt_regs *pt)
{
	print_color("GENERAL PROTECTION: error=", 0x0c);
	print_num_color(pt->number, 0x0c);
	show_regs(pt);
	halt();
}

extern void page_fault(void);
void do_page_fault(struct pt_regs *pt)
{
	unsigned long cr2;

	asm volatile("movl %%cr2, %0" : "=r" (cr2));

	print_color("PAGE FAULT: error=", 0x0c);
	print_num_color(pt->number, 0x0c);
	print_color("CR2=", 0x0c);
	print_num_color(cr2, 0x0c);
	show_regs(pt);
	halt();
}

extern void alignment_check(void);
void do_alignment_check(struct pt_regs *pt)
{
	print_color("ALIGNMENT CHECK: error=", 0x0c);
	print_num_color(pt->number, 0x0c);
	show_regs(pt);
	halt();
}

extern void machine_check(void);
void do_machine_check(struct pt_regs *pt)
{
	print_color("MACHINE CHECK: ", 0x0c);
	show_regs(pt);
	halt();
}

extern void default_fault_handler(void);
void do_default_fault_handler(struct pt_regs *pt)
{
	print_color("UNHANDLED FAULT: ", 0x0c);
	show_regs(pt);
	halt();
}

void do_irq(struct pt_regs *pt)
{
	irq_handlers[pt->number](pt->number);
}

static void default_handler(unsigned int irq)
{
	print_color("UNHANDLED INTERRUPT: ", 0x0c);
	print_num_color(irq, 0x0c);
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
	extern const struct handler __handlers[];
	extern const unsigned long vectors[];
	const struct handler *h;
	unsigned int a;

	for (a = 0; a < 0x20; a++)
		set_intr(a, default_fault_handler);

	set_intr(0, div_by_zero);
	set_intr(2, nmi);
	set_intr(3, breakpoint);
	set_intr(4, overflow);
	set_intr(5, bound_exceeded);
	set_intr(6, inval_opcode);
	set_intr(8, double_fault);
	set_intr(11, segment_np);
	set_intr(12, ss_fault);
	set_intr(13, general_protection);
	set_intr(14, page_fault);
	set_intr(17, alignment_check);
	set_intr(18, machine_check);

	/*
	 * see irq_entry.s for vectors[] generation
	 * it's an array of pointers to
	 * push irq_number
	 * jmp irq_handler
	 * for each irq 0x20..0xff
	 */
	for (a = 0x20; a < IDESCS; a++)
		set_intr(a, (void (*)(void))vectors[a - 0x20]);

	for (h = __handlers; h->handler; h++)
		irq_handlers[h->irq] = h->handler;
}

void init_irq(void)
{
	setup_handlers();
	asm volatile("lidt %0" : : "m" (idt));
}
