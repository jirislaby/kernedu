/* tell assembler we are in real mode, don't use real 32-bit instructions */
#include <code16.h>

extern void print(const char *);

void main(void)
{
	char text[] = "Hi\n\r";

	print("Hello world from C\n\r");
	print(text);
	(*text) += 2;
	print(text);
}
