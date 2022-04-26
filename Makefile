CC=gcc --std=gnu99 -g

all: smallsh

shellinfo.o: shellinfo.h shellinfo.c
	$(CC) -c shellinfo.h shellinfo.c

cmdline.o: cmdline.c cmdline.h
	$(CC) -c cmdline.c

functions.o: functions.c functions.h
	$(CC) -c functions.c

smallsh: main.c cmdline.o functions.o shellinfo.o
	$(CC) cmdline.o functions.o shellinfo.o main.c -o smallsh

clean:
	rm -rf cmdline.o functions.o shellinfo.o shellinfo.h.gch junk smallsh-test-dir smallsh ~/testdir*

run:
	bash ./p3testscript-1

test:
	bash ./VALGRINDp3testscript-1
