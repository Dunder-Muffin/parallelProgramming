CC = gcc
CFLAGS = -g -Wall -Werror -O0 -lpapi 
all:  PAPI_main  gen_a gen_b PAPI_main
  
gen_a: gen_a.c
	$(CC) $(CFLAGS)  -o gen_a $<
gen_b: gen_b.c
	$(CC) $(CFLAGS)  -o gen_b $<
PAPI_main: 
	$(CC) $(CFLAGS) PAPI_main.c  -lpapi   -o PAPI_main $<
clean:
	rm a b c  gen_a gen_b  PAPI_main

