OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup)

PHDRS
{
	text PT_LOAD FLAGS(5);
	data PT_LOAD FLAGS(6);
}

SECTIONS
{
	. = 0;
	.text.loader : {
		*/loader*.o(.text.loader)
	} :text
	. = 0xa0;
	.data.loader : {
		*/loader*.o(.data.loader)
	} :data
	. = 0x1fe;
	.rodata.loader_magic : {
		SHORT(0xaa55);
	}
}
