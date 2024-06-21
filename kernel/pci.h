#ifndef __PCI_H__
#define __PCI_H__

#include "stdint.h"
#include "cpu.h"
#include "stdio.h"
#include "segment.h"
#include "mem.h"
#include "string.h"

#define PCI_SERVICE		(('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))
#define PCI_SIGNATURE		(('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))
#define PCIBIOS_PCI_BIOS_PRESENT 	0xb101

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))

typedef struct bios32{
    uint32_t address;
    uint32_t signature;
    uint8_t revision;
    uint8_t length;
    uint8_t checksum;
    uint8_t reserved[5];
} bios32;

typedef struct pcibios{
	uint32_t signature;
	uint8_t present_status;
	uint8_t major_revision;
	uint8_t minor_revision;
} pcibios;

void lspci();

int pci_read_config_byte(unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char *value);
int pci_read_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short *value);
int pci_read_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int *value);

uint32_t * find_bios32_service_dir_header();
void read_bios32(uint32_t* start_address, bios32* bios32_info);
uint32_t* read_bios32_service(uint32_t service, bios32* bios32_info);
uint32_t read_pcibios(uint32_t* pcibios_entry, pcibios* pcibios_info);

#endif
