all: parta.o partb.o
	gcc -g parta.c -o parta -lrt

partb.o:
	gcc -g partb.c -o partb -lrt
	
clean:
	rm -f *.o *.txt exec partb parta

