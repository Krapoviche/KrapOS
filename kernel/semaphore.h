#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include "process.h"

#define MAX_SEMAPHORES 100
#define SEMAPHORE_MAX_VALUE 10

typedef struct semaphore {
    short int count;
    link* queue;
} semaphore_t;

typedef semaphore_t* semaphore_table_t[MAX_SEMAPHORES];

int32_t alloc_free_sid(semaphore_t* sem);

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
 * @return 0 if successful, negative value if bad sid
*/
int sreset(int sid, short int count);

/**
 * @brief Waits for a semaphore to allow access
 * @param sid The sid of the semaphore to wait for
 * @return 0 if successful, negative value if bad sid
*/
int swait(int sid);

/**
 * @brief Tries to wait for a semaphore but does not block if it is not available
 * @param sid The sid of the semaphore to reset
 * @return 0 if successful, negative value if bad sid
*/
int try_wait(int sid);

extern semaphore_table_t semaphore_table;

#endif