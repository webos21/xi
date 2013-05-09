/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009
 * Robert Lougher <rob@jamvm.org.uk>.
 *
 * This file is part of JamVM.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CREATING
#include "xi/xi_thread.h"
#include "xi/xi_mem.h"


/* Thread states */

#define CREATING      0
#define STARTED       1
#define RUNNING       2
#define WAITING       3
#define TIMED_WAITING 4
#define BLOCKED       5
#define SUSPENDED     6

/* thread priorities */

#define MIN_PRIORITY   1
#define NORM_PRIORITY  5
#define MAX_PRIORITY  10

/* Enable/Disable suspend modes */

#define SUSP_BLOCKING 1
#define SUSP_CRITICAL 2

/* Park states */

#define PARK_BLOCKED 0
#define PARK_RUNNING 1
#define PARK_PERMIT  2

typedef struct thread Thread;

typedef struct monitor {
	xi_thread_mutex_t lock;
    Thread *owner;
    Object *obj;
    int count;
    int in_wait;
    xuintptr entering;
    int wait_count;
    Thread *wait_set;
    struct monitor *next;
} Monitor;

struct thread {
    int id;
    xi_thread_t tid;
    char state;
    char suspend;
    char blocking;
    char park_state;
    char interrupted;
    char interrupting;
    ExecEnv *ee;
    void *stack_top;
    void *stack_base;
    Monitor *wait_mon;
    Monitor *blocked_mon;
    Thread *wait_prev;
    Thread *wait_next;
    xi_thread_cond_t wait_cv;
    xi_thread_cond_t park_cv;
    xi_thread_mutex_t park_lock;
    long long blocked_count;
    long long waited_count;
    Thread *prev, *next;
    unsigned int wait_id;
    unsigned int notify_id;
};

extern Thread *threadSelf();
extern Thread *jThread2Thread(Object *jThread);
extern Thread *vmThread2Thread(Object *vmThread);
extern long long javaThreadId(Thread *thread);

extern void *getStackTop(Thread *thread);
extern void *getStackBase(Thread *thread);

extern int getThreadsCount();
extern int getPeakThreadsCount();
extern void resetPeakThreadsCount();
extern long long getTotalStartedThreadsCount();

extern void threadInterrupt(Thread *thread);
extern void threadSleep(Thread *thread, long long ms, int ns);
extern void threadYield(Thread *thread);

extern int threadIsAlive(Thread *thread);
extern int threadInterrupted(Thread *thread);
extern int threadIsInterrupted(Thread *thread);
extern int systemIdle(Thread *self);

extern void threadPark(Thread *thread, int absolute, long long time);
extern void threadUnpark(Thread *thread);

extern void suspendAllThreads(Thread *thread);
extern void resumeAllThreads(Thread *thread);

extern void createVMThread(char *name, void (*start)(Thread*));

extern void disableSuspend0(Thread *thread, void *stack_top);
extern void enableSuspend(Thread *thread);
extern void fastEnableSuspend(Thread *thread);

extern Thread *attachJNIThread(char *name, char is_daemon, Object *group);
extern void detachJNIThread(Thread *thread);

extern char *getThreadStateString(Thread *thread);
extern int getThreadStatus(Thread *thread); // by jshwang - added

extern Thread *findThreadById(long long id);
extern Thread *findRunningThreadByTid(int tid);
extern void suspendThread(Thread *thread);
extern void resumeThread(Thread *thread);

extern void disableSuspend(Thread *thread);

#if 0
#define fastDisableSuspend(thread)      \
{                                       \
    thread->blocking = SUSP_CRITICAL;   \
    MBARRIER();                         \
}
#endif // 0

#define fastDisableSuspend(thread)               \
{                                                \
	xi_thread_disable_suspend_fast(thread->tid); \
}

typedef struct {
    xi_thread_mutex_t lock;
    xi_thread_cond_t  cv;
} VMWaitLock;

typedef xi_thread_mutex_t VMLock;

#define initVMLock(lock) xi_thread_mutex_create(&lock, "VMLock")
#define initVMWaitLock(wait_lock) {                         \
		xi_thread_mutex_create(&wait_lock.lock, "VMWLock"); \
		xi_thread_cond_create(&wait_lock.cv, "VMWCond");    \
}

#define lockVMLock(lock, self)  do { \
    self->state = BLOCKED;           \
    xi_thread_mutex_lock(&lock);     \
    self->state = RUNNING;           \
} while(0)

#define tryLockVMLock(lock, self) \
    (xi_thread_mutex_trylock(&lock) == 0)

#define unlockVMLock(lock, self) if(self) xi_thread_mutex_unlock(&lock)

#define lockVMWaitLock(wait_lock, self) lockVMLock(wait_lock.lock, self)
#define unlockVMWaitLock(wait_lock, self) unlockVMLock(wait_lock.lock, self)

#define waitVMWaitLock(wait_lock, self) {                        \
    self->state = WAITING;                                       \
    xi_thread_cond_wait(&wait_lock.cv, &wait_lock.lock);         \
    self->state = RUNNING;                                       \
}

#define timedWaitVMWaitLock(wait_lock, self, ms) {               \
    self->state = TIMED_WAITING;                                 \
    xi_thread_cond_timedwait(&wait_lock.cv, &wait_lock.lock, ms); \
    self->state = RUNNING;                                       \
}

#define notifyVMWaitLock(wait_lock, self) xi_thread_cond_signal(&wait_lock.cv)
#define notifyAllVMWaitLock(wait_lock, self) xi_thread_cond_broadcast(&wait_lock.cv)
#endif

