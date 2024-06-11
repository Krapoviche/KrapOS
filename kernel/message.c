#include "message.h"

message_table_t message_table;

/**
 * @brief Initialize a message table
 * @return The initialized message table
*/
message_table_t* init_message_table() {
    message_table_t* table = (message_table_t*)mem_alloc(sizeof(message_table_t));
    return table;
}

/**
 * @brief creates a message queue
 * @return The initialized message queue
*/
message_queue_t* new_message_queue() {
    message_queue_t* queue = (message_queue_t*)mem_alloc(sizeof(message_queue_t));
    queue->waiting_queue = mem_alloc(sizeof(link));
    link head_waiting_queue = LIST_HEAD_INIT(*queue->waiting_queue);
    memcpy(queue->waiting_queue,&head_waiting_queue, sizeof(link));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

/**
 * @brief Allocate a free fid in the message table
 * @param queue The message queue to allocate the fid for
 * @return The fid allocated, -1 if no free fid
*/
int32_t alloc_free_fid(message_queue_t* queue) {
    for (uint32_t i = 0; i < MAX_MESSAGES_QUEUES; i++) {
        if (message_table[i] == NULL) {
            message_table[i] = queue;
            return i;
        }
    }
    return -1;
}

/**
 * @brief gets a message queue from a message table
 * @param fid The queue id
 * @return The message queue, NULL if the fid is invalid or no corresponding queue
*/
message_queue_t* get_message_queue(int fid) {
    if (fid < 0 || fid >= MAX_MESSAGES_QUEUES) { return NULL; }
    return message_table[fid];
}

/**
 * @brief pops a message from a message queue
 * @param queue The message queue to pop from
 * @return The message popped, NULL if the queue is empty
*/
message_t* pop(message_queue_t* queue) {
    message_t* msg = queue->head;
    if (msg == NULL) { return NULL; }
    queue->head = msg->next;
    queue->size--;
    return msg;
}

/**
 * @brief pushes a message to a message queue
 * @param queue The message queue to push to
 * @param msg The message to push
*/
void push(message_queue_t* queue, message_t* msg) {
    if (msg == NULL || is_full(queue)) { return; }
    if (is_empty(queue)) {
        queue->head = msg;
    } else {
        queue->tail->next = msg;
    }
    queue->tail = msg;
    queue->size++;
}

/**
 * @brief Check if a message queue is full
 * @param queue The message queue to check
 * @return true if the queue is full, false otherwise
*/
bool is_full(message_queue_t* queue) {
    return queue->size >= queue->max_size;
}

/**
 * @brief Check if a message queue is empty
 * @param queue The message queue to check
 * @return true if the queue is empty, false otherwise
*/
bool is_empty(message_queue_t* queue) {
    return queue->size <= 0;
}

/**
 * @brief Wake up the first process waiting for a message in a message queue, if any. Does nothing if none existing
 * @param queue The message queue to wake up a process waiting for that queue to change
*/
void try_wake_first_waiting(message_queue_t* queue) {
    if (!queue_empty(queue->waiting_queue)) {
        process_t* proc = queue_out(queue->waiting_queue, process_t, queue_link);
        set_runnable(proc);
        scheduler();
    }
}

/**
 * @brief Create a message queue
 * @param count The maximum number of messages in the queue
 * @return The fid of the created message queue, -1 if failed
*/
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

/**
 * @brief Count the number of messages in a message queue
 * @param queue The message queue to count
 * @param count The adress where the count should be stored
 * @return 0 if successful, negative value if bad fid
*/
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

/**
 * @brief Send a message to a message queue
 * @param queue The message queue to send to
 * @param content The content of the message
 * @return 0 if successful, negative value if failed (queue is NULL or was reset)
*/
int psend(int fid, int message) {
    message_queue_t* queue = get_message_queue(fid);
    if (queue == NULL) { return -1; }
    process_t* running = process_table->running;

    while (is_full(queue)) {
        // Lock waiting to send message when a spot is available
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
    }
    message_t* msg = (message_t*)mem_alloc(sizeof(message_t));
    msg->content = message;
    msg->next = NULL;
    push(queue, msg);
    // if queue was empty before we put our msg
    if (queue->size == 1) {
        // Immediately try to wake up first preceive waiting process (if none, does nothing)
        try_wake_first_waiting(queue);
    }
    return 0;
}

/**
 * @brief Receive a message from a message queue
 * @param queue The message queue to receive from
 * @param message The adress where message should be stored
 * @return 0 if successful, negative value if failed (queue is NULL or was reset)
*/
int preceive(int fid, int* message) {
    message_queue_t* queue = get_message_queue(fid);
    if (queue == NULL) { return -1; }
    process_t* running = process_table->running;
    while (is_empty(queue)) {
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
    }
    // If the queue is neither empty nor full, just pop the message
    message_t* msg = pop(queue);
    // Caller of preceive reads the message
    *message = msg->content;
    // Don't forget to free message
    mem_free(msg, sizeof(message_t));
    // if queue was full before we took our msg
    if (queue->size == queue->max_size - 1) {
        // Immediately try to wake up first psend waiting process (if none, does nothing)
        try_wake_first_waiting(queue);
    }
    return 0;
}
/**
 * @brief Empty a message queue
 * @param queue pointer to the message_queue_t to reset
 * @return 0 if success. < 0 otherwise.
*/
int reset_message_queue(message_queue_t* queue){
    if (queue == NULL) { return -1; }

    // Free all messages in the queue
    message_t* msg;
    while ((msg = pop(queue)) != NULL) {
        mem_free(msg, sizeof(message_t));
    }
    process_t* proc;
    // Wake up waiting processes with negative return value
    while (!queue_empty(queue->waiting_queue)) {
        proc = queue_out(queue->waiting_queue, process_t, queue_link);
        proc->retval = -1;
        set_runnable(proc);
    }
    return 0;
}

/**
 * @brief Reset a message queue
 * @param fid The fid of the message queue to reset
 * @return 0 if successful, negative value if bad fid
*/
int preset(int fid) {
    message_queue_t* queue = get_message_queue(fid);
    int reset = reset_message_queue(queue);
    if(reset == 0){
        scheduler();
        return 0;
    }
    return reset;
}

/**
 * @brief Delete a message queue
 * @param fid The fid of the message queue to delete
 * @return 0 if successful, negative value if bad fid
*/
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

/**
 * @brief Destroy the message table
*/
void destroy_message_table() {
    for (uint32_t i = 0; i < MAX_MESSAGES_QUEUES; i++) {
        pdelete(i);
    }
    mem_free(message_table, sizeof(message_table_t));
}