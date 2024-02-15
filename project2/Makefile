default: myshell.c builtin.o argparse.o builtin_ls.o builtin_cp.o builtin_env.o
	gcc -g -Wall -o myshell myshell.c *.o

argparse.o: argparse.c argparse.h
	gcc -g -c -Wall argparse.c

builtin.o: builtin.c builtin.h builtin_ls.o builtin_cp.o builtin_env.o
	gcc -g -c -Wall -o builtin.o builtin.c 

builtin_ls.o: builtin_ls.c builtin_ls.h
	gcc -g -c -Wall -o builtin_ls.o  builtin_ls.c

builtin_cp.o: builtin_cp.c builtin_cp.h
	gcc -g -c -Wall -o builtin_cp.o builtin_cp.c

builtin_env.o: builtin_env.c builtin_env.h
	gcc -g -c -Wall -o builtin_env.o builtin_env.c

clean:
	rm *.o *.s || true
	rm test myshell || true
	rm *.out || true
	rm *.gch || true
