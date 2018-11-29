#include <string.h>
#include <errno.h>
#include <signal.h>
#include "Thread.h"

Thread::Thread() : m_thread_state(NOTSTARTED)
{
  pthread_mutex_init(&m_thread_mutex, NULL);
}

Thread::~Thread()
{
    int rc;
    pthread_mutex_lock(&m_thread_mutex);
    if (m_thread_state==RUNNING)
    {
        rc = pthread_cancel(m_thread_id);
        if (rc != 0)
        {
            PRINT_LOG("pthread_cancel failed, rc:%d:%s", rc, strerror(rc));
        }
    }
    pthread_mutex_unlock(&m_thread_mutex);
    pthread_mutex_destroy(&m_thread_mutex);
}

int Thread::thread_start()
{
    pthread_mutex_lock(&m_thread_mutex);
    if (m_thread_state==RUNNING)
    {
        PRINT_LOG("thread already started");
        assert(0);
        pthread_mutex_unlock(&m_thread_mutex);
        return -1;
    }
    int rc;

    m_thread_state = RUNNING;

    rc = pthread_create(&m_thread_id, NULL, thread_routine_wrapper, static_cast<void*>(this));
    if (rc)
    {
        m_thread_state = LAUNCHFAILED;
        PRINT_LOG("pthread_create sigchld_thread failed, rc:%d:%s", rc, strerror(rc));
        assert(0);
        pthread_mutex_unlock(&m_thread_mutex);
        return -1;
    }

    pthread_mutex_unlock(&m_thread_mutex);
    return 0;
}

int Thread::thread_join()
{
    int rc;
    void* value_ptr;
    pthread_t pid = 0;

    pthread_mutex_lock(&m_thread_mutex);
    pid = m_thread_id;
    pthread_mutex_unlock(&m_thread_mutex);

    rc = pthread_join(pid, &value_ptr);
    if (rc)
    {
        PRINT_LOG("pthread_join failed, rc:%d:%s \n", rc, strerror(rc));
        assert(0);
        pthread_mutex_unlock(&m_thread_mutex);
        return -1;
    }

    pthread_mutex_lock(&m_thread_mutex);
    if ((m_thread_id == pid) && (RUNNING==m_thread_state)) { 
        PRINT_LOG("pthread sate FINISH \n");
        m_thread_state = FINISH;
    }
    pthread_mutex_unlock(&m_thread_mutex);
    return 0;
}

int Thread::thread_detach()
{
    int rc;
    pthread_mutex_lock(&m_thread_mutex);
    rc = pthread_detach(m_thread_id);
    if (rc)
    {
        PRINT_LOG("pthread_detach failed, rc:%d:%s", rc, strerror(rc));
        assert(0);
        pthread_mutex_unlock(&m_thread_mutex);
        return -1;
    }
    m_thread_state = FINISH;
    pthread_mutex_unlock(&m_thread_mutex);
    return 0;
}

void* Thread::thread_routine_wrapper(void* data)
{
    Thread* thread = static_cast<Thread*>(data);

    thread->thread_routine();
    pthread_exit(0);
}

int Thread::kill(int sig)
{
    pthread_mutex_lock(&m_thread_mutex);
    if (m_thread_state != RUNNING) {
        pthread_mutex_unlock(&m_thread_mutex);
        return ESRCH;
    }
    //Have to check rc value !!!
    int rc = pthread_kill(m_thread_id, sig);
    if ((sig == SIGABRT) || (sig == SIGKILL) || (sig == SIGTERM))
    {
        m_thread_state = FINISH;
    }

    pthread_mutex_unlock(&m_thread_mutex);
    return rc;
}

Thread::State Thread::getState() const
{
    pthread_mutex_lock(&m_thread_mutex);
    Thread::State s = m_thread_state;
    pthread_mutex_unlock(&m_thread_mutex);
    return s;
}


//////////////////////// Test App code

class myApp : public Thread
{
public:
    /**
     * @brief Constructor.
     */
    myApp();
    /**
     * @brief Distructor.
     */
    ~myApp();
    /**
     * @brief start the Daemon Thread
     */
    int Start();
    /**
     * @brief wait for thread to finish
     */
    int Join();
    /**
     @brief stop the Daemon Thread
    */
    int Stop(int s);
    /*
      Stop Encode and Exit
    */
    
   void* thread_routine_cef(void *vargp);
protected:
    /* thread routine */
   void thread_routine();
private:
}

void myApp::thread_routine()
{
  while(getState() == RUNNING)
  {
    thread_routine_cef(NULL);
    PRINT_LOG("sleeping 1 sec .....\n");
    sleep(1);
  }
  /*clean up the memory*/
  PRINT_LOG("Exit .....\n");
  sleep(1);
}

void main()
{
   myApp obj;
   obj.Start();
   obj.Join();
   obj.Stop();
}


