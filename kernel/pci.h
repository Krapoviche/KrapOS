#ifndef __PCI_H__
#define __PCI_H__

#include "stdint.h"
#include "cpu.h"
#include "stdio.h"
#include "segment.h"
#include "mem.h"
#include "string.h"
#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))
void lspci();
int pci_read_config_byte(unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char *value);
int pci_read_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short *value);
int pci_read_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int *value);
#endif
