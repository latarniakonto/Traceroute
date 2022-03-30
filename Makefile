# -*- MakeFile -*-
# Makefile for traceroute program
CFLAGS=-std=gnu17 -Wall -Wextra
# .KEEP_STATE:
CC=gcc

traceroute: traceroute.o traceroute_essentials.o
	$(CC) traceroute.o traceroute_essentials.o -o traceroute

traceroute.o: traceroute.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute.c

traceroute_essentials.o: traceroute_essentials.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute_essentials.c

clean:
	rm -f *.o

distclean:
	rm -f *.o traceroute
