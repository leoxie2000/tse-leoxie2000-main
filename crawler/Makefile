# Makefile for 'crawler' 
#
# Yi Xie, April, 2021
#

OBJS = crawler.o  
LIBS = -lm
L = ../libcs50
L2 = ../common
LLIBS = $L/libcs50-given.a $(L2)/common.a
# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$(L2)
CC = gcc
MAKE = make

crawler: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) -o $@

crawler.o: $L/bag.h $L/hashtable.h $L/webpage.h  $(L2)/pagedir.h

.PHONY: test valgrind clean

# expects a file `test.names` to exist; it can contain any text.
test: crawler
	bash -v testing.sh >& testing.out
# expects a file `test.names` to exist; it can contain any text.
valgrind: crawler
	valgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 2
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core

