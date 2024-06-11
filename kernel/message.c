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
 * @brief Destroy the message table
*/
void destroy_message_table() {
    for (uint32_t i = 0; i < MAX_MESSAGES_QUEUES; i++) {
        pdelete(i);
    }
    mem_free(message_table, sizeof(message_table_t));
}