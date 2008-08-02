SUBDIRS=common/ 01_lds/ 02_hello/ 03_c/ 04_protected/ timer/
MAKEFLAGS += -rR --no-print-directory

all: $(addsuffix .built,$(SUBDIRS))

clean: $(addsuffix .cleaned,$(SUBDIRS))

%/.built: %
	@$(MAKE) -C $^ all

%/.cleaned: %
	@$(MAKE) -C $^ clean

FORCE:

.PHONY: all clean FORCE
