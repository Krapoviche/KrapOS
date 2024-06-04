#include <stdio.h>
#include "cpu.h"
#include "it.h"
#include "screen.h"
#include "segment.h"
#include "process.h"

uint32_t ticks;

void init_IT_handlers(int32_t num_IT, void (*traitant)(void)) {
    uint32_t* tableaddr = (uint32_t *)(IDT + 8 * num_IT);

    *tableaddr = (KERNEL_CS << 16) + ((uint32_t) traitant & 0x0000FFFF);
    tableaddr += 1;
    *tableaddr = ((uint32_t)traitant & 0xFFFF0000) + (CST_IT);
}

void init_clock(void){
    outb(0x34,0x43);
    outb((QUARTZ / CLOCKFREQ) & 0xFF, 0x40);
    outb((QUARTZ / CLOCKFREQ) >> 8, 0x40);
}

uint32_t uptime() {
    return ticks / CLOCKFREQ;
}

void tic_PIT(void){
    outb(0x20,0x20);
    ticks++;

    scheduler();
}

void mask_IRQ(uint32_t IRQ_number, bool mask) {
    uint8_t bits_tab;
    bits_tab = (uint8_t) (inb(0x21));
    if (mask) {
        bits_tab |= 1 << IRQ_number;
    } else {
        bits_tab &= ~(1 << IRQ_number);
    }
    outb(bits_tab, 0x21);
}