#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

struct mystruct{
	int a;
	int b;
};
struct mystruct myst = {10,20};
timer_t firstTimerID;



int var=1,wait =1;
unsigned long long prev_time_micros;

void print_time()
{
  struct  timeval tv;
  struct  timezone   tz;
  //time_t nowtime;
  //struct tm *nowtm;
  gettimeofday(&tv,&tz);
  //nowtm = localtime(&nowtime);
  //printf("%Y-%m-%d %H:%M:%S::%s.%06ld", nowtm,tv.tv_usec);
  long long int time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
  printf("var=%d, start_time==%Ld,stoptime=%Ld,gap=%Ld ",var, prev_time_micros, time_in_micros,(time_in_micros - prev_time_micros)/1000);
}

static void timerHandler( int sig, siginfo_t *si, void *uc )
{
 timer_t *tidp;
 tidp = si->si_value.sival_ptr;
 printf("signal caught %d => ",sig);
 struct mystruct *p = (struct mystruct *)tidp;
 //printf("\n a=%d, b= %d",p->a,p->b);
 print_time();
 int k = timer_delete(firstTimerID);
 //printf("\n del status =%d",k);
 wait = 0;
 var++;
}


static int makeTimer(char *name,int expireMS, int intervalMS)
{
    struct sigevent         te;
    struct itimerspec       its;
    struct sigaction        sa;
    int                     sigNo = SIGRTMIN;

    /* Set up signal handler. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(sigNo, &sa, NULL) == -1)
    {
        fprintf(stderr, "Failed to setup signal handling for %s.\n",name);
        return(-1);
    }

    /* Set and enable alarm */
    te.sigev_notify = SIGEV_SIGNAL;
    te.sigev_signo = sigNo;
    te.sigev_value.sival_ptr = &myst;
    timer_create(CLOCK_REALTIME, &te, &firstTimerID);

    its.it_value.tv_sec = expireMS / 1000;
    its.it_value.tv_nsec = (expireMS % 1000)*1000000;
    its.it_interval.tv_sec = intervalMS / 1000;
    its.it_interval.tv_nsec = intervalMS * 1000000;
    timer_settime(firstTimerID, 0, &its, NULL);
    return(0);
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
    printf("\nsetting timer  %3 ms",var);
    if(makeTimer("First Timer",var, var))
    {
      printf("\n timer error\n");
      return(1);
    }
    wait = 1;
    while(wait)
    {
      usleep(1);
     // printf("\n waiting to fnish timer of %d ms",var);
    }
    int k = timer_delete(firstTimerID);
    var++;
   // printf("\n del status =%d",k);
   }
}
