#ifndef __VIRTIO_H__
#define __VIRTIO_H__

#include "stdint.h"
#include "pci.h"

typedef struct virtio_pci_cap {
    uint8_t cap_vndr;
    uint8_t cap_next;
    uint8_t cap_len;
    uint8_t cfg_type;
    uint8_t bar;
    uint8_t padding[3];
    uint32_t offset;
    uint32_t length;
} virtio_pci_cap;


void test_somth(pci_device* device);

#endif
