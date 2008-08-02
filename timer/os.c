#include <code16.h>

#include <io.h>
#include <output.h>

#include "timer.h"

static void handler(int irq)
{
	print_num(irq);
	print("HANDLER\n");
	halt();
}
IRQ_HANDLER(handler, 0); /* div by zero */
IRQ_HANDLER(handler, 3); /* breakpoint */
IRQ_HANDLER(handler, 6); /* invalid opcode */

static void pic_set(unsigned int irq, unsigned char state);

static void cause_irq(int irq)
{
	switch (irq) {
	case 0: {
		unsigned char a = 1, b = 0;
		a /= b;
		asm volatile("" : : "g" (a)); /* mark it used */
		break;
	}
	case 3:
		asm volatile("int3");
		break;
	case 6:
		asm volatile("ud2");
		break;
/*	case 14:
		asm volatile("pushw %%es\npushw %%ax\npushw %%di\n"
			"movw 0x1234, %%ax\n"
			"movw %%ax, %%es\n"
			"xorw %%di, %%di\n"
			"stosw\n"
			"popw %%di\npopw %%ax\npopw %%es" : :);
		break;*/
	default:
		asm volatile("int %0" : : "N" (irq));
		break;
	}
}
static void timer(int irq)
{
	static unsigned int a;

	a++;

	if (a == 20)
		cause_irq(6);

	print_num(a);
	print("TIMER\n");
	outb(0x20, 0x20);
}
IRQ_HANDLER(timer, 32);

static void init_pic(void)
{
	/* ICW1 */
	outb(0x11, 0x20); /* master */
	outb(0x11, 0xa0); /* slave */
	/* ICW2 */
	outb(0x20, 0x21);
	outb(0x28, 0xa1);
	/* ICW3 */
	outb(0x04, 0x21);
	outb(0x02, 0xa1);
	/* ICW4 */
	outb(0x01, 0x21);
	outb(0x01, 0xa1);
	/* disable ints */
	outb(0xfb, 0x21); /* except cascade */
	outb(0xff, 0xa1);
}

static void pic_set(unsigned int irq, unsigned char state)
{
	unsigned char old;
	unsigned short base = 0x21;

	if (irq >= 8) {
		irq -= 8;
		base = 0xa1;
	}

	old = inb(base);
	if (state)
		old &= ~(1 << irq);
	else
		old |= 1 << irq;
	outb(old, base);
}

static void timer_enable(unsigned short intval)
{
#define TIMER_CTL_WORD(timer, rw, mode, format) \
	(((timer) << 6) | \
	 ((rw) << 4) | \
	 ((mode) << 1) | \
	 (format))
	outb(TIMER_CTL_WORD(0, 0x3, 0x3, 0), 0x43);
	outb(intval & 0xff, 0x43);
	outb(intval >> 8, 0x43);
}

void main(void)
{
	extern char _text[];

	clear_screen();

	print("Running from 0x");
	print_num((unsigned long)_text);
	print("\n");

	init_pic();
	sti();
	timer_enable(0xff00);
	pic_set(0, 1);

	while (1)
		asm volatile("hlt");
}
