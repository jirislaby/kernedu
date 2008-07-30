#include <stdio.h>

const char text_from_rodata_global[] = "AB";
static const char text_from_rodata_local[] = "CD";
char text_from_data_global[] = "EF";
static char text_from_data_local[] = "GH";

int main(void)
{
	extern char text_from_rodata_lds[];
	extern char text_from_data_lds[];

	puts(text_from_rodata_global);
	puts(text_from_rodata_local);
	puts(text_from_data_global);
	puts(text_from_data_local);

	puts(text_from_data_lds);
	*text_from_data_lds = 'X';
	puts(text_from_data_lds);

	puts(text_from_rodata_lds);
	puts("We should die here:");
	fflush(stdout);
	*text_from_rodata_lds = 'X'; /* this should sigsegv on real platforms */

	return 0;
}
