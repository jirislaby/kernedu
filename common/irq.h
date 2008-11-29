#ifndef __IRQ_H_FILE
#define __IRQ_H_FILE

struct pt_regs {
	unsigned long ss;
	unsigned long gs;
	unsigned long fs;
	unsigned long es;
	unsigned long ds;
	unsigned long di;
	unsigned long si;
	unsigned long bp;
	unsigned long sp;
	unsigned long bx;
	unsigned long dx;
	unsigned long cx;
	unsigned long ax;
	unsigned long reserved;
	unsigned long number;
	unsigned long ip;
	unsigned long cs;
	unsigned long eflags;
};

extern void init_irq(void);

struct handler {
	void (*handler)(unsigned int);
	int irq;
};

#define IRQ_HANDLER(func, irqn) \
	static const struct handler __irq_handler_ ## func ## irqn \
		__attribute__((section(".rodata.handlers"), used)) = { \
		.handler = func, \
		.irq = irqn, \
	}

#endif
