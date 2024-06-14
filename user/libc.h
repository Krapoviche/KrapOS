#ifndef __LIBC_H__
#define __LIBC_H__
#include "stdint.h"

/**
 * @brief Change the priority of a process
 * @param pid: process id
 * @param newprio: new priority
 * @return the old priority of the process, negative value if the process does not exist
*/
int chprio(int pid, int prio);


/**
 * @brief Get clock quartz and ticks settings
 * @param quartz: pointer where to write the quartz setting
 * @param ticks: pointer where to write the ticks setting 
*/
void clock_settings(unsigned long *quartz, unsigned long *ticks);

/**
 * @brief Write bytes to the console
 * @param s: string to write
 * @param len: length of the string
*/
void console_putbytes(const char *s, int len);

/**
 * @brief Read current clock
 * @return current clock count
*/
uint32_t current_clock(void);


/**
 * @brief Get the process id of the currently running process
 * @return the process id of the currently running process
*/
int getpid(void);

/**
 * @brief Get the process id of the parent of the currently running process
 * @return the process id of the parent of the currently running process
*/
int getppid(void);

int getprio(int pid);

void exit(int retval);

int kill(int pid);

/**
 * @brief Waits for a given time in milliseconds
 * @param msecs: milliseconds to wait
*/
void millisleep(unsigned long msecs);

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
 * @brief Wait for a given time in seconds
 * @param msecs: seconds to wait
*/
void sleep(unsigned long secs);

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
 * @brief Waits for any child process to end. Equivalent to waitpid(-1, &retvalp)
 * @param retvalp: pointer to the return value of the waited process
*/
int wait(int* retvalp);

/**
 * @brief Waits until time in `clock`
 * @param clock: clock time to wait to
*/
void wait_clock(uint32_t clock);

/**
 * @brief Waits for a child process to end
 * @param pid: child process id to wait for (-1 waits for any child)
 * @param retvalp: pointer to the return value of the waited process
*/
int waitpid(int pid, int* retvalp);

#endif
