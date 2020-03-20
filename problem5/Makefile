CC = gcc

STR := $(shell basename `dirname ${PWD}`)
CFLAGS = -fopenmp\
	-ggdb\
	-Wall\
	-Werror\
	-pthread\
	-O2

all:
	$(CC) run.c payload.c sha256.c $(CFLAGS) -o run

test: all
	./run $(STR) 4 4

clean:
	rm run

.PHONY: all test clean
