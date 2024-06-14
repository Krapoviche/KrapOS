#ifndef __IT_H__
#define __IT_H__

#define IDT 0x1000
#define CST_IT 0x8E00
#define USER_CST_IT 0xEE00

#ifndef uint8_t
#include <stdint.h>
#include <stdbool.h>
#endif
#define QUARTZ 0x1234DD
#define CLOCKFREQ 1000

extern void IT_32_handler();
extern void IT_49_handler();
void init_IT_handlers(int32_t num_IT, void (*traitant)(void));
void tic_PIT(void);
uint32_t current_clock();
void clock_settings(unsigned long *quartz, unsigned long *ticks);
void mask_IRQ(uint32_t IRQ_number, bool mask);
void init_clock(void);

#endif
