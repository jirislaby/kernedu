#include <io.h>
#include <output.h>

void main(void)
{
	unsigned char b;

	clear_screen();

	print_color("Keyboard example\n", 0x09);

	outb(0x20, 0x64);	/* CMD: read controller command byte */
	b = inb(0x60);		/* READ */

	print_num(b);
	b &= ~0x01;		/* disable key intrs */

	outb(0x60, 0x64);	/* CMD: write controller command byte */
	outb(b, 0x60);		/* WRITE */

	outb(0x20, 0x64);
	print_num(inb(0x60));
	print("= status register before and after\n");

	print_color("Press a key to see a scan code, Esc for reboot...\n",
			0x0a);

	while (1) {
		b = inb(0x64);		/* status */
		print_num(b);
		if (b & 1) {		/* output buffer full */
			b = inb(0x60);
			print_num(b);
			if (b == 0x01)
				outb(0xfe, 0x64);	/* reboot */
		}
		print("\n");

		if (!(b & 1)) {		/* no data -> wait a bit */
			unsigned int a;
			for (a = 0; a < 0xfffffff; a++)
				barrier();
		}
	}
}
