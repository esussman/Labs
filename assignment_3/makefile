all: parta.o partb.o
	gcc -g -Wall parta.c -o parta -pthread

partb.o:
	gcc -g -Wall partb.c -o partb -pthread
	
clean:
	rm -f *.o *.txt exec partb parta

