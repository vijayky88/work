#include <stdio.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h> 

int main(int argc , char *argv[])
{
  if (argc == 5) 
  {
     if((strcmp("-m",argv[1]) == 0) && strcmp("get",argv[2]) == 0)
     {
         key_t shmkey;
         shmkey = ftok(argv[3], atoi(argv[4]));
         printf("ftok returns %d\n",shmkey);
     }
     else
     {
       printf("uses : \n 1. <exe> -m get <key> <id> \n");
     }
  }
  else printf("uses : \n 1. <exe> -m get <key> <id> \n");
  return 1 ;
}
