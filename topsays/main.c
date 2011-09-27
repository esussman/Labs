#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void createChild(int n, int* pipeArr)
{
	if(n == 1)
	{
		//fprintf(stderr,"Child %d created\n", n);
		close(pipeArr[1]);
		char buf[20];
		read(pipeArr[0], buf, 20);
		fprintf(stderr,"%s\n", buf);
		//fprintf(stderr,"Child %d finished\n", n);
	}
	else
	{
		pid_t grandChildpid = fork();
		if(grandChildpid == 0)
		{
			//fprintf(stderr,"Child %d created\n", n);
			createChild(n-1, pipeArr);
		}
		else if(grandChildpid > 0)
		{
			int status;
			waitpid(grandChildpid, &status, NULL);
			//fprintf(stderr,"Child %d finished\n", n);
		}
	}
}

int main(int argc, char ** argv)
{
	int pipeArr[2];
	pid_t childpid;
	int n = atoi(argv[1]);
	if(pipe(pipeArr) != 0)
	{
		fprintf(stderr,"Piping Error\n", n);
		exit(0);
	}
	if(argv[1] > 5)
		exit(0);
	
	childpid = fork();
	if(childpid == 0)
	{
		createChild(n-1, pipeArr);
	}
	else
	{
		close(pipeArr[0]);
		write(pipeArr[1], "Hello World!", 20);
		wait(NULL);
		
	}

	
	return 0;
}

