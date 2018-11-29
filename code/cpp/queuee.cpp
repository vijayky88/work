#include <stdio.h> 
#include <sys/msg.h> 
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

//msgget
//mgsnd/msgrcv
//msgclt

//The kernel stores each message in the queue within the framework of the msg structure. It is defined for us in linux/msg.h as follows:
/* one msg structure for each message
struct msg {
    struct msg *msg_next;   // next message on queue 
    long  msg_type;          
    char *msg_spot;         // message text address
    short msg_ts;           // message text size
};

//one msqid structure for each queue on the system 
struct msqid_ds {
    struct ipc_perm msg_perm;
    struct msg *msg_first;  // first message on queue 
    struct msg *msg_last;   // last message in queue 
    time_t msg_stime;       // last msgsnd time 
    time_t msg_rtime;       // last msgrcv time 
    time_t msg_ctime;       // last change time 
    struct wait_queue *wwait;
    struct wait_queue *rwait;
    ushort msg_cbytes;    
    ushort msg_qnum;     
    ushort msg_qbytes;      // max number of bytes on queue/
    ushort msg_lspid;       // pid of last msgsnd 
    ushort msg_lrpid;       // last receive pid 
};
  
//The kernel stores permission information for IPC objects in a structure of type ipc_perm
struct ipc_perm
{
  key_t  key;
  ushort uid;   // owner euid and egid
  ushort gid;
  ushort cuid;  // creator euid and egid 
  ushort cgid;
  ushort mode;  // access modes see mode flags below 
  ushort seq;   // slot usage sequence number
};

PROTOTYPE: int msgget ( key_t key, int msgflg );                                             
RETURNS: message queue identifier on success  -1 on error: errno = EACCESS (permission denied)
                                  EEXIST (Queue exists, cannot create)
                                  EIDRM (Queue is marked for deletion)
                                  ENOENT (Queue does not exist)
                                  ENOMEM (Not enough memory to create queue)
                                  ENOSPC (Maximum queue limit exceeded)

PROTOTYPE: int msgsnd ( int msqid, struct msgbuf *msgp, int msgsz, int msgflg );
    RETURNS: 0 on success
             -1 on error: errno = EAGAIN (queue is full, and IPC_NOWAIT was asserted)
                                  EACCES (permission denied, no write permission)
                                  EFAULT (msgp address isn't accessable - invalid)
                                  EIDRM  (The message queue has been removed)
                                  EINTR  (Received a signal while waiting to write)
                                  EINVAL (Invalid message queue identifier, nonpositive
                                          message type, or invalid message size) 
                                  ENOMEM (Not enough memory to copy message buffer)

PROTOTYPE: int msgctl ( int msgqid, int cmd, struct msqid_ds *buf );  RETURNS: 0 on success -1 on error: errno = EACCES (No read permission and cmd is IPC_STAT)
                                  EFAULT (Address pointed to by buf is invalid with IPC_SET and
                                          IPC_STAT commands)
                                  EIDRM  (Queue was removed during retrieval)
                                  EINVAL (msgqid invalid, or msgsz less than 0)
                                  EPERM  (IPC_SET or IPC_RMID command was issued, but
                                          calling process does not have write (alter)
                                          access to the queue)
*/
// structure for local message queue 
struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 
  
// C Program for Message Queue (Writer Process) 
int writer()
{ 
    key_t key; 
    int msgid; 
  
    // ftok to generate unique key 
    key = ftok("progfile", 65); 
  
    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(key, 0666 | IPC_CREAT); 
    message.mesg_type = 1; 
  
    printf("writer::Write Data : "); 
    gets(message.mesg_text); 
  
    // msgsnd to send message 
    msgsnd(msgid, &message, sizeof(message), 0); 
  
    // display the message 
    printf("writer:Data send is : %s \n", message.mesg_text); 
  
    return 0; 
}

int reader()
{ 
    key_t key; 
    int msgid; 
  
    // ftok to generate unique key 
    key = ftok("progfile", 65); 
  
    // msgget creates a message queue 
    // and returns identifier 
    msgid = msgget(key, 0666 | IPC_CREAT); 
  
    // msgrcv to receive message 
    msgrcv(msgid, &message, sizeof(message), 1, 0); 
  
    // display the message 
    printf("reader::Data Received is : %s \n", message.mesg_text); 
  
    // to destroy the message queue 
    msgctl(msgid, IPC_RMID, NULL); 
  
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
      reader();
      exit(0);
   }
   else if(pid > 0)
   {
      printf("\nmain::parent[server] is  created %d %d %d",pid,getpid(),getppid());
      writer();
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

