#include "virtio.h"

void test_somth(pci_device* device){
    pci_capability* capa = device->capabilities;
    // virtio_pci_cap virtio_capa;
    uint8_t value,bar;
    uint32_t value32, offset;

    while(capa != NULL){
        if(capa->capability_id == 0x9){
            pci_bios_read_config_byte(device->bus, (device->slot << 3) + device->function, capa->offset + 0x4, &bar);
            // printf("BAR FOR CAPABILITY IS %d\n", bar);
            pci_bios_read_config_byte(device->bus, (device->slot << 3) + device->function, capa->offset + 0x3, &value);
            // printf("TYPE OF CONFIG IS %d\n", value);
            pci_bios_read_config_dword(device->bus, (device->slot << 3) + device->function, capa->offset + 0x8, &offset);
            // printf("OFFSET IN BAR %d\n", offset);
            pci_bios_read_config_dword(device->bus, (device->slot << 3) + device->function, capa->offset + 0xC, &value32);
            // printf("LENGTH IN BAR %d\n", value32);

            if(device->base_address_registers[bar].type == MEMORY){
                uint32_t* bar_address = device->base_address_registers[bar].address;

                bar_address += offset;
                printf("Writing 0\n");
                *bar_address = 0;
                uint32_t device_feature = *(bar_address + 1);
                printf("Device feature is 0x%x\n", device_feature);

                printf("Writing 1\n");
                *bar_address = 1;
                device_feature = *(bar_address + 1);
                printf("Device feature is 0x%x\n", device_feature);

            }
        }
        capa = capa->next;
    }
}
