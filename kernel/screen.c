#include "screen.h"
#include "string.h"
#include "cpu.h"
#include "stdio.h"
#include "it.h"

uint16_t CURSOR_LINE = 0;
uint16_t CURSOR_COLUMN = 0;

uint16_t *ptr_mem(uint32_t lig, uint32_t col){
    return (uint16_t *)(0xB8000 + 2 * (lig * NB_COL + col)); // Calcul de l'addresse en mémoire. On caste l'entier en uint16
}

void write_char(uint32_t lig, uint32_t col, char c, uint8_t fc, uint8_t bg){
    uint16_t *ptr = ptr_mem(lig,col);
    *ptr = c | (bg << 4) | (fc << 8);
}

void reset_screen(){
    for (int i = 0; i < NB_COL -1 ; i++){
        for (int j = 0; j < NB_LINE - 1 ; j++){
            write_char(j,i,' ', 15, 0);
        }
    }
}

void place_cursor(uint32_t lig, uint32_t col){
    uint16_t pos = col + ( lig * 80 );

    outb(0x0F,0x3D4);
    outb(pos ,0x3D5);
    outb(0x0E,0x3D4);
    pos = pos >> 8;
    outb(pos ,0x3D5);
    CURSOR_LINE = lig;
    CURSOR_COLUMN = col;
}

void treat_char(char c){
    switch (c) {
        case '\b' :
            if(CURSOR_COLUMN != 0){
                CURSOR_COLUMN -= 1;
            }
            break;

        case '\t' :
            uint16_t new_col = CURSOR_COLUMN + (8 - CURSOR_COLUMN % 8);
            if(new_col <= NB_COL){
                CURSOR_COLUMN = new_col;
                break;
            }
            CURSOR_COLUMN = NB_COL - 1;
            break;

        case '\n' :
            CURSOR_LINE += 1;
            CURSOR_COLUMN = 0;

            if(CURSOR_LINE >= NB_LINE){
                scroll();
                CURSOR_LINE = NB_LINE - 1;
            }

            break;

        case '\f' :
            reset_screen();
            CURSOR_LINE = 0;
            CURSOR_COLUMN = 0;
            break;

        case '\r' : 
            CURSOR_COLUMN = 0;
            break;
            
        default :
            write_char(CURSOR_LINE,CURSOR_COLUMN,c,15,0);
            if(CURSOR_COLUMN == NB_COL){
                CURSOR_COLUMN = 0;
                CURSOR_LINE += 1;
            } else {
                CURSOR_COLUMN += 1;

                if(CURSOR_LINE >= NB_LINE){
                    scroll();
                    CURSOR_LINE = NB_LINE - 1;
                }
            }
            break;
        
    }

    place_cursor(CURSOR_LINE,CURSOR_COLUMN);
}

void scroll(){
    uint16_t* source = ptr_mem(1,0);
    uint16_t* dest = ptr_mem(0,0);

    memmove(dest, source, sizeof(uint16_t) * NB_COL * (NB_LINE - 1));

    CURSOR_LINE -= 1;
    for(int i = 0 ; i < NB_COL ; i++){
        write_char(CURSOR_LINE, i, ' ', 15, 0);
    }
    place_cursor(CURSOR_LINE - 1, 0);
}

void console_putbytes(const char *s, int len){
    for(int i = 0 ; i < len ; i++){
        treat_char(*(s+(i * sizeof(char))));
    }
}

void display_time(void) {
    uint32_t processor_time = current_clock();
    char formatted_time[9];
    sprintf(formatted_time,"%02d:%02d:%02d",processor_time/3600,processor_time/60, processor_time%60);
    for (int i = 0; i<8; i++) {
        write_char(0, NB_COL-8+i, formatted_time[i], 15, 0);
    }
}