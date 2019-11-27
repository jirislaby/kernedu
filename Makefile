SUBDIRS=common/ 01_lds/ 02_hello/ 03_c/ 04_protected/ 05_loading/ 06_keyboard/ \
	07_interrupt/ 09_timer/ 10_cpuid/ 11_pci/
MAKEFLAGS += -rR

all: $(addsuffix .built,$(SUBDIRS))

clean: $(addsuffix .cleaned,$(SUBDIRS))

%/.built: %
	@$(MAKE) -C $^ all

%/.cleaned: %
	@$(MAKE) -C $^ clean

FORCE:

.PHONY: all clean FORCE
