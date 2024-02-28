default: myshell.c builtin.o argparse.o 
	gcc -g -Wall -o myshell myshell.c *.o

argparse.o: argparse.c argparse.h
	gcc -g -c -Wall argparse.c

builtin.o: builtin.c builtin.h
	gcc -g -c -Wall -o builtin.o builtin.c 

clean:
	rm argparse.o builtin.o
	rm myshell
