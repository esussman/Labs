#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

 /* make it a 1K shared memory segment */
#define SHM_SIZE 1024

typedef struct{
  char message[32];
  int flag;
} data;

int main(int argc, char ** argv)
{
  char* connectionType;
  if(argc > 1)
    connectionType = argv[1];
  else
    connectionType = "q";

if(strcmp(connectionType, "s") == 0)
	{
		printf("Server!\n");
		key_t key = 99;
    int shmid;

		data *waitingForMessage;

    if((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) < 0)
		{
			printf("SHMGET IS WRONG!!!\n");
			return -1;
		}
    waitingForMessage = shmat(shmid, (void *)0, 0);
    if (waitingForMessage == (data *)(-1))
    {
      perror("shmat");
      exit(1);
    }
    waitingForMessage->flag = 0;

    while(1)
    {
      if(waitingForMessage->flag)
      {
        printf("Message from client: %s", waitingForMessage->message);
        waitingForMessage->flag = 0;
        break;
      }

    }

		 /* delete the segment */
       if( shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
       }
	}
	else if(strcmp(connectionType, "c") == 0)
	{
		printf("Client!\nMessage to send to server: ");

    key_t key = 99;
		int shmid;
		data *prepareData;

	  if((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) < 0)
		{
			printf("SHMGET IS WRONG!!!\n");
			return -1;
		}

    prepareData = shmat(shmid, (void *)0, 0);

    if (prepareData == (data *)(-1))
    {
      perror("shmat");
      exit(1);
    }
    char c = ' ';
    int charIndex = 0;
    while(c != '\n')
    {
      c = getchar();
      prepareData->message[charIndex] = c;
      charIndex++;
    }
    prepareData->flag = 1;
  }




	return 0;
}

