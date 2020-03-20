#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

int check_open(const char *pathname, int flags, mode_t mode) {
    int fd=open(pathname, flags, mode);
    if (fd==-1) {
        perror("open");
        exit(1);
    }
    return fd;
}

int32_t** check_mallocd(int32_t size) {
	int32_t i;

    int32_t **mem=(int32_t **) calloc(size, sizeof(int32_t *));
	if (mem==NULL) {
        fprintf(stderr, "malloc");
        exit(2);
    }

    for (i=0;i<size;i++) {
    	mem[i]=(int32_t *)calloc(size, sizeof(int32_t));
	    if (mem[i]==NULL) {
	        fprintf(stderr, "malloc");
	        exit(2);
	    }
	}
    return mem;
}

int64_t** check_mallocl(int32_t size) {
	int32_t i;

	int64_t **mem=(int64_t **)calloc(size, sizeof(int64_t *));
	if (mem==NULL) {
        fprintf(stderr, "malloc");
        exit(2);
    }
    for (i=0;i<size;i++) {
    	mem[i]=(int64_t *)calloc(size, sizeof(int64_t));
    	if (mem[i]==NULL) {
	        fprintf(stderr, "malloc");
	        exit(2);
	    }
	}
    return mem;
}

void check_read(int fd, void *buf, int32_t count) {
    if (read(fd, buf, count)!=count) {
        perror("read");
        exit(4);
    }
}

void check_write(int fd, void *buf, int32_t count) {
	if(write(fd, buf, count)==-1)  {
		perror("write");
		exit(22);
	}
}

void writed(int fd, int32_t **buf, int32_t *count, char *lit) {
	int32_t i;
	check_write(fd, lit, 1);
	check_write(fd, count, 4);
	for (i=0;i<*count;i++)
		check_write(fd, buf[i], *count*sizeof(int32_t));
}

void writel(int fd, int64_t **buf, int32_t *count, char *lit) {
	int32_t i;
	check_write(fd, lit, 1);
	check_write(fd, count, 4);

	for (i=0;i<*count;i++)
		check_write(fd, buf[i], *count*sizeof(int64_t));			
}

int32_t get_size(int fd)
{
    int32_t size=0;
    check_read(fd, &size, sizeof(int32_t));

    return size;
}


int32_t** get_matrixd(int fd, int32_t **matrix, int32_t size)
{
	int32_t i;
	for(i=0;i<size;i++) {
		if(read(fd, matrix[i], size*sizeof(int32_t))==-1) {
			perror("read");
			exit(4);
		}
	}
	return matrix;
}

int64_t** get_matrixl(int fd, int64_t **matrix, int32_t size) {
	int32_t i;
	for(i=0;i<size; i++) {
		if(read(fd, matrix[i], size*sizeof(int64_t))==-1) {
			perror("read");
			exit(4);
		}
	}
	return matrix;
}

char check_key(int fd, char *lit) {
	read(fd, lit, sizeof(char));
	if ('d'==*lit)
		return *lit;
	else if ('l'==*lit)
		return *lit;
	else {
		fputs("This file is not matrix in format 'd/l' in header\n", stderr);
        exit(7);
	}
}

void multd_ijk(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			int32_t sum=0;
			for(k=0;k<size;k++)
				sum+=a[i][k]*b[k][j];
			c[i][j]=sum;
		}	
	}
}

void multd_ikj(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for(i=0;i<size;i++) {
		for(k=0;k<size;k++) {
			int32_t sum=0;
			for(j=0;j<size;j++)
				sum+=a[i][j]*b[j][k];
			c[i][k]=sum;
		}
	}
}

void multd_kij(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for(k=0;k<size;k++) {
		for(i=0;i<size;i++) {
			int32_t sum=0;
			for(j=0;j<size;j++)
				sum+=a[i][j]*b[j][k];
			c[i][k]=sum;
		}
	}
}

void multd_jik(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for (j=0;j<size;j++) {
		for(i=0;i<size;i++) {
			int32_t sum=0;
			for(k=0;k<size;k++)
				sum+=a[i][k]*b[k][j];
			c[i][j]=sum;       
		}
	}
}

void multd_jki(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for(j=0;j<size;j++) {
		for(k=0;k<size;k++) {
			int32_t sum=0;
			for(i=0;i<size;i++)
				sum+=a[k][i]*b[i][j];
			c[k][j]=sum;
		}
	}
}

void multd_kji(int32_t **a, int32_t **b, int32_t **c, int32_t size) {
	int32_t i,j,k;
	for(k=0;k<size;k++) {
		for(j=0;j<size;j++) {
			int32_t sum=0;
			for(i=0;i<size;i++)
				sum+=a[k][i]*b[i][j];
			c[k][j]=sum;
		}
	}
}

void multl_ijk(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(i=0;i<size;i++) {
		for(j=0;j<size;j++) {
			int64_t sum=0;
			for(k=0;k<size;k++)
				sum+=a[i][k]*b[k][j];
			c[i][j]=sum;
		}	
	}
}

void multl_ikj(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(i=0;i<size;i++) {
		for(k=0;k<size;k++) {
			int64_t sum=0;
			for(j=0;j<size;j++)
				sum+=a[i][j]*b[j][k];
			c[i][k]=sum;
		}
	}
}

void multl_kij(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(k=0;k<size;k++) {
		for(i=0;i<size;i++) {
			int64_t sum=0;
			for(j=0;j<size;j++)
				sum+=a[i][j]*b[j][k];
			c[i][k]=sum;
		}
	}
}

void multl_jik(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(j=0;j<size;j++) {
		for(i=0;i<size;i++) {
			int64_t sum=0;
			for(k=0;k<size;k++)
				sum+=a[i][k]*b[k][j];
			c[i][j]=sum; 
		}
	}
}

void multl_jki(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(j=0;j<size;j++) {
		for(k=0;k<size;k++) {
			int64_t sum=0;
			for(i=0;i<size;i++)
				sum+=a[k][i]*b[i][j];
			c[k][j]=sum;
		}
	}
}

void multl_kji(int64_t **a, int64_t **b, int64_t **c, int64_t size) {
	int64_t i,j,k;
	for(k=0;k<size;k++) {
		for(j=0;j<size;j++) {
			int64_t sum=0;
			for(i=0;i<size;i++)
				sum+=a[k][i]*b[i][j];
			c[k][j]=sum;
		}
	}
}

void multd(int32_t **a32, int32_t **b32, int32_t **c32, int32_t size, int32_t mode) {
	switch(mode) {
		case 0:
			multd_ijk(a32, b32, c32, size);
			break;
		case 1:
			multd_ikj(a32, b32, c32, size);
			break;
		case 2:
			multd_kij(a32, b32, c32, size);
			break;
		case 3:
			multd_jik(a32, b32, c32, size);
			break;
		case 4:
			multd_jki(a32, b32, c32, size);
			break;
		case 5:
			multd_kji(a32, b32, c32, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

void multl(int64_t **a64, int64_t **b64, int64_t **c64, int32_t size, int32_t mode) {
	switch(mode) {
		case 0:
			multl_ijk(a64, b64, c64, size);
			break;
		case 1:
			multl_ikj(a64, b64, c64, size);
			break;
		case 2:
			multl_kij(a64, b64, c64, size);
			break;
		case 3:
			multl_jik(a64, b64, c64, size);
			break;
		case 4:
			multl_jki(a64, b64, c64, size);
			break;
		case 5:
			multl_kji(a64, b64, c64, size);
			break;
		default:
			perror("Put correct command");
			exit(15);
	}
}

int main(int argc, char const *argv[]) {
    if (argc!=5) {
        fputs("usage: ./mul_matrix <pathname_a> <pathname_b> <pathname_c> <mode>\n", stderr);
        return 6;
    }

	int afd=check_open(argv[1], O_RDONLY, 0600);
	int bfd=check_open(argv[2], O_RDONLY, 0600);
	int cfd=check_open(argv[3], O_WRONLY | O_CREAT, 0600);
    int32_t size=0;
    int32_t mode=0;
    mode=atoi(argv[4]);
    char lit=check_key(afd, &lit);
 
	if(lit=='d') {
    	size=get_size(afd);
    	int32_t **ad=check_mallocd(size);
    	int32_t **bd=check_mallocd(size);
		int32_t **cd=check_mallocd(size);
		get_matrixd(afd, ad, size);
		close(afd);
		if(lseek(bfd, sizeof(char)+sizeof(int32_t), SEEK_SET)==-1) {
				perror("lseek");
				exit(23);
			}
		get_matrixd(bfd, bd, size);
		close(bfd);
		multd(ad, bd, cd, size, mode);
		writed(cfd, cd, &size, &lit);
		close(cfd);
		int32_t i;
		for(i=0;i<size;i++) {
			free(ad[i]);
			free(bd[i]);
			free(cd[i]);
		}
			free(ad);
			free(bd);
			free(cd);
	}
	else if(lit=='l') {
    	size=get_size(afd);
		int64_t **a_64=check_mallocl(size);
		int64_t **b_64=check_mallocl(size);
		int64_t **c_64=check_mallocl(size);
		get_matrixl(afd, a_64, size);
		close(afd);
		if(lseek(bfd, sizeof(char)+sizeof(int32_t), SEEK_SET)==-1) {
				perror("lseek");
				exit(23);
			}
		get_matrixl(bfd, b_64, size);
		close(bfd);
		multl(a_64, b_64, c_64, size, mode);
		writel(cfd, c_64, &size, &lit);
		close(cfd);
		int32_t i;
		for(i=0;i<size;i++) {
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