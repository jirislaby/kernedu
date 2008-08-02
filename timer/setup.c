#include <code16.h>

#include <io.h>
#include <output.h>

#include "timer.h"

static void default_irq_handler(int irq)
{
	print_num(irq);
	print("DEFAULT HANDLER\n");
	halt();
}

static void (*irq_handlers[256])(int);

void do_irq(unsigned char irq)
{
	irq_handlers[irq](irq);
}

static void setup_handlers(void)
{
	extern struct handler __handlers[];
	extern unsigned short vectors[];
	struct handler *h;
	unsigned int a;
	unsigned short oe;

	for (h = __handlers; h->handler; h++)
		irq_handlers[h->irq] = h->handler;

	oe = set_es(0);
	for (a = 0; a < 256; a++) {
		unsigned int x;
		if (!irq_handlers[a])
			irq_handlers[a] = default_irq_handler;
		asm volatile("stosw\n"
			"mov %%cs, %%ax\n"
			"stosw" : "=D" (x), "=a" (x) :
			"a" (vectors[a]), "D" (a * 4));
	}
	set_es(oe);
}

void init(void)
{
	setup_handlers();
	main();
}
