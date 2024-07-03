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
 * @brief Clears console
 * @return 0 if successful, negative value if failed
*/
int clear();

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
 * @brief Changes the echo mode of the console
 * @param on: 0 to disable echo, != 0 to enable
*/
void cons_echo(int on);

/**
 * @brief Read characters from the keyboard
 * @param string: string to read to
 * @param length: length of the string
*/
int cons_read(char *string, unsigned long length);

/**
 * @brief Write bytes to the console
 * @param s: string to write
 * @param len: length of the string
*/
void cons_write(const char *str, long size);

/**
 * @brief Read current clock
 * @return current clock count
*/
uint32_t current_clock(void);

/**
 * @brief print stack of current process
*/
void dump_stack(void);

/**
 * @brief Get the process name
 * @param name: pointer to the string where the name should be stored
*/
void getname(char *name);

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

void lspci();

/**
 * @brief Waits for a given time in milliseconds
 * @param msecs: milliseconds to wait
*/
void millisleep(unsigned long msecs);

/**
 * @brief Counts the number of messages in a message queue
 * @param fid fid of the message queue to count
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
 * @param fid The fid of the message queue to receive from
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
 * @brief Print current processes state
*/
void ps(void);

/**
 * @brief Sends a message to a message queue
 * @param fid The fid of the message queue to send to
 * @param message The content of the message
 * @return 0 if successful, negative value if failed (queue is NULL or was reset)
*/
int psend(int fid, int message);

/**
 * @brief Registers process as shell process
 * @return 0 if successful, negative value if failed
*/
int register_shell();

/**
 * @brief Atomically gets the semaphore count value
 * @param sid The sid of the semaphore to count
 * @return 0 if successful, negative value if bad sid
*/
int scount(int sid);

/**
 * @brief Atomically creates a semaphore
 * @param count The maximum value of the semaphore
 * @return The sid of the created semaphore, negative value if failed
*/
int screate(short int count);

/**
 * @brief Atomically deletes a semaphore
 * @param sid The sid of the semaphore to delete
 * @return 0 if successful, negative value if bad sid
*/
int sdelete(int sid);

/**
 * @brief Atomically sends a signal to a semaphore to allow access for one process
 * @param sid The sid of the semaphore to send signal to
 * @return 0 if successful, negative value if bad sid
*/
int signal(int sid);

/**
 * @brief Atomically sends `count` signal to a semaphore to allow access for one process
 * @param sid The sid of the semaphore to send signal to
 * @param count The count of signals to send
 * @return 0 if successful, negative value if bad sid or bad count
*/
int signaln(int sid, short int count);

/**
 * @brief Atomically resets a semaphore
 * @param sid The sid of the semaphore to reset
 * @param count The count to reset the semaphore to
 * @return 0 if successful, negative value if bad sid
*/
int sreset(int sid, short int count);

/**
 * @brief Waits for a semaphore to allow access
 * @param sid The sid of the semaphore to reset
 * @return 0 if successful, negative value if bad sid
*/
int swait(int sid);

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
 * @brief Tries to wait for a semaphore but does not block if it is not available
 * @param sid The sid of the semaphore to reset
 * @return 0 if successful, negative value if bad sid
*/
int try_wait(int sid);

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
