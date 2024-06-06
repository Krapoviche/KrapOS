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

int32_t alloc_free_pid(process_t* proc){
    for(int i = 0; i < NBPROC; i++){
        if(process_table->table[i] == NULL){
            process_table->table[i] = proc;
            return i;
        }
    }
    return -1;
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
int32_t start_multi_args(int (*pt_func)(void*), uint32_t ssize, int prio, const char *name, uint32_t argc, ...){
    // Allocate memory for the new process
    process_t* new_proc = mem_alloc(sizeof(process_t));
    new_proc->waiting_for = -2; // -1 is actually used for waiting for any child
    new_proc->awaken_by = -1;
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
            process_table->nbproc += 1;

                // Set the process pid & state
            new_proc->pid = alloc_free_pid(new_proc);

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
                new_proc->stack[ssize - argc - 1] = (uint32_t)exit;
                // Potential params in the stack
                for(uint32_t i = 0; i < argc; i++) {
                    void* arg = va_arg(args, void*);
                    new_proc->stack[ssize - argc + i] = (uint32_t)arg;
                }
                new_proc->register_save_zone[1] = (uint32_t)&new_proc->stack[ssize - argc - 2];

            // Add to waiting queue
                queue_add(new_proc, process_table->runnable_queue,process_t,queue_link,priority);

                va_end(args);

                return new_proc->pid;
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
 * Starts a process
 * @param pt_func: function pointer to the process
 * @param ssize: stack size
 * @param prio: priority
 * @param name: process name
 * @param arg: argument for the executed function
*/
int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg) {
    return start_multi_args(ptfunc, ssize, prio, name, 1, arg);
}

/**
 * Stops the current process
 * @param retval: return value of the process
*/
void exit(int retval){
    process_t* child = process_table->running;
    if(retval == child->pid - 1){
        retval = 0;
    }
    child->retval = retval;
    child->state = DYING;
    int32_t ppid = child->ppid;
    if (ppid >= 0 && process_table->table[ppid] != NULL) {

        int32_t waiting_for = process_table->table[ppid]->waiting_for;
        // If parent is waiting for this child or for any child
        if (waiting_for == child->pid || waiting_for == -1){

            process_table->table[ppid]->waiting_for = -2; // reset parent waiting_for
            process_table->table[ppid]->awaken_by = child->pid; // tell parent who woke it up
            process_table->table[ppid]->state = RUNNABLE; // Wake up parent
            queue_add(process_table->table[ppid], process_table->runnable_queue, process_t, queue_link, priority);
        }
    }
    scheduler();
    while(1);
}

/**
 * Wait for a certain amount of clock ITs
 * @param ticks: number of clock IT to wait for
*/
void wait_clock(uint32_t ticks){
    uint32_t start = current_clock();
    process_table->running->state = SLEEPING;
    // (start + ticks) = time when the process should wake up
    // we count backwards "UINT32_MAX - (start + ticks)" to have the queue sorted in the right way
    // In reality the uptime will still be start + ticks when it should wake up
    process_table->running->wake_up_time = UINT32_MAX - (start + ticks);
    queue_add(process_table->running, process_table->sleeping_queue, process_t, queue_link, wake_up_time);
    scheduler();
}

void sleep(uint32_t secs) {
    wait_clock(secs * CLOCKFREQ);
}

/**
 * Wait for a child process to end
 * @param pid: child process id to wait for (-1 waits for any child)
 * @param retvalp: pointer to the return value of the waited process
*/
int waitpid(int pid, int *retvalp) {
    if (pid < -1 || pid >= NBPROC) {
        return -2;
    }
    process_t* parent = process_table->running;
    process_t* child = process_table->table[pid];
    if (pid >= 0) {
        // If the child does not exist or is not a child of the parent
        if (child == NULL || child->ppid != parent->pid) {
            return -1;
        }
        // If child already finished we should already have the return value
        if (child->state == ZOMBIE || child->state == DYING) {
            *retvalp = child->retval;
            return 2;
        }
    }
    // Lock waiting for pid
    parent->waiting_for = pid;
    parent->state = LOCKED_CHILD;
    // We don't want keep running until waken up
    scheduler();
    // Here we are elected and therefore waken up
    if (retvalp != NULL) {
        if (pid >= 0) {
            *retvalp = child->retval;
        } else if (pid == -1) {
            *retvalp = process_table->table[parent->awaken_by]->retval;
        }
    }
    return 1;
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

        // Anyway, children of this process should become orphans
        queue_for_each(child,process->children,process_t,parent_link){
            child->ppid = -1;
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
            process_table->table[process->pid] = NULL;
        }

    }
}

int get_pid(void){
    return process_table->running->pid;
}

char* get_name(void){
    return process_table->running->name;
}
