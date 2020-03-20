#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


#define NUM_EV 2
#define COUNTERS PAPI_L1_LDM, PAPI_L1_STM


void table(long long *values, int32_t mode)
{
	FILE *file = fopen("table.txt","a");

	fprintf(file, "%d | %20lld | %20lld |\n", mode, values[0], values[1]);
	fclose(file);	
}

int
check_open(const char *pathname, int flags, mode_t mode)
{
    int fd = open(pathname, flags, mode);
    if (fd == -1)
    {
        perror("open");
        exit(1);
    }
    return fd;
}

int32_t**
check_malloc32(int32_t size)
{
	int32_t i;

    int32_t **mem = (int32_t **) calloc(size, sizeof(int32_t *));
    for (i = 0; i < size; ++i)
    	mem[i] = (int32_t *) calloc(size, sizeof(int32_t));

    if (mem == NULL)
    {
        perror("malloc");
        exit(2);
    }

    return mem;
}

int64_t**
check_malloc64(int32_t size)
{
	int32_t i;
	int64_t **mem = (int64_t **) calloc(size, sizeof(int64_t *));
    for (i = 0; i < size; ++i)
    	mem[i] = (int64_t *) calloc(size, sizeof(int64_t));

    if (mem == NULL)
    {
        perror("malloc");
        exit(2);
    }

    return mem;
}

void
check_lseek(int fd, off_t offset, int whence)
{
    if (lseek(fd, offset, whence) == (off_t) - 1)
    {
        perror("lseek");
        exit(3);
    }
}

void
check_read(int fd, void *buf, int32_t count)
{
    if (read(fd, buf, count) != count)
    {
        perror("read");
        exit(4);
    }
}

char
check_key(int fd, char *read_key)
{
	read(fd, read_key, sizeof(char));
	if ('d' == *read_key)
		return *read_key;
	else if ('l' == *read_key)
		return *read_key;
	else
	{
		fputs("This file is not matrix in format 'd/l' in header\n", stderr);
        exit(7);
	}
}

void 
timeprint(int32_t mode, char key, clock_t begin, int32_t size)
{
	FILE* fd = NULL;
		
	switch(mode)
	{
		case 0:
			fd = fopen("./timecount0.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		case 1:
			fd = fopen(".//timecount1.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		case 2:
			fd = fopen("./timecount2.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		case 3:
			fd = fopen("./timecount3.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		case 4:
			fd = fopen("./timecount4.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		case 5:
			fd = fopen("./timecount5.txt", "a");
			if(fd == NULL)
				exit(20);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}

	clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	fprintf(fd,"%lf %d\n", time_spent, size);
	fclose(fd);
}

void
write_matrix32(int fd, int32_t **buf, int32_t *count, char *read_key)
{
	int32_t i;
	write(fd, read_key, 1);
	write(fd, count, 4);

		for (i = 0; i < *count; ++i)
		{
			write(fd, buf[i], *count*sizeof(int32_t));
			
		}
}

void
write_matrix64(int fd, int64_t **buf, int32_t *count, char *read_key)
{
	int32_t i;
	write(fd, read_key, 1);
	write(fd, count, 4);


		for (i = 0; i < *count; ++i)
		{
			write(fd, buf[i], *count*sizeof(int64_t));			
		}
}

int32_t
get_size(int fd)
{
	int32_t size = 0;
    check_read(fd, &size, sizeof(int32_t));

    return size;
}

int32_t **
get_matrix32(int fd, int32_t **matrix, int32_t size)
{
	int32_t i;
	for(i = 0; i < size; ++i)
	{
		if(read(fd, matrix[i], size*sizeof(int32_t)) == -1)
		{
			perror("read");
			exit(4);
		}
	}

	return matrix;
}

int64_t **
get_matrix64(int fd, int64_t **matrix, int32_t size)
{
	int32_t i;
	for(i = 0; i < size; ++i)
	{
		if(read(fd, matrix[i], size*sizeof(int64_t)) == -1)
		{
			perror("read");
			exit(4);
		}
	}
	return matrix;
}

void
mul_ijk32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;
	for (i = 0; i < size; ++i) 
	{

		for (j = 0; j < size; ++j) 
		{
			
			int32_t sum = 0;
			for(k = 0; k < size; ++k)
				sum +=a[i][k] * b[k][j];
			c[i][j] = sum;

		}
	}
}

void
mul_ikj32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;

	for (i = 0; i < size; ++i) 
	{
		for (k = 0; k < size; ++k) 
		{
			int32_t r = a[i][k];
			for(j = 0; j < size; ++j)
				c[i][j] += r*b[k][j];
		}
	}
}

void
mul_kij32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;

	for (k = 0; k < size; ++k) 
	{
		for (i = 0; i < size; ++i) 
		{
			for(j = 0; j < size; ++j)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_jik32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;

	for (j = 0; j < size; ++j) 
	{
		for (i = 0; i < size; ++i) 
		{
			for(k = 0; k < size; ++k)
				c[i][j] += a[i][k]*b[k][j];
            
		}
	}
}

void
mul_jki32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;

	for (j = 0; j < size; ++j) 
	{
		for (k = 0; k < size; ++k) 
		{
			for(i = 0; i < size; ++i)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_kji32(int32_t **a, int32_t **b, int32_t **c, int32_t size)
{
	int32_t i, j, k;

	for (k = 0; k < size; ++k) 
	{
		for (j = 0; j < size; ++j) 
		{
			for(i = 0; i < size; ++i)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_ijk64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (i = 0; i < size; ++i) 
	{
		for (j = 0; j < size; ++j) 
		{
			for(k = 0; k < size; ++k)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_ikj64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (i = 0; i < size; ++i) 
	{
		for (k = 0; k < size; ++k) 
		{
			for(j = 0; j < size; ++j)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_kij64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (k = 0; k < size; ++k) 
	{
		for (i = 0; i < size; ++i) 
		{
			for(j = 0; j < size; ++j)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_jik64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (j = 0; j < size; ++j) 
	{
		for (i = 0; i < size; ++i) 
		{
			for(k = 0; k < size; ++k)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_jki64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (j = 0; j < size; ++j) 
	{
		for (k = 0; k < size; ++k) 
		{
			for(i = 0; i < size; ++i)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_kji64(int64_t **a, int64_t **b, int64_t **c, int32_t size)
{
	int32_t i, j, k;

	for (k = 0; k < size; ++k) 
	{
		for (j = 0; j < size; ++j) 
		{
			for(i = 0; i < size; ++i)
				c[i][j] += a[i][k]*b[k][j];
		}
	}
}

void
mul_matrix32(int32_t **a32, int32_t **b32, int32_t **c32, int32_t size, int32_t mode)
{
	clock_t begin;

	switch(mode)
	{
		case 0:
			begin = clock();
			mul_ijk32(a32, b32, c32, size);
			timeprint( 0, 'd', begin, size);
			break;
		case 1:
			begin = clock();
			mul_ikj32(a32, b32, c32, size);
			timeprint(1, 'd', begin, size);
			break;
		case 2:
			begin = clock();
			mul_kij32(a32, b32, c32, size);
			timeprint( 2, 'd', begin, size);
			break;
		case 3:
			begin = clock();
			mul_jik32(a32, b32, c32, size);
			timeprint( 3, 'd', begin, size);
			break;
		case 4:
			begin = clock();
			mul_jki32(a32, b32, c32, size);
			timeprint( 4, 'd', begin, size);
			break;
		case 5:
			begin = clock();
			mul_kji32(a32, b32, c32, size);
			timeprint( 5, 'd', begin, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

void
mul_matrix64(int64_t **a64, int64_t **b64, int64_t **c64, int32_t size, int32_t mode)
{
	clock_t begin;

	switch(mode)
	{
		case 0:
			begin = clock();
			mul_ijk64(a64, b64, c64, size);
			timeprint(0, 'l', begin, size);
			break;
		case 1:
			begin = clock();
			mul_ikj64(a64, b64, c64, size);
			timeprint( 1, 'l', begin, size);
			break;
		case 2:
			begin = clock();
			mul_kij64(a64, b64, c64, size);
			timeprint( 2, 'l', begin, size);
			break;
		case 3:
			begin = clock();
			mul_jik64(a64, b64, c64, size);
			timeprint( 3, 'l', begin, size);
			break;
		case 4:
			begin = clock();
			mul_jki64(a64, b64, c64, size);
			timeprint( 4, 'l', begin, size);
			break;
		case 5:
			begin = clock();
			mul_kji64(a64, b64, c64, size);
			timeprint( 5, 'l', begin, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

int
main(int argc, char const *argv[])
{

    if (argc != 5)
    {
        fputs("usage: ./mul_matrix <pathname_a> <pathname_b> <pathname_c> <mode>\n", stderr);
        return 6;
    }

	int fd_a = check_open(argv[1], O_RDONLY, 0600);
	int fd_b = check_open(argv[2], O_RDONLY, 0600);
	int fd_c = check_open(argv[3], O_WRONLY | O_CREAT, 0600);

    int32_t size = 0;
    int32_t mode = 0;

    mode = atoi(argv[4]);

    char read_key = check_key(fd_a, &read_key);
 
	if(read_key == 'd')
	{
    	size = get_size(fd_a);

    	int32_t **a_32 = check_malloc32(size);
    	int32_t **b_32 = check_malloc32(size);
		int32_t **c_32 = check_malloc32(size);

		get_matrix32(fd_a, a_32, size);
		close(fd_a);

		check_lseek(fd_b, sizeof(char)+sizeof(int32_t), SEEK_SET);
		get_matrix32(fd_b, b_32, size);
		close(fd_b);
		
		long long values[NUM_EV];	
		PAPI_library_init(PAPI_VER_CURRENT);
		int events[NUM_EV] = {COUNTERS};
		PAPI_start_counters( events, NUM_EV); 

		mul_matrix32(a_32, b_32, c_32, size, mode);

		PAPI_stop_counters( values, NUM_EV );
		table(values, mode);

		write_matrix32(fd_c, c_32, &size, &read_key);
		close(fd_c);
		int32_t i;
		for(i = 0; i < size; ++i)
		{
			free(a_32[i]);
			
			free(b_32[i]);
			
			free(c_32[i]);
			
		}
			free(a_32);
			free(b_32);
			free(c_32);
	}
	else if(read_key == 'l')
    {
    	size = get_size(fd_a);

		int64_t **a_64 = check_malloc64(size);
		int64_t **b_64 = check_malloc64(size);
		int64_t **c_64 = check_malloc64(size);

		get_matrix64(fd_a, a_64, size);
		close(fd_a);

		check_lseek(fd_b, sizeof(char)+sizeof(int32_t), SEEK_SET);
		get_matrix64(fd_b, b_64, size);
		close(fd_b);

		long long values[NUM_EV];	
		PAPI_library_init(PAPI_VER_CURRENT);
		int events[NUM_EV] = {COUNTERS};
		PAPI_start_counters(events, NUM_EV); 

		mul_matrix64(a_64, b_64, c_64, size, mode);

		PAPI_stop_counters( values, NUM_EV );
		table(values, mode);

		write_matrix64(fd_c, c_64, &size, &read_key);
		close(fd_c);

		int32_t i;
		for(i = 0; i < size; ++i)
		{
			free(a_64[i]);
			
			free(b_64[i]);
			
			free(c_64[i]);
			
		}
		free(a_64);
		free(b_64);
		free(c_64);
	}
	
	return 0;
}