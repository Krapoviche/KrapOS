#include "pci.h"

static struct {
	unsigned long address;
	unsigned short segment;
} bios32_indirect = { 0, KERNEL_CS };


static struct {
	unsigned long address;
	unsigned short segment;
} pci_indirect = { 0, KERNEL_CS };


int pci_read_config_byte(unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char *value)
{
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = inb(0xCFC + (where&3));
    return 0;
}

int pci_read_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short *value)
{
    if (where&1) return -1;
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = inw(0xCFC + (where&2));
    return 0;
}

int pci_read_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int *value)
{
    if (where&3) return -1;
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    *value = inl(0xCFC);
    return 0;
}


int pci_write_config_byte (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned char value)
{
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    outb(value, 0xCFC + (where&3));
    return 0;
}

int pci_write_config_word (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned short value)
{
    if (where&1) return -1;
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    outw(value, 0xCFC + (where&2));
    return 0;
}

int pci_write_config_dword (unsigned char bus, unsigned char device_fn, unsigned char where, unsigned int value)
{
    if (where&3) return -1;
    outl(CONFIG_CMD(bus,device_fn,where), 0xCF8);
    outl(value, 0xCFC);
    return 0;
}

uint16_t get_vendor_id(uint8_t bus, uint8_t device_fn, unsigned short *value)
{
        uint32_t r0 = pci_read_config_word(bus, device_fn, 0, value);
        return r0;
}

uint16_t get_device_id(uint16_t bus, uint16_t device_fn, unsigned short *value)
{
        uint32_t r0 = pci_read_config_word(bus, device_fn, 2, value);
        return r0;
}
void lspci(){
    unsigned short* value16 = mem_alloc(sizeof(unsigned short));
    unsigned char* value8 = mem_alloc(sizeof(unsigned char));
    char* vendor_name = mem_alloc(sizeof(char) * 50);
    char* device_name = mem_alloc(sizeof(char) * 50);
    unsigned short vendor_id, device_id;

    uint32_t* service_dir_header = find_bios32_service_dir_header();
    bios32* bios32_info =  mem_alloc(sizeof(bios32));
    read_bios32(service_dir_header, bios32_info);
    uint32_t* pci_service = read_bios32_service(PCI_SERVICE, bios32_info);
    pcibios* pcibios_info =  mem_alloc(sizeof(pcibios));
    read_pcibios(pci_service, pcibios_info);

    printf("PCI BIOS VERSION %d.%d\n", pcibios_info->major_revision, pcibios_info->minor_revision);

    for(uint32_t bus = 0; bus < 256; bus++)
    {
        for(uint32_t slot = 0; slot < 256; slot++)
        {
                get_vendor_id(bus, slot, value16);
                vendor_id = *value16;

                if(vendor_id != 0xFFFF){

                    get_device_id(bus, slot, value16);
                    device_id = *value16;

                    switch (vendor_id)
                    {
                        case 0x8086:
                            strcpy(vendor_name, "Intel Corporation");
                            break;
                        case 0x1013:
                            strcpy(vendor_name, "Cirrus");
                            break;
                        case 0x1af4:
                            strcpy(vendor_name, "virtio");
                            break;
                        default:
                            strcpy(vendor_name, "UNDEFINED");
                            break;
                    }

                    switch (device_id)
                    {
                        case 0x1050:
                            strcpy(device_name, "Virtio GPU");
                            break;
                        case 0x29c0:
                            strcpy(device_name, "DRAM Controller");
                            break;
                        case 0x100e:
                            strcpy(device_name, "82540EM Gigabit Ethernet Controller");
                            break;
                        case 0x2918:
                            strcpy(device_name, "LPC Interface Controller");
                            break;
                        case 0x2922:
                            strcpy(device_name, "6 port SATA AHCI Controller");
                            break;
                        case 0x2930:
                            strcpy(device_name, "SMBus Controller");
                            break;
                        default:
                            strcpy(device_name, "UNDEFINED");
                            break;
                    }

                    printf("%04d:%04d, %s, %s\n", bus, slot, vendor_name, device_name);
                }
        }
    }
    mem_free(value16, sizeof(unsigned short));
    mem_free(value8, sizeof(unsigned char));
    mem_free(vendor_name, sizeof(char) * 50);
    mem_free(device_name, sizeof(char) * 50);
}

uint32_t * find_bios32_service_dir_header(){
    uint32_t* start_address = (uint32_t*) 0xE0000;
    uint32_t* end_address = (uint32_t*) 0xFFFFF;

    for( ; start_address < end_address ;  start_address++){
        if(*start_address == 0x5F32335F){
            return start_address;
        }
    }
    return (uint32_t*)0xFFFF;
}

void read_bios32(uint32_t* start_address, bios32* bios32_info){
    bios32_info->signature = *start_address;
    bios32_info->address = *(start_address + 1);

    uint8_t* tmp = (uint8_t*)(start_address + 2); // Next 3 fields are 1 byte long

    bios32_info->revision = *tmp;
    bios32_info->length = *(tmp + 1);
    bios32_info->checksum = *(tmp + 2);

    bios32_info->reserved[0] = *(tmp + 3); // First byte of reserved
    bios32_info->reserved[1] = *(start_address + 3); // Last 4 bytes of reserved (start address + 3 == start address + 2 + 1 byte * 4)
}

uint32_t* read_bios32_service(uint32_t service, bios32* bios32_info){
    bios32_indirect.address = bios32_info->address;

	unsigned char return_code;	/* %al */
	unsigned long address;		/* %ebx */
	unsigned long length;		/* %ecx */
	unsigned long entry;		/* %edx */

	__asm__("lcall (%%edi)"
		: "=a" (return_code),
		  "=b" (address),
		  "=c" (length),
		  "=d" (entry)
		: "0" (service),
		  "1" (0),
		  "D" (&bios32_indirect));

    if(return_code != 0x80 && return_code != 0x81)
        return  (uint32_t*)(address + entry);
    return NULL;
}

uint32_t read_pcibios(uint32_t* pcibios_entry, pcibios* pcibios_info){
    pci_indirect.address = (uint32_t)pcibios_entry;

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
