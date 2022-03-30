# -*- MakeFile -*-
# Makefile for traceroute program
CFLAGS=-std=gnu17 -Wall -Wextra
# .KEEP_STATE:
CC=gcc

traceroute: traceroute.o traceroute_essentials.o traceroute_sending_essentials.o traceroute_receiving_essentials.o
	$(CC) traceroute.o traceroute_essentials.o traceroute_sending_essentials.o traceroute_receiving_essentials.o -o traceroute

traceroute.o: traceroute.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute.c

traceroute_essentials.o: traceroute_essentials.c traceroute_essentials.h
	$(CC) $(CFLAGS) -c traceroute_essentials.c

traceroute_sending_essentials.o: traceroute_sending_essentials.c traceroute_sending_essentials.h
	$(CC) $(CFLAGS) -c traceroute_sending_essentials.c

traceroute_receiving_essentials.o: traceroute_receiving_essentials.c traceroute_receiving_essentials.h
	$(CC) $(CFLAGS) -c traceroute_receiving_essentials.c

clean:
	rm -f *.o

distclean:
	rm -f *.o traceroute
