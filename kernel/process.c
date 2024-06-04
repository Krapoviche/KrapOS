#include "process.h"
#include "string.h"
#include "stdio.h"
#include "mem.h"

process_table_t* process_table;

process_table_t* init_process_table(){
    process_table = mem_alloc(sizeof(process_table_t));
    process_table->procs = mem_alloc(sizeof(process_t) * NBPROC);
    process_table->pid_count = 0;
    process_table->running_count = 0;
    return process_table;
}

void scheduler(){
    process_t* elected_proc;
    process_t* running_proc;

    // Find a waiting process
    for(uint32_t i = 0 ; i < process_table->running_count ; i++){
        if(process_table->procs[i].state == WAITING){
            elected_proc = &process_table->procs[i];
            running_proc = process_table->running;

            // Swap running and elected in the queue
            for(uint32_t j = 0 ; j < process_table->running_count ; j++){
                if(process_table->procs[j].pid == running_proc->pid){

                    // Swap
                    process_table->procs[i] = *elected_proc;
                    process_table->procs[j] = *running_proc;

                    // Set state
                    process_table->procs[i].state = RUNNING;
                    process_table->procs[j].state = WAITING;

                    // Set running process
                    process_table->running = elected_proc;

                    // Context switch
                    ctx_sw(running_proc->register_save_zone, elected_proc->register_save_zone);

                    return;
                }
            }
        }
    }
    
    return;
}

int32_t add_process(process_t* new_proc, uint32_t fct_addr){
    int i = 0;
    if(process_table->running_count < NBPROC){
        if(strlen(new_proc->name) < MAX_PROC_NAME_SIZE){
            for(i = 0 ; (uint32_t)i < process_table->running_count ; i++){

                // Replace the first dead process found
                if(process_table->procs[i].state == DEAD){
                    process_table->running_count -= 1;
                    break;
                }
            }
            process_table->pid_count += 1;
            process_table->running_count += 1;

            // Load fields of process to add 
            new_proc->pid = process_table->pid_count;
            new_proc->state = WAITING;
            new_proc->stack[MAX_STACK_SIZE - 1] = fct_addr;
            new_proc->register_save_zone[1] = (uint32_t)&new_proc->stack[MAX_STACK_SIZE - 1];

            // Add new process to process table
            process_table->procs[i] = *new_proc;

            return process_table->pid_count;
        }
    }
    return -1;
}

int get_pid(void){
    return process_table->running->pid;
}

char* get_name(void){
    return process_table->running->name;
}