/*
 * Copyright (C) 2018-2019 TVUNetworks, All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of TVUNetworks and is confidential in nature.
 * Under no circumstances is this software to be exposed to or placed
 * under an Open Source License of any type without the expressed written
 * permission of TVUNetworks.
 */

#ifndef __Thread_h__
#define __Thread_h__

#include <vector>
#include <map>
#include <deque>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

#include "Common.h"
/**
 * @brief Thread implementation.
 *
 * Implemented via POSIX threads (man pthread_create)
 */
class Thread
{
public:
    /**
     * @brief Possible values for thread state.
     */
    typedef enum {
        NOTSTARTED,
        RUNNING,
        FINISH,
        LAUNCHFAILED
    } State;

    /**
     * @brief Constructor.
     */
    Thread();

    /**
     * @brief Destructor.
     */
    virtual ~Thread();

    /**
     * @brief Create a new thread and run the thread_routine() virtual method.
     */
    int thread_start();

    /**
     * @brief Waits for the previously started thread to exit.
     */
    int thread_join();

    /**
     * @brief Places the thread into detached state.
     */
    int thread_detach();

    /**
     * @brief Gets thread state.
     *
     * @see #State.
     */
    State getState() const;

    /**
     * @brief Sends a signal to the running thread.
     */
    int kill(int);

protected:
    /**
     * @brief Method execute in thread.
     */
    virtual void thread_routine() = 0;

    /**
     * @brief Wrapper calling the thread_routine virtual method.
     *
     * @param data Instance of the Thread class asking to start a new thread.
     * @return NULL
     */
    static void* thread_routine_wrapper(void * data);

private:
    pthread_t m_thread_id;
    State m_thread_state;
    mutable pthread_mutex_t m_thread_mutex;
};

#endif
