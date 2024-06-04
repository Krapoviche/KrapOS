#include <stdio.h>
#include "cpu.h"
#include "it.h"
#include "screen.h"
#include "segment.h"

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

void tic_PIT(void){
    outb(0x20,0x20);
    ticks++;
    uint32_t processor_time = ticks / 50;

    char formatted_time[9];
    sprintf(formatted_time,"%02d:%02d:%02d",processor_time/3600,processor_time/60, processor_time%60);
    display_time(formatted_time);
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