/*
ipcrm <msg | sem | shm>  <IPC ID>
ipcs     -q:    Show only message queues
ipcs     -s:    Show only semaphores
ipcs     -m:    Show only shared memory
ipcs --help:    Additional arguments
*/
//semget
//semop
//semctl
As with message queues, the kernel maintains a special internal data structure for each semaphore set which exists within its addressing space.
/* One semid data structure for each set of semaphores in the system. */
        struct semid_ds {
                struct ipc_perm sem_perm;       // permissions .. see ipc.h 
                time_t          sem_otime;      // last semop time 
                time_t          sem_ctime;      // last change time 
                struct sem      *sem_base;      // ptr to first semaphore in array 
                struct wait_queue *eventn;
                struct wait_queue *eventz;
                struct sem_undo  *undo;         // undo requests on this array 
                ushort          sem_nsems;      // no. of semaphores in array 
        };
In the semid_ds structure, there exists a pointer to the base of the semaphore array itself. Each array member is of the sem structure type.
One semaphore structure for each semaphore in the system.
        struct sem {
                short   sempid;         // pid of last operation 
                ushort  semval;         // current value */
                ushort  semncnt;        // num procs awaiting increase in semval 
                ushort  semzcnt;        // num procs awaiting semval = 0
        };                                                         
PROTOTYPE: int semget ( key_t key, int nsems, int semflg );                              
RETURNS: semaphore set IPC identifier on success -1 on error: errno = EACCESS (permission denied)
                                  EEXIST (set exists, cannot create (IPC_EXCL))
                                  EIDRM (set is marked for deletion)
                                  ENOENT (set does not exist, no IPC_CREAT was used)
                                  ENOMEM (Not enough memory to create new set)
                                  ENOSPC (Maximum set limit exceeded)
								                                                     
PROTOTYPE: int semop ( int semid, struct sembuf *sops, unsigned nsops);
RETURNS: 0 on success (all operations performed)-1 on error: errno = E2BIG (nsops greater than max number of ops allowed atomically)
                                  EACCESS (permission denied)
                                  EAGAIN (IPC_NOWAIT asserted, operation could not go through)
                                  EFAULT (invalid address pointed to by sops argument)
                                  EIDRM (semaphore set was removed)
                                  EINTR (Signal received while sleeping)
                                  EINVAL (set doesn't exist, or semid is invalid)
                                  ENOMEM (SEM_UNDO asserted, not enough memory to create the undo structure necessary)
                                  ERANGE (semaphore value out of range)
								  
PROTOTYPE: int semctl ( int semid, int semnum, int cmd, union semun arg ); 
RETURNS: positive integer on success -1 on error: errno = EACCESS (permission denied)
                                  EFAULT (invalid address pointed to by arg argument)
                                  EIDRM (semaphore set was removed)
                                  EINVAL (set doesn't exist, or semid is invalid)
                                  EPERM (EUID has no privileges for cmd in arg)
                                  ERANGE (semaphore value out of range)

