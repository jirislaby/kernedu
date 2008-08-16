#include <io.h>
#include <irq.h>
#include <output.h>
#include <setup.h>

unsigned int divisor, zero, overflow_value;

static void __attribute__((unused)) demonstrate_bound_intr(void)
{
	unsigned long limits[2] = { 0, 999 };

	asm volatile("boundl %1, %0" : "+m" (limits) : "r" (1000));
}

static void demonstrate_overflow_intr(void)
{
	unsigned long tmp;

	/* overflow interrupt, unsigned test */
	overflow_value = 0x40000000;
	asm volatile("into");
	asm volatile("mull %2\n"
			"into" : "+a" (overflow_value), "=d" (tmp) : "r" (2));
	print("overflow_value = ");
	print_num(overflow_value);
	print("\nInterrupt should occur now:\n");
	asm volatile("mull %2\n"
			"into" : "+a" (overflow_value), "=d" (tmp) : "r" (2));
	print("overflow_value = ");
	print_num(overflow_value);
	print("\n");

	/* signed test */
	overflow_value = 0x20000000;
	asm volatile("into");
	asm volatile("imull %2\n"
			"into" : "+a" (overflow_value), "=d" (tmp) : "r" (2));
	print("overflow_value = ");
	print_num(overflow_value);
	print("\nInterrupt should occur now:\n");
	asm volatile("imull %2\n"
			"into" : "+a" (overflow_value), "=d" (tmp) : "r" (2));
	print("overflow_value = ");
	print_num(overflow_value);
	print("\n");
}

void main(void)
{
	clear_screen();

	print_color("Interrupts example\n", 0x09);

	init_irq();

	/* breakpoint interrupt test */
	asm volatile("int3");
	asm volatile("int3");

	demonstrate_overflow_intr();

#if 0
	divisor /= zero;
#elif 0
	asm volatile("ud2");
#else
	demonstrate_bound_intr();
#endif

	halt();
}
