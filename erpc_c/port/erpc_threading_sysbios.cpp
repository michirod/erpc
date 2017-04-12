/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "erpc_threading.h"
#if ERPC_THREADS_IS(SYSBIOS)

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/gates/GateAll.h>
#include <ti/sysbios/gates/GateMutex.h>
#include <errno.h>
#include <time.h>

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

Thread *Thread::s_first = NULL;
GateAll_Handle Thread::list_lock = NULL;

Task_Handle Thread::gc_handle = NULL;
Mailbox_Handle Thread::gc_list = NULL;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

Thread::Thread(const char *name)
: m_name(name)
, m_entry(0)
, m_arg(0)
, m_stackSize(0)
, m_priority(0)
, m_task(0)
, m_next(0)
{
    initLocks();
}

Thread::Thread(thread_entry_t entry, uint32_t priority, uint32_t stackSize, const char *name)
: m_name(name)
, m_entry(entry)
, m_arg(0)
, m_stackSize(stackSize)
, m_priority(priority)
, m_task(0)
, m_next(0)
{
    initLocks();
}

Thread::~Thread()
{
    Task_Handle t_task = m_task;
    IArg key = GateMutex_enter(gc_mutex);
    if(m_task != NULL){
        m_task = NULL;
    } else {
        t_task = NULL;
    }
    GateMutex_leave(gc_mutex, key);

    if(t_task != NULL){
        if(Task_getMode(t_task) == Task_Mode_RUNNING){
            Task_block(t_task);
        }
        Mailbox_post(gc_list, (xdc_Ptr)&t_task, BIOS_WAIT_FOREVER);
    }
}

void Thread::initLocks()
{
    if(list_lock == NULL){
        bool tbd = false;
        GateAll_Params gateParams;
        GateAll_Params_init(&gateParams);
        GateAll_Handle new_mutex = GateAll_create(&gateParams, NULL);
        IArg key = GateAll_enter(new_mutex);
        if(list_lock == NULL){
            list_lock = new_mutex;
        } else {
            tbd = true;
        }
        GateAll_leave(new_mutex, key);
        if(tbd){
            GateAll_delete(&new_mutex);
        }
    }
    if(gc_handle == NULL){
        IArg key = GateAll_enter(list_lock);
        if(gc_handle == NULL){
            gc_list = Mailbox_create(sizeof(Task_Handle), 1, NULL, NULL);
            Task_Params gc_params;
            Task_Params_init(&gc_params);
            gc_params.instance->name = "Garbage Collector";
            gc_params.arg0 = (UArg) &gc_list;
            gc_handle = Task_create(garbage_collector, &gc_params, NULL);
        }
        GateAll_leave(list_lock, key);
    }
    GateMutex_Params gc_mutexParams;
    GateMutex_Params_init(&gc_mutexParams);
    gc_mutex = GateMutex_create(&gc_mutexParams, NULL);
}

void Thread::init(thread_entry_t entry, uint32_t priority, uint32_t stackSize)
{
    m_entry = entry;
    m_stackSize = stackSize;
    m_priority = priority;
}

void Thread::start(void *arg)
{
    Task_Params params;

    m_arg = arg;

    Task_Params_init(&params);
    params.instance->name = (xdc_String) (m_name ? m_name : "task");
    params.stackSize = ((m_stackSize + sizeof(uint32_t) - 1) / sizeof(uint32_t)); // Round up number of words.;
    params.priority = m_priority;
    params.arg0 = (UArg) m_arg;

    IArg key = GateAll_enter(list_lock);

    // Link in this thread to the list.
    if (s_first)
    {
        m_next = s_first;
    }
    s_first = this;

    GateAll_leave(list_lock, key);

    m_task = Task_create(threadEntryPointStub, &params, NULL);
}

bool Thread::operator==(Thread &o)
{
    return m_task == o.m_task;
}

Thread *Thread::getCurrentThread()
{
    Task_Handle thisTask = Task_self();

    // Walk the threads list to find the Thread object for the current task.
    IArg key = GateAll_enter(list_lock);

    Thread *it = s_first;
    while (it)
    {
        if (it->m_task == thisTask)
        {
            break;
        }
        it = it->m_next;
    }

    GateAll_leave(list_lock, key);

    return it;
}

void Thread::sleep(uint32_t usecs)
{
    Task_sleep(usecs / Clock_tickPeriod);
}

void Thread::threadEntryPoint()
{
    if (m_entry)
    {
        m_entry(m_arg);
    }
}

Void Thread::threadEntryPointStub(UArg arg0, UArg arg1)
{
    Thread *_this = reinterpret_cast<Thread *>(arg0);
    if (_this)
    {
        _this->threadEntryPoint();
    }

    // Remove this thread from the linked list.
    IArg key = GateAll_enter(list_lock);

    Thread *it = s_first;
    Thread *prev = NULL;
    while (it)
    {
        if (it == _this)
        {
            if (it == s_first)
            {
                s_first = _this->m_next;
            }
            else if (prev)
            {
                prev->m_next = _this->m_next;
            }
            _this->m_next = NULL;

            break;
        }
        prev = it;
        it = it->m_next;
    }

    GateAll_leave(list_lock, key);

    Task_Handle t_task = m_task;
    key = GateMutex_enter(gc_mutex);
    if(m_task != NULL){
        m_task = NULL;
    } else {
        t_task = NULL;
    }
    GateMutex_leave(gc_mutex, key);
    if(t_task != NULL){
        Mailbox_post(gc_list, (xdc_Ptr)&t_task, BIOS_WAIT_FOREVER);
    }
}

Void Thread::garbage_collector(UArg arg0, UArg arg1){
    Mailbox_Handle *condemned_queue = (Mailbox_Handle *) arg0;
    Task_Handle condemned;

    while(1){
        Mailbox_pend(*condemned_queue, &condemned, BIOS_WAIT_FOREVER);

        while(Task_getMode(condemned) == Task_Mode_RUNNING){
            Task_sleep((100 * 1000) / Clock_tickPeriod);
        }

        Task_delete(&condemned);
    }
}

Mutex::Mutex()
: m_mutex(0),
  m_key(0),
  m_taken(false)
{
    GateMutex_Params gateParams;
    GateMutex_Params_init(&gateParams);
    m_mutex = GateMutex_create(&gateParams, NULL);
}

Mutex::~Mutex()
{
    GateMutex_delete(&m_mutex);
}

bool Mutex::tryLock()
{
    if(!m_taken){
        return lock();
    } else {
        return false;
    }
}

bool Mutex::lock()
{
    m_key = GateMutex_enter(m_mutex);
    m_taken = true;
    return true;
}

bool Mutex::unlock()
{
    m_taken = false;
    GateMutex_leave(m_mutex, m_key);
    return true;
}

Semaphore::Semaphore(int count)
: m_sem(0)
{
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_COUNTING;
    m_sem = Semaphore_create(count, &semParams, NULL);
}

Semaphore::~Semaphore()
{
    Semaphore_delete(&m_sem);
}

void Semaphore::put()
{
    Semaphore_post(m_sem);
}

bool Semaphore::get(uint32_t timeout)
{
    return Semaphore_pend(m_sem, timeout);
}

int Semaphore::getCount() const
{
    return Semaphore_getCount(m_sem);
}
#endif
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
