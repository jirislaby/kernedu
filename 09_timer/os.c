#include <io.h>
#include <irq.h>
#include <output.h>

static void pic_set(unsigned int irq, unsigned char state);

static void timer(unsigned int irq)
{
	static unsigned int a;

	a++;

	print_num(a);
	print("TIMER\n");
	outb(0x20, 0x20);
}
IRQ_HANDLER(timer, 32);

static void init_pic(void)
{
	/* ICW1 */
#define ICW1		(1 << 4)
#define ICW1_DO_ICW4	(1 << 0)
	outb(ICW1 | ICW1_DO_ICW4, 0x20); /* master */
	outb(ICW1 | ICW1_DO_ICW4, 0xa0); /* slave */
	/* ICW2 */
	outb(0x20, 0x21); /* master IRQ base */
	outb(0x28, 0xa1); /* slave IRQ base */
	/* ICW3 */
	outb(1 << 2, 0x21); /* slave is on IRQ2 */
	outb(0x02, 0xa1);  /* slave ID */
	/* ICW4 */
	outb(0x01, 0x21); /* "intel" mode */
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
	clear_screen();

	print_color("Timer example\n", 0x09);

	init_irq();

	init_pic();
	sti();
	timer_enable(0xff00);
	pic_set(0, 1);

	while (1)
		hlt();
}
