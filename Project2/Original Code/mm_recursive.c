
/*
 * mm_recursive.c
 *
 * Routines to realize the recursive matrix multiplication.
 * RecMult multiplies n by n matrices a and b, putting the result in c, and
 * using the matrix d as scratch space.  The recursive algorithm is: 
 *
 *      d11 = a11 * b11
 *      c11 = a12 * b21
 *      d12 = a11 * b12
 *      c12 = a12 * b22
 *      d21 = a21 * b11
 *      c21 = a22 * b21
 *      d22 = a21 * b12
 *      c22 = a22 * b22
 *      c11 = c11 + d11
 *      c12 = c12 + d12
 *      c21 = c21 + d21
 *      c22 = c22 + d22
 *
 * where the double indices refer to submatrices in an obvious way.
 * Each line of RecMult() that recursively calls itself computes one
 * of the submatrices.  Four scratch half-size matrices are required by the
 * sequence of computations here.
 *
 * The small matrix computations (i.e., for n <= block) can be
 * optimized considerably from those given here; in particular, this
 * is important to do before the value of block is chosen optimally. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mm_recursive.h"

matrix newmatrix(int);		/* allocate storage */
void freematrix (matrix, int); /*free storage */
void randomfill(int, matrix);	/* fill with random values in the the range [0,1) */
void auxrandomfill(int, matrix, int, int);
void print (int, matrix, FILE *); /*print matrix in file*/
void auxprint(int, matrix, FILE *, int, int );
void check(int, char *);	/* check for error conditions */

int block;

int main(int argc, char **argv) {

	struct timeval ts,tf;
	double tt;
    	int n;
    	matrix a, b, c;

    	check(argc >= 3, "main: Need matrix size and block size on command line");
    	n = atoi(argv[1]);
	block=atoi(argv[2]);

    	a = newmatrix(n);
    	b = newmatrix(n);
    	c = newmatrix(n);
    	randomfill(n, a);
    	randomfill(n, b);

	gettimeofday(&ts,NULL);
	RecMult(n, a, b, c);	/* strassen algorithm */
	gettimeofday(&tf,NULL);
	tt=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;

	printf("Recursive Size %d Block %d Time %lf\n",n,block,tt);

	char *filename=malloc(30*sizeof(char));
	sprintf(filename,"res_mm_recursive_%d",n);
	FILE * f=fopen(filename,"w");
	print(n,c,f);
	fclose(f);

	freematrix(a,n);
	freematrix(b,n);
	freematrix(c,n);

	return 0;
}

/* c = a*b */
void RecMult(int n, matrix a, matrix b, matrix c)
{

	matrix d;
	
	if (n <= block) {
		double sum, **p = a->d, **q = b->d, **r = c->d;
		int i, j, k;

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				for (sum = 0., k = 0; k < n; k++)
				  	sum += p[i][k] * q[k][j];
				r[i][j] = sum;
	    		}
		}
    	} 
	else {
		d=newmatrix(n);
		n /= 2;
		RecMult(n, a11, b11, d11);
		RecMult(n, a12, b21, c11);
		RecAdd(n, d11, c11, c11);
		RecMult(n, a11, b12, d12);
		RecMult(n, a12, b22, c12);
		RecAdd(n, d12, c12, c12);
		RecMult(n, a21, b11, d21);
		RecMult(n, a22, b21, c21);
		RecAdd(n, d21, c21, c21);
		RecMult(n, a21, b12, d22);
		RecMult(n, a22, b22, c22);
		RecAdd(n, d22, c22, c22);
		freematrix(d,n*2);
	}
}

/* c = a+b */
void RecAdd(int n, matrix a, matrix b, matrix c) {
	if (n <= block) {
		double **p = a->d, **q = b->d, **r = c->d;
		int i, j;
		for (i = 0; i < n; i++) 
			for (j = 0; j < n; j++) 
				r[i][j] = p[i][j] + q[i][j];
    	} 
	else {
		n /= 2;
		RecAdd(n, a11, b11, c11);
		RecAdd(n, a12, b12, c12);
		RecAdd(n, a21, b21, c21);
		RecAdd(n, a22, b22, c22);
    	}
}

/* Recursive randomfill -- possibly useful for NUMA archs*/
/* Fill the matrix a with random values between 0 and 1 */
/*
void randomfill(int n, matrix a) {
    if (n <= block) {
		int i, j;
		double **p = a->d, T = -(double)(1 << 31);

		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				p[i][j] = random() / T;
	} 
	else {
		n /= 2;
		randomfill(n, a11);
		randomfill(n, a12);
		randomfill(n, a21);
		randomfill(n, a22);
	}
}
*/


/* fill n by n matrix with random numbers */
void randomfill(int n, matrix a) {
	int i,j;
	double T = -(double)(1 << 31);

    	if (n <= block) {
		for (i = 0; i < n; i++)
	    		for (j = 0; j < n; j++)
				a->d[i][j] = rand() / T;
    	} 		
	else {
		for (i=0;i<n;i++)
			for (j=0;j<n;j++) 
				auxrandomfill(n,a,i,j);
    	}
}

void auxrandomfill(int n, matrix a, int i, int j) {

	double T = -(double)(1 << 31);
	if (n<=block) 
		a->d[i][j]=rand()/T;
	else 
		auxrandomfill(n/2,a->p[(i>=n/2)*2+(j>=n/2)],i%(n/2),j%(n/2));
}

/* return new square n by n matrix */
matrix newmatrix(int n) {

    	matrix a;
    	a = (matrix)malloc(sizeof(*a));
    	check(a != NULL, "newmatrix: out of space for matrix");
    	if (n <= block) {
		int i;
		a->d = (double **)calloc(n, sizeof(double *));
		check(a->d != NULL,
			"newmatrix: out of space for row pointers");
		for (i = 0; i < n; i++) {
			a->d[i] = (double *)calloc(n, sizeof(double));
			check(a != NULL, "newmatrix: out of space for rows");
		}
    	} 
	else {
		n /= 2;
		a->p = (matrix *)calloc(4, sizeof(matrix));
		check(a->p != NULL,"newmatrix: out of space for submatrices");
		a11 = newmatrix(n);
		a12 = newmatrix(n);
		a21 = newmatrix(n);
		a22 = newmatrix(n);
    	}
	return a;
}

/* free square n by n matrix m */
void freematrix (matrix m, int n) {
	if (n<=block) {
		int i;
		for (i=0;i<n;i++)
			free(m->d[i]);
		free(m->d);
	}
	else {
		n/=2;	
		freematrix(m->p[0],n);
		freematrix(m->p[1],n);
		freematrix(m->p[2],n);
		freematrix(m->p[3],n);
	}
}

/* print n by n matrix into file f*/
void print(int n, matrix a, FILE * f) {
	int i,j;
	
    	if (n <= block) {
		for (i = 0; i < n; i++) {
	    		for (j = 0; j < n; j++)
				fprintf(f,"%lf ",a->d[i][j]);
			fprintf(f,"\n");	
		}
    	} 		
	else {
		for (i=0;i<n;i++) {
			for (j=0;j<n;j++) 
				auxprint(n,a,f,i,j);
			fprintf(f,"\n");
		}
    	}
}

void auxprint(int n, matrix a, FILE * f, int i, int j) {

	if (n<=block) 
		fprintf(f,"%lf ",a->d[i][j]);
	else 
		auxprint(n/2,a->p[(i>=n/2)*2+(j>=n/2)],f,i%(n/2),j%(n/2));
}

/*
 * If the expression e is false print the error message s and quit. 
 */

void check(int e, char *s)
{
    if (!e) {
		fprintf(stderr, "Fatal error -> %s\n", s);
		exit(1);
    }
}






