#include <code16.h>

#include <io.h>
#include <output.h>

static inline void print_char(short ch, short x, short y)
{
	asm volatile("stosw" : : "a" (ch), "D" (x*2 + y*80*2));
}

static unsigned char scr_x, scr_y;

void clear_screen(void)
{
	unsigned short x, oe = set_es(0xb800);

	for (x = 0; x < 80 * 25; x++)
		print_char(0x0700, x % 80, x / 80);

	set_es(oe);

	scr_x = 0;
	scr_y = 0;
}

void print(const char *text)
{
	unsigned short oe = set_es(0xb800);

	while (*text) {
		if (*text == '\r' || *text == '\n')
			goto line_feed;
		if (scr_y >= 25)
			clear_screen();
		print_char(0x0700 | *text, scr_x, scr_y);
		if (++scr_x >= 80) {
line_feed:
			scr_y++;
			scr_x = 0;
		}
		text++;
	}

	set_es(oe);
}

void print_num(unsigned long num)
{
	static const char HEX[] = "0123456789abcdef";
	unsigned int a;
	char text[10];

	for (a = 0; a < 8; a++)
		text[a] = HEX[(num >> (4*(7-a))) & 0xf];
	text[8] = ' ';
	text[9] = 0;
	print(text);
}
