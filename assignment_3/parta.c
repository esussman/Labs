#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>
 /* make it a 1K shared memory segment */
#define SHM_SIZE 1024

typedef struct{
  int turn;
  int maxTurns;
} data;
int delete_sharedmem(int shmid)
{
 	if( shmctl(shmid, IPC_RMID, NULL) == -1) 
   {                 
		perror("shmctl");
		return -1;
   }  
	return 0;
}
int forkChildren(int i, int shmid, int p)
{
	int pid = fork();
	if(pid == 0)
	{
			int me = i;
			int counter = 0;
			int next = 0;
			if(i == p)
				next = 1;
			else
				next = i+1;
				
			data *turnController = shmat(shmid, (void *)0, 0);
			if(turnController == (data *)(-1))
			{
				perror("shmat");
				delete_sharedmem(shmid);
				exit(1);
			}
			while(1)
			{
				while(turnController->turn != me);
				counter = counter + 1;
				printf("Process %d completed %d turns\n", me, counter);
				turnController->turn = next;
				if(counter == turnController->maxTurns)
				{
					printf("Process %d finished\n", me);
					exit(0);
				}
			}
	}
	else
	{
		return pid;
	}
}
int main(int argc, char ** argv)
{
	int p = atoi(argv[1]);
	int n = atoi(argv[2]);
	key_t key = 99;
  	int shmid;
	data *turnController;
	if(argc != 3)
	{
		printf("Not enough arguments\n");
		exit(1);
	}
	if((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) < 0)
	{
		printf("SHMGET IS WRONG!!!\n");
		return -1;
	}
	turnController = shmat(shmid, (void *)0, 0);
	if(turnController == (data *)(-1))
	{
		perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	}
	turnController->turn = 1;
	turnController->maxTurns = n;
  	int i = 1;
  	int waitLastChild;
  	for(i = 1; i <= p; i++)
		waitLastChild = forkChildren(i, shmid, p);
  
  	waitpid(waitLastChild, NULL, 0);
   delete_sharedmem(shmid);
  return 0;
}
