# 2016/03/11 YSF
#export CC = rsdk-linux-gcc

LDFLAGS = -s -Wall
CFLAGS := -s -Wall -c -Os

OBJECTS := ev_rcserver.o rc_protocol.o

all:		ev_rcserver

ev_rcserver:		$(OBJECTS)
		$(CC) $(LDFLAGS) -o ev_rcserver $(OBJECTS) -pthread

ev_rcserver.o:		ev_rcserver.c rc_protocol.h
		$(CC) $(CFLAGS) -o ev_rcserver.o ev_rcserver.c

rc_protocol.o:		rc_protocol.c rc_protocol.h
		$(CC) $(CFLAGS) -o rc_protocol.o rc_protocol.c

romfs:
		$(ROMFSINST) ev_rcserver /bin/ev_rcserver

clean:
		rm -f *.o
		rm -f ev_rcserver
		rm -f core

