#include <io.h>
#include <output.h>
#include <setup.h>

static void i8042_flush_out(void)
{
	while (!(inb(0x64) & 0x01))	/* output buffer full */
		barrier();
}

static void i8042_wait_in(void)
{
	while (inb(0x64) & 0x02)	/* input buffer full */
		barrier();
}

static void init_8042(void)
{
	i8042_wait_in();

	outb(0xaa, 0x64);	/* self test */
	i8042_flush_out();
	if (inb(0x60) != 0x55) {
		print_color("8042 self test failed\n", 0x0c);
		halt();
	}

	outb(0xab, 0x64);	/* interface test */
	i8042_flush_out();
	if (inb(0x60) != 0x00) {
		print_color("8042 interface test failed\n", 0x0c);
		halt();
	}

	outb(0x60, 0x64);	/* write command */
	i8042_wait_in();
	outb(0x68, 0x60);	/* translate, disable mouse, ignore kbd lock */
	i8042_wait_in();

	outb(0xff, 0x60);	/* kbd reset */
	/* */
	i8042_flush_out();
	inb(0x60);		/* 0xfc (ACK) */
	inb(0x60);		/* 0xaa (BAT successful) */

	outb(0xee, 0x60);
	/* */
	i8042_flush_out();
	if (inb(0x60) != 0xee) {
		print_color("8042 echo test failed\n", 0x0c);
		halt();
	}

	outb(0xf4, 0x60);	/* enable kbd */
	i8042_flush_out();
	inb(0x60);		/* 0xfc (ACK) */
}

void main(void)
{
	unsigned char b;

	clear_screen();

	print_color("Keyboard example (polling)\n", 0x09);

	init_8042();

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
				asm volatile("rep; nop\n");
		}
	}
}
