#include "acpi.h"

RSDP_t * discover_rsdp(void){
    uint64_t* start_address = (uint64_t*) 0xE0000;
    uint64_t* end_address = (uint64_t*) 0xFFFFF;

    for( ; start_address < end_address ;  start_address++){
        if(*start_address == RSDP_SIGNATURE_1){
            RSDP_t* rsdp = (RSDP_t*)start_address;
            uint8_t* bytes = (uint8_t*) rsdp;
            uint8_t checksum = 0;
            for (int i = 0; i < 20; i++) {  // RSDP is 20 bytes for revision 1
                checksum += bytes[i];
            }
            if (checksum == 0) {
                return rsdp;
            }
        }
    }

    return NULL;
}
