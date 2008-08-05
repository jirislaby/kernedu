#include <output.h>
#include <setup.h>

void main(void)
{
	extern char _text[];

	clear_screen();

	print("Running from 0x");
	print_num((unsigned long)_text);
	print("main is at 0x");
	print_num((unsigned long)main);
	print("\n");

	halt();
}
