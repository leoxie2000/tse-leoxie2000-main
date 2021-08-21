# Makefile for 'indexer' 
#
# Yi Xie, April, 2021
#

OBJS = indexer.o
OBJS1 = indextest.o
LIBS = -lm
L = ../libcs50
L2 = ../common
LLIBS = $(L2)/common.a $L/libcs50-given.a 
# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$(L2)
CC = gcc
MAKE = make
all: indexer indextest
indexer: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o indexer

indextest: $(OBJS1) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS1) $(LLIBS) -o indextest
indexer.o: $L/counters.h $L/webpage.h  $(L2)/pagedir.h $(L2)/word.h $(L2)/index.h 
indextest.o: $L/file.h

.PHONY: test valgrind clean

test: testing.sh
	bash -v testing.sh >& testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f core
	rm -f indextest

