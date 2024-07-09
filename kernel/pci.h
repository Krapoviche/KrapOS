#ifndef __PCI_H__
#define __PCI_H__

#include "pci_const.h"
#include "stdint.h"
#include "stdbool.h"
#include "cpu.h"
#include "bios32.h"
#include "stdio.h"
#include "segment.h"
#include "mem.h"
#include "string.h"

#define MAX_PCI_DEVICES (MAX_PCI_BUS * MAX_PCI_SLOT * MAX_PCI_SLOT) - 1
#define MAX_PCI_BUS 256
#define MAX_PCI_SLOT 32
#define MAX_PCI_FUNCTION 8

#define CONFIG_CMD(bus, device_fn, where)   (0x80000000 | (bus << 16) | (device_fn << 8) | (where & ~3))

typedef enum bar_type {MEMORY, IO} bar_type;

typedef struct pci_bar {
    bar_type type;
    uint8_t mem_size_type;
    bool prefetchable;
    uint32_t base_value;
    uint32_t size;
    uint32_t* address;
} pci_bar;

typedef struct pci_capability {
    uint8_t capability_id;
    uint8_t offset;
    struct pci_capability* next;
} pci_capability;

typedef struct pci_device {
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t status;
    uint16_t command;
    uint8_t revision_id;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t header_type;
    pci_bar base_address_registers[6];
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint8_t capabilities_pointer;
    pci_capability* capabilities;
} pci_device;

extern pci_device* PCI_DEVICES[MAX_PCI_DEVICES];
extern void* next_bar_address;

void discover_pci_devices(void);
void lspci(void);
pci_device* get_device_info(uint8_t bus, uint8_t slot, uint8_t function);
void print_device_info(pci_device* device);
void enable_master_device(pci_device* device);
uint32_t get_bar_size(pci_device* device, uint8_t bar_num);
void discover_capabilities_list(pci_device* device);
uint32_t* allocate_bar_memory_address(uint32_t size);

uint32_t read_pcibios(uint32_t* pcibios_entry, bios_service* pcibios_info);

// Read from pcibios
int pci_bios_read_config_byte(unsigned char bus,	unsigned char device_fn, unsigned char where, unsigned char *value);
int pci_bios_read_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short *value);
int pci_bios_read_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int *value);

// Write to pci bios
// static int pci_bios_write_config_byte (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char value);
int pci_bios_write_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short value);
int pci_bios_write_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int value);

uint32_t find_pci_class_code(uint8_t classcode, uint32_t index);

#endif
