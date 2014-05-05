// SO2 IS1 211A LAB07
// Michal Warkoczynski
// mjwarkoczynski@wi.zut.edu.pl

#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
  
 
 int i=0;
 
 int proc_num; // liczzba procesow
 int** sockets;
 char** socket_buffs;
 char* filename; // nazwa pliku
 int file; //handler do pliku
 char character; // szukany znak
 int chars_in_file; // liczba znakow w pliku
 int* chars_for_proc; // liczbaa znakow na kazdy process
 char** string_for_proc;
 int chars_found = 0; // liczba znalezionych znakow
 char* content; // tresc pliku
 int chars_in_file_tmp;
 
 pid_t pid;
 
 
 int char_count(char* chain, int range)
 {
  int chars_found=0;
  int i=0;
  for(i = 0; i< range;i=i+1)
  {
      if(chain[i] == character)
      {
	chars_found = chars_found+1;;
      }
  }
  return chars_found;
 }

 
int main(int argc, char** argv) {
  
  if(argc < 4)
  {
   return(-1); 
  }
  
  filename = argv[1];
  character = argv[2][0];
  proc_num = atoi(argv[3]);
  
  
  sockets = malloc(sizeof(int*)*proc_num);
  socket_buffs = malloc(sizeof(char)*proc_num);
  
  file = open ( filename , O_RDONLY );
  
  chars_in_file = lseek(file,0,SEEK_END);
  lseek(file,0,SEEK_SET);
  printf("char in files : %d\n",chars_in_file );
  content = (char*)malloc(sizeof(char) * chars_in_file + 1);
  read(file,content,chars_in_file);
 // close(file);
  
  chars_for_proc = (int*)malloc(sizeof(int) * proc_num);
  string_for_proc = malloc(sizeof(char*)*proc_num);
  chars_in_file_tmp = chars_in_file;
  
 int cThread = 0; 
  
  while(chars_in_file_tmp > 0)
  {
    chars_for_proc[cThread] = chars_for_proc[cThread]+1;
    cThread = cThread + 1;
    if(cThread == proc_num) cThread = 0;
    chars_in_file_tmp = chars_in_file_tmp - 1;      
  }

int j=0;
int k=0;

for(i = 0; i<proc_num; i=i+1)
{
  string_for_proc[i] = malloc(sizeof(char*)*chars_for_proc[i]);
  for(j=0; j<chars_for_proc[i];j++)
  {
   string_for_proc[i] = string_for_proc[i] + content[k];
   k=k+1;
  }
  sockets[i] = malloc(sizeof(int)*2);
  if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets[i]) < 0)
  {
      exit(-1);
  }
}

  //threads = (pthread_t*)malloc(sizeof(pthread_t) * threads_num);
  
 
  
  for(i = 0; i<proc_num; i++)
  {
     pid=fork();
     if(pid == 0) //dziecko
     {
       socket_buffs[i] = malloc(sizeof(char)*chars_for_proc[i]);
       if(read(sockets[i][1],socket_buffs[i], chars_for_proc[i],0) < 0)
       {
	 printf("Child: receve error\n");
       }
       int* tmp_found;
       tmp_found = malloc(sizeof(int));
       *tmp_found = char_count(socket_buffs[i],chars_for_proc[i]);
       
       if(write(sockets[i][0], tmp_found, sizeof(int)) < 0)
       {
	  printf("Child: send error\n");
       }
       return(0);
     }
     else if(pid == -1)
     {
       printf("process creation error\n");
     }
     else if(pid > 0)//rodzic
     {
       if(write(sockets[i][1], string_for_proc[i], chars_for_proc[i]) < 0)
       {
	  printf("Parent: send error\n");
       }
     }
  }
  
  sleep(1);
  
  int* tmp_value;
  tmp_value = malloc(sizeof(int));
  
  for(i=0; i<proc_num; i=i+1)
  {
    if(read(sockets[i],tmp_value,sizeof(int),0)<0)
    {
      printf("Parent: receve error\n"); 
    }
   chars_found = chars_found+tmp_value;
   close(sockets[i][1]);
   close(sockets[i][0]);
  }
  
  printf("Found %d of %c\n", chars_found,character); 
 
  return(0);
  
}
