#
# Ensimag - Projet système
# Copyright (C) 2001-2003 - Simon Nieuviarts
# Copyright (C) 2012-2014 - Damien Dejean <dam.dejean@gmail.com>
# Copyright (C) 2014,2019 - Grégory Mounie <Gregory.Mounie@imag.fr>

### Tools ###
CC := gcc
AS := gcc
LD := ld
AR := ar
DEP := gcc
OBJCOPY := objcopy
MKDIR := mkdir
CP := cp
GEN_SECTIONS := build/generate-link-sections.sh
GEN_TABLE := build/generate-symbols-table.sh


### Platform tools ###
$(PLATFORM_TOOLS): | $(OUTPUT)
	build/generate-platform-tools.sh > $@

ifneq "$(MAKECMDGOALS)" "$(PLATFORM_TOOLS)"
include $(PLATFORM_TOOLS)
endif


### Override with quiet tools by default
ifneq "$(VERBOSE)" "1"
include build/quiet.mk
endif

### Tools flags ###
DEFS   := -D__KERNEL__
CFLAGS := -m32 \
		  -Wall -Wextra -Werror -std=c11 \
		  -g \
		  -pipe \
		  -nostdinc \
		  -nostdlib \
		  -fno-stack-protector \
		  -fno-pie -no-pie \
		  -march=pentium \
		  $(DEFS)
ASFLAGS := -m32 -g\
		   -DASSEMBLER \
		   $(DEFS)
ARFLAGS := rcs
LDFLAGS := -g -melf_i386 -no-pie

# Import facilities
include build/utils.mk

### Compilation sources ###
# Use DIRS variable to add source sub-directories
DIRS := . boot
SRC  :=	\
	$(call all-asm-files-under, $(DIRS)) \
	$(call all-c-files-under , $(DIRS))
INCLUDES := $(addprefix -I, $(DIRS) ../shared)

### Output files ###
OUTPUT      := out
DEPS        := $(addprefix $(OUTPUT)/, $(call generate-deps, $(SRC)))
OBJS        := $(addprefix $(OUTPUT)/, $(call objetize, $(SRC))) \
	$(OUTPUT)/task_dump_screen.o
OUTPUT_DIRS := $(addprefix $(OUTPUT)/, $(DIRS))

### Shared lib build ###
SHARED_LIB_DIR := ../shared
include build/shared-lib.mk


### Linux keyboard support build ###
KBD_LINUX_DIR := kbd-linux
include build/kbd-linux.mk

### User lib build ###
.PHONY: $(OUTPUT)/userdata.o
$(OUTPUT)/userdata.o: ../user/user.bin
	$(MAKE) -C ../user
	objcopy $(OUTPUT)/empty.o --add-section=.user=$^ --set-section-flags=.user=contents,alloc,load,data $@


### Generate the kernel, ready to run ###
kernel.bin: build/kernel.lds $(OBJS) $(SHARED_LIB_TARGET) $(KBD_LINUX_TARGET) $(APPS_OBJS) $(OUTPUT)/userdata.o
	$(LD) $(LDFLAGS) -e entry -T$< $(filter-out %.lds, $^) -o $@

### insert the kernel in an iso file (cdrom) suitable for bochs and USB key
cd.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot
	printf 'set timeout=0\nset default=0\nmenuentry "kernel.bin" {\n multiboot /boot/kernel.bin\n boot\n}' > iso/boot/grub/grub.cfg
	grub-mkrescue -o cd.iso iso
	rm -r iso/

### lancement de bochs: il faut faire [6] Begin Smulation puis 'cont'
### pour commencer la simulation avec gdbstub activé comme qemu
.PHONY: bochs
bochs: cd.iso
	bochs -q 'memory: guest=256,host=256' 'boot: cdrom' 'ata0-master: type=cdrom, path="cd.iso", status=inserted' 'gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0'

### Generic rules ###
$(OUTPUT)/%.d: %.c | $(OUTPUT)
	@echo -n "$(@D)/" > $@
	$(DEP) $(INCLUDES) -MM $< >> $@

$(OUTPUT)/%.d: %.S | $(OUTPUT)
	@echo -n "$(@D)/" > $@
	$(DEP) $(INCLUDES) -MM $< >> $@

$(OUTPUT)/%.o: %.c | $(OUTPUT)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OUTPUT)/%.o: %.S | $(OUTPUT)
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

# A text screen for exception dump
$(OUTPUT)/task_dump_screen.o: $(OUTPUT)/empty.o task_dump_screen.txt
	$(OBJCOPY) $< \
		--add-section=.task_dump_screen=$(filter-out $<, $^) \
		--set-section-flags=.task_dump_screen=contents,alloc,load,data $@


### Build output directory ###

# Add a dependency between each object file and the directory that will contain
# it.
$(foreach obj-file, $(OBJS), $(eval 			\
	$(call generate-build-dir-dep, $(obj-file)) \
))
# Create a target for each build output directory
$(foreach build-dir, $(OUTPUT_DIRS), $(eval 		\
	$(call generate-build-dir-target, $(build-dir))	\
))

$(OBJS): | $(OUTPUT)
$(DEPS): | $(OUTPUT)
$(OUTPUT):
	$(MKDIR) -p $@


### Clean everything ###
.PHONY: clean
clean:
	rm -rf $(OUTPUT) kernel.bin cd.iso

