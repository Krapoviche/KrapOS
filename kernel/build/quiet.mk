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

# Define a quiet version of each tool
CC = @echo -e "    CC [K]\t $@"; $(QCC)
AS = @echo -e "    AS [K]\t $@"; $(QAS)
LD = @echo -e "    LD [K]\t $@"; $(QLD)
AR = @echo -e "    AR [K]\t $@"; $(QAR)
DEP = @echo -e "    DEP [K]\t $@"; $(QDEP)
CP = @echo -e "    CP\t\t $< -> $@"; $(QCP)
MKDIR = @echo -e "    MKDIR\t $@"; $(QMKDIR)
OBJCOPY =  @echo -e "    OBJCOPY\t $@"; $(QOBJCOPY)
GEN_SECTIONS = @echo -e "    GEN-SECTIONS $@"; $(QGENSECTIONS)
GEN_TABLE = @echo -e "    GEN-TABLE\t $@"; $(QGENTABLE)

