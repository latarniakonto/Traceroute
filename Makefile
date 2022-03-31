# -*- MakeFile -*-
# Makefile for traceroute program
CFLAGS=-std=gnu17 -Wall -Wextra
# .KEEP_STATE:
CC=gcc

traceroute: traceroute.o traceroute_essentials.o traceroute_sending.o traceroute_receiving.o
	$(CC) traceroute.o traceroute_essentials.o traceroute_sending.o traceroute_receiving.o -o traceroute

traceroute.o: traceroute.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute.c

traceroute_essentials.o: traceroute_essentials.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute_essentials.c

traceroute_sending.o: traceroute_sending.c traceroute_sending.h
	$(CC) $(CFLAGS) -c traceroute_sending.c

traceroute_receiving.o: traceroute_receiving.c traceroute_receiving.h
	$(CC) $(CFLAGS) -c traceroute_receiving.c

clean:
	rm -f *.o

distclean:
	rm -f *.o traceroute
