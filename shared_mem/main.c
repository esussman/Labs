#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

int main(int argc, char ** argv)
{
	if(strcmp(argv[1], "s") == 0)
	{
		printf("Server!\n");
		key_t key = ftok("test",'9');
		size_t size = 1024;
		int shmid;
		char *data;
		
		if(shmid = shmget(key, size, 0644 | IPC_CREAT | IPC_EXCL) < 0)
		{
			printf("SHMGET IS WRONG!!!\n");
			return -1;
		}
		
		shmid = shmget(key, SHM_SIZE, 0644);
		
		 /* delete the segment */
       if( shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
       }
		
	}
	else if(strcmp(argv[1], "c") == 0)	
	{
		printf("Client!\n");
	}
	
	
	

	
	return 0;
}

