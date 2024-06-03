.PHONY: clean all

all:
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)

debug: run-kernel-debug run-debugger

run: run-kernel

run-kernel :
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin

run-kernel-debug:
	killall qemu-system-i386 || true
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio > qemu-output.txt &

run-debugger:
	/usr/bin/gdb kernel/kernel.bin -ex 'target remote localhost:1234'

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

