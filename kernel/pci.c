#include "pci.h"

static struct {
	unsigned long address;
	unsigned short segment;
} pci_indirect = { 0, KERNEL_CS };

pci_device* PCI_DEVICES[MAX_PCI_DEVICES];

uint32_t read_pcibios(uint32_t* pcibios_entrypoint, bios_service* pcibios_info){
    pci_indirect.address = (uint32_t)pcibios_entrypoint;

    uint32_t pack;


    __asm__("lcall (%%edi)\n\t"
        "jc 1f\n\t"
        "xor %%ah, %%ah\n"
        "1:\tshl $8, %%eax\n\t"
        "movw %%bx, %%ax"
        : "=d" (pcibios_info->signature),
            "=a" (pack)
        : "1" (PCIBIOS_PCI_BIOS_PRESENT),
            "D" (&pci_indirect)
        : "bx", "cx");


    pcibios_info->present_status = (pack >> 16) & 0xff;
    pcibios_info->major_revision = (pack >> 8) & 0xff;
    pcibios_info->minor_revision = pack & 0xff;

    return 0;
}

uint32_t find_pci_class_code(uint8_t classcode, uint32_t index){
    uint32_t device, ret;

    __asm__("lcall (%%edi)\n\t"
        "jc 1f\n\t"
        "xor %%ah, %%ah\n"
        "1:"
        : "=b" (device),
          "=a" (ret)
        : "1" (PCIBIOS_FIND_PCI_CLASSCODE),
            "c" (classcode),
            "S" ((int)index),
            "D" (&pci_indirect));

    printf(" device is 0x%x",device);

    if(ret >> 8 == 0x86){
        return 0;
    }

    return 1;
}

int pci_bios_read_config_byte(unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_BYTE),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

int pci_bios_read_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_WORD),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

int pci_bios_read_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int *value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_DWORD),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

// static int pci_bios_write_config_byte (unsigned char bus,
// 	unsigned char device_fn, unsigned char where, unsigned char value)
// {
// 	unsigned long ret;
// 	unsigned long bx = (bus << 8) | device_fn;

// 	__asm__("lcall (%%esi)\n\t"
// 		"jc 1f\n\t"
// 		"xor %%ah, %%ah\n"
// 		"1:"
// 		: "=a" (ret)
// 		: "0" (PCIBIOS_WRITE_CONFIG_BYTE),
// 		  "c" (value),
// 		  "b" (bx),
// 		  "D" ((long) where),
// 		  "S" (&pci_indirect));
// 	return (int) (ret & 0xff00) >> 8;
// }

int pci_bios_write_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (PCIBIOS_WRITE_CONFIG_WORD),
		  "c" (value),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

int pci_bios_write_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int value)
{
	unsigned long ret;
	unsigned long bx = (bus << 8) | device_fn;

	__asm__("lcall (%%esi)\n\t"
		"jc 1f\n\t"
		"xor %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (PCIBIOS_WRITE_CONFIG_DWORD),
		  "c" (value),
		  "b" (bx),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

void discover_pci_devices(void){
    pci_device* cur_device;
    pci_bar cur_bar;
    uint32_t value32;
    uint16_t value16;
    uint8_t value8;
    for(uint16_t bus = 0 ; bus < MAX_PCI_BUS; bus++){
        for(uint8_t slot = 0 ; slot < MAX_PCI_SLOT ; slot++){
            for(uint8_t function = 0 ; function < MAX_PCI_FUNCTION ; function++){
                pci_bios_read_config_dword(bus, (slot << 3) + function, 0x0, &value32);
                if(value32 != 0xffffffff){
                    cur_device = (pci_device*)mem_alloc(sizeof(pci_device));

                    cur_device->vendor_id = value32 & 0xffff;
                    cur_device->device_id = value32 >> 16;

                    pci_bios_read_config_dword(bus, (slot << 3) + function, 0x4, &value32);
                    cur_device->status = value32 >> 16;
                    cur_device->command = value32 & 0xffff;

                    pci_bios_read_config_byte(bus, (slot << 3) + function, 0x8, &value8);
                    cur_device->revision_id = value8;

                    pci_bios_read_config_word(bus, (slot << 3) + function, 0xA, &value16);
                    cur_device->class_code = value16 >> 8;
                    cur_device->subclass = value16 & 0xff;

                    pci_bios_read_config_byte(bus, (slot << 3) + function, 0xE, &value8);
                    cur_device->header_type = value8;

                    pci_bios_read_config_dword(bus, (slot << 3) + function, 0x2C, &value32);
                    cur_device->subsystem_id = value32 >> 16;
                    cur_device->subsystem_vendor_id = value32 & 0xffff;

                    pci_bios_read_config_byte(bus, (slot << 3) + function, 0x34, &value8);
                    cur_device->capabilities_pointer = value8 & 0b11111100;

                    cur_device->bus = bus;
                    cur_device->slot = slot;
                    cur_device->function = function;

                    discover_capabilities_list(cur_device);

                    enable_master_device(cur_device);

                    for(int barnumb = 0 ; barnumb < 6 ; barnumb++){
                        pci_bios_read_config_dword(bus, (slot << 3) + function, 0x10 + (barnumb * 0x4), &value32);
                        cur_bar.base_value = value32;
                        cur_bar.type = value32 & 0b1;
                        cur_bar.prefetchable = value32 & 0b1000;
                        cur_bar.size = get_bar_size(cur_device, barnumb);
                        cur_bar.address = (uint32_t*)(value32 & 0xfffffff0);

                        if(cur_bar.type == MEMORY) cur_bar.mem_size_type = (value32 & 0b110) > 1;

                        cur_device->base_address_registers[barnumb] = cur_bar;
                    }

                    PCI_DEVICES[(bus << 8) + (slot << 3) + function] = cur_device;
                }
            }
        }
    }
}

void lspci(void){
    pci_device* device;
    for(uint16_t bus = 0 ; bus < MAX_PCI_BUS ; bus++){
        for(uint8_t slot = 0 ; slot < MAX_PCI_SLOT ; slot++){
            for(uint8_t function = 0 ; function < MAX_PCI_FUNCTION ; function++){
                device = PCI_DEVICES[(bus << 8) + (slot << 3) + function];
                if(device != NULL)
                    printf("%04d:%04d.%1d -> 0x%x from 0x%x\n", bus, slot, function, device->device_id, device->vendor_id);
            }
        }
    }
}

pci_device* get_device_info(uint8_t bus, uint8_t slot, uint8_t function){
    return PCI_DEVICES[(bus << 8) + (slot << 3) + function];
}

void print_device_info(pci_device* device){
    printf("%x of %x, subsystem : %x, revision id %d\n", device->device_id, device->vendor_id, device->subsystem_id, device->revision_id);
    printf("Capabilities list starts at offset 0x%x : ", device->capabilities_pointer);
    pci_capability* capa = device->capabilities;
    while (capa != NULL)
    {
        printf("-> ID : 0x%x OFFSET : 0x%x", capa->capability_id, capa->offset);
        capa = capa->next;
    }

    printf("\ndevice class is %x:%x and type is : 0x%x\n", device->class_code, device->subclass, device->header_type);
    printf("device status 0x%x\n", device->status);
    printf("device command 0x%x\n", device->command);
    for(int i = 0 ; i < 6 ; i++){
        printf("BAR %d : TYPE : %d and 0x%x PREFECTABLE : %x @ : %p. Size : %d\n", i, device->base_address_registers[i].type, device->base_address_registers[i].mem_size_type, device->base_address_registers[i].prefetchable ,device->base_address_registers[i].address, device->base_address_registers[i].size);
    }
}

void enable_master_device(pci_device* device){
    uint16_t value;

    // Read command bits
    pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, 0x4, &value);

    value = value | 0b100;
    pci_bios_write_config_word(device->bus, (device->slot << 3) + device->function, 0x4, value);

    // Read command bits
    pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, 0x4, &value);
    device->command = value;
}

uint32_t get_bar_size(pci_device* device, uint8_t bar_num){
    uint32_t bar_size;
    uint16_t io_mem_info, value;
    // Read command bits
    pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, 0x4, &io_mem_info);
    value = io_mem_info & 0xfffc;

    // Disable IO and Memory command bits
    pci_bios_write_config_word(device->bus, (device->slot << 3) + device->function, 0x4, value);

    // Get memory size from the BAR (Write ones, read return value, reset register value)
    uint8_t bar_address = (bar_num * 0x4) + 0x10;
    // Write ones
    pci_bios_write_config_dword(device->bus, (device->slot << 3) + device->function, bar_address, 0xffffffff);

    // Read return value
    pci_bios_read_config_dword(device->bus, (device->slot << 3) + device->function, bar_address, &bar_size);

    // Write back the original value
    pci_bios_write_config_dword(device->bus, (device->slot << 3) + device->function, bar_address, device->base_address_registers[bar_num].base_value);

    // Reset IO and Memory command bits to their original value
    pci_bios_write_config_word(device->bus, (device->slot << 3) + device->function, 0x4, io_mem_info);

    return (~(bar_size & 0xfffffff0)) + 1;
}

void discover_capabilities_list(pci_device* device){
    if(device->status & 0b10000){
        uint8_t offset = device->capabilities_pointer;
        uint16_t value;
        pci_capability* head = mem_alloc(sizeof(pci_capability));
        pci_capability* capa;
        pci_capability* tmp;


        // Start reading capabilities
        pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, offset, &value);

        while (offset != 0)
        {
            // printf("device : %x VALUE %d OFFSET %x\n", device->device_id, value, device->capabilities_pointer + offset);
            capa = mem_alloc(sizeof(pci_capability));

            // Fill the new capability with associated values
            capa->capability_id = value & 0xff;
            capa->offset = offset;
            capa->next = NULL;

            offset = value >> 8;

            // Add new capability to the list
            tmp = head;
            while(tmp->next != NULL)
                tmp = tmp->next;
            tmp->next = capa;

            // Go to next capability
            capa = capa->next;
            // Pre-read the next capability values in order to fill it or not
            pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, offset, &value);
        }

        // Remove "disposable" head and store the final list
        device->capabilities = head->next;
        mem_free(head, sizeof(pci_capability));
    }
}
