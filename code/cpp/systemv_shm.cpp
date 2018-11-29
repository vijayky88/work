/*
 * shm-client - client program to demonstrate shared memory.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include <iostream>
#include <unistd.h>
#define SHMSZ     27

//1.ftok(): is use to generate a unique key.

//2.shmget(): int shmget(key_t,size_tsize,intshmflg); upon successful completion, shmget() returns an identifier for the shared memory segment.

//3.shmat(): Before you can use a shared memory segment, you have to attach yourself
//to it using shmat(). void *shmat(int shmid ,void *shmaddr ,int shmflg);
//shmid is shared memory id. shmaddr specifies specific address to use but we should set
//it to zero and OS will automatically choose the address.
//char *shm = shmat(shmid, NULL, 0);

//4.shmdt(): When you’re done with the shared memory segment, your program should
//detach itself from it using shmdt(). 
//int shmdt(void *shmaddr);

//5.shmctl(): when you detach from shared memory,it is not destroyed. So, to destroy
//shmctl() is used. 
//shmctl(int shmid,IPC_RMID,NULL); IPC_STAT/IPC_SET/IPC_RMID/IPC_INFO/SHM_INFO/SHM_STAT/SHM_LOCK/SHM_UNLOCK/        

/* One shmid data structure for each shared memory segment in the system.
struct shmid_ds {
                struct ipc_perm shm_perm;        // operation perms 
                int     shm_segsz;               // size of segment (bytes)
                time_t  shm_atime;               // last attach time 
                time_t  shm_dtime;               // last detach time 
                time_t  shm_ctime;               // last change time 
                unsigned short  shm_cpid;        // pid of creator 
                unsigned short  shm_lpid;        // pid of last operator 
                short   shm_nattch;              // no. of current attaches 
                                                 // the following are private 
                unsigned short   shm_npages;     // size of segment (pages) 
                unsigned long   *shm_pages;      // array of ptrs to frames -> SHMMAX
                struct vm_area_struct *attaches; // descriptors for attaches 
               };
int shmget ( key_t key, int size, int shmflg ); RETURNS: shared memory segment identifier on success  -1 on error: errno = EINVAL (Invalid segment size specified)
     EEXIST (Segment exists, cannot create)
     EIDRM (Segment is marked for deletion, or was removed)
     ENOENT (Segment does not exist)
     EACCES (Permission denied)
     ENOMEM (Not enough memory to create segment)
	                                                     
PROTOTYPE: int shmat ( int shmid, char *shmaddr, int shmflg);
RETURNS: address at which segment was attached to the process, or -1 on error: errno = EINVAL (Invalid IPC ID value or attach address passed)
     ENOMEM (Not enough memory to attach segment)
     EACCES (Permission denied)

PROTOTYPE: int shmctl ( int shmqid, int cmd, struct shmid_ds *buf );
RETURNS: 0 on success -1 on error: errno = EACCES (No read permission and cmd is IPC_STAT)
     EFAULT (Address pointed to by buf is invalid with IPC_SET and IPC_STAT commands)
     EIDRM  (Segment was removed during retrieval)
     EINVAL (shmqid invalid)
     EPERM  (IPC_SET or IPC_RMID command was issued, but calling process does not have write (alter)access to the segment)                                                        

  PROTOTYPE: int shmdt ( char *shmaddr );
    RETURNS: -1 on error: errno = EINVAL (Invalid attach address passed)
*/
void client()
{
    int shmid;
    key_t key;
    char *shm, *s;

    /*
     * We need to get the segment named
     * "5678", created by the server.
     */
    key = 5678;

    /*
     * Locate the segment.
     */
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now read what the server put in the memory.
     */
    printf("\nclient::reading");
    for (s = shm; *s != NULL; s++)
        putchar(*s);
    putchar('\n');
    sleep(200);
    /*
     * Finally, change the first character of the 
     * segment to '*', indicating we have read 
     * the segment.
     */
    printf("client::writting * on shared memory to signaling for exitting the server\n");
    *shm = '*';

    //exit(0); terminate the process
}

void server()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;

    /* make the key using some data present on disk */
    if ((key = ftok("key.txt", 'R')) == -1) /*Here the file must exist */ 
    {
        perror("ftok");
        exit(1);
    }
    else {printf("\nshm key is %d",key);}

    
    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = (char*)shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    /*
     * Now put some things into the memory for the
     * other process to read.
     */
    s = shm;
    printf("\nserver::writting a-z");

    for (c = 'a'; c <= 'z'; c++)
        *s++ = c;
    *s = NULL;

    /*
     * Finally, we wait until the other process 
     * changes the first character of our memory
     * to '*', indicating that it has read what 
     * we put there.
     */
    while (*shm != '*')
    {
        printf("\nserver::waiting here untill get * here\n");
        sleep(1);
    }

    //exit(0); terminate the process
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
