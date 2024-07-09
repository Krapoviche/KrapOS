#ifndef __ACPI_H__
#define __ACPI_H__

#include "stdio.h"
#include "stdint.h"

#define RSDP_SIGNATURE_1 0x2052545020445352  // "RSD PTR "

typedef struct ACPISDTHeader_t {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__ ((packed)) ACPISDTHeader_t;

typedef struct RSDP_t {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 ACPISDTHeader_t* RsdtAddress;
} __attribute__ ((packed)) RSDP_t;

RSDP_t * discover_rsdp(void);

#endif
