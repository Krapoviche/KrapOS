#include "kbd.h"
#include "stdio.h"
#include "cpu.h"
#include "mem.h"
#include "string.h"
#include "process.h"
#include "queue.h"

kbd_buf keyboard_buffer;
bool writing;

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
        if(keyboard_buffer.count != KBD_BUF_SIZE){
            // Write typed char to the kbd buffer
            keyboard_buffer.buf[keyboard_buffer.write_head] = c;

            // printf("%d\n",keyboard_buffer.read_head);
            // printf("%c\n",keyboard_buffer.buf[keyboard_buffer.read_head]);

            // Increase kbd buffers counters
            keyboard_buffer.write_head = (keyboard_buffer.write_head + 1) % KBD_BUF_SIZE;
            keyboard_buffer.count++;

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

int cons_read(char *string, unsigned long length){
    if(length <= 0) return 0;
    writing = true;
    long unsigned int read = 0;
    char buffer[length];
    
    // If precedent call left a cons_read 
    if(keyboard_buffer.buf[keyboard_buffer.read_head] == 13){
        keyboard_buffer.count--;
        keyboard_buffer.read_head = (keyboard_buffer.read_head + 1) % KBD_BUF_SIZE;
        return 0;
    }

    // Lock until 13 char
    process_table->running->state = LOCKED_IO;
    queue_add(process_table->running, process_table->io_queue, process_t, queue_link, priority);
    scheduler();

    for(read = 0 ; read < length ; read++){
        if(keyboard_buffer.buf[keyboard_buffer.read_head] == 13){
            keyboard_buffer.read_head = (keyboard_buffer.read_head + 1) % KBD_BUF_SIZE;
            keyboard_buffer.count--;
            break;
        }
        // Copy keyboard buffer to final buffer, atomically
        buffer[read] = keyboard_buffer.buf[keyboard_buffer.read_head];
        keyboard_buffer.count--;
        keyboard_buffer.read_head = (keyboard_buffer.read_head + 1) % KBD_BUF_SIZE;
    }

    // Copy to caller buffer
    memcpy(string, buffer, read);

    // End of writing phase
    writing = false;

    return read;
}
