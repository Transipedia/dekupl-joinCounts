CC=gcc
CFLAGS=-g -Wall -O2 -Wno-unused-function
HEADERS=kstring.h
OBJECTS=$(HEADERS:.h=.o)
LIBS=-lz

all:joinCounts

joinCounts: joinCounts.c $(HEADERS) $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $< -o $@ $(LIBS)
