# 2016/03/11 YSF
#export CC = rsdk-linux-gcc

LDFLAGS = -s -Wall
CFLAGS := -s -Wall -c -Os

OBJECTS := ev_rcserver.o rc_protocol.o ev_utility.o

all:		ev_rcserver

ev_rcserver:		$(OBJECTS)
		$(CC) $(LDFLAGS) -o ev_rcserver $(OBJECTS) -pthread

ev_rcserver.o:		ev_rcserver.c ev_utility.h rc_protocol.h
		$(CC) $(CFLAGS) -o ev_rcserver.o ev_rcserver.c

rc_protocol.o:		rc_protocol.c rc_protocol.h
		$(CC) $(CFLAGS) -o rc_protocol.o rc_protocol.c

ev_utility.o:		ev_utility.c ev_utility.h
		$(CC) $(CFLAGS) -o ev_utility.o ev_utility.c

romfs:
		$(ROMFSINST) ev_rcserver /bin/ev_rcserver

clean:
		rm -f *.o
		rm -f ev_rcserver
		rm -f core

