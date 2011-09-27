all: main.o
	gcc -o exec -g main.o
	
main.o: main.c
	gcc -c -g main.c
	
clean:
	rm -f *.o *.txt exec
