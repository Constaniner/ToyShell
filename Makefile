all:shell

shell:shell.c parse.o
	gcc shell.c parse.o -g -o shell

parse.o:parse.c
	gcc -c parse.c

.PHONY:clean

clean:
	rm -f parse.o shell
