#include <output.h>
#include <setup.h>
#include <string.h>

static void cpuid(unsigned int id, unsigned int count, unsigned int *eax,
		unsigned int *ebx, unsigned int *ecx, unsigned int *edx)
{
	asm volatile("cpuid"
		: "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
		: "a" (id), "c" (count));
}

void main(void)
{
	unsigned int eax, ebx, ecx, edx, a;
	char id[17];

	clear_screen();

	print_color("cpuid example\n", 0x09);

	cpuid(0, 0, &eax, &ebx, &ecx, &edx);
	print_num(eax);
	memcpy(id, &ebx, 4);
	memcpy(id + 4, &edx, 4);
	memcpy(id + 8, &ecx, 4);
	id[12] = 0;
	print(id);
	print("\n");

	cpuid(1, 0, &eax, &ebx, &ecx, &edx);
	print("version: ");
	print_num(eax);

	for (a = 0x80000002; a <= 0x80000004; a++) {
		cpuid(a, 0, &eax, &ebx, &ecx, &edx);
		memcpy(id, &eax, 4);
		memcpy(id + 4, &ebx, 4);
		memcpy(id + 8, &ecx, 4);
		memcpy(id + 12, &edx, 4);
		id[16] = 0;
		print(id);
		print("\n");
	}

	halt();
}
