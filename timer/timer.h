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
