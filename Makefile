.PHONY: clean all

all:
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

debug: run-kernel run-debugger

run-kernel:
	killall qemu-system-i386 || true
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio > qemu-output.txt &

run-debugger:
	/usr/bin/gdb kernel/kernel.bin -ex 'target remote localhost:1234'

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

