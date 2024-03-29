/*
 * Copyright 2013 Cheolmin Jo (webos21@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if !defined(_WIN32) && !defined(__APPLE__)

/**
 * @brief Memory Barrier API for linux
 *
 * @file membar_linux.h
 * @date 2011-04-10
 * @author Cheolmin Jo (webos21@gmail.com)
 */

#ifdef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
#define _xi_mem_barrier_rdwr() __sync_synchronize()
#define _xi_mem_barrier_wr()   __sync_synchronize()
#else
#include <atomic.h>
#define _xi_mem_barrier_rdwr() atomic_full_barrier()
#define _xi_mem_barrier_wr()   atomic_write_barrier()
#endif

#endif // !defined(_WIN32) && !defined(__APPLE__)
