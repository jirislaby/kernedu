TOPDIR=..
TARGET=os os.bin
OBJS=loader.o os.o
CLEAN_FILES += $(OBJS) $(TARGET)

include $(TOPDIR)/Makefile.rules

os: os.lds $(OBJS)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $@ $(OBJS)
