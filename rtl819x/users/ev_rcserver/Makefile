# 2016/03/11 YSF
#export CC = rsdk-linux-gcc

LDFLAGS = -s -Wall
CFLAGS := -s -Wall -c -Os

all:		ev_rcserver

ev_rcserver:		ev_rcserver.o
		$(CC) $(LDFLAGS) -o ev_rcserver ev_rcserver.o -pthread

ev_rcserver.o:		ev_rcserver.c
		$(CC) $(CFLAGS) -o ev_rcserver.o ev_rcserver.c

romfs:
		$(ROMFSINST) ev_rcserver /bin/ev_rcserver

clean:
		rm -f *.o
		rm -f ev_rcserver
		rm -f core

