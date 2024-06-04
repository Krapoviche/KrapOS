#include "process.h"
#include "string.h"
#include "stdio.h"
#include "mem.h"

process_table_t* process_table;

// Initialize the process table with proper field values
process_table_t* init_process_table(){
    // Process table itself
    process_table = mem_alloc(sizeof(process_table_t));
    process_table->runnable_queue = mem_alloc(sizeof(link));

    // Initiate process queue
    link head_runnable_queue = LIST_HEAD_INIT(*process_table->runnable_queue);
    memcpy(process_table->runnable_queue,&head_runnable_queue, sizeof(link));
    
    // Default values
    process_table->last_pid = 0;
    process_table->nbproc = 0;
    
    return process_table;
}

void scheduler(){
    process_t* elected_proc;
    process_t* old_proc;

    if(!queue_empty(process_table->runnable_queue)){
        // Store the currently running one as old
        old_proc = process_table->running;

        // Add old process to waiting queue since it does now wait
        queue_add(old_proc,process_table->runnable_queue,process_t,queue_link,priority);

        // Get the process with the most priority in waiting processes
        elected_proc = queue_out(process_table->runnable_queue, process_t, queue_link);
        
        if(elected_proc != old_proc){
            // Update processes state
            old_proc->state = RUNNABLE;
            elected_proc->state = RUNNING;

            // Update running process
            process_table->running = elected_proc;

            // Context switch between the two processes
            ctx_sw(old_proc->register_save_zone, elected_proc->register_save_zone);
        }
    }
    
    return;
}

int32_t add_process(process_t* new_proc, uint32_t fct_addr){
    if(process_table->nbproc < NBPROC){
        if(strlen(new_proc->name) < MAX_PROC_NAME_SIZE){
            process_table->last_pid += 1;
            process_table->nbproc += 1;

            // Load fields of process to add 
            new_proc->pid = process_table->last_pid;
            new_proc->state = RUNNABLE;
            new_proc->stack[MAX_STACK_SIZE - 1] = fct_addr;
            new_proc->register_save_zone[1] = (uint32_t)&new_proc->stack[MAX_STACK_SIZE - 1];
	        
            // Add to waiting queue
            queue_add(new_proc, process_table->runnable_queue,process_t,queue_link,priority);

            return process_table->last_pid;
        }
        return -2;
    }
    return -1;
}

int get_pid(void){
    return process_table->running->pid;
}

char* get_name(void){
    return process_table->running->name;
}