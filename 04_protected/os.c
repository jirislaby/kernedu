/* we need to save space, so the noinline */
static __attribute__((noinline)) void print(const char *text)
{
	/* we haven't cleared .bss, do the zero assignment explicitly */
	static unsigned short x = 0;
	/* loader.s reads this from BIOS (in real mode) */
	extern unsigned short cursor_line;
	/*
	 * Video ram is at physical address 0xb8000 in physical addressing
	 * (btw. right after 640 KB of nobody-needs-more space and below 1M).
	 * Our data segment according to the definition in loader.s (gdt data
	 * structure) begins at 0x7c00 physical (BIOS loads us there and we
	 * don't move with the code and data) -- logical address 0 is physcal
	 * adddres 0x7c00, so we need to subtract this base to get real in
	 * physical 0xb8000.
	 *
	 * See e.g. http://en.wikipedia.org/wiki/VGA for the addressing.
	 */
	unsigned short *vram = (void *)(0xb8000-0x7c00);

	while (*text) {
		if (*text == '\n') {
			x = 0;
			cursor_line++;
		} else
			vram[x++ + cursor_line*80] = *text | 0x1f00;
		text++;
	}
}

void main(void)
{
	char text[] = "Hi\n";

	print("Hello world from C\n");
	print(text);
	(*text) += 2;
	print(text);
}
