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

static void handle_prof_signal(int sig_no, siginfo_t* info, void *context)
{
  printf("Done: signal caught\n");
  struct mystruct *p = (struct mystruct *)context;
  printf("\n a=%d, b= %d",p->a,p->b);
  exit(1);
}

void main()
{
  struct sigaction sig_action;
  memset(&sig_action, 0, sizeof(sig_action));
  sig_action.sa_sigaction = handle_prof_signal;
  sig_action.sa_flags = SA_RESTART | SA_SIGINFO;
  sigemptyset(&sig_action.sa_mask);
  sigaction(SIGPROF, &sig_action, 0);

  struct itimerval timeout={0};
  timeout.it_value.tv_sec=1;
  setitimer(ITIMER_PROF, &timeout, 0);

  do { } while(1);
}
