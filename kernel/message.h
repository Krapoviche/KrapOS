#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "process.h"
#include "stdbool.h"
#include "mem.h"
#include "string.h"

#define MAX_MESSAGE_QUEUE_SIZE 512
#define MAX_MESSAGES_QUEUES 512

typedef struct message message_t;
struct message
{
    int content;
    message_t* next;
};

typedef struct message_queue
{
    message_t* head;
    message_t* tail;
    uint32_t size;
    uint32_t max_size;
    link* waiting_queue;
} message_queue_t;

typedef message_queue_t *message_table_t[MAX_MESSAGES_QUEUES];

message_queue_t* new_message_queue();
int32_t alloc_free_fid(message_queue_t* queue);
message_queue_t* get_message_queue(int fid);
message_t* pop(message_queue_t* queue);
void push(message_queue_t* queue, message_t* msg);
bool is_full(message_queue_t* queue);
bool is_empty(message_queue_t* queue);
int reset_message_queue(message_queue_t* queue);

extern message_table_t message_table;

#endif