// SO2 IS1 211A LAB12
// Michal Warkoczynski
// mjwarkoczynski@wi.zut.edu.pl

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>

key_t seg_key;
key_t sem_key;

int seg_id;
int sem_id;

struct sembuf* sop;

struct game_info
{
  char board[3][3];
  bool player;
  bool end;
  
};

void DisplayBoard(struct game_info* gaem)
{
  printf("\nBOARD:\n");
  int h,w;
  for(h=0; h<3; h=h+1)
  {
    for(w=0; w<3; w=w+1)
    {
      printf("%c ",gaem->board[h][w]);
    }
    printf("\n");
  }
}

void GameStart(struct game_info* gaem)
{
  //printf("\nBOARD:\n"");
  int h,w;
  for(h=0; h<3; h=h+1)
  {
    for(w=0; w<3; w=w+1)
    {
      gaem->board[h][w] = '#';
    }
    //printf("\n");
  }
  gaem->end = false;
  gaem->player = 0;
}

bool Turn(int x, int y, struct game_info* gaem)
{
  if(x<1 || x>3)
  {
    printf("Wrong position on X\n");
    return(false);
  }
  if(y<1 || y>3)
  {
    printf("Wrong position on Y\n");
    return(false);
  }
  x = x-1;
  y = y-1;
  
  if(gaem->board[x][y] != '#')
  {
    printf("\n%d, %d is taken\n",x,y);
    return(false);
  }
  else
  {
    if(gaem->player == 0)
    {
      gaem->board[x][y] = 'X';
      return(true);
    }
    else
    {
      gaem->board[x][y] = 'O';
      return(true);
    }
  }
}

void isEnd(struct game_info* gaem)
{
   int h,w;
   int counter;
   
   counter = 0;
   for(h=0;h<3; h=h+1)
   {
     if( (gaem->board[h][0] == gaem->board[h][1]) && (gaem->board[h][0] == gaem->board[h][2]))
     {
       gaem->end = true;
     }
     else if( (gaem->board[0][h] == gaem->board[1][h]) && (gaem->board[0][h] == gaem->board[2][h]) )
     {
       gaem->end = true;
     }
     
     for(w=0;w<3;w=w+1)
     {
       if(gaem->board[h][w]== '#')
       {
	 counter = counter + 1;
       }
     }
     
   }
   if(counter == 9)
   {
     gaem->end = true;
   }
   else
   {
     gaem->end = false;
   }
}

struct game_info* game;
struct game_info* buffor;

int x,y;

int main(int argc, char** argv) {
  
  if(argc < 3)
  {
    return(-1); 
  }
  
  seg_key = atoi(argv[1]);
  sem_key = atoi(argv[2]);
  
  sop = (struct sembuf*) malloc(sizeof(struct sembuf));
  sop[0].sem_flg = 0;
  
  seg_id = shmget(seg_key,sizeof(struct game_info), 0777);
  
  if(seg_id < 0)
  {
    sem_id = semget(sem_key,2, IPC_CREAT | 0600);
    semctl(sem_id,0,SETVAL,1);
    semctl(sem_id,1,SETVAL,0);
    
    seg_id = shmget(seg_key,sizeof(struct game_info), IPC_CREAT | 0777);
    
    game = shmat(seg_id,NULL,0);
    
    if(game  == (struct game_info*)-1)
    {
      printf("HOST ERROR\n\n");
    }
    
    buffor = game;
    
    GameStart(buffor);
    
    while(buffor->end != true)
    {
      printf("\nPlease wait\n");
      
      sop[0].sem_num = 0;
      sop[0].sem_op = 0;
      semop(sem_id,sop,1);
      
      DisplayBoard(buffor);
      if(buffor->end == true)
      {
	break;
      }
      
      do
      {
	printf("\nMake a turn\n");
	printf("X: ");
	scanf("%d",&x);
	printf("\nY: ");
	scanf("%d",&y);
      }while(!Turn(x,y,buffor));
      
      DisplayBoard(buffor);
      isEnd(buffor);
      buffor->player=1;
      
      sop[0].sem_num = 0;
      sop[0].sem_op = 1;
      semop(sem_id,sop,1);
      sop[0].sem_num = 1;
      sop[0].sem_op = -1;
      semop(sem_id,sop,1);
      
    }
    
    if(buffor->end)
    {
     printf("The game has ended\n"); 
    }
    
    sop[0].sem_num = 1;
    sop[0].sem_op = -1;
    semop(sem_id,sop,1);
    
  }
  else
  {
    sem_id = semget(sem_key,2, IPC_CREAT | 0600);
    
    game = shmat(seg_id,NULL,0);
    
    if(game == (struct game_info*)-1)
    {
      printf("GUEST ERROR\n\n");
    }
    
    buffor = game;
    
    //GameStart(buffor);
    
    while(buffor->end != true)
    {
      printf("\nPlease wait\n");
            
      DisplayBoard(buffor);
      if(buffor->end == true)
      {
	break;
      }
      
      do
      {
	printf("\nMake a turn\n");
	printf("X: ");
	scanf("%d",&x);
	printf("\nY: ");
	scanf("%d",&y);
      }while(!Turn(x,y,buffor));
      
      DisplayBoard(buffor);
      isEnd(buffor);
      buffor->player=0;
      
      if(buffor->end == false) printf("\nPlease wait\n");
      
      sop[0].sem_num = 0;
      sop[0].sem_op = -1;
      semop(sem_id,sop,1);
      
      sop[0].sem_num = 1;
      sop[0].sem_op = 1;
      semop(sem_id,sop,1);
      
      sop[0].sem_num = 1;
      sop[0].sem_op = 0;
      semop(sem_id,sop,1);
      
    }
    
    if(buffor->end)
    {
     printf("The game has ended\n"); 
    }
    
    sop[0].sem_num = 1;
    sop[0].sem_op = -1;
    semop(sem_id,sop,1);
    
    shmdt(game);
    shmdt(buffor);
    semctl(sem_id, 0, IPC_RMID);
    semctl(sem_id, 1, IPC_RMID);
    
    
  }
  
/*  game = malloc(sizeof(struct game_info));
  GameStart(game);
  DisplayBoard(game);
  Turn(1,1,game);
  DisplayBoard(game);
  
 */ 
}
