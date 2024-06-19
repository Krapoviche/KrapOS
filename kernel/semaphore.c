#include "semaphore.h"
#include "cpu.h"
#include "string.h"
#include "mem.h"

semaphore_table_t semaphore_table;

int32_t alloc_free_sid(semaphore_t* sem) {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        if (semaphore_table[i] == NULL) {
            semaphore_table[i] = sem;
            return i;
        }
    }
    return -1;
}

semaphore_t* get_semaphore(int sid) {
    if (sid < 0 || sid >= MAX_SEMAPHORES) {
        return NULL;
    }
    return semaphore_table[sid];
}

int scount(int sid) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    int count = sem->count;
    sti();
    return count;
}

int screate(short int count) {
    cli();
    if (count < 0) {
        sti();
        return -1;
    }
    semaphore_t* sem = (semaphore_t*)mem_alloc(sizeof(semaphore_t));
    int sid = alloc_free_sid(sem);
    if (sid < 0) {
        mem_free(sem, sizeof(semaphore_t));
        sti();
        return -1;
    }
    sem->count = count;
    sem->queue = mem_alloc(sizeof(link));
    link head_queue = LIST_HEAD_INIT(*sem->queue);
    memcpy(sem->queue, &head_queue, sizeof(link));
    sti();
    return sid;
}

int sdelete(int sid) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    while (!queue_empty(sem->queue)) {
        process_t* proc = queue_out(sem->queue, process_t, queue_link);
        proc->retval = -3;
        set_runnable(proc);
    }
    mem_free(semaphore_table[sid]->queue, sizeof(link));
    mem_free(semaphore_table[sid], sizeof(semaphore_t));
    semaphore_table[sid] = NULL;
    sti();
    scheduler();
    return 0;
}

int sreset(int sid, short int count) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL || count < 0)
    {
        sti();
        return -1;
    }
    sem->count = count;
    int wokeup = 0;
    while (!queue_empty(sem->queue)) {
        process_t* proc = queue_out(sem->queue, process_t, queue_link);
        proc->retval = -4;
        set_runnable(proc);
        wokeup++;
    }
    sti();
    if (wokeup > 0) {
        scheduler();
    }
    return 0;
}

int signal(int sid) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    if (sem->count == INT16_MAX) {
        sti();
        return -2;
    }
    sem->count++;
    if (sem->count <= 0) {
        process_t* proc = queue_out(sem->queue, process_t, queue_link);
        set_runnable(proc);
        sti();
        scheduler();
        cli();
    }
    sti();
    return 0;
}

int signaln(int sid, short int count) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    int woke_up = 0;
    for (short int i = 0; i < count; i++) {
        if (sem->count == INT16_MAX) {
            sti();
            return -2;
        }
        sem->count++;
        if (sem->count <= 0) {
            woke_up++;
            set_runnable(queue_out(sem->queue, process_t, queue_link));
        }
    }
    if (woke_up > 0) {
        sti();
        scheduler();
        cli();
    }
    sti();
    return 0;
}

int swait(int sid) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    if (sem->count == INT16_MIN) {
        sti();
        return -2;
    }
    sem->count--;
    if (sem->count < 0) {
        process_t* running = process_table->running;
        running->state = LOCKED_SEM;
        queue_add(running, sem->queue, process_t, queue_link, priority);
        sti();
        scheduler();
        cli();
        // If something happened while waiting (sreset or sdelete) return the code they left
        int retval = running->retval;
        if (retval < 0 && retval > INT32_MIN) {
            running->retval = INT32_MIN;
            sti();
            return retval;
        }
    }
    sti();
    return 0;
}

int try_wait(int sid) {
    cli();
    semaphore_t* sem;
    if ((sem = get_semaphore(sid)) == NULL) {
        sti();
        return -1;
    }
    if (sem->count == INT16_MIN) {
        sti();
        return -2;
    }
    if (sem->count > 0) {
        sem->count--;
    } else {
        sti();
        return -3;
    }
    sti();
    return 0;
}