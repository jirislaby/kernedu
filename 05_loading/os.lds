OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(startup)
SECTIONS
{
	. = 0;
	.text.loader : {
		loader.o(.text.loader)
	}
	. = 0x150;
	.data.loader : {
		loader.o(.data.loader)
	}
	. = 0x1fe;
	.rodata.loader_magic : {
		SHORT(0xaa55);
	}
	__os_start = .;
	.rodata.os_start_magic : {
		__os_start_magic = .;
		LONG(0xdedaeda0);
	}
	.text : {
		_text = .;
		*(.text*)
	}
	.data : {
		*(.data)
	}
	.rodata : {
		*(.rodata)
	}
	.rodata.handlers : {
		__handlers = .;
		*(.rodata.handlers)
		LONG(0);
	}
	.bss : {
		__bss_start = .;
		*(.bss)
		__bss_end = .;
	}
	.rodata.os_end_magic : {
		. = ALIGN(512) + 508;
		__os_end_magic = .;
		LONG(0xdedaedaf);
	}
	__os_end = .;

	/DISCARD/ : {
		*(.eh_frame)
	}
}
