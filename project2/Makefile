default: myshell.c builtin.o argparse.o
	gcc -g -Wall -o myshell myshell.c argparse.o builtin.o

argparse.o: argparse.c argparse.h
	gcc -g -c -Wall argparse.c

builtin.o: builtin.c builtin.h
	gcc -g -c -Wall builtin.c

test: builtin.o
	gcc -g test.c -o test builtin.o

clean:
	rm *.o *.s || true
	rm test myshell || true
