#include "pci.h"
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
