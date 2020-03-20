CC = mpicc

CFLAGS =-ggdb\
        -Wall\
        -Werror\
        -O2
        -std=c99

all:
	$(CC) run.c $(CFLAGS) -o run

test: all
	mpirun -np 4 ./run 1024

clean:
	rm run

.PHONY: all test clean 
