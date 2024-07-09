.PHONY: clean all

all:
ifeq ($(OS),Windows_NT)
	docker run -i --platform linux/amd64 --entrypoint 'make' --workdir /KrapOS --rm -v C:\_Lejus\KrapOS:/KrapOS gcc:11.4.0
else
	$(MAKE) -C user/ all VERBOSE=$(VERBOSE)
	$(MAKE) -C kernel/ kernel.bin VERBOSE=$(VERBOSE)
endif

macos: docker.macos run

test-kernel:
	./kernel/run_tests.sh

test-user:
	./user/run_tests.sh

debug: run-kernel-debug run-debugger
debug.macos: run-kernel-debug run-debugger.macos
debug.msw: run-kernel-debug.msw run-debugger.msw

run: run-kernel

run-kernel :
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -device e1000,bus=pcie.0 -device virtio-gpu-pci,bus=pcie.0 -smbios type=0,uefi=on

run-kernel-test :
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -debugcon stdio > qemu-output.txt 2> /dev/null &

run-kernel-debug:
	killall qemu-system-i386 || true
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio > qemu-output.txt &

run-kernel-debug.msw:
	taskkill /F /IM qemu-system-i386.exe /T > nul 2>&1
	qemu-system-i386 -machine q35 -m 256 -kernel kernel/kernel.bin -s -S -debugcon stdio > qemu-output.txt

run-debugger:
	/usr/bin/gdb kernel/kernel.bin -ex 'target remote localhost:1234'

run-debugger.macos:
	lldb kernel/kernel.bin -o 'gdb-remote localhost:1234'
run-debugger.msw:
	gdb kernel/kernel.bin -ex 'target remote localhost:1234'

clean.msw:
	$(MAKE) clean.msw -C kernel/
	$(MAKE) clean.msw -C user/
clean:
	$(MAKE) clean -C kernel/
	$(MAKE) clean -C user/

