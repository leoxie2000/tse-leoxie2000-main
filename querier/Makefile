# Makefile for 'querier' 
#
# Yi Xie, April, 2021
#

OBJS = querier.o
OBJS1 = fuzzquery.o
LIBS = -lm
L = ../libcs50
L2 = ../common
LLIBS = $(L2)/common.a $L/libcs50-given.a 
# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$(L2)
CC = gcc
MAKE = make
all: querier fuzzquery

querier: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o querier

fuzzquery: $(OBJS1) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS1) $(LLIBS) -o fuzzquery

fuzzquery.o: $L/mem.h $L/file.h
querier.o: $(L2)/pagedir.h  $(L2)/index.h $(L2)/word.h $(L)/counters.h $(L)/hashtable.h $L/file.h

.PHONY: test valgrind clean

test: querier fuzzquery
	bash -v testing.sh >& testing.out
valgrind: querier
	valgrind --leak-check=full --show-leak-kinds=all ./querier ../data/toscrape-1 ../data/toscrape-1.index


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f core
	rm -f vgcore*
	rm -f fuzzquery

