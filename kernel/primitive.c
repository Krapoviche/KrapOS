#include "primitive.h"
#include "process.h"
#include "message.h"
#include "cpu.h"
#include "it.h"

int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg) {
    return start_multi_args(ptfunc, ssize, prio, name, 1, arg);
}

int getprio(int pid){
    process_t* process;
    if((process = get_process(pid))){
        return process->priority;
    }
    return -1;
}

int chprio(int pid, int newprio){
    process_t* process;
    if(newprio > 0 && (process = get_process(pid))){
        int prio = process->priority;
        // Do nothing if the priority stays the same
        if(prio != newprio){
            process->priority = newprio;

            // If the process was runnable, recharge the queue
            if(process->state == RUNNABLE){
                queue_del(process, queue_link);
                queue_add(process,process_table->runnable_queue,process_t,queue_link,priority);
                scheduler();
            } else if(process->state == LOCKED_MESS){
                // If process is in message waiting queue, it has specified which in waiting_for
                link* waiting_queue = get_message_queue(process->waiting_for)->waiting_queue;
                // We ne to remove it and put it back in the right queue to keep priority sorting
                queue_del(process, queue_link);
                queue_add(process, waiting_queue, process_t, queue_link,priority);
                // No scheduler call here, the process will be waken up by the next sender
            } else if (process->state == RUNNING){
                scheduler();
            }
        }
        return prio;
    }
    return -1;
}

int getpid(void){
    return process_table->running->pid;
}

int getppid(void){
    return process_table->running->ppid;
}

void wait_clock(uint32_t clock){
    process_table->running->state = SLEEPING;
    // clock = time when the process should wake up
    // we count backwards "UINT32_MAX - clock" to have the queue sorted in the right way
    // In reality the uptime will still be clock when it should wake up
    process_table->running->wake_up_time = UINT32_MAX - clock;
    queue_add(process_table->running, process_table->sleeping_queue, process_t, queue_link, wake_up_time);
    scheduler();
}

int waitpid(int pid, int *retvalp) {
    if (pid < -1 || pid >= NBPROC) {
        return -2;
    }
    process_t* parent = process_table->running;
    process_t* child = NULL;
    if (queue_empty(parent->children) ) {
        return -1;
    }
    if (pid >= 0) {
        child = process_table->table[pid];
        // If the child does not exist or is not the same as parent or is not a child of the parent or is not already dying.
        if (child == NULL || child->pid == parent->pid || child->ppid != parent->pid || child->state == DYING) {
            return -1;
        }
    } else {
        // Search for a child that is a zombie
        process_t* iter_proc;
        queue_for_each(iter_proc, parent->children, process_t, parent_link) {
            if (iter_proc->state == ZOMBIE) {
                child = iter_proc;
                break;
            }
        }
    }
    if (child != NULL) {
        // If child already finished we should already have the return value
        if (child->state == ZOMBIE) {
            if(retvalp != NULL) {
                *retvalp = child->retval;
            }
            // Now that this process has no return value to give anymore, destroy it
            child->state = DYING;
            queue_add(child, process_table->dead_queue, process_t, queue_link, priority);
            queue_del(child, parent_link);
            return child->pid;
        }
    }
    // Lock waiting for pid
    parent->waiting_for = pid;
    parent->state = LOCKED_CHILD;
    // We don't want keep running until waken up
    scheduler();
    // Here we are elected and therefore waken up
    if (pid == -1) {
        child = process_table->table[parent->awaken_by];
        while(child->ppid != parent->pid){
            // Lock waiting for pid
            parent->waiting_for = pid;
            parent->state = LOCKED_CHILD;
            scheduler();
        }
    }
    // if the child is a zombie, we can get the return value and let it die peacefully
    if (child->state == ZOMBIE) {
        // Now that this process has no return value to give anymore, destroy it
        child->state = DYING;
        queue_add(child, process_table->dead_queue, process_t, queue_link, priority);
        queue_del(child, parent_link);
    }
    if (retvalp != NULL) {
        *retvalp = child->retval;
    }
    return parent->awaken_by;
}

void exit(int retval){
    end_process_life(process_table->running->pid, retval);
    scheduler();
    while(1);
}

int kill(int pid){
    // DON'T KILL IDLE PROCESS
    if(pid < 1) return -1;

    int ret = end_process_life(pid, 0);
    if(ret == 0){
        // When killing yourself, new process needs to be elected, this for, schedule.
        if(pid == process_table->running->pid){
            scheduler();
        }
    }
    return ret;
}

int pcount(int fid, int* count) {
    message_queue_t* queue = get_message_queue(fid);
    if (queue == NULL) { return 0; }
    if (is_empty(queue)) {
        // If the queue is empty, count the number of processes waiting (virtually negative number of messages in queue)
        *count = -count_queue_processes(queue->waiting_queue);
    } else if (is_full(queue)){
        // Same for full queue
        *count = queue->size + count_queue_processes(queue->waiting_queue);
    }    
    else {
        *count = queue->size;
    }
    return 0;
}

int pcreate(int count) {
    if (count <= 0 || count > MAX_MESSAGE_QUEUE_SIZE) { return -1; }
    message_queue_t* queue = new_message_queue();
    queue->max_size = count;
    int fid = alloc_free_fid(queue);
    if (fid < 0) {
        mem_free(queue, sizeof(message_queue_t));
    }
    return fid;
}

int psend(int fid, int message) {
    message_queue_t* queue = get_message_queue(fid);
    if (queue == NULL) { return -1; }

    if (is_full(queue)) {
        process_t* running = process_table->running;

        // Lock waiting to send message when a spot is available
        running->waiting_for = fid;
        queue_add(running, queue->waiting_queue, process_t, queue_link, priority);
        running->state = LOCKED_MESS;
        running->retval = 0;
        running->msg = message;
        scheduler();
        running->waiting_for = -2;
        // If queue was reset we need to return negative value
        if (running->retval < 0) {
            return running->retval;
        }
        return 0;
    }
    message_t* msg = (message_t*)mem_alloc(sizeof(message_t));
    msg->content = message;
    msg->next = NULL;
    push(queue, msg);
    // if queue was empty before we put our msg
    if (queue->size == 1 && !queue_empty(queue->waiting_queue)) {
        process_t* waiting = queue_out(queue->waiting_queue, process_t, queue_link);

        message_t* msg = pop(queue);
        waiting->msg = msg->content;
        mem_free(msg, sizeof(message_t));

        // Immediately try to wake up first preceive waiting process (if none, does nothing)
        set_runnable(waiting);
        scheduler();
    }
    return 0;
}

int preceive(int fid, int* message) {
    message_queue_t* queue = get_message_queue(fid);
    if (queue == NULL) { return -1; }
    process_t* running = process_table->running;
    if (is_empty(queue)) {
        // Lock waiting to receive message when one is pushed
        running->waiting_for = fid;
        queue_add(running, queue->waiting_queue, process_t, queue_link, priority);
        running->state = LOCKED_MESS;
        running->retval = 0;
        scheduler();
        running->waiting_for = -2;
        // If queue was reset we need to return negative value
        if (running->retval < 0) {
            return running->retval;
        }
        if(message != NULL) *message = running->msg;
        return 0;
    }
    // If the queue is neither empty nor full, just pop the message
    message_t* msg = pop(queue);
    // Caller of preceive reads the message (if message is not NULL)
    if (message != NULL) {
        *message = msg->content;
    }
    // Don't forget to free message
    mem_free(msg, sizeof(message_t));

    // if queue was full before we took our msg and if someone is waiting to produce
    if(queue->size == queue->max_size -1 && !queue_empty(queue->waiting_queue)){
        // Read message from waiting queue
        process_t* waiting = queue_out(queue->waiting_queue, process_t, queue_link);
        
        message_t* msg = (message_t*)mem_alloc(sizeof(message_t));
        msg->content = waiting->msg;
        msg->next = NULL;
        push(queue, msg);

        // Set waiting as runnable since "psend" happened
        set_runnable(waiting);
        scheduler();
    }
    return 0;
}

int preset(int fid) {
    message_queue_t* queue = get_message_queue(fid);
    int reset = reset_message_queue(queue);
    if(reset == 0){
        scheduler();
        return 0;
    }
    return reset;
}

int pdelete(int fid) {
    message_queue_t* queue = get_message_queue(fid);
    int reset = reset_message_queue(queue);
    if(reset == 0){
        mem_free(queue, sizeof(message_queue_t));
        message_table[fid] = NULL;
        scheduler();
        return 0;
    }
    return reset;
}