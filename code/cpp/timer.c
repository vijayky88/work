/*
***************************************************************************
*
* Author: Teunis van Beelen
*
* Copyright (C) 2010, 2011, 2012 Teunis van Beelen
*
* teuniz@gmail.com
*
***************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
***************************************************************************
*
* This version of GPL is at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*
***************************************************************************
*/


#include <stdio.h>
#include <stdlib.h>


#ifdef __linux__
#include <sys/time.h>
#include <signal.h>
#else //__linux__
/* this code only works on win2000, XP, Vista, 7 and up */
/* win95, win98 and ME are not supported                */
/* WINVER must have value 0x500 or higher               */
#ifndef WINVER
#define WINVER 0x500
#endif //WINVER
#if WINVER < 0x500
#error "WINVER is < 0x500, cannot compile for old windows versions"
#endif
#include <windows.h>
#endif //__linux__


#ifndef timer_INCLUDED
#define timer_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif


int  start_timer(int, void (*)(void));
void stop_timer(void);


#ifdef __cplusplus
} /* extern "C" */
#endif
#endif //timer_INCLUDED




void (*timer_func_handler_pntr)(void);


#ifdef __linux__
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

#else //__linux__
HANDLE win_timer;
VOID CALLBACK timer_sig_handler(PVOID, BOOLEAN);
int start_timer(int mSec, void (*timer_func_handler)(void))
{
  timer_func_handler_pntr = timer_func_handler;

  if(CreateTimerQueueTimer(&win_timer, NULL, (WAITORTIMERCALLBACK)timer_sig_handler, NULL, mSec, mSec, WT_EXECUTEINTIMERTHREAD) == 0)
  {
    printf("\nCreateTimerQueueTimer() error\n");
    return(1);
  }
  return(0);
}
VOID CALLBACK timer_sig_handler(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
  timer_func_handler_pntr();
}
void stop_timer(void)
{
  DeleteTimerQueueTimer(NULL, win_timer, NULL);
  CloseHandle(win_timer);
}
#endif //__linux__





#include <time.h>
#include<sys/time.h>

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














