#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
 /* make it a 1K shared memory segment */
#define SHM_SIZE 1024

typedef struct{
  int maxTurns;
  sem_t semArray[];
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
			if(i == p-1)
				next = 0;
			else
				next = i+1;

			data *turnController = shmat(shmid, (void *)0, 0);
			if(turnController == (data *)(-1))
			{
				perror("shmat");
				delete_sharedmem(shmid);
				exit(1);
			}
			while(counter != turnController->maxTurns)
			{
        			sem_wait(&turnController->semArray[me]);
				struct timeval time;
				gettimeofday(&time, NULL);
			 	printf("%ld\n",(time.tv_sec*1000000)+time.tv_usec);
				counter = counter + 1;
			//	printf("Process %d completed %d turns\n", me, counter);
        			sem_post(&turnController->semArray[next]);
			}
			//printf("Process %d finished\n", me);
			exit(0);
	}
	else
	{
		return pid;
	}
}
void init_sems(int shmid, int numProcess)
{
	data *setSemaphores = shmat(shmid, (void *)0, 0);
	if(setSemaphores == (data *)(-1))
	{
		perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	}

	sem_init(&setSemaphores->semArray[0], 1, 1);
	int i;
	for(i = 1; i < numProcess; i++)
		sem_init(&setSemaphores->semArray[i], 1, 0);

}
int main(int argc, char ** argv)
{
	int p = atoi(argv[1]);
	int *pidArr = malloc(sizeof(int)*p);
	int n = atoi(argv[2]);
	key_t key = 39;
	int shmid;
	data *turnController;
	if(argc != 3)
	{
		printf("Not enough arguments\n");
		exit(1);
	}
	size_t size = sizeof(data) + p*sizeof(sem_t);
	if((shmid = shmget(key, size, 0644 | IPC_CREAT)) < 0)
	{
		printf("SHMGET IS WRONG!!!\n");
		delete_sharedmem(shmid);
		return -1;
	}
	turnController = shmat(shmid, (void *)0, 0);
	if(turnController == (data *)(-1))
	{
		perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	}
	turnController->maxTurns = n;
	init_sems(shmid, p);
  int i = 0;
  for(i = 0; i < p; i++)
    pidArr[i] = forkChildren(i, shmid, p);

  for(i = 0; i < p; i++)
  	waitpid(pidArr[i], NULL, 0);
  delete_sharedmem(shmid);
  return 0;
}
