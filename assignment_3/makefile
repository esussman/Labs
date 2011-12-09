all: parta.o partb.o
	gcc -g parta.c -o parta -pthread

partb.o:
	gcc -g partb.c -o partb -pthread
	
clean:
	rm -f *.o *.txt exec partb parta

