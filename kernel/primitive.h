#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__
#include "stdint.h"
#define MAX_STACK_SIZE 8192

extern void ctx_sw(uint32_t* old, uint32_t* new);

/**
 * @brief Changes the priority of a process
 * @param pid: process id
 * @param newprio: new priority
 * @return the old priority of the process, negative value if the process does not exist
*/
int chprio(int pid, int newprio);

/**
 * @brief Gets the process id of the currently running process
 * @return the process id of the currently running process
*/
int getpid(void);

/**
 * @brief Gets the process id of the parent of the currently running process
 * @return the process id of the parent of the currently running process
*/
int getppid(void);

/**
 * @brief Gets the priority of a process
 * @param pid: process id
 * @return the priority of the process, negative value if the process does not exist
*/
int getprio(int pid);

/**
 * Stops the current process
 * @param retval: return value of the process
*/
void exit(int retval);

/**
 * @brief Kills a process
 * @param pid: id of the process to kill
 * @return 0 if the process was killed, negative value if the process could not be killed
*/
int kill(int pid);

/**
 * @brief Counts the number of messages in a message queue
 * @param queue The message queue to count
 * @param count The adress where the count should be stored
 * @return 0 if successful, negative value if bad fid
*/
int pcount(int fid, int* count);

/**
 * @brief Creates a message queue
 * @param count The maximum number of messages in the queue
 * @return The fid of the created message queue, -1 if failed
*/
int pcreate(int count);

/**
 * @brief Deletes a message queue
 * @param fid The fid of the message queue to delete
 * @return 0 if successful, negative value if bad fid
*/
int pdelete(int fid);

/**
 * @brief Receives a message from a message queue
 * @param queue The message queue to receive from
 * @param message The adress where message should be stored
 * @return 0 if successful, negative value if failed (queue is NULL or was reset)
*/
int preceive(int fid, int* message);

/**
 * @brief Resets a message queue
 * @param fid The fid of the message queue to reset
 * @return 0 if successful, negative value if bad fid
*/
int preset(int fid);

/**
 * @brief Sends a message to a message queue
 * @param queue The message queue to send to
 * @param content The content of the message
 * @return 0 if successful, negative value if failed (queue is NULL or was reset)
*/
int psend(int fid, int message);

/**
 * @brief Starts a process with a single argument
 * @param pt_func: function pointer to the process
 * @param ssize: stack size
 * @param prio: priority
 * @param name: process name
 * @param arg: argument for the executed function
*/
int start(int (*ptfunc)(void *), unsigned long ssize, int prio, const char *name, void *arg);

/**
 * @brief Waits for a certain amount of clock ITs
 * @param ticks: number of clock IT to wait for
*/
void wait_clock(uint32_t clock);

/**
 * @brief Waits for a child process to end
 * @param pid: child process id to wait for (-1 waits for any child)
 * @param retvalp: pointer to the return value of the waited process
*/
int waitpid(int pid, int *retvalp);

#endif