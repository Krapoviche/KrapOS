#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "stdint.h"
#include "queue.h"

#define MAX_STACK_SIZE 512
#define NBPROC 30
#define REGISTER_SAVE_COUNT 5
#define MAX_PROC_NAME_SIZE 128

extern void ctx_sw(uint32_t* old, uint32_t* new);

typedef enum process_state{RUNNING, RUNNABLE, DYING, LOCKED_MESS, LOCKED_SEM, LOCKED_IO, LOCKED_CHILD, SLEEPING, ZOMBIE} process_state;

// queue_link & priority are fields related to Queue management
// See shared/queue.h
typedef struct process_t
{
    int32_t pid;
    int32_t ppid;
    char name[MAX_PROC_NAME_SIZE];
    process_state state;
    uint32_t wake_up_time;
    uint32_t register_save_zone[REGISTER_SAVE_COUNT];
    uint32_t* stack;
    uint32_t stack_size;
    link queue_link;
    link parent_link;
    link* children;
    int32_t waiting_for;
    int32_t awaken_by;
    int32_t retval;
    int priority;
} process_t;


typedef struct process_table_t
{
    process_t* table[NBPROC];
    link* runnable_queue;
    link* sleeping_queue;
    link* dead_queue;
    link* zombie_queue;
    process_t* running;
    uint32_t nbproc;
} process_table_t;

process_table_t* init_process_table();
void scheduler(void);
int32_t alloc_free_pid(process_t* proc);
int32_t cancel_start(uint32_t err_code, process_t* created_proc);
int32_t start(int (*pt_func)(void*), uint32_t ssize, int prio, const char *name, uint32_t argc, ...);
void exit(int retval);
int get_pid(void);
char* get_name(void);
void wait_clock(uint32_t ticks);
void sleep(uint32_t secs);
int waitpid(int pid, int *retvalp);
void seek_for_awaking_processes();
void clear_dead_processes();

extern process_table_t* process_table;
extern uint32_t idle_registers[REGISTER_SAVE_COUNT];

#endif
