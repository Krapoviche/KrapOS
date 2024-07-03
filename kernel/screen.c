#include "screen.h"

uint16_t CURSOR_LINE = 0;
uint16_t CURSOR_COLUMN = 0;

uint16_t *ptr_mem(uint32_t lig, uint32_t col){
    return (uint16_t *)(0xB8000 + 2 * (lig * NB_COL + col)); // Calcul de l'addresse en mémoire. On caste l'entier en uint16
}

process_t* find_target_shell() {
    if (process_table->running_shell < 0) return NULL;
    if (echoing) {
        if (queue_empty(process_table->io_queue)) return NULL;
        return queue_top(process_table->io_queue, process_t, queue_link);
    } else {
        return get_process(process_table->running->shell_pid);
    }
}

void write_char(uint32_t lig, uint32_t col, char c, uint8_t fc, uint8_t bg) {
    uint16_t tmp = c | (bg << 4) | (fc << 8);
    *ptr_mem(lig,col) = tmp;
    process_t* shell;
    if (echoing) outb('1', 0xE9);
    else outb('0', 0xE9);
    if ((shell = find_target_shell())) {
        shell->shell_props->screen_buffer->visible_screen[col + lig*NB_COL] = tmp;
    }
}

void reset_screen() {
    for (int i = 0; i < NB_COL -1 ; i++){
        for (int j = 0; j < NB_LINE - 1 ; j++){
            write_char(j,i,' ', 15, 0);
        }
    }
}

void cleanup_line() {
    for(int i = 0 ; i < NB_COL ; i++){
        write_char(CURSOR_LINE, i, ' ', 15, 0);
    }
}

void scroll_up() {
    process_t* shell;
    if (process_table->running_shell >= 0 && (shell = get_process(process_table->running_shell))) {
        screen_buf_t* screen_buffer = shell->shell_props->screen_buffer;
        if (screen_buffer->older_lines > 0 && screen_buffer->newer_lines < SCREEN_BUFFER_LEN) {
            uint16_t* source = ptr_mem(0,0);
            uint16_t* dest = ptr_mem(1,0);
            uint16_t* bottom_line = ptr_mem(NB_LINE - 1, 0);

            // Save the bottom line
            uint16_t* tmp = (uint16_t*)mem_alloc(sizeof(uint16_t) * NB_COL);
            memmove(tmp, bottom_line, sizeof(uint16_t) * NB_COL);
            // Move the visible screen lines down
            memmove(dest, source, sizeof(uint16_t) * NB_COL * (NB_LINE - 1));
            // Visible screen operations
            uint16_t cur_line = CURSOR_LINE;
            CURSOR_LINE = 0;
            cleanup_line();
            CURSOR_LINE = cur_line;
            // Move the most recent line to the top of the screen
            memmove(source, screen_buffer->buf[--screen_buffer->older_lines], sizeof(uint16_t) * NB_COL);
            mem_free(screen_buffer->buf[screen_buffer->older_lines], sizeof(uint16_t) * NB_COL);
            // Move the saved bottom line to the buffer
            screen_buffer->newer_lines += 1;
            screen_buffer->buf[SCREEN_BUFFER_LEN - screen_buffer->newer_lines] = tmp;
        }
    }
}

void scroll_down() {
    process_t* shell;
    if (process_table->running_shell >= 0 && (shell = get_process(process_table->running_shell))) {
        screen_buf_t* screen_buffer = shell->shell_props->screen_buffer;
        if (screen_buffer->newer_lines > 0 && screen_buffer->older_lines < SCREEN_BUFFER_LEN) {

            uint16_t* source = ptr_mem(1,0);
            uint16_t* dest = ptr_mem(0,0);
            uint16_t* bottom_line = ptr_mem(NB_LINE - 1, 0);

            // Save the top line
            uint16_t* tmp = (uint16_t*)mem_alloc(sizeof(uint16_t) * NB_COL);
            memmove(tmp, dest, sizeof(uint16_t) * NB_COL);
            // Move the visible screen lines up
            memmove(dest, source, sizeof(uint16_t) * NB_COL * (NB_LINE - 1));
            // Visible screen operations
            uint16_t cur_line = CURSOR_LINE;
            CURSOR_LINE = NB_LINE - 1;
            cleanup_line();
            CURSOR_LINE = cur_line;
            // Move the most recent line to the bottom of the screen
            memmove(bottom_line, screen_buffer->buf[SCREEN_BUFFER_LEN - screen_buffer->newer_lines], sizeof(uint16_t) * NB_COL);
            mem_free(screen_buffer->buf[SCREEN_BUFFER_LEN - screen_buffer->newer_lines], sizeof(uint16_t) * NB_COL);
            // Move the saved bottom line to the buffer
            screen_buffer->newer_lines -= 1;
            screen_buffer->buf[screen_buffer->older_lines++] = tmp;
        }
    }
}

void new_line() {
    process_t* shell;
    if ((shell = find_target_shell())) {
        screen_buf_t* screen_buffer = shell->shell_props->screen_buffer;

        uint16_t* source = ptr_mem(1,0);
        uint16_t* dest = ptr_mem(0,0);
        // If the buffer is not full yet, add a line to it
        if (screen_buffer->older_lines < SCREEN_BUFFER_LEN) {
            // Increase the total number of lines (screen + buffer)
            screen_buffer->total_lines += 1;
            // If the screen is full
            if (screen_buffer->total_lines > NB_LINE) {
                // Save the top line to the first available buffer line
                screen_buffer->buf[screen_buffer->older_lines] = mem_alloc(sizeof(uint16_t) * NB_COL);
                memmove(screen_buffer->buf[screen_buffer->older_lines++], dest, sizeof(uint16_t) * NB_COL);
            }
        } else {
            // Free the oldest buffer line
            mem_free(screen_buffer->buf[0], sizeof(uint16_t) * NB_COL);
            // If the buffer is full, move all the lines up (down in the buffer)
            for (uint8_t i = 1; i < SCREEN_BUFFER_LEN; i++) {
                screen_buffer->buf[i-1] = screen_buffer->buf[i];
            }
            // Save the top line to the last buffer line
            screen_buffer->buf[SCREEN_BUFFER_LEN-1] = mem_alloc(sizeof(uint16_t) * NB_COL);
            memmove(screen_buffer->buf[SCREEN_BUFFER_LEN-1], dest, sizeof(uint16_t) * NB_COL);
        }
        // Visible screen operations
        memmove(dest, source, sizeof(uint16_t) * NB_COL * (NB_LINE - 1));
        CURSOR_LINE -= 1;
        cleanup_line();
        place_cursor(CURSOR_LINE - 1, 0);
    }
}

void place_cursor(uint32_t lig, uint32_t col) {
    uint16_t pos = col + ( lig * 80 );

    outb(0x0F,0x3D4);
    outb(pos ,0x3D5);
    outb(0x0E,0x3D4);
    pos = pos >> 8;
    outb(pos ,0x3D5);
    CURSOR_LINE = lig;
    CURSOR_COLUMN = col;
}

void treat_char(char c) {
    process_t* shell;
    if ((shell = find_target_shell())) {
        screen_buf_t* screen_buffer = shell->shell_props->screen_buffer;
        while (screen_buffer->newer_lines > 0) {
            scroll_down();
        }
        switch (c) {
            case '\b' :
                if(CURSOR_COLUMN != 0){
                    CURSOR_COLUMN -= 1;
                    write_char(CURSOR_LINE,CURSOR_COLUMN,' ',15,0);
                    break;
                }
                CURSOR_COLUMN = NB_COL - 1;
                CURSOR_LINE -= 1;
                write_char(CURSOR_LINE,CURSOR_COLUMN,' ',15,0);
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
                screen_buffer->total_lines += 1;

                if(CURSOR_LINE >= NB_LINE){
                    new_line();
                    CURSOR_LINE = NB_LINE - 1;
                }
                save_screen();

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
                        outb('a', 0xE9);
                        new_line();
                        CURSOR_LINE = NB_LINE - 1;
                    }
                }
                break;
            
        }
        
        if ((shell = find_target_shell())) {
            screen_buf_t* screen_buf = shell->shell_props->screen_buffer;
            screen_buf->cursor_pos[0] = CURSOR_LINE;
            screen_buf->cursor_pos[1] = CURSOR_COLUMN;
        }

        place_cursor(CURSOR_LINE,CURSOR_COLUMN);
    }
}

void display_time(void) {
    uint32_t processor_time = current_clock();
    char formatted_time[9];
    uint32_t secs = processor_time/CLOCKFREQ;
    sprintf(formatted_time,"%02d:%02d:%02d", secs/3600, secs/60, secs%60);
    for (int i = 0; i<8; i++) {
        printf("%c", formatted_time[i]);
    }
}

void console_putbytes(const char *s, int len){
    for(int i = 0 ; i < len ; i++){
        treat_char(*(s+(i * sizeof(char))));
    }
}