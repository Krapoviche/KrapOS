#ifndef __SCREEN_H__
#define __SCREEN_H__

#define NB_COL 80
#define NB_LINE 25
#define MEM_VIDEO 0xB8000

#include "stdint.h"
#include "string.h"
#include "cpu.h"
#include "stdio.h"
#include "it.h"

uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void write_char(uint32_t lig, uint32_t col, char c, uint8_t fc, uint8_t bg);
void reset_screen();
void place_cursor(uint32_t lig, uint32_t col);
void treat_char(char c);
void cleanup_line();
void new_line();
void scroll_down();
void scroll_up();
void display_time(void);
/**
 * @brief Write bytes to the console
 * @param s: string to write
 * @param len: length of the string
*/
void console_putbytes(const char *s, int len);

extern uint16_t CURSOR_COLUMN;
extern uint16_t CURSOR_LINE;

#endif