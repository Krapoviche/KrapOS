#ifndef __BIOS32_H__
#define __BIOS32_H__

#include "stdint.h"
#include "segment.h"
#include "cpu.h"
#include "string.h"

typedef struct bios32{
    uint32_t signature;
    uint32_t* entrypoint;
    uint8_t revision;
    uint8_t length;
    uint8_t checksum;
    uint8_t reserved[5];
} bios32;

typedef struct bios_service{
	uint32_t signature;
	uint8_t present_status;
	uint8_t major_revision;
	uint8_t minor_revision;
} bios_service;

bios32* find_bios32_service_dir_header();
uint32_t* read_bios32_service(uint32_t service, bios32* bios32_info);

#endif
