#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "stdint.h"

#define MAX_STACK_SIZE 512
#define NBPROC 2
#define REGISTER_SAVE_COUNT 5
#define MAX_PROC_NAME_SIZE 128

extern void ctx_sw(uint32_t* old, uint32_t* new);

typedef enum process_state{RUNNING, WAITING, LOCKED_MESS, LOCKED_SEM, LOCKED_IO, LOCKED_CHILD, SLEEPING, ZOMBIE, DEAD} process_state;

typedef struct process_t
{
    int32_t pid;
    char name[MAX_PROC_NAME_SIZE];
    process_state state;
    uint32_t register_save_zone[REGISTER_SAVE_COUNT];
    uint32_t stack[MAX_STACK_SIZE];
} process_t;

typedef struct process_table_t
{
    process_t*  procs;
    process_t* running;
    uint32_t running_count;
    uint32_t pid_count;
} process_table_t;

process_table_t* init_process_table();
void scheduler(void);
int get_pid(void);
char* get_name(void);
int32_t add_process(process_t* new_proc, uint32_t fct_addr);

extern process_table_t* process_table;


#endif