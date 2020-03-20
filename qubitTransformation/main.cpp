#include <complex>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <cmath>
#include <omp.h>
#include <time.h>
typedef std::complex<double> complexd;
using namespace std;

enum errors
{
	success=0,
	argNumError=1
};
void qEvolution(complexd *in, complexd *out, complexd U[2][2], size_t n, size_t k);
complexd comlRand();
class Vector
{
	friend ostream& operator<<(ostream& os, const Vector& vec);
/*	friend void operator=(const T& other);*/

	private: 
	size_t size;
	size_t k;
	size_t n;
	size_t pow_k;  
/* we store this for not to calculate power in each call in case if it would be necessary to append program */
	complexd U[2][2];
	complexd *a;  
	public:

	Vector(const Vector& other): size(other.size), k(other.k), 
					n(other.n), pow_k(other.pow_k), U(other.U)
	{
		a = new complexd[size];

	for(size_t i=0; i<size; i++)
	{
		a[i] = other.a[i];
	}
	}
	Vector(complexd U1[2][2], size_t k1, size_t n1): size(1<<n1), k(k1), n(n1), pow_k(1<<(n1-k1))
	{
		U[0][0]=U1[0][0];
		U[0][1]=U1[0][1];
		U[1][0]=U1[1][0];
		U[1][1]=U1[1][1];
		a = new complexd[size];

	for(size_t i=0; i<size; i++)
	{
		a[i] = comlRand();
	}
	}
	void vecTransformation(const Vector& other)
	{
		qEvolution(other.a, a, U, n, k);
	}

	~Vector()
	{
		delete[] a;
	}
};

complexd comlRand()
{
	/*let maxRe and maxIm be 50*/
	return complexd(rand() % 50, rand() % 50);
}
void qEvolution(complexd *in, complexd *out, complexd U[2][2], size_t n, size_t k)
{
	const size_t shift = n-k;
	//Все биты нулевые, кроме соответствующего позиции преобразуемого кубита  
	const size_t pow2q=1<<(n-k);
	const size_t size=1<<n;
	#pragma omp parallel for
	for	(size_t i=0; i<size; i++)
	{
		//Обнуления меняющегося бита
		int i0 = i & ~pow2q;
		
		//Установка меняющегося бита
		int i1 = i | pow2q;
		
		//Получение значения меняющегося бита
		int iq = (i & pow2q) >> shift;

		out[i] = U[iq][0] * in[i0] + U[iq][1] *in[i1];
	}
}
ostream& operator<<(ostream& os, const Vector& vec)
{
	for(size_t i=1; i<vec.size; i++)
	{
    os <<vec.a[i-1]<<", " << vec.a[i]<<endl;
	}

    return os;
}


int main (int argc, char** argv)
{
	size_t k;
	size_t n;
	if (argc<4 || (k=atoi(argv[1]))>(n = atoi(argv[2])))
	{
		cout<<"Usage: ./<name> <k> <n> <num theads>"<<endl;
		exit(argNumError);
	}
	omp_set_dynamic(0);
	const size_t P = atoi(argv[3]);
	omp_set_num_threads(P);
	srand (time(NULL)); 

	double time_spent=0;
	long            ns; // Nanoseconds
    time_t          s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s  = spec.tv_sec;
    ns = spec.tv_nsec; 
    if (ns > 999999999) {
        s++;
        ns = 0;
    }
	const double h = 1/sqrt(2);
	complexd U[2][2] = { h, h,
				 		 h,-h };

	Vector qa(U, k, n), qb(qa);
	qb.vecTransformation(qa);
	clock_gettime(CLOCK_REALTIME, &spec);
	
   	s = spec.tv_sec-s;
    ns = spec.tv_nsec-ns; 
    if (ns<0)
    {
    	s--;
    	ns += 1000000000;
    }
    time_spent= s + ns/1000000000.;
	cout.setf( ios::scientific| ios::showpos);
	cout.precision(1);
	cout <<endl<<"k = "<<k<<endl;
	cout <<"H ="<<endl <<U[0][0]<<" "<<U[0][1]<<
		endl<<U[1][0]<<" "<<U[1][1]<<endl;
	//cout <<"a ="<<endl<<qa<<endl;
	//cout <<"b ="<<endl<< qb<<endl;
	cout<<"time: "<<time_spent<<"sec."<<endl;
	return success;
}
