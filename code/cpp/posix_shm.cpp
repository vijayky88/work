#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <fcntl.h>

int server()
{
    /* the size (in bytes) of shared memory object */
    const int SIZE = 4096;
 
    /* name of the shared memory object */
    const char* name = "OS";
 
    /* strings written to shared memory */
    const char* message_0 = "Hello";
    const char* message_1 = "World!";
 
    /* shared memory file descriptor */
    int shm_fd;
 
    /* pointer to shared memory obect */
    void* ptr;
 
    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
 
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE);
 
    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
 
    /* write to the shared memory object */
    sprintf((char*)ptr, "%s", message_0);
 
    ptr += strlen(message_0);
    sprintf((char*)ptr, "%s", message_1);
    ptr += strlen(message_1);
    return 0;
}

int client()
{
    /* the size (in bytes) of shared memory object */
    const int SIZE = 4096;

    /* name of the shared memory object */
    const char* name = "OS";

    /* shared memory file descriptor */
    int shm_fd;

    /* pointer to shared memory object */
    void* ptr;

    /* open the shared memory object */
    shm_fd = shm_open(name, O_RDONLY, 0666);

    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    /* read from the shared memory object */
    printf("%s", (char*)ptr);

    /* remove the shared memory object */
    shm_unlink(name);
    return 0;
}

int main()
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
      client();
      exit(0);
   }
   else if(pid > 0)
   {
      printf("\nmain::parent[server] is  created %d %d %d",pid,getpid(),getppid());
      server();
      wait(); // wait for chile to completed
   }
   else
   {
      printf("\nmain::forking failed");

   }
   printf("\nmain::ending pid %d %d %d\n",pid,getpid(),getppid());
   printf("main::ended\n");
  return 1 ;
}

