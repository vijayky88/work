#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

//`sem_open'
//`sem_wait'
//`sem_post'
//`sem_close'
//`sem_unlink'


int process1(int argc, char * argv[]) {
    sem_t * mutex;
    char * c = "This is CSCI3150--An operating system course.\n";
    // specify no buffering for stderr
    setbuf(stderr, NULL);
    mutex = sem_open("mutex_for_stderr", O_CREAT, 0666, 1);
    sem_wait(mutex);
    while (* c != '\0') {
            fputc(* c, stderr);
            c++;
            sleep(1);
    }
    sem_post(mutex);
    sem_close(mutex);
    sem_unlink("mutex_for_stderr");
    return 0;
}

int process2(int argc, char * argv[]) {
    sem_t * mutex;
    char * c = "This is CSCI3150--An operating system course.\n";
    // specify no buffering for stderr
    setbuf(stderr, NULL);  
    mutex = sem_open("mutex_for_stderr", O_CREAT, 0666, 1);
    sem_wait(mutex);
    while (* c != '\0') {
        fputc(* c, stderr);
        c++;
        sleep(rand()%2+1);
    }
    sem_post(mutex);
    sem_close(mutex);
    sem_unlink("mutex_for_stderr");
    return 0;
}

int main(int argc, char * argv[])
{
   printf("main::started\n");
   pid_t pid = getpid();
   printf("\nmain::pid before forking is %d %d %d",pid,getpid(),getppid());
   fflush(stdout);
   pid = fork();
   printf("\nmain::pid after forking is %d %d %d",pid,getpid(),getppid());
   if(pid == 0)
   {
      printf("\nmain::Client[child] is created");
      process1(argc,argv);
      exit(0);
   }
   else if(pid > 0)
   {
      printf("\nmain::parent[server] is  created %d %d %d",pid,getpid(),getppid());
      process2(argc,argv);
      wait(); // wait for chile to completed
   }
   else
   {
      printf("\nmain::forking failed");

   }
   printf("\nmain::ending pid %d %d %d\n",pid,getpid(),getppid());
   printf("main::ended\n");
   return 0;
}
