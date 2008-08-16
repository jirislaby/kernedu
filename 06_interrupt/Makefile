TOPDIR=..
TARGET=os os.bin
LOBJS=os.o loader.o
OBJS=$(LOBJS) $(COMMON_OBJS)
CLEAN_FILES += $(LOBJS) $(TARGET)

include $(TOPDIR)/Makefile.rules

os: os.lds $(OBJS)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $@ $(OBJS)
