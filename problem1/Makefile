CC = gcc
CFLAGS = -Wall -Werror -O0
all: gen_a gen_b mainx
gen_a: gen_a.c
	$(CC) $(CFLAGS) -o gen_a $<
gen_b: gen_b.c
	$(CC) $(CFLAGS) -o gen_b $<
mainx: mainx.c
	$(CC) $(CFLAGS) -o mainx $<
test: san bash 
san: 
	$(CC) $(CFLAGS) -fsanitize=address mainx.c -o testx $<
bash: 
	./test.sh ./testx $<
clean:
	rm a c b gen_a gen_b mainx
