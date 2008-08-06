TOPDIR=..
TARGET=os os.bin
LOBJS=os.o loader.o irq.o entry.o
OBJS=$(LOBJS) $(COMMON_OBJS)
CLEAN_FILES += $(LOBJS) $(TARGET)
USE_COMMON_LIB=1

include $(TOPDIR)/Makefile.rules

os: os.lds $(OBJS)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $@ $(OBJS)
