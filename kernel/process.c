#include "process.h"
#include "message.h"
#include "string.h"
#include "stdio.h"
#include "mem.h"
#include "user_stack_mem.h"
#include "it.h"
#include "stdarg.h"
#include "processor_structs.h"

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
    process_table->io_queue = mem_alloc(sizeof(link));

    // Initiate process queues
    link head_runnable_queue = LIST_HEAD_INIT(*process_table->runnable_queue);
    link head_sleeping_queue = LIST_HEAD_INIT(*process_table->sleeping_queue);
    link head_dead_queue = LIST_HEAD_INIT(*process_table->dead_queue);
    link head_zombie_queue = LIST_HEAD_INIT(*process_table->zombie_queue);
    link head_io_queue = LIST_HEAD_INIT(*process_table->io_queue);

    memcpy(process_table->runnable_queue,&head_runnable_queue, sizeof(link));
    memcpy(process_table->sleeping_queue,&head_sleeping_queue, sizeof(link));
    memcpy(process_table->dead_queue,&head_dead_queue, sizeof(link));
    memcpy(process_table->zombie_queue,&head_zombie_queue, sizeof(link));
    memcpy(process_table->io_queue,&head_io_queue, sizeof(link));

    // Default values
    process_table->running_shell = -1;
    process_table->nbproc = 0;

    return process_table;
}

int is_user_address(uint32_t addr){
    return addr == 0 || addr > 0xffffff;
}

int count_queue_processes(link* queue) {
    int count = 0;
    process_t* process;
    queue_for_each(process, queue, process_t, queue_link) {
        count++;
    }
    return count;
}

void set_runnable(process_t* proc){
    if (proc != NULL) {
        proc->state = RUNNABLE;
        queue_add(proc, process_table->runnable_queue, process_t, queue_link, priority);
    }
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
    // Check if the process can be added
    if(process_table->nbproc >= NBPROC)
    { return -1; }
    // Check if the name is not too long
    if(strlen(name) > MAX_PROC_NAME_SIZE)
    { return -2; }
    // Check the stack size requirements
    if (ssize > MAX_STACK_SIZE)
    { return -3; }

    // Allocate memory for the new process
    process_t* new_proc = mem_alloc(sizeof(process_t));
    new_proc->waiting_for = INT32_MIN; // -1 is actually used for waiting for any child
    new_proc->awaken_by = -1;
    // Set the proc prio
    new_proc->priority = prio;
    // Set the proc name
    new_proc->name[0] = '\0';
    strcpy(new_proc->name, name);

    va_list args;
    // Start reading the params ... list
    va_start(args, argc);
    // Increment the last used pid and the number of processes existing
    process_table->nbproc += 1;

    // Set the process pid & state
    new_proc->pid = alloc_free_pid(new_proc);

    // Add necessary space to user stacksize
    ssize = ssize + argc + 2 + MIN_STACK_SIZE;
    new_proc->stack_size = ssize;

    // Set Parent PID, add to parent's children
    // After checking wether it's orphan processes or not
    if(process_table->running != NULL){
        // Set ppid to running
        new_proc->ppid = process_table->running->pid;
        queue_add(new_proc,process_table->running->children,process_t,parent_link,pid);

        // Alloc user stack in user memory
        new_proc->user_stack = user_stack_alloc(sizeof(uint32_t) * ssize);

        // Potential params in the stack
        for(uint32_t i = 0; i < argc; i++) {
            void* arg = va_arg(args, void*);
            new_proc->user_stack[ssize - argc + i] = (uint32_t)arg;
        }

        new_proc->user_stack[ssize - argc - 1] = (uint32_t)0x1100000;

        // // Deal with the kernel stack (fill it properly)
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 1] = SS_USER;
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 2] = (uint32_t)&new_proc->user_stack[ssize - argc - 1];
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 3] = EFLAGS;
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 4] = CS_USER;
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 5] = (uint32_t)pt_func;
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 6] = (uint32_t)do_iret;

        // Put the kernel stack function address in the save zone for the first context switch
        new_proc->register_save_zone[1] = (uint32_t)&new_proc->kernel_stack[KERNEL_STACK_SIZE - 6];

    }
    // Idle. No user stack, just deal with the standard kernel stack
    else {
        // Set ppid to -1 (orphan)
        new_proc->ppid = -1;

        new_proc->kernel_stack[KERNEL_STACK_SIZE - 2] = (uint32_t)pt_func;
        new_proc->kernel_stack[KERNEL_STACK_SIZE - 1] = (uint32_t)0x1100000;
        new_proc->register_save_zone[1] = (uint32_t)&new_proc->kernel_stack[KERNEL_STACK_SIZE - 2];
    }

    // Initialize Children queue
    new_proc->children = mem_alloc(sizeof(link));
    link head_children_queue = LIST_HEAD_INIT(*new_proc->children);
    memcpy(new_proc->children, &head_children_queue, sizeof(link));

    // Add to waiting queue
    set_runnable(new_proc);
    va_end(args);

    if (process_table->running && process_table->running->shell_pid >= 0) {
        new_proc->shell_pid = process_table->running->shell_pid;
    } else {
        new_proc->shell_pid = -1;
    }

    if(new_proc->pid > 1){
        scheduler();
    }

    return new_proc->pid;
}

int register_shell() {
    process_t* running = process_table->running;
    // Alloc shell properties
    running->shell_props = mem_alloc(sizeof(shell_props_t));
    shell_props_t* shell_props = running->shell_props;
    shell_props->cmd_hist = mem_alloc(sizeof(cmd_hist_t));
    shell_props->screen_buffer = mem_alloc(sizeof(screen_buf_t));
    // Init cursor position
    shell_props->screen_buffer->cursor_pos[0] = 0;
    shell_props->screen_buffer->cursor_pos[1] = 0;
    
    int32_t shell_pid = process_table->running_shell;
    // A shell is its own shell to send output to
    running->shell_pid = running->pid;
    if (shell_pid >= 0) {
        // save_screen();
    }
    // The shell associated to this process is now himself
    process_table->running_shell = running->pid;
    reset_screen();
    place_cursor(0,0);
    return 0;
}

/**
 * Get the name of the currently running process
 * @return the name of the currently running process
*/
char* getname(void){
    return process_table->running->name;
}

void load_screen() {
    screen_buf_t* sb = get_process(process_table->running_shell)->shell_props->screen_buffer;
    memmove(ptr_mem(0,0), sb->visible_screen, sizeof(uint16_t)*NB_COL*NB_LINE);
    place_cursor(sb->cursor_pos[0], sb->cursor_pos[1]);
}

void save_screen() {
    screen_buf_t* sb = get_process(process_table->running_shell)->shell_props->screen_buffer;
    sb->cursor_pos[0] = CURSOR_LINE;
    sb->cursor_pos[1] = CURSOR_COLUMN;
    for (int i = 0; i < NB_LINE - 1 ; i++){
        memmove(sb->visible_screen, ptr_mem(0,0), sizeof(uint16_t)*NB_COL*NB_LINE);
    }
}

/**
 * @brief Scheduler calling the context switch after checking for processes dying, sleeping and electing the next process to run
*/
void scheduler(){
    // Handle dead processes
    clear_dead_processes();

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
        } else if (old_proc->state == RUNNING) {
            old_proc->state = RUNNABLE;
        }
        elected_proc->state = RUNNING;

        // If elected proc is a shell
        if (elected_proc->shell_props != NULL) {
            process_table->running_shell = elected_proc->pid;
            load_screen();
        }

        // Update running process
        process_table->running = elected_proc;

        // TSS address is read when an interruption happens (provoked by the userspace)
        // We store at this address the top of the kernel_stack so that
        // when interrupting (and switching from userspace to kernelspace)
        // the stack is properly switched the same way.
        tss.esp0 = (uint32_t)&elected_proc->kernel_stack[KERNEL_STACK_SIZE - 1];
        // Context switch between the two processes
        ctx_sw(old_proc->register_save_zone, elected_proc->register_save_zone);
    }
    return;
}

int end_process_life(int32_t pid, int retval){
    process_t* parent;
    process_t* child;
    if(!(child = get_process(pid))){
        return -1;
    }

    if (process_table->running_shell == pid) {
        process_table->running_shell = -1;
    }

    child->retval = retval;
    int32_t ppid = child->ppid;

    // Treat parent case (stop waiting if waiting for this process)
    if (ppid >= 0 && (parent = get_process(ppid))) {
        int32_t waiting_for = parent->waiting_for;
        // If parent is waiting for this child or for any child
        if (waiting_for == child->pid || waiting_for == -1){

            parent->waiting_for = INT32_MIN; // reset parent waiting_for
            parent->awaken_by = child->pid; // tell parent who woke it up
            set_runnable(parent); // Wake up parent
        }
        if(child->state == RUNNABLE || child->state == SLEEPING || child->state == LOCKED_MESS || child->state == LOCKED_SEM || child->state == LOCKED_IO){
            queue_del(child, queue_link);
        }
        child->state = ZOMBIE;
    } else {
        if(child->state == RUNNABLE || child->state == SLEEPING || child->state == LOCKED_MESS || child->state == LOCKED_SEM || child->state == LOCKED_IO){
            queue_del(child, queue_link);
            queue_add(child, process_table->dead_queue, process_t, queue_link, priority);
        }
        child->state = DYING;
    }

    // Treat children case (Their parent dies -> ppid = -1)
    process_t* child_of_child;

    while(!queue_empty(child->children)){
        child_of_child = queue_out(child->children,process_t,parent_link);
        child_of_child->ppid = -1;
    }

    return 0;
}

/**
 * Wake-up processes that need to and put it back to the runnable queue
*/
void seek_for_awaking_processes(){
    process_t* proc;
    while (!queue_empty(process_table->sleeping_queue)
        && ( proc = queue_top(process_table->sleeping_queue, process_t, queue_link) )->wake_up_time >= UINT32_MAX - current_clock() )
    {
        queue_del(proc, queue_link);
        set_runnable(proc);
    }
    return;
}

void clear_dead_processes(){
    process_t* process;

    while(!queue_empty(process_table->dead_queue)){
        // Get all dying processes
        process = queue_out(process_table->dead_queue,process_t,queue_link);
        process_table->table[process->pid] = NULL;
        mem_free(process->children, sizeof(link));
        if (process->shell_props != NULL) {
            mem_free(process->shell_props->cmd_hist, sizeof(cmd_hist_t));
            mem_free(process->shell_props->screen_buffer, sizeof(screen_buf_t));
            mem_free(process->shell_props, sizeof(shell_props_t));
        }
        user_stack_free(process->user_stack, sizeof(uint32_t)*process->stack_size);
        mem_free(process, sizeof(process_t));
        process_table->nbproc -= 1;

    }
}

process_t* get_process(int pid){
    if(pid < NBPROC && pid >= 0){
        process_t* process = process_table->table[pid];
        if(process && process->state != ZOMBIE)
            return process;
    }
    return NULL;
}

int cmd_hist_up() {
    cmd_hist_t* hist = (queue_top(process_table->io_queue, process_t, queue_link))->shell_props->cmd_hist;
    if (hist->count <= 0) {
        return -1;
    }
    // Check if their are still commands in the history that we did not read already
    if(hist->count_read < hist->count){
        // If the is not the first time going up in the cmd history, we need to erase the current command
        if (hist->count_read > 0) { // May be negative
            char c;
            char del[2] = {127, '\0'};
            uint8_t i = 0;
            uint32_t prev_index = (hist->index + 1) % MAX_COMMANDS_HIST;
            while((c = hist->buf[prev_index][i]) != '\0' && i < MAX_COMMAND_LENGTH) {
                keyboard_data(del);
                i++;
            }
        } else {
            for (int i = 0; i < MAX_COMMAND_LENGTH; i++) {
                char del[2] = {127, '\0'};
                keyboard_data(del);
            }
        }
        keyboard_data(hist->buf[hist->index]);
        if (hist->index == 0) {
            hist->index = MAX_COMMANDS_HIST - 1;
        } else {
            hist->index = hist->index - 1;
        }
        hist->count_read++;
    }
    return 0;
}

int cmd_hist_down() {
    cmd_hist_t* hist = (queue_top(process_table->io_queue, process_t, queue_link))->shell_props->cmd_hist;
    if (hist->count <= 0) {
        return -1;
    }
    // Check if we did not already displayed the most recent command in the history
    // If buffer is full, that translates into the index pointing to the same call as the max
    if(hist->count_read > 0){
        hist->index = (hist->index + 1) % MAX_COMMANDS_HIST;
        char c;
        char del[2] = {127, '\0'};
        uint8_t i = 0;
        uint32_t prev_index = hist->index;
        while((c = hist->buf[prev_index][i]) != '\0' && i < MAX_COMMAND_LENGTH) {
            // printf("%c", c);
            // del[0] = del[0];
            keyboard_data(del);
            i++;
        }
        hist->count_read--;
        if (hist->count_read > 0) {
            keyboard_data(hist->buf[(hist->index+1)%MAX_COMMANDS_HIST]);
        }
    }
    return 0;
}
