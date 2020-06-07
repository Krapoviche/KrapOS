# Copyright (C) 2001-2003 by Simon Nieuviarts

# Files to compile
FILES=$(wildcard *.S *.c) printf.c sprintf.c doprnt.c panic.c string.c
DIRS=. ../shared

# Directory and output object files
OUTPUT := out
OBJS   := $(addprefix $(OUTPUT)/, $(patsubst %.S, %.o, $(patsubst %.c, %.o, $(FILES))))

CC=gcc -m32
AS=gcc -m32
LD=ld -melf_i386

KFLAGS=-Wall -Wextra -g -gstabs -pipe -std=c99 -nostdinc -fno-stack-protector
#KFLAGS=-Wall -O2 -fomit-frame-pointer -pipe -std=c99 -nostdinc

DEF=
#DEF=-DTELECOM_TST

INC=$(patsubst %,-I%,$(DIRS))
CCL=$(CC) -Werror $(KFLAGS) $(DEF) $(INC)
ASL=$(AS) -DASSEMBLER $(DEF) $(INC)
DEPS := $(patsubst %.o, %.d, $(OBJS))

# Where to search for source files
vpath %.c $(DIRS)
vpath %.h $(DIRS)
vpath %.S $(DIRS)

# Targets that are not files
.PHONY: clean all

# The default target
all: user.bin

# Automatic rules to build dependency files
$(OUTPUT)/%.d: %.c
	@$(SHELL) -ec '$(CCL) -M $< | sed '\''s|\($*\)\.o[ :]*|\1.o $@ : Makefile |g'\'' > $@; [ -s $@ ] || rm -f $@'

$(OUTPUT)/%.d: %.S
	@$(SHELL) -ec '$(ASL) -M $< | sed '\''s|\($*\)\.o[ :]*|\1.o $@ : Makefile |g'\'' > $@; [ -s $@ ] || rm -f $@'

# Build and include dependency files
-include $(DEPS)

# Automatic rules to build the object files
$(OUTPUT)/%.o: %.S
	$(ASL) -c $< -o $@

$(OUTPUT)/%.o: %.c
	$(CCL) -c $< -o $@

$(OUTPUT)/%.s: %.c
	$(CCL) -S -c $< -o $@

user.debug: user.lds $(OBJS)
	$(LD) -e entry -oformat=binary -Tuser.lds $(OBJS) -o $@

user.bin: user.debug
	objcopy -O binary $< $@

# Create build directory for targets but don't depends on its creation date
$(OBJS): | $(OUTPUT)
$(DEPS): | $(OUTPUT)
$(OUTPUT):
	mkdir $@

clean:
	rm -rf $(OUTPUT) user.bin user.debug
