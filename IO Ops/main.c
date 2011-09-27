#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void parentProcess(int pid)
{
	printf("parent\n");

}

int childProcess(char** argv)
{
	printf("child\n");
	char* outputFile = argv[2];
	printf("%s\n",outputFile);
	int file = open(outputFile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	
   if(file < 0)
   	return 0;
  
   if(dup2(file, 1) < 0)
   	return 0;
 
   system(argv[1]);
 
   return 1;
	
}


int main(int argc, char ** argv)
{
	
	int pid = fork();
	
	if(pid == 0)
		childProcess(argv);
	else if(pid != -1)
		parentProcess(pid);
	else
	{
		printf("Failure");
	}
		

	return 0;
}

