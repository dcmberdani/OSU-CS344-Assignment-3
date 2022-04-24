CC=gcc --std=gnu99 -g

all: smallsh

assignment3.o: assignment3.c assignment3.h
	$(CC) -c assignment3.c

smallsh: main.c assignment3.o
	$(CC) assignment3.o main.c -o smallsh

clean:
	rm -rf assignment3.o smallsh
