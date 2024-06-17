#include "kbd.h"
#include "stdio.h"
#include "cpu.h"
#include "mem.h"
#include "string.h"
#include "process.h"
#include "queue.h"

kbd_buf keyboard_buffer;
bool writing;
int echo = 1;

void init_keyboard_buffer(void){
    keyboard_buffer.count = 0;
    keyboard_buffer.write_head = 0;
    keyboard_buffer.read_head = 0;
    writing = false;
}

void kbd_int(void){
    // Read the scancode from the keyboard port
    unsigned char scancode = inb(0x60);

    // Acknowledge the interruption
    outb(0x20,0x20);

    // Translate scancode to a character
    do_scancode((char) scancode);
}

void keyboard_data(char *str){
    int i = 0;
    char c = str[i];

    while(c != '\0' && writing){
        if(keyboard_buffer.count < KBD_BUF_SIZE){
            // Write typed char to the kbd buffer
            keyboard_buffer.buf[keyboard_buffer.write_head] = c;

            // If the char is a backspace, remove the last char from the buffer
            if (c == 127) {
                if (keyboard_buffer.count > 0) {
                    keyboard_buffer.write_head = (keyboard_buffer.write_head - 1) % KBD_BUF_SIZE;
                    keyboard_buffer.count--;
                }
            } else {
                // Echo the char to the console if enabled
                if (echo) {
                    if (c == '\r') cons_write("\n", 1);
                    cons_write(&c, 1);
                }
                // Increase kbd buffers counters
                keyboard_buffer.write_head = (keyboard_buffer.write_head + 1) % KBD_BUF_SIZE;
                keyboard_buffer.count++;
            }

            // Wake up process waiting for I/O
            if(!queue_empty(process_table->io_queue) && c == 13){
                process_t* proc = queue_out(process_table->io_queue, process_t, queue_link);
                queue_add(proc, process_table->runnable_queue, process_t, queue_link, priority);
                proc->state = RUNNABLE;
                scheduler();
            }
        }
        i++;
        c = str[i];
    }
}

void kbd_leds(unsigned char leds){
    // Write the led status to the keyboard port
    outb(0xED, 0x60);
    for (int i = 0; i < 100000; i++);
    outb(leds, 0x60);
}