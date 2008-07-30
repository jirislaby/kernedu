TOPDIR=..
TARGET=intro
OBJS=intro.o
CLEAN_FILES += $(OBJS)

include $(TOPDIR)/Makefile.rules

LD=gcc
CFLAGS=$(CFLAGS_HOST)

intro: intro.lds $(OBJS)
	$(LD) -g $(LDFLAGS)  intro.lds -o $@ $(OBJS)
