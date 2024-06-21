#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "stdint.h"
#include "queue.h"
#include "primitive.h"
#include "screen.h"

#define NBPROC 1024
#define REGISTER_SAVE_COUNT 5
#define MAX_PROC_NAME_SIZE 128
#define MIN_STACK_SIZE 16
#define KERNEL_STACK_SIZE 512
#define TSS_ADDRESS 0x20000
#define SS_KERNEL 0x18
#define SS_USER 0x4B
#define CS_USER 0x43
#define EFLAGS 0x202
#define SCREEN_BUFFER_LEN 128
#define MAX_COMMAND_LENGTH 150
#define MAX_COMMANDS_HIST 4

extern void ctx_sw(uint32_t* old, uint32_t* new);

typedef enum process_state{RUNNING, RUNNABLE, DYING, LOCKED_MESS, LOCKED_SEM, LOCKED_IO, LOCKED_CHILD, SLEEPING, ZOMBIE} process_state;

// queue_link & priority are fields related to Queue management
// See shared/queue.h

typedef struct screen_buffer {
    uint16_t* buf[SCREEN_BUFFER_LEN];
    uint16_t visible_screen[NB_LINE*NB_COL];
    uint32_t cursor_pos[2];
    int newer_lines;
    int older_lines;
    int total_lines;
} screen_buf_t;

typedef struct cmd_hist {
    char buf[MAX_COMMANDS_HIST][MAX_COMMAND_LENGTH];
    uint32_t max;
    uint32_t index;
    uint32_t count;
    uint32_t count_read;
} cmd_hist_t;

typedef struct shell_props {
    screen_buf_t* screen_buffer;
    cmd_hist_t* cmd_hist;
} shell_props_t;

typedef struct process_t
{
    int32_t pid;
    int32_t ppid;
    char name[MAX_PROC_NAME_SIZE];
    process_state state;
    uint32_t wake_up_time;
    uint32_t register_save_zone[REGISTER_SAVE_COUNT];
    uint32_t* user_stack;
    uint32_t kernel_stack[KERNEL_STACK_SIZE];
    uint32_t stack_size;
    link queue_link;
    link parent_link;
    link* children;
    int32_t waiting_for;
    int32_t awaken_by;
    int32_t retval;
    int priority;
    int msg;
    shell_props_t* shell_props;
} process_t;

typedef struct process_table_t
{
    process_t* table[NBPROC];
    link* runnable_queue;
    link* sleeping_queue;
    link* dead_queue;
    link* zombie_queue;
    link* io_queue;
    process_t* running;
    int32_t running_shell;
    uint32_t nbproc;
} process_table_t;

process_table_t* init_process_table();
int is_user_address(uint32_t addr);
int count_queue_processes(link* queue);
void set_runnable(process_t* proc);
int32_t alloc_free_pid(process_t* proc);
int32_t cancel_start(uint32_t err_code, process_t* created_proc);
int32_t start_multi_args(int (*pt_func)(void*), uint32_t ssize, int prio, const char *name, uint32_t argc, ...);
int register_shell();
void save_screen();
void load_screen();
char* getname(void);
void scheduler();
extern void do_iret();
int end_process_life(int32_t pid, int retval);
void seek_for_awaking_processes();
void clear_dead_processes();
process_t* get_process(int pid);

int cmd_hist_up();
int cmd_hist_down();

extern process_table_t* process_table;
extern uint32_t idle_registers[REGISTER_SAVE_COUNT];

#endif