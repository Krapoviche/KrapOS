#ifndef __SCREEN_H__
#define __SCREEN_H__

#define NB_COL 80
#define NB_LINE 25
#define MEM_VIDEO 0xB8000

#ifndef uint8_t
#include <stdint.h>
#endif

uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void write_char(uint32_t lig, uint32_t col, char c, uint8_t fc, uint8_t bg);
void reset_screen();
void place_cursor(uint32_t lig, uint32_t col);
void treat_char(char c);
void scroll();
void console_putbytes(const char *s, int len);
void display_time(char* s);
#endif