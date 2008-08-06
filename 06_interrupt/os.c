#include <output.h>
#include <setup.h>

#include "irq.h"

unsigned int divisor, zero;

void main(void)
{
	clear_screen();

	print("Interrupts example\n");

	init_irq();

#if 0
	divisor /= zero;
#elsif 0
	asm volatile("ud2");
#else
	asm volatile("int $3");
#endif

	halt();
}
