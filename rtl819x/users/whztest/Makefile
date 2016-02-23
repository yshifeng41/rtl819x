# 2016/01/15 Whz
#export CC = rsdk-linux-gcc

LDFLAGS = -s -Wall
CFLAGS := -s -Wall -c -Os

all:		whztest

whztest:		whztest.o
		$(CC) $(LDFLAGS) -o whztest whztest.o -pthread

whztest.o:		whztest.c
		$(CC) $(CFLAGS) -o whztest.o whztest.c

romfs:
		$(ROMFSINST) whztest /bin/whztest

clean:
		rm -f *.o
		rm -f whztest
		rm -f core

