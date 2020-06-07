.PHONY: clean all

all:
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

