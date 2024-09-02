#include <io.h>
#include <irq.h>
#include <output.h>

#define APIC_ID			0x020
#define APIC_VER		0x030
#define APIC_EOI		0x0b0
#define APIC_SPUR		0x0f0
#define APIC_ISR(x)		(0x100 + (x) * 0x10)
#define APIC_IRR(x)		(0x200 + (x) * 0x10)
#define APIC_ICR0		0x300
#define APIC_ICR1		0x310
#define APIC_LVT_TIMER		0x320
#define APIC_TIMER_INIT_CNT	0x380
#define APIC_TIMER_CUR_CNT	0x390
#define APIC_TIMER_DIVIDE	0x3e0

#define IA32_APIC_BASE	0x1b

static inline u32 read_apic(unsigned int reg)
{
	volatile u32 *apic = va(0xfee00000 + reg);

	return *apic;
}

static inline void write_apic(unsigned int reg, u32 val)
{
	volatile u32 *apic = va(0xfee00000 + reg);

	*apic = val;
}

#define IOAPIC_IND	0xfec00000
#define IOAPIC_DAT	0xfec00010
#define IOAPIC_EOI	0xfec00040

static inline u32 read_ioapic(unsigned int reg)
{
	volatile u32 *ind = va(IOAPIC_IND);
	volatile u32 *dat = va(IOAPIC_DAT);

	*ind = reg;

	return *dat;
}

static inline void write_ioapic(unsigned int reg, u32 val)
{
	volatile u32 *ind = va(IOAPIC_IND);
	volatile u32 *dat = va(IOAPIC_DAT);

	*ind = reg;
	*dat = val;
}

static void pic_eoi()
{
	outb(0x20, 0x20);
}

static void apic_eoi()
{
	write_apic(APIC_EOI, 0);
}

static void (*eoi)() = pic_eoi;

static void ioapic_eoi(unsigned int vector)
{
	write_ioapic(IOAPIC_EOI, vector);
}

#define RTC_IND		0x70
#define RTC_DAT		0x71
#define RTC_SEC		0x00
#define RTC_MIN		0x02
#define RTC_HR		0x04

static inline unsigned char rtc_decode(unsigned char rtc_val)
{
	return ((rtc_val >> 4) & 0x7) * 10 + (rtc_val & 0xf);
}

static inline unsigned char rtc_read(unsigned int reg)
{
	outb(reg, RTC_IND);

	return inb(RTC_DAT);
}

static inline unsigned char rtc_read_decode(unsigned int reg)
{
	return rtc_decode(rtc_read(reg));
}

static void ack_int(unsigned int vector)
{
	if (eoi == apic_eoi)
		ioapic_eoi(vector);
	eoi();
}

#define INT_TIMER_PIT	0x20
static void timer_pit(unsigned int vector)
{
	static unsigned int a;
	static short last_sec = -1;

	if (last_sec < 0)
		last_sec = rtc_read_decode(RTC_SEC);

#define TICK	2000
	if (!(a++ % TICK)) {
		print_num(a);
		unsigned char sec = rtc_read_decode(RTC_SEC);
		unsigned char diff = (sec + 60 - last_sec) % 60;
		print_num(diff);
		last_sec = sec;
		print("-- TIMER PIT: INTERRUPTS SECONDS_PASSED\n");
	}
	ack_int(vector);
}
IRQ_HANDLER(timer_pit, INT_TIMER_PIT);

#define INT_KBD		0x21
static void kbd_handler(unsigned int vector)
{
	print_num(vector);
	unsigned char key = inb(0x60);
	print_num(key);
	print("-- KBD handled\n");
	ioapic_eoi(vector);
	eoi();
	if (key == 0x01)
		outb(0xfe, 0x64);       /* reboot */
}
IRQ_HANDLER(kbd_handler, INT_KBD);

#define INT_TIMER_APIC	0x30
static void timer_apic(unsigned int vector)
{
	print("TIMER APIC\n");
	ack_int(vector);
}
IRQ_HANDLER(timer_apic, INT_TIMER_APIC);

#define INT_IPI0	0xe0
#define INT_IPI1	0xe1
static void ipi_handler(unsigned int vector)
{
	print_num(vector);
	print("-- handled\n");
	eoi();
}
IRQ_HANDLER(ipi_handler, INT_IPI0);
IRQ_HANDLER(ipi_handler, INT_IPI1);

static void disable_pic(_Bool completely)
{
	unsigned char master = completely ? 0xff : 0xfb;

	/* disable ints */
	outb(master, 0x21); /* except cascade */
	outb(0xff, 0xa1);
}

static void init_pic(void)
{
	print("PIC\n");
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

	disable_pic(0);
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
	outb(TIMER_CTL_WORD(0b00, 0b11, 0b011, 0b0), 0x43);
	outb(intval & 0xff, 0x40);
	outb(intval >> 8, 0x40);
}

static void init_apic(void)
{
	print("APIC\n");
	print("    BASE       ID      VER     SPUR     ICR1     ICR0\n");
	print_num(rdmsr(IA32_APIC_BASE));
	print_num(read_apic(APIC_ID));
	print_num(read_apic(APIC_VER));
	print_num(read_apic(APIC_SPUR));
	print_num(read_apic(APIC_ICR1));
	print_num(read_apic(APIC_ICR0));
	print("\n");

	write_apic(APIC_ICR0, BIT(18) | INT_IPI0);
	write_apic(APIC_ICR0, BIT(18) | INT_IPI1);
	print("    ICR0      IRR      ISR\n");
	print_num(read_apic(APIC_ICR0));
	print_num(read_apic(APIC_IRR(1)));
	print_num(read_apic(APIC_ISR(1)));
	print("\n");

#define IOAPIC_ID		0x00
#define IOAPIC_VER		0x01
#define IOAPIC_REDIR0(x)	(0x10 + (x) * 2)
#define IOAPIC_REDIR1(x)	(0x11 + (x) * 2)
	print("I/O APIC\n");
	print("      ID      VER   REDIRS\n");
	print_num(read_ioapic(IOAPIC_ID));
	unsigned ver = read_ioapic(IOAPIC_VER);
	print_num(ver & 0xff);
	unsigned redirs = (ver >> 16) & 0xff;
	print_num(redirs);
	print("\n");
	write_ioapic(IOAPIC_REDIR0(1), INT_KBD);
	write_ioapic(IOAPIC_REDIR0(2), INT_TIMER_PIT);
#ifdef DUMP_REDIRS
	for (unsigned a = 0; a <= redirs; a++) {
		if (a && !(a % 2))
			print("\n");
		print_num(a);
		print_num(read_ioapic(IOAPIC_REDIR0(a)));
		print_num(read_ioapic(IOAPIC_REDIR1(a)));
	}
	print("\n");
#endif
}

void main(void)
{
	clear_screen();

	print_color("Timer example\n", 0x09);

	init_irq();

	init_pic();
	sti();
	pic_set(0, 1);
	pic_set(1, 1);
#define PIT_FREQ	1193182U
	/* 1000 interrupts / s */
	timer_enable(PIT_FREQ / 1000);

	/* wait for 7 s (7000 interrupts) */
	for (unsigned a = 0; a < 7 * 1000; a++)
		hlt();

	cli();
	disable_pic(1);
	eoi = apic_eoi;
	init_apic();

	sti();
	print_num(read_apic(APIC_IRR(1)));
	print_num(read_apic(APIC_ISR(1)));
	print("\n");

	/* 0b01 == periodic */
	write_apic(APIC_LVT_TIMER, (0b01 << 17) | INT_TIMER_APIC);
	write_apic(APIC_TIMER_DIVIDE, 0b1000); /* divide by 128 */
	write_apic(APIC_TIMER_INIT_CNT, 0x0a000000);

	while (1)
		hlt();
}
