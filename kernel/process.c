#include "process.h"
#include "string.h"
#include "stdio.h"
#include "mem.h"
#include "it.h"

process_table_t* process_table;
uint32_t idle_registers[REGISTER_SAVE_COUNT];

// Initialize the process table with proper field values
process_table_t* init_process_table(){
    // Process table itself
    process_table = mem_alloc(sizeof(process_table_t));
    process_table->runnable_queue = mem_alloc(sizeof(link));
    process_table->sleeping_queue = mem_alloc(sizeof(link));
    process_table->dead_queue = mem_alloc(sizeof(link));

    // Initiate process queues
    link head_runnable_queue = LIST_HEAD_INIT(*process_table->runnable_queue);
    link head_sleeping_queue = LIST_HEAD_INIT(*process_table->sleeping_queue);
    link head_dead_queue = LIST_HEAD_INIT(*process_table->dead_queue);

    memcpy(process_table->runnable_queue,&head_runnable_queue, sizeof(link));
    memcpy(process_table->sleeping_queue,&head_sleeping_queue, sizeof(link));
    memcpy(process_table->dead_queue,&head_dead_queue, sizeof(link));
    
    // Default values
    process_table->last_pid = 0;
    process_table->nbproc = 0;
    
    return process_table;
}

void scheduler(){
    // Handle dead processes
    clear_dead_processes();

    // Handle sleeping processes
    seek_for_awaking_processes();
    
    process_t* elected_proc;
    // Store the currently running one as old
    process_t* old_proc = process_table->running;

        if (old_proc->state == RUNNING){
            // Add old process to waiting queue since it does now wait
            queue_add(old_proc,process_table->runnable_queue,process_t,queue_link,priority);
        }

    // Get the process with the most priority in waiting processes
    elected_proc = queue_out(process_table->runnable_queue, process_t, queue_link);
    
    // Avoid ctx switch if useless (if running process is still the highest priority)
    if(elected_proc != old_proc){
        // Update processes state
        if(old_proc->state == DYING){
            old_proc->priority = 0;
            queue_add(old_proc,process_table->dead_queue,process_t,queue_link,priority);
        } else if (old_proc-> state != SLEEPING) {
            old_proc->state = RUNNABLE;
        }
        elected_proc->state = RUNNING;

        // Update running process
        process_table->running = elected_proc;

        // Context switch between the two processes
        ctx_sw(old_proc->register_save_zone, elected_proc->register_save_zone);
        return;
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
            new_proc->stack[MAX_STACK_SIZE - 2] = fct_addr;
            new_proc->stack[MAX_STACK_SIZE - 1] = (uint32_t)end_process;
            new_proc->register_save_zone[1] = (uint32_t)&new_proc->stack[MAX_STACK_SIZE - 2];
	        
            // Add to waiting queue
            queue_add(new_proc, process_table->runnable_queue,process_t,queue_link,priority);

            return process_table->last_pid;
        }
        return -2;
    }
    return -1;
}

void sleep(uint32_t secs) {
    uint32_t start = uptime();
    process_table->running->state = SLEEPING;
    
    // (start + secs) = time when the process should wake up
    // UINT32_MAX - this to revert the value
    // Since the queue priority is higher first
    process_table->running->wake_up_time = UINT32_MAX - (start + secs);
    queue_add(process_table->running, process_table->sleeping_queue, process_t, queue_link, wake_up_time);
    scheduler();
}

/**
 * Wake-up processes that need to and put it back to the runnable queue
*/
void seek_for_awaking_processes(){
    process_t* proc;
    while (!queue_empty(process_table->sleeping_queue)
        && ( proc = queue_top(process_table->sleeping_queue, process_t, queue_link) )->wake_up_time >= UINT32_MAX - uptime() )
    {
        proc->state = RUNNABLE;
        queue_del(proc, queue_link);
        queue_add(proc, process_table->runnable_queue, process_t, queue_link, priority);
    }
    return;
}

void end_process(){
    process_table->running->state = DYING;
    scheduler();
}

void clear_dead_processes(){
    process_t* process;
    while(!queue_empty(process_table->dead_queue)){
        process = queue_out(process_table->dead_queue,process_t,queue_link);
        mem_free(process, sizeof(process_t));
        process_table->nbproc -= 1;
    }
}

int get_pid(void){
    return process_table->running->pid;
}

char* get_name(void){
    return process_table->running->name;
}