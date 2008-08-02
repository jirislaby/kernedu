#include <code16.h>

#include <setup.h>

void clear_bss(void)
{
	extern char __bss_start[], __bss_end[];
	char *a;

	for (a = __bss_start; a < __bss_end; a++)
		*a = 0;
}
