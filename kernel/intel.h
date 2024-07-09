#ifndef __INTEL_H__
#define __INTEL_H__

#include "pci.h"

pci_capability* get_intel_pci_capability(pci_device* device);
void retrieve_intel_pci_max_link_speed(pci_device* device, pci_capability* capability);

#endif
