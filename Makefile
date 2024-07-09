.PHONY: clean all

all:
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ cd.iso VERBOSE=$(VERBOSE)

test-kernel:
	./kernel/run_tests.sh

test-user:
	./user/run_tests.sh

docker:
	docker run -i --platform linux/amd64 --entrypoint 'make' --workdir /psys-base --rm -v $(PWD):/psys-base gcc:11.4.0

debug: run-kernel-debug run-debugger
debug.macos: run-kernel-debug run-debugger.macos

run: run-kernel

run-kernel :
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -device virtio-net-pci,bus=pcie.0 -device virtio-gpu-pci,bus=pcie.0 -smbios type=0,uefi=on

run-kernel-test :
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -debugcon stdio > qemu-output.txt 2> /dev/null &

run-kernel-debug:
	killall qemu-system-i386 || true
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio > qemu-output.txt &

run-debugger:
	/usr/bin/gdb kernel/kernel.bin -ex 'target remote localhost:1234'

run-bochs:
	bochs -q -f bochs/macos.bxrc

run-debugger.macos:
	lldb kernel/kernel.bin -o 'gdb-remote localhost:1234'

clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

