#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <papi.h>


#define NUM_EV 6
#define COUNTERS PAPI_L1_LDM, PAPI_L1_STM, PAPI_L1_TCM, PAPI_L2_LDM, PAPI_L2_STM, PAPI_L2_TCM


void table(long long *values,int32_t mode)
{
	FILE *FD = fopen("table.txt","a");

	fprintf(FD, "%d | %20lld | %20lld | %20lld | %20lld | %20lld | %20lld |\n", mode, values[0], values[1], values[2], values[3], values[4], values[5]);
	fclose(FD);	
}


void timeprint(int32_t mode, clock_t begin, int32_t size){
	FILE* file=NULL;	
	switch(mode){
	case 0:
		file=fopen("./result/timecount0.txt", "a");
		if(file==NULL)
		exit(20);
		break;
	case 1:
	file=fopen("./result/timecount1.txt", "a");
	if(file==NULL)
	exit(20);
	break;
	case 2:
		file=fopen("./result/timecount2.txt", "a");
		if(file==NULL)
		exit(20);
		break;
	case 3:
		file=fopen("./result/timecount3.txt", "a");
		if(file==NULL)
		exit(20);
		break;
	case 4:
		file=fopen("./result/timecount4.txt", "a");
		if(file==NULL)
		exit(20);
		break;
	case 5:
		file=fopen("./result/timecount5.txt", "a");
		if(file==NULL)
		exit(20);
		break;
	default:
		perror("Put correct command");
		exit(20);}

	clock_t end=clock();
    double time_spent=(double)(end-begin)/CLOCKS_PER_SEC;
	fprintf(file,"%lf %d\n", time_spent, size);
	fclose(file);}

void write_matd(FILE* file, int32_t **buf, int32_t count){
	for (int32_t i=0;i <count; i++)
	{

	fwrite(buf[i], count*sizeof(int32_t), 1, file);
	printf("%d\n", i);
	}
}

void write_matl(FILE* file, int64_t **buf, int32_t count) {
	for (int32_t i=0; i<count; i++)
	{
	fwrite(buf[i], count*sizeof(int64_t), 1, file);
	printf("%d\n", i);
	}
}

int32_t read_size(FILE *file) {
int32_t size=0;
if(fread(&size,  sizeof(int32_t), 1, file)!= 1) {
	fprintf(stderr, "error in read at 69");
	exit(4); }

return size; }

int32_t ** read_matd(FILE* file, int32_t size, int32_t **matrix){
	fseek(file, sizeof(int32_t), SEEK_CUR);
	for(int32_t i=0; i <size; i++) 
		if(fread(matrix[i], size*sizeof(int32_t), 1, file) != 1) {
		fprintf(stderr, "error in read at 80");
		exit(7);
		}
	return matrix;}

int64_t ** read_matl(FILE* file, int32_t size, int64_t **matrix) {
	fseek(file,  sizeof(int32_t), SEEK_CUR);
	for(int32_t i=0; i<size; i++)
		if(fread(matrix[i], size*sizeof(int64_t), 1, file) !=1) {
		fprintf(stderr, "error in read at 90");
		exit(7);}
	return matrix;
}


void ijk_d(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for(i=0; i<size; i++) {
	for(j=0; j<size; j++){
		int32_t sum = 0;
		for(k=0; k<size; k++)
			sum+=a[i][k]*b[k][j];
		c[i][j]=sum; }
	}
}

void mul_ikj32(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for (i=0; i<size; i++) {
	for (k=0; k<size; k++){
		int32_t r=a[i][k];	
		for(j = 0; j< size; j++)
		c[i][j] +=r*b[k][j];}
	}
}

void mul_kij32(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for (k=0; k<size; k++) 	{
	for (i=0; i < size; i++){
	for(j=0;j<size; j++)
		c[i][j]+=a[i][k]*b[k][j];}
	}
}

void jik_d(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for (j=0; j<size; j++) 	{
	for (i=0; i<size; i++){
	for(k=0; k<size; k++)
		c[i][j]+=a[i][k]*b[k][j];}
	}
}

void jki_d(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for (j=0; j<size; j++) 	{
	for (k=0; k<size; k++){
	for(i=0; i<size; i++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void kji_d(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i, j, k;
	for (k=0; k<size; k++) 	{
	for (j=0; j<size; j++){
	for(i=0; i<size; i++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void ijk_l(int64_t **a, int64_t **b, int64_t **c, int32_t size) {
	int32_t i, j, k;
	for (i = 0; i < size; i++) 	{
	for (j = 0; j < size; j++){
	for(k = 0; k < size; k++)
		c[i][j] += a[i][k]*b[k][j];}
	}
}

void ikj_l(int64_t **a, int64_t **b, int64_t **c, int32_t size){
	int32_t i, j, k;
	for (i=0; i<size; i++) 	{
	for (k=0; k<size; k++){
	for(j=0; j<size; j++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void kij_l(int64_t **a, int64_t **b, int64_t **c, int32_t size){
	int32_t i, j, k;
	for (k=0; k<size; k++) 	{
	for (i=0; i<size; i++){
	for(j=0; j<size; j++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void jik_l(int64_t **a, int64_t **b, int64_t **c, int32_t size){
	int32_t i, j, k;
	for (j=0; j<size; j++) 	{
	for (i=0; i<size; i++) {
	for(k=0; k<size; k++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void jki_l(int64_t **a, int64_t **b, int64_t **c, int32_t size){
	int32_t i, j, k;
	for (j=0; j<size; j++) 	{
	for (k=0; k<size; k++){
	for(i=0; i<size; i++)
		c[i][j] +=a[i][k]*b[k][j];	}
	}
}

void kji_l(int64_t **a, int64_t **b, int64_t **c, int32_t size){
	int32_t i, j, k;
	for (k =0; k<size; k++){
	for (j=0; j<size; j++){
	for(i=0; i<size; i++)
		c[i][j] +=a[i][k]*b[k][j];}
	}
}

void mat_multd(int32_t **ad, int32_t **bd, int32_t **cd, int32_t size, int32_t mode) {
	clock_t begin;
	switch(mode)
	{
		case 0:
			begin = clock();
			ijk_d(ad, bd, cd, size);
			timeprint(0, begin, size);
			break;
		case 1:
			begin = clock();
			mul_ikj32(ad, bd, cd, size);
			timeprint(1, begin, size);
			break;
		case 2:
			begin = clock();
			mul_kij32(ad, bd, cd, size);
			timeprint(2, begin, size);
			break;
		case 3:
			begin = clock();
			jik_d(ad, bd, cd, size);
			timeprint(3, begin, size);
			break;
		case 4:
			begin = clock();
			jki_d(ad, bd, cd, size);
			timeprint(4, begin, size);
			break;
		case 5:
			begin = clock();
			kji_d(ad, bd, cd, size);
			timeprint(5, begin, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

void mat_multl(int64_t **al, int64_t **bl, int64_t **cl, int32_t size, int32_t mode) {
	clock_t begin;
	switch(mode)
	{
		case 0:
			begin = clock();
			ijk_l(al, bl, cl, size);
			timeprint(0, begin, size);
			break;
		case 1:
			begin = clock();
			ikj_l(al, bl, cl, size);
			timeprint(1, begin, size);
			break;
		case 2:
			begin = clock();
			kij_l(al, bl, cl, size);
			timeprint(2, begin, size);
			break;
		case 3:
			begin = clock();
			jik_l(al, bl, cl, size);
			timeprint(3, begin, size);
			break;
		case 4:
			begin = clock();
			jki_l(al, bl, cl, size);
			timeprint(4, begin, size);
			break;
		case 5:
			begin = clock();
			kji_l(al, bl, cl, size);
			timeprint(5, begin, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

void mat_mult(int32_t **ad, int64_t **al, int32_t **bd, int64_t **bl, int32_t **cd,
int64_t **cl, int32_t size, int32_t mode, char lit) {	
	if (lit == 'd')
	mat_multd(ad, bd, cd, size, mode);
	else if (lit == 'l')
	mat_multl(al, bl, cl, size, mode);
	else{
		perror("int32/int64 error");
        exit(8);}
}

int main(int argc, char const **argv){
	FILE* fil1=fopen(argv[1], "r");
	if(fil1==NULL){
	fprintf(stderr, "error at 478");
	exit(1);}

	FILE* fil2=fopen(argv[2], "r");
	if(fil2==NULL){
	fprintf(stderr, "error at 484");
	exit(1);}

	FILE* fil3=fopen(argv[3], "w");
	if(fil3==NULL){
	fprintf(stderr, "error at 489");
	exit(1);}
    int32_t mode=0;
    mode=atoi(argv[4]);
    char lit;
    fread(&lit, sizeof(char), 1, fil1);
    int32_t size=read_size(fil1);
    if(argc!=5){
    fprintf(stderr,"wrong number of arguments in command line\n");
    exit(2);}

    int32_t i;
    int32_t **Ad;
    int64_t **Al;

    size=read_size(fil1);
    if(lit == 'd')
    {
    	Ad =(int32_t**)malloc(size*sizeof(int32_t *));
	    for(i=0; i<size; i++)
	    	Ad[i]=(int32_t*)malloc(size*sizeof(int32_t));
	    if(Ad==NULL){
	    	 fprintf(stderr,"error malloc at 418\n");
	    	exit(3);}
	    int32_t **Bd;
	    Bd=(int32_t**)malloc(size*sizeof(int32_t *));
	    for(i=0; i<size; i++)
	    	Bd[i]=(int32_t*)malloc(size*sizeof(int32_t));
	    if(Bd==NULL){
	    	fprintf(stderr,"error malloc at 424\n");
	    	exit(3);}
		int32_t **Cd;
		Cd=(int32_t**)calloc(size,sizeof(int32_t *));
	    for(i=0; i<size; i++)
	    	Cd[i]=(int32_t*)calloc(size,sizeof(int32_t));
	    if (Cd ==NULL){
	    	fprintf(stderr,"error malloc at 431\n");
	    	exit(3);}
	   	read_matd(fil1, size, Ad);
			fclose(fil1);
		fseek(fil2, sizeof(char), SEEK_SET);
		fseek(fil2, sizeof(int32_t), SEEK_CUR);
		read_matd(fil2, size, Bd);
			fclose(fil2);
		long long values[NUM_EV];	
		PAPI_library_init(PAPI_VER_CURRENT);
		int events[NUM_EV] = {COUNTERS};
		PAPI_start_counters( events, NUM_EV); 
		mat_multd(Ad, Bd, Cd, size, mode);
		PAPI_stop_counters( values, NUM_EV );
		table(values, mode);
		write_matd(fil3, Cd, size);
		fclose(fil3);
		for(i=0; i<size; i++)
		free(Ad[i]);
		free(Ad);
		for(i=0; i<size; i++)
		free(Bd[i]);
		free(Bd);
		for(i=0; i<size; i++)
		free(Cd[i]);
		free(Cd);
	}	
	else if(lit=='l')
	{
		Al=(int64_t**)malloc(size*sizeof(int64_t *));
	    for(i=0; i<size; i++)
		   	Al[i]=(int64_t*)malloc(size*sizeof(int64_t));
		if (Al==NULL){
		   	fprintf(stderr,"error malloc at 439\n");
			exit(3);}
		int64_t **Bl;
		Bl=(int64_t**)malloc(size*sizeof(int64_t *));
		for(i=0; i<size; i++)
		   	Bl[i]=(int64_t*)malloc(size*sizeof(int64_t));
		if (Bl==NULL){
		   	fprintf(stderr,"error malloc at 444\n");
		   		exit(3);}
		int64_t **Cl;
		Cl=(int64_t**)calloc(size,sizeof(int64_t *));
		for(i=0; i<size; i++)
		   	Cl[i]=(int64_t*)calloc(size,sizeof(int64_t));
		if (Cl==NULL){
		   	fprintf(stderr,"error malloc at 452\n");
		   		exit(3);}
		read_matl(fil1, size, Al);
			fclose(fil1);
		fseek(fil2, sizeof(char), SEEK_SET);
		fseek(fil2, sizeof(int32_t), SEEK_CUR);
		read_matl(fil2, size, Bl);
			fclose(fil2);
		long long values[NUM_EV];	
		PAPI_library_init(PAPI_VER_CURRENT);
		int events[NUM_EV] = {COUNTERS};
		PAPI_start_counters( events, NUM_EV); 
		mat_multl(Al, Bl, Cl, size, mode);
		PAPI_stop_counters( values, NUM_EV );
		table(values, mode);
		write_matl(fil3, Cl, size);
		fclose(fil3);
		for(i=0; i<size; i++)
		free(Al[i]);
		free(Al);
		for(i=0; i<size; i++)
		free(Bl[i]);
		free(Bl);
		for(i=0; i<size; i++)
		free(Cl[i]);
		free(Cl);
	}
	else
	{
		fprintf(stderr, "not d or l\n");
	}
    
	return 0;
}