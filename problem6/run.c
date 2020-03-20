//apt install mpich
#include <mpi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>

#define MASTER 0


void tableprintN(double S,  double E, double T, int64_t N)
{
		FILE *fileSN = NULL;
		FILE *fileEN = NULL;
		FILE *fileTN = NULL;
		fileSN = fopen("tableSN.txt", "a+");
		fprintf(fileSN, "%f\t%ld\n", S, (long)N);
		fileEN = fopen("tableEN.txt", "a+");
		fprintf(fileEN, "%f\t%ld\n", E, (long)N);
		fileTN = fopen("tableTN.txt", "a+");
		fprintf(fileTN, "%f\t%ld\n", T, (long)N);
		fclose(fileEN);
		fclose(fileSN);
		fclose(fileTN);
}


static int seed(const int rank, const int size)
{
	int *seeds = NULL;
	if (rank == MASTER) {
		srand(time(NULL));
		seeds = malloc(size * sizeof(int));
		if (seeds == NULL) {
			return -1;
		}
		int i=0;
		for (; i < size; i++) 
		{
			seeds[i] = rand();
		}
	}
	int seed;
	MPI_Scatter(seeds, 1, MPI_INTEGER, &seed, 1, MPI_INTEGER, MASTER, MPI_COMM_WORLD);
	if (rank == MASTER) {
		free(seeds);
	}
	srand(seed);
	return 0;
}

static int *gen_numbers(const int n)
{
	int *nums = malloc(n * sizeof(int));
	if (nums == NULL) 
		return NULL;
	int i = 0;
	for (i = 0; i < n; i++) 
	{
		nums[i] = rand() % 100500;
	}
	return nums;
}

static int is_sorted(const int *nums, const int n)
{
	int i=0;
	for (; i < n - 1; i++) 
	{
		if (nums[i] > nums[i + 1]) {
			return 0;
		}
	}
	return 1;
}

static void min_max(const int *nums, const int n, int *min, int *max)
{
	*min = nums[0];
	*max = nums[0];
	int i;
	for ( i = 1; i < n; i++) {
		if (nums[i] > *max) {
			*max = nums[i];
		}
		if (nums[i] < *min) {
			*min = nums[i];
		}
	}
}

static int sort_cmp(const void *el1, const void *el2)
{
	return *((int *)el1) - *((int *)el2);
}


static int check(const int rank, const int size, const int *nums, const int n)
{
	if (!is_sorted(nums, n)) {
		return -1;
	}
	int pair[2];
	min_max(nums, n, pair, &pair[1]);
	int *buf = NULL;
	if (rank == MASTER) {
		buf = malloc(2 * size * sizeof(int));
		if (buf == NULL) {
			return -2;
		}
	}
	MPI_Gather(pair, 2, MPI_INTEGER, buf, 2, MPI_INTEGER, MASTER, MPI_COMM_WORLD);
	if (rank == MASTER) {
		int rc = is_sorted(buf, 2 * size);
		free(buf);
		return rc == 0;
	}
	return 0;
}

static void merge(int* data, const int size)
{
  int s1 = 0;
  int s2 = 0;
  int *result = calloc(2*size, sizeof(int));
	int i;
  for(i = 0; i < size * 2; i++)
  {
    if(((s2 == size) || (data[s1] <= data[size+s2])) && (s1 != size))
    {
      result[i] = data[s1++];
    }
    else if(s2 != size)
    {
      result[i] = data[size+s2++];
    }
  }
 
  for (i = 0; i < size * 2; i++)
  {
  	data[i] = result[i];
  }
  free(result);////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

static void odd_sort(const int rank, const int poolSize, int* nums, const int chunkSize)
{
  if ((rank % 2 == 0) && (rank != 0))
  {
    MPI_Send(nums, chunkSize, MPI_INTEGER, rank - 1, 0, MPI_COMM_WORLD);
  }
  if ((rank % 2 == 1) && (rank != poolSize - 1))
  {
    MPI_Recv(&nums[chunkSize], chunkSize, MPI_INTEGER, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    merge(nums, chunkSize);

    MPI_Send(&nums[chunkSize], chunkSize, MPI_INTEGER, rank + 1, 1, MPI_COMM_WORLD);

  }
  if ((rank % 2 == 0) && (rank != 0))
  {
    MPI_Recv(nums, chunkSize, MPI_INTEGER, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  }
}

static void even_sort(const int rank, const int poolSize, int* nums, const int chunkSize)
{
	if ((rank % 2 == 1))
	{
	   MPI_Send(nums, chunkSize, MPI_INTEGER, rank - 1, 0, MPI_COMM_WORLD);
	}
	if ((rank % 2 == 0) && (rank != poolSize - 1))
	{
		MPI_Recv(&nums[chunkSize], chunkSize, MPI_INTEGER, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    merge(nums, chunkSize);

	    MPI_Send(&nums[chunkSize], chunkSize, MPI_INTEGER, rank + 1, 1, MPI_COMM_WORLD);
	}
	if ((rank % 2 == 1))
	{
		MPI_Recv(nums, chunkSize, MPI_INTEGER, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}

static void sort(const int rank, const int poolSize, int *nums, const int chunkSize)
{

 	qsort(nums, chunkSize, sizeof(int), sort_cmp);
 	int breakCheck = -1;

   	nums  = realloc(nums, 2*chunkSize * sizeof(int)); /* we could allocate this memory earlier, in 'gen_numbers()', but i did't want to change already written code */

    while (1) 
    {
	    odd_sort(rank, poolSize, nums, chunkSize);
	    MPI_Barrier(MPI_COMM_WORLD);
	    even_sort(rank, poolSize, nums, chunkSize);

	   	MPI_Barrier(MPI_COMM_WORLD);

	   	breakCheck = !check(rank, poolSize, nums, chunkSize);
	   	MPI_Bcast(&breakCheck, 1, MPI_INTEGER, MASTER, MPI_COMM_WORLD);

	   if(breakCheck)
	   {
	   		break;
	   }
	   
    }
}

int main(int argc, char **argv)
{
	double E, S;
	double time_spent = 0;
	clock_t begin = clock();
	MPI_Init(&argc, &argv);


	if (argc != 2) {
		return -1;
	}
	int n = atoi(argv[1]);
	assert(n > 0);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rc = seed(	rank, size);
	assert(rc == 0);
	if(rc==1);
	int *nums = gen_numbers(n);
	assert(nums);
 	
	sort(rank, size, nums, n);
	rc = check(rank, size, nums, n);

	assert(rc == 0);

	free(nums);
	MPI_Finalize();
	clock_t end = clock();
	time_spent = (((double)(end - begin) / CLOCKS_PER_SEC));
	printf("%f\n", time_spent);
	S = 0.440000 / time_spent;
	E = S / size;
	tableprintN(S, E, time_spent, size);

return 0;

}

