#
# Ensimag - Projet système
# Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
#

# Use temporary variables to store the tools
QCC := $(CC)
QAS := $(AS)
QLD := $(LD)
QAR := $(AR)
QCP := $(CP)
QDEP := $(DEP)
QMKDIR := $(MKDIR)
QOBJCOPY := $(OBJCOPY)
QGENSECTIONS := $(GEN_SECTIONS)
QGENTABLE := $(GEN_TABLE)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	ECHO := echo
endif
ifeq ($(UNAME_S),Darwin)
	ECHO := gecho
endif

# Define a quiet version of each tool
CC = @$(ECHO) -e "    CC [K]\t $@"; $(QCC)
AS = @$(ECHO) -e "    AS [K]\t $@"; $(QAS)
LD = @$(ECHO) -e "    LD [K]\t $@"; $(QLD)
AR = @$(ECHO) -e "    AR [K]\t $@"; $(QAR)
DEP = @$(ECHO) -e "    DEP [K]\t $@"; $(QDEP)
CP = @$(ECHO) -e "    CP\t\t $< -> $@"; $(QCP)
MKDIR = @$(ECHO) -e "    MKDIR\t $@"; $(QMKDIR)
OBJCOPY =  @$(ECHO) -e "    OBJCOPY\t $@"; $(QOBJCOPY)
GEN_SECTIONS = @$(ECHO) -e "    GEN-SECTIONS $@"; $(QGENSECTIONS)
GEN_TABLE = @$(ECHO) -e "    GEN-TABLE\t $@"; $(QGENTABLE)

