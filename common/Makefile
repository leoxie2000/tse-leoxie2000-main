# Makefile for 'set' module
#
# Yi Xie, April, 2021
#

L = ../libcs50
OBJS = pagedir.o word.o index.o
LIB = common.a
LLIBS = $L/libcs50-given.a

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(LLIBS)  $(OBJS)

index.o: $L/hashtable.h $L/counters.h $L/file.h
pagedir.o: $L/webpage.h $L/file.h
.PHONY: clean


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core
	rm -f $(LIB) *~ *.o
	rm -f *.a


