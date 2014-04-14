#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int limit;
int server_qid;
int priority;
int client_qid;

struct send{
  int rand;
  long mtype;
  int backtrace_gid;
};
  
int main(int argc, char** argv) {
  if(argc < 4)
  {
   printf("Argc Error\n");
   return(-1);
  }
  
  server_qid = atoi(argv[1]);
  limit = atoi(argv[2]);
  priority = atoi(argv[3]);
  
  // tworzenie kolejki
  
  client_qid = msgget(getpid(), IPC_CREAT|0600);
  if(client_qid != 0)
  {
   printf("Client: Queue creation Error\n");
   return(-1);
  }
  
 struct send * msg = malloc(sizeof(struct send));
 msg->rand = limit;
 msg->mtype = priority;
 msg->backtrace_gid = client_qid;
 
 //Wysylanie
 
 if(msgsnd(server_qid, msg, sizeof(struct send) == -1)
   {
    printf("Client: Sending msg error\n");
    return(-1);
   }
 
 free(msg);
 
 // Odbieranie
 
 struct send * msg_get = malloc(sizeof(struct send));
 
 if(msgrcv(server_qid,msg_get,sizeof(struct send),-5,0) == -1)
 {
  printf("Client: receiving msg error\n"); 
 }
 else
 {
  printf("Client: Random from sever = %d\n",msg_get->rand) ;
 }
 
 // Sprzatanie
 
 free(msg_get);
 
 if(msgctl(server_qid,IPC_RMID,NULL) == -1)
 {
   printf("Client: Server's Queue termination error\n");
   return(-1);
 }
 else
 {
   printf("Client: Server's Queue termination complited");
 }
 
 if(msgctl(client_qid,IPC_RMID,NULL) == -1)
 {
   printf("Client: Queue termination error\n");
   return(-1);
 }
 else
 {
   printf("Client: Queue termination complited");
 }
 
 return(0);
}
