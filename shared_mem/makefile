CC=gcc -Wall -g

all: main.o
	$(CC) -o chat main.o
	
main.o: main.c
	$(CC) -c main.c
	
clean:
	rm -f *.o *.txt exec chat
