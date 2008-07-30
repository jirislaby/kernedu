TOPDIR=..
TARGET=os os.bin
os_OBJS= os.o output.o setup.o loader.o entry.o
CLEAN_FILES += $(os_OBJS) $(TARGET)

include $(TOPDIR)/Makefile.rules

os: os.lds $(os_OBJS)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $@ $(os_OBJS)
