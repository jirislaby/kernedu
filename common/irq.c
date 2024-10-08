#include <io.h>
#include <irq.h>
#include <output.h>
#include <setup.h>

struct idt_desc {
	unsigned int a;
	unsigned int b;
} __attribute__((packed)) idesc[IRQ_DESCS] __attribute__((aligned(8))) = { };

struct idt {
	unsigned short limit;
	unsigned long base;
} __attribute__((packed)) idt = { sizeof(idesc) - 1, pa(idesc) };

static void default_handler(unsigned int irq);

static void (*irq_handlers[IRQ_DESCS - IRQ_FIRST])(unsigned int) = {
	[ 0 ... IRQ_DESCS - 1 - IRQ_FIRST ] = default_handler
};

enum {
	SHOW_ERROR = BIT(1),
	SHOW_NOSTACK = BIT(2),
};

static void show_regs(struct pt_regs *pt, unsigned int show)
{
	unsigned long sp = get_sp();

	print_color("EIP=", 0x0c);
	print_num_color(pt->ip, 0x0c);
	print_color("CR2=", 0x0c);
	print_num_color(get_cr2(), 0x0c);
	print_color("SP=", 0x0c);
	print_num_color(sp, 0x0c);
	if (show & SHOW_ERROR) {
		print_color("error=", 0x0c);
		print_num_color(pt->number, 0x0c);
	}
	print_color("\n", 0x0c);

	if (show & SHOW_NOSTACK)
		return;

	unsigned int *ptr = (unsigned int *)sp;
	for (unsigned a = 0; a < 32; a++) {
		if (a && !(a % 4))
			print("\n");
		print_num((unsigned long)&ptr[a]);
		print_num(ptr[a]);
	}
}

extern void div_by_zero(void);
void do_div_by_zero(struct pt_regs *pt)
{
	print_color("DIVISION BY ZERO: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

extern void nmi(void);
void do_nmi(struct pt_regs *pt)
{
	print_color("NMI: ", 0x0c);
	show_regs(pt, 0);
}

extern void overflow(void);
void do_overflow(struct pt_regs *pt)
{
	print_color("OVERFLOW: ", 0x0c);
	show_regs(pt, SHOW_NOSTACK);
}

extern void bound_exceeded(void);
void do_bound_exceeded(struct pt_regs *pt)
{
	print_color("BOUND EXCEEDED: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

extern void breakpoint(void);
void do_breakpoint(struct pt_regs *pt)
{
	print_color("BREAKPOINT: ", 0x0c);
	show_regs(pt, SHOW_NOSTACK);
}

extern void inval_opcode(void);
void do_inval_opcode(struct pt_regs *pt)
{
	print_color("INVAL OPCODE: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

extern void double_fault(void);
void do_double_fault(struct pt_regs *pt)
{
	print_color("DOUBLE FAULT: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

extern void segment_np(void);
void do_segment_np(struct pt_regs *pt)
{
	print_color("SEGMENT NOT PRESENT: ", 0x0c);
	show_regs(pt, SHOW_ERROR);
	halt();
}

extern void ss_fault(void);
void do_ss_fault(struct pt_regs *pt)
{
	print_color("STACK SEGMENT FAULT: ", 0x0c);
	show_regs(pt, SHOW_ERROR);
	halt();
}

extern void general_protection(void);
void do_general_protection(struct pt_regs *pt)
{
	print_color("GENERAL PROTECTION: ", 0x0c);
	show_regs(pt, SHOW_ERROR);
	halt();
}

extern void page_fault(void);
void do_page_fault(struct pt_regs *pt)
{
	print_color("PAGE FAULT: ", 0x0c);
	show_regs(pt, SHOW_ERROR);
	halt();
}

extern void alignment_check(void);
void do_alignment_check(struct pt_regs *pt)
{
	print_color("ALIGNMENT CHECK: ", 0x0c);
	show_regs(pt, SHOW_ERROR);
	halt();
}

extern void machine_check(void);
void do_machine_check(struct pt_regs *pt)
{
	print_color("MACHINE CHECK: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

extern void default_fault_handler(void);
void do_default_fault_handler(struct pt_regs *pt)
{
	print_color("UNHANDLED FAULT: ", 0x0c);
	show_regs(pt, 0);
	halt();
}

void do_irq(struct pt_regs *pt)
{
	unsigned int off = pt->number - IRQ_FIRST;

	if (off >= ARRAY_SIZE(irq_handlers)) {
		print(__func__);
		print(": IRQ out of bounds: ");
		print_num(pt->number);
		print("\n");
		return;
	}

	irq_handlers[off](pt->number);
}

static void default_handler(unsigned int irq)
{
	print_color("UNHANDLED INTERRUPT: ", 0x0c);
	print_num_color(irq, 0x0c);
	halt();
}

static inline void set_intr(_Bool trap, unsigned int n, void (*fn)(void))
{
#define IDT_PRESENT		(1 << 15)
#define IDT_32BIT		(1 << 11)
#define IDT_INTERRUPT_GATE	0x0600
#define IDT_TRAP_GATE		0x0700
	unsigned long f = (unsigned long)fn;
	unsigned int gate = trap ? IDT_TRAP_GATE : IDT_INTERRUPT_GATE;

	/* code segment is 0x0008 */
	idesc[n].a = (0x0008 << 16) | (f & 0xffff);
	idesc[n].b = (f & 0xffff0000) | IDT_PRESENT | IDT_32BIT | gate;
}

static void setup_handlers(void)
{
	extern const struct handler __handlers[];
	extern const unsigned long vectors[];
	const struct handler *h;
	unsigned int a;

	for (a = 0; a < IRQ_FIRST; a++)
		set_intr(1, a, default_fault_handler);

	set_intr(1, 0, div_by_zero);
	set_intr(1, 2, nmi);
	set_intr(1, 3, breakpoint);
	set_intr(1, 4, overflow);
	set_intr(1, 5, bound_exceeded);
	set_intr(1, 6, inval_opcode);
	set_intr(1, 8, double_fault);
	set_intr(1, 11, segment_np);
	set_intr(1, 12, ss_fault);
	set_intr(1, 13, general_protection);
	set_intr(1, 14, page_fault);
	set_intr(1, 17, alignment_check);
	set_intr(1, 18, machine_check);

	/*
	 * see irq_entry.s for vectors[] generation
	 * it's an array of pointers to
	 * push irq_number
	 * jmp irq_handler
	 * for each irq 0x20..0xff
	 */
	for (a = IRQ_FIRST; a < IRQ_DESCS; a++)
		set_intr(0, a, (void (*)(void))vectors[a - IRQ_FIRST]);

	for (h = __handlers; h->handler; h++) {
		unsigned int off = h->irq - IRQ_FIRST;
		if (off >= ARRAY_SIZE(irq_handlers)) {
			print(__func__);
			print(": IRQ handler out of bounds: ");
			print_num(h->irq);
			print_num((unsigned long)h->handler);
			print("\n");
			continue;
		}
		irq_handlers[off] = h->handler;
	}
}

void init_irq(void)
{
	setup_handlers();
	asm volatile("lidt %0" : : "m" (idt));
}
