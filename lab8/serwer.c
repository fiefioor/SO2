#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

int server_qid;
int random;

struct send{
  int rand;
  long mtype;
  int backtrace_gid;
};

int main(int argc, char** argv) {
 
  srand(time(NULL));
  
  server_qid = msgget(getpid(), IPC_CREAT|0600);
  if(server_qid < 0)
  {
    printf("Server: Queue creation Error\n");
    return(-1);
  }
  
  printf("Server: QID = %d\n",server_qid);
  
  struct send * msg_get = malloc(sizeof(struct send));
  -
  while((msgrcv(server_qid,msg_get,sizeof(struct send),-5,0)))
  {
    struct send * msg_send = malloc(sizeof(struct send));
    
   msg_send->rand = rand() % msg_get->rand + 1;
   msg_send->backtrace_gid = server_qid;
   
   if(msgsnd(msg_get->backtrace_gid, msg_send, sizeof(struct send) == -1)
   {
    printf("Server: Sending msg error\n");
    return(-1);
   }
  }
  
  return(0);
}
