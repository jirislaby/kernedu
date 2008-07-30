CC=gcc
CFLAGS=-m32 -Wall -g -Os -ffreestanding -nostdinc -I. -MMD -MF .$@.d
OBJCOPY=objcopy
ASFLAGS=-g --32
TARGET=os
TARGET_OBJ= os.o output.o setup.o loader.o entry.o
DEPS=$(wildcard .*.d)

all: $(TARGET) $(TARGET).bin

$(TARGET): os.lds $(TARGET_OBJ)
	$(LD) $(LDFLAGS) -T os.lds -melf_i386 -o $(TARGET) $(TARGET_OBJ)

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

%.bin: %
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(TARGET_OBJ) $(TARGET) $(TARGET).bin $(DEPS)

.PHONY: all clean

-include $(DEPS)
