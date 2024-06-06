#include "process.h"
#include "string.h"
#include "stdio.h"
#include "mem.h"
#include "it.h"
#include "stdarg.h"

process_table_t* process_table;
uint32_t idle_registers[REGISTER_SAVE_COUNT];

// Initialize the process table with proper field values
process_table_t* init_process_table(){
    // Process table itself
    process_table = mem_alloc(sizeof(process_table_t));
    process_table->runnable_queue = mem_alloc(sizeof(link));
    process_table->sleeping_queue = mem_alloc(sizeof(link));
    process_table->dead_queue = mem_alloc(sizeof(link));
    process_table->zombie_queue = mem_alloc(sizeof(link));

    // Initiate process queues
    link head_runnable_queue = LIST_HEAD_INIT(*process_table->runnable_queue);
    link head_sleeping_queue = LIST_HEAD_INIT(*process_table->sleeping_queue);
    link head_dead_queue = LIST_HEAD_INIT(*process_table->dead_queue);
    link head_zombie_queue = LIST_HEAD_INIT(*process_table->zombie_queue);

    memcpy(process_table->runnable_queue,&head_runnable_queue, sizeof(link));
    memcpy(process_table->sleeping_queue,&head_sleeping_queue, sizeof(link));
    memcpy(process_table->dead_queue,&head_dead_queue, sizeof(link));
    memcpy(process_table->zombie_queue,&head_zombie_queue, sizeof(link));
    
    // Default values
    process_table->last_pid = 0;
    process_table->nbproc = 0;
    
    return process_table;
}

/**
 * Scheduler calling the context switch after checking for processes dying, sleeping and electing the next process to run
*/
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

int32_t cancel_start(uint32_t err_code, process_t* created_proc) {
    mem_free(created_proc, sizeof(process_t));
    return err_code;
}

/**
 * Starts a process
 * @param pt_func: function pointer to the process
 * @param ssize: stack size
 * @param prio: priority
 * @param name: process name
 * @param argc: number of arguments given after this one
 * @param ...: arguments for the executed function
*/
int32_t start(int (*pt_func)(void*), uint32_t ssize, int prio, const char *name, uint32_t argc, ...){
    // Allocate memory for the new process
    process_t* new_proc = mem_alloc(sizeof(process_t));
    // Set the proc prio
    new_proc->priority = prio;
    // Set the proc name
    new_proc->name[0] = '\0';
    new_proc->stack_size = ssize;
    strcpy(new_proc->name, name);

    // Check if the process can be added
    if(process_table->nbproc < NBPROC){
        // Check if the name is not too long
        if(strlen(new_proc->name) < MAX_PROC_NAME_SIZE){
            // Check the stack size requirements
            if (ssize <= MAX_STACK_SIZE && ssize > 2+argc) {
                va_list args;
                // Start reading the params ... list
                va_start(args, argc);
                // Increment the last used pid and the number of processes existing
            process_table->last_pid += 1;
            process_table->nbproc += 1;

                // Set the process pid & state
            new_proc->pid = process_table->last_pid;

            // Set Parent PID, add to parent's children
            // After checking wether it's orphan processes or not
            if(process_table->running != NULL){
                new_proc->ppid = process_table->running->pid;
                queue_add(new_proc,process_table->running->children,process_t,parent_link,pid);
            } else {
                new_proc->ppid = -1;
            }

            // Initialize Children queue
            new_proc->children = mem_alloc(sizeof(link));
            link head_children_queue = LIST_HEAD_INIT(*new_proc->children);
            memcpy(new_proc->children, &head_children_queue, sizeof(link));

            new_proc->state = RUNNABLE;
                // Allocate memory for the stack & fill it with the function pointer and the stop function as exit()
                new_proc->stack = mem_alloc(sizeof(uint32_t)*ssize);
                new_proc->stack[ssize - argc - 2] = (uint32_t)pt_func;
                new_proc->stack[ssize - argc - 1] = (uint32_t)stop;
                // Potential params in the stack
                for(uint32_t i = 0; i < argc; i++) {
                    uint32_t arg = va_arg(args, uint32_t);
                    new_proc->stack[ssize - argc + i] = arg;
                }
                new_proc->register_save_zone[1] = (uint32_t)&new_proc->stack[ssize - argc - 2];
	        
            // Add to waiting queue
            queue_add(new_proc, process_table->runnable_queue,process_t,queue_link,priority);

                va_end(args);

            return process_table->last_pid;
            }
            // If error call for cancel_start to free the memory allocated for the process
            return cancel_start(-3, new_proc);
        }
        // If error call for cancel_start to free the memory allocated for the process
        return cancel_start(-2, new_proc);
    }
    // If error call for cancel_start to free the memory allocated for the process
    return cancel_start(-1, new_proc);
}

/**
 * Stops the current process
*/
void stop(void){
    process_table->running->state = DYING;
    scheduler();
}

void wait_clock(uint32_t ticks){
    uint32_t start = current_clock();
    process_table->running->state = SLEEPING;
    process_table->running->wake_up_time = UINT32_MAX - (start + ticks);
    queue_add(process_table->running, process_table->sleeping_queue, process_t, queue_link, wake_up_time);
    scheduler();
}

void sleep(uint32_t secs) {
    wait_clock(secs * CLOCKFREQ);
}

/**
 * Wake-up processes that need to and put it back to the runnable queue
*/
void seek_for_awaking_processes(){
    process_t* proc;
    while (!queue_empty(process_table->sleeping_queue)
        && ( proc = queue_top(process_table->sleeping_queue, process_t, queue_link) )->wake_up_time >= UINT32_MAX - current_clock() )
    {
        proc->state = RUNNABLE;
        queue_del(proc, queue_link);
        queue_add(proc, process_table->runnable_queue, process_t, queue_link, priority);
    }
    return;
}

void clear_dead_processes(){
    process_t* process;
    process_t* child;

    while(!queue_empty(process_table->dead_queue)){
        // Get all dying processes
        process = queue_out(process_table->dead_queue,process_t,queue_link);

        // Anyway, children of this process should die
        queue_for_each(child,process->children,process_t,parent_link){
            child->ppid = -1;
            child->state = DYING;
            queue_del(child,queue_link);
            queue_add(child,process_table->dead_queue,process_t,queue_link,priority);
        }

        // If a parent exists, this process becomes a ZOMBIE until it's parent dies
        if(process->ppid != -1){
            process->state = ZOMBIE;
            queue_add(process,process_table->zombie_queue,process_t,queue_link,priority);
        }
        // It there is no parent, this process should now be deleted
        else {
            mem_free(process->children, sizeof(link));
            mem_free(process->stack, sizeof(uint32_t)*process->stack_size);
            mem_free(process, sizeof(process_t));
        process_table->nbproc -= 1;
        }

    }
}

int get_pid(void){
    return process_table->running->pid;
}

char* get_name(void){
    return process_table->running->name;
}
