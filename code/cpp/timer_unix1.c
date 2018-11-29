#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include<time.h>

int  start_timer(int, void (*)(void));
void stop_timer(void);
void (*timer_func_handler_pntr)(void);

struct itimerval timervalue;
struct sigaction new_handler, old_handler;
void timer_sig_handler(int);

int start_timer(int mSec, void (*timer_func_handler)(void))
{
  timer_func_handler_pntr = timer_func_handler;
  timervalue.it_interval.tv_sec = mSec / 1000;
  timervalue.it_interval.tv_usec = (mSec % 1000) * 1000;
  timervalue.it_value.tv_sec = mSec / 1000;
  timervalue.it_value.tv_usec = (mSec % 1000) * 1000;
  if(setitimer(ITIMER_REAL, &timervalue, NULL))
  {
    printf("\nsetitimer() error\n");
    return(1);
  }

  new_handler.sa_handler = &timer_sig_handler;
  new_handler.sa_flags   = SA_NOMASK;
  if(sigaction(SIGALRM, &new_handler, &old_handler))
  {
    printf("\nsigaction() error\n");
    return(1);
  }
  return(0);
}


void timer_sig_handler(int arg)
{
  timer_func_handler_pntr();
}


void stop_timer(void)
{
  timervalue.it_interval.tv_sec = 0;
  timervalue.it_interval.tv_usec = 0;
  timervalue.it_value.tv_sec = 0;
  timervalue.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timervalue, NULL);
  sigaction(SIGALRM, &old_handler, NULL);
}

int var=1,wait =1;
unsigned long long prev_time_micros;

void print_time()
{
  struct  timeval tv;
  struct  timezone   tz;
  time_t nowtime;
  struct tm *nowtm;
  char tmbuf[64], buf[64];
  
  gettimeofday(&tv,&tz);
  nowtm = localtime(&nowtime);
  strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
  snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);
  unsigned long time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
  printf("var=%d, currtime=%sms, start_time==%Ld,stoptime=%Ld,gap=%Ld ",var, buf, prev_time_micros, time_in_micros, (time_in_micros - prev_time_micros)/1000);

}

void timer_handler(void)
{
  printf("\ntimer: var %i called::", var++);
  print_time();
  wait = 0;
}

int main(void)
{
  printf("\npress ctl-c to quit.\n");
  while(1)
  {
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv,&tz);
    prev_time_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    if(start_timer(var, &timer_handler))
    {
      printf("\n timer error\n");
      return(1);
    }
    wait = 1;
    while(wait)
    {
      usleep(1000);
      //printf("\n starting timer of %d ms",var);
    }
    stop_timer();
   }
   return(0);
}














