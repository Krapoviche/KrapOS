#include "bios32.h"

static struct {
	unsigned long address;
	unsigned short segment;
} bios32_indirect = { 0, KERNEL_CS };

bios32 * find_bios32_service_dir_header(){
    uint32_t* start_address = (uint32_t*) 0xE0000;
    uint32_t* end_address = (uint32_t*) 0xFFFFF;

    for( ; start_address < end_address ;  start_address++){
        if(*start_address == 0x5F32335F){
            return (bios32*)start_address;
        }
    }
    return (bios32*)0xFFFF;
}

uint32_t* read_bios32_service(uint32_t service, bios32* bios32_info){
    bios32_indirect.address = (uint32_t) bios32_info->entrypoint;

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
