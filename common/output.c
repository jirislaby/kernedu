#include <io.h>
#include <output.h>

static unsigned char scr_x, scr_y;
static unsigned short *vram = va(0xb8000);

static inline void print_char(unsigned short ch, unsigned char x,
		unsigned char y)
{
	vram[x + y * 80] = ch;
}

void clear_screen(void)
{
	unsigned short x;

	for (x = 0; x < 80 * 25; x++)
		print_char(0x0700, x % 80, x / 80);

	scr_x = 0;
	scr_y = 0;
}

/* causes to move lines one line upper and clean the bottom line */
static void roll_display(void)
{
	unsigned int x, y;

	for (y = 1; y < 25; y++)
		for (x = 0; x < 80; x++) {
			vram[x + (y - 1) * 80] = vram[x + y * 80];
			if (y == 24)
				vram[x + y * 80] = 0x0700;
		}
}

void print_color(const char *text, unsigned char color)
{
	while (*text) {
		if (*text == '\r' || *text == '\n')
			goto line_feed;
		if (scr_y >= 25) {
			roll_display();
			scr_y--;
		}
		print_char((color << 8) | *text, scr_x, scr_y);
		if (++scr_x >= 80) {
line_feed:
			scr_y++;
			scr_x = 0;
		}
		text++;
	}
}

void print_num_color(unsigned long num, unsigned char color)
{
	static const char HEX[] = "0123456789abcdef";
	unsigned int a;
	char text[10];

	for (a = 0; a < 8; a++)
		text[a] = HEX[(num >> (4*(7-a))) & 0xf];
	text[8] = ' ';
	text[9] = 0;
	print_color(text, color);
}
