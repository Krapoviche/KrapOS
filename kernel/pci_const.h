#ifndef __PCI_CONST_H__
#define __PCI_CONST_H__

// PCI INPUT CODES

#define PCIBIOS_PCI_BIOS_PRESENT 	        0xb101
#define PCIBIOS_FIND_PCI_DEVICE 	        0xb102
#define PCIBIOS_FIND_PCI_CLASSCODE 	        0xb103
#define PCIBIOS_GENERATE_SPECIAL_CYCLE	    0xb106
#define PCIBIOS_READ_CONFIG_BYTE    	    0xb108
#define PCIBIOS_READ_CONFIG_WORD	        0xb109
#define PCIBIOS_READ_CONFIG_DWORD	        0xb10a
#define PCIBIOS_WRITE_CONFIG_BYTE	        0xb10b
#define PCIBIOS_WRITE_CONFIG_WORD	        0xb10c
#define PCIBIOS_WRITE_CONFIG_DWORD	        0xb10d
#define PCIBIOS_GET_IRQ_ROUTING_OPTIONS	    0xb10e
#define PCIBIOS_PCI_IRQ                     0xb10f

// PCI CLASSES

#define PCI_CLASS_UNCLASSED                 0x00
#define PCI_CLASS_STORAGE                   0x01
#define PCI_CLASS_NETWORK                   0x02
#define PCI_CLASS_DISPLAY                   0x03
#define PCI_CLASS_MULTIMEDIA                0x04
#define PCI_CLASS_MEMORY                    0x05
#define PCI_CLASS_BRIDGE                    0x06
#define PCI_CLASS_SIMPLE_COMMUNICATION      0x07
#define PCI_CLASS_BASE_PERIPHERAL           0x08
#define PCI_CLASS_INPUT_DEVICE              0x09
#define PCI_CLASS_DOCKING                   0x0a
#define PCI_CLASS_PROCESSOR                 0x0b
#define PCI_CLASS_SERIAL_BUS                0x0c
#define PCI_CLASS_WIRELESS_CONTROLLER       0x0d
#define PCI_CLASS_INTELLIGENT_IO            0x0e
#define PCI_CLASS_SATELLITE_COMMUNICATION   0x0f
#define PCI_CLASS_ENCRYPTION_DECRYPTION     0x10
#define PCI_CLASS_DATA_SIGNAL               0x11
#define PCI_CLASS_ACCELERATORS              0x12
#define PCI_CLASS_NON_ESSENTIAL             0x13
#define PCI_CLASS_UNDEFINED                 0xff

#define PCI_SERVICE		(('$' << 0) + ('P' << 8) + ('C' << 16) + ('I' << 24))
#define PCI_SIGNATURE		(('P' << 0) + ('C' << 8) + ('I' << 16) + (' ' << 24))

#endif
