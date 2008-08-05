OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup)
SECTIONS
{
	. = 0;
	.text.loader : {
		loader.o(.text.loader)
	}
	. = 0xa0;
	.data.loader : {
		loader.o(.data.loader)
	}
	. = 0x1fe;
	.rodata.loader_magic : {
		SHORT(0xaa55);
	}
}
