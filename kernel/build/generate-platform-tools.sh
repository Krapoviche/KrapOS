#!/bin/bash
#
# Ensimag - Projet Système
# Copyright (C) 2014 - Damien Dejean <dam.dejean@gmail.com>
#

#
# Kernel cannot use the gold linker. On platforms where the both linkers exist
# (bfd and gold), explicitely choose the gold one.
#
BFD_LINKER=`which ld.bfd 2> /dev/null`
if [ -n "${BFD_LINKER}" -a -x "${BFD_LINKER}" ]; then
    echo "LD := ld.bfd"
else
    echo "LD := ld"
fi

ECHO=`which echo`
echo "ECHO := ${ECHO}"
