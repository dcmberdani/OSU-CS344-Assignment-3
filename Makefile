CC=gcc --std=gnu99 -g

all: smallsh

cmdline.o: cmdline.c cmdline.h  shellinfo.h
	$(CC) -c cmdline.c shellinfo.h

functions.o: functions.c functions.h  shellinfo.h
	$(CC) -c functions.c  shellinfo.h

smallsh: main.c cmdline.o functions.o shellinfo.h
	$(CC) cmdline.o functions.o shellinfo.h main.c -o smallsh

clean:
	rm -rf cmdline.o functions.o smallsh shellinfo.h.gch
