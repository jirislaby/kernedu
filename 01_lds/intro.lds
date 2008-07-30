/* we use implicit linker script _plus_ this one */
SECTIONS
{
	/* add this to data */
	.data : {
		/* store this address into text_from_lds */
		text_from_data_lds = .;
		LONG(0x41424344) /* "DCBA" string */
		BYTE(0x20) /* space */
		SHORT(0x4645) /* "EF" string */
		BYTE(0) /* string terminator */
	}
	/* to rodata */
	.rodata : {
		text_from_rodata_lds = .;
		SHORT(0x5958) /* "XY" string */
		BYTE(0) /* string terminator */
	}
}
