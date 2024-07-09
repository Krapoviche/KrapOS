#include "intel.h"

pci_capability* get_intel_pci_capability(pci_device* device){
    pci_capability* capability = device->capabilities;

    while (capability != NULL && capability->capability_id != 0x1)
    {
        capability = capability->next;
    }

    return capability;
}

void retrieve_intel_pci_max_link_speed(pci_device* device, pci_capability* capability){
    uint16_t value;
    // printf("reading, %x", capability->offset);
    pci_bios_read_config_word(device->bus, (device->slot << 3) + device->function, capability->offset + 4, &value);

    printf("VERSION ID %x\n", value & 0b11);
}
