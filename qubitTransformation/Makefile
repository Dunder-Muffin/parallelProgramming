CC = g++
CFLAGS = -Wall  -O0 -ggdb -g -lm -fopenmp
all: main 

main: main.cpp
	$(CC) $< $(CFLAGS) -o run 

clean:
	rm run
