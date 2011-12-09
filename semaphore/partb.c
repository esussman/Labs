#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

 /* make it a 1K shared memory segment */
#define SHM_SIZE 1024

typedef struct{
  int numOne;
  int numTwo;
  sem_t mutex;
  sem_t childsem;
  sem_t sem1;
  sem_t sem2;
  int flag;
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
void parent_print(int shmid)
{
	data *parentData;
	parentData = shmat(shmid, (void *)0, 0);
	if(parentData == (data *)(-1))
	{
		perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	}

	sem_wait(&parentData->sem2);
	sem_wait(&parentData->mutex);
	printf("The product is %d\n", parentData->numOne);
	printf("The addition is %d\n", parentData->numTwo);
	sem_post(&parentData->mutex);
	sem_post(&parentData->sem1);

}
int parent_attach(int shmid, int argc, char ** argv)
{
	 data *parentData;
	 if(argc > 2)
	 {
		parentData = shmat(shmid, (void *)0, 0);
		if(parentData == (data *)(-1))
		{
			perror("shmat");
			delete_sharedmem(shmid);
			exit(1);
		}
	 }
	 
	 sem_wait(&parentData->sem2);
	 sem_wait(&parentData->mutex);
	 //Execute code
	 parentData->numOne = atoi(argv[1]);
	 parentData->numTwo = atoi(argv[2]);
	 sem_post(&parentData->mutex);
	 sem_post(&parentData->sem1);
}

int child_attach(int shmid)
{
	 data *childData;
	 childData = shmat(shmid, (void *)0, 0);
	 if(childData == (data *)(-1))
	 {
	 	perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	 }
	int numone;
	int numtwo;
	sem_wait(&childData->sem1);
	sem_wait(&childData->mutex);
	//Execute
	numone = childData->numOne;
	numtwo = childData->numTwo;
	sem_post(&childData->mutex);
	sem_post(&childData->childsem);
	sem_wait(&childData->sem1);
	//Child 2 should be done reading.
	childData->numOne = numone * numtwo;
	sem_post(&childData->childsem);

}

int childtwo_attach(int shmid)
{
	 data *childData;
	 childData = shmat(shmid, (void *)0, 0);
	 if(childData == (data *)(-1))
	 {
	 	perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	 }
	int numone;
	int numtwo;
	sem_wait(&childData->childsem);
	sem_wait(&childData->mutex);
	//Execute
	numone = childData->numOne;
	numtwo = childData->numTwo;
	sem_post(&childData->mutex);
	sem_post(&childData->sem1);
	sem_wait(&childData->childsem);
	childData->numTwo = numone + numtwo;
	sem_post(&childData->sem2);

}

void init_sems(int shmid)
{
	data *setSemaphores = shmat(shmid, (void *)0, 0);
	if(setSemaphores == (data *)(-1))
	{
		perror("shmat");
		delete_sharedmem(shmid);
		exit(1);
	}
	
	sem_init(&setSemaphores->mutex, 1, 1);
	sem_init(&setSemaphores->sem1, 1, 0);
	sem_init(&setSemaphores->sem2, 1, 1);
	sem_init(&setSemaphores->childsem, 1, 1);
}
int main(int argc, char ** argv)
{
	key_t key = 99;
  	int shmid;
	data *setSemaphores;
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

	init_sems(shmid);
	
  	int pid = fork();
  	if(pid == 0)
  	{
  	  //Child
  	 	child_attach(shmid);
  	}
  	else
  	{
  	  int pid2 = fork();
  	  if(pid2 == 0)
  	  {
  	  		childtwo_attach(shmid);
  	  }
  	  else
  	  {
  	   parent_attach(shmid, argc, argv);
  	  	parent_print(shmid);
  	  }
	}
  	
   /* delete the segment */
   if( shmctl(shmid, IPC_RMID, NULL) == -1) 
   {                 
	perror("shmctl");
        exit(1);
   }  

  return 0;
}

