extern void main(void);
extern void halt(void);

struct handler {
	void (*handler)(int);
	int irq;
};

#define IRQ_HANDLER(func, irqn) \
	static const struct handler __irq_handler_ ## func ## irqn \
		__attribute__((section(".rodata.handlers"), used)) = { \
		.handler = func, \
		.irq = irqn, \
	}

static inline unsigned short set_es(unsigned short es)
{
	unsigned short old_es;
	asm volatile("pushw %%es\n"
		"movw %%ax, %%es\n"
		"popw %%ax" : "=a" (old_es) : "a" (es));
	return old_es;
}

static inline void outb(unsigned char val, unsigned short p)
{
	asm volatile("outb %0,%1" : : "a" (val), "dN" (p));
}
static inline unsigned char inb(unsigned short p)
{
	unsigned char val;
	asm volatile("inb %1,%0" : "=a" (val) : "dN" (p));
	return val;
}

static inline void sti(void)
{
	asm volatile("sti" : : : "memory");
}

static inline void cli(void)
{
	asm volatile("cli" : : : "memory");
}
