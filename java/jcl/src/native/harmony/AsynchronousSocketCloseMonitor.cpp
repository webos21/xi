/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "AsynchronousSocketCloseMonitor.h"

#include "xi/xi_log.h"
#include "xi/xi_mem.h"

//#include "signal.h"

/**
 * We use an intrusive doubly-linked list to keep track of blocked threads.
 * This gives us O(1) insertion and removal, and means we don't need to do any allocation.
 * (The objects themselves are stack-allocated.)
 * Waking potentially-blocked threads when a socket is closed is O(n) in the total number of
 * blocked threads (not the number of threads actually blocked on the socket in question).
 * For now at least, this seems like a good compromise for Android.
 */
static xi_thread_mutex_t blockedThreadListMutex = -1;
static AsynchronousSocketCloseMonitor* blockedThreadList = NULL;

//static const int BLOCKED_THREAD_SIGNAL = SIGRTMIN + 2;
//
//static void blockedThreadSignalHandler(int) {
//	// Do nothing
//}

void AsynchronousSocketCloseMonitor::init() {
	// Ensure that the signal we send interrupts system calls but doesn't kill threads.
	// Using sigaction(2) lets us ensure that the SA_RESTART flag is not set.
	// (The whole reason we're sending this signal is to unblock system calls!)

	//	log_print(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//	log_print(XDLOG, "It called!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//	log_print(XDLOG, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	// XXX : We already have thread_suspend
	xi_thread_mutex_create(&blockedThreadListMutex, (xchar *) "AsyncSockMon");

	//	struct sigaction sa;
	//	xi_mem_set(&sa, 0, sizeof(sa));
	//	sa.sa_handler = blockedThreadSignalHandler;
	//	sa.sa_flags = 0;
	//	int rc = sigaction(BLOCKED_THREAD_SIGNAL, &sa, NULL);
	//	if (rc == -1) {
	//		log_error(XDLOG, "setting blocked thread signal handler failed!!!");
	//	}
}

void AsynchronousSocketCloseMonitor::signalBlockedThreads(int fd) {
	ScopedPthreadMutexLock lock(&blockedThreadListMutex);
	for (AsynchronousSocketCloseMonitor* it = blockedThreadList; it != NULL; it
			= it->mNext) {
		if (it->mFd == fd) {
			xi_thread_enable_suspend_fast(it->mThread);
			//pthread_kill(it->mThread, BLOCKED_THREAD_SIGNAL);
		}
	}
}

AsynchronousSocketCloseMonitor::AsynchronousSocketCloseMonitor(int fd) {
	ScopedPthreadMutexLock lock(&blockedThreadListMutex);
	// Who are we, and what are we waiting for?
	mThread = xi_thread_self();
	mFd = fd;
	// Insert ourselves at the head of the intrusive doubly-linked list...
	mPrev = NULL;
	mNext = blockedThreadList;
	if (mNext != NULL) {
		mNext->mPrev = this;
	}
	blockedThreadList = this;
	//log_info(XDLOG, "[DS] xi_thread_disable_suspend_fast\n");
	xi_thread_disable_suspend_fast(mThread);
}

AsynchronousSocketCloseMonitor::~AsynchronousSocketCloseMonitor() {
	ScopedPthreadMutexLock lock(&blockedThreadListMutex);
	// Unlink ourselves from the intrusive doubly-linked list...
	if (mNext != NULL) {
		mNext->mPrev = mPrev;
	}
	if (mPrev == NULL) {
		blockedThreadList = mNext;
	} else {
		mPrev->mNext = mNext;
	}
	//log_info(XDLOG, "[ES] xi_thread_enable_suspend_fast\n");
	xi_thread_enable_suspend_fast(mThread);
}
