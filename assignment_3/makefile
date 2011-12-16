CC = gcc -Wall -pthread
all: parta partb

parta.o: parta.c
	$(CC) -c parta.c -o parta.o
parta: parta.o
	$(CC) parta.o -o parta

partb.o:	
	$(CC) -c partb.c -o partb.o


partb: partb.o
	$(CC) partb.o -o partb

clean:
	rm -f *.o *.txt exec partb parta

