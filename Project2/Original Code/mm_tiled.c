
/*
 * tiled.c
 *
 * Routines to realize the tiled matrix multiplication.
 *
 * The small matrix computations (i.e., for n <= block) can be
 * optimized considerably from those given here; in particular, this
 * is important to do before the value of block is chosen optimally. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mm_tiled.h"


matrix newmatrix(int);		/* allocate storage */
void freematrix (matrix, int); /*free storage */
void randomfill(int, matrix);	/* fill with random values in the range [0,1) */
void print (matrix, int, FILE *);
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
	check(n%block == 0, "main: Matrix size must be a multiple of block size");

    	a = newmatrix(n);
    	b = newmatrix(n);
    	c = newmatrix(n);
    	randomfill(n, a);
   	randomfill(n, b);

	gettimeofday(&ts,NULL);
	TiledMult(n, a, b, c);	// tiled algorithm 
	gettimeofday(&tf,NULL);
	tt=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;

	printf("Tiled Size %d Block %d Time %lf\n",n,block,tt);

	char *filename=malloc(30*sizeof(char));
	sprintf(filename,"res_mm_tiled_%d",n);
	FILE * f=fopen(filename,"w");
	print(c,n,f);
	fclose(f);

	freematrix(a,n);
	freematrix(b,n);
	freematrix(c,n);	
    	return 0;
}

/* c = a*b */
void TiledMult(int n, matrix a, matrix b, matrix c)
{
	int i, j, k;


	if (n <= block) 
    		SerialMult(block, a, b, c);
	else {
		for (i=0;i<n/block;i++)
			for (j=0;j<n/block;j++)
				for (k=0;k<n/block;k++) 
					SerialMult(block,a->p[i][k],b->p[k][j],c->p[i][j]);
	}
}

void SerialMult(int n, matrix a, matrix b, matrix c) {
	double **p, ** q, ** r;
	int i, j, k;
	p = a->d, q = b->d, r = c->d;
	for (i = 0; i < n; i++) 
	    	for (j = 0; j < n; j++) 
			for (k = 0; k < n; k++)
			    	r[i][j] += p[i][k] * q[k][j];
}


/*Tiled randomfill---Possibly useful on NUMA archs*/
/* Fill the matrix a with random values between 0 and 1 */
/*
void randomfill(int n, matrix a) {
	int i, j;
	if (n <= block) {

		double **q =a->d;
		double T = -(double)(1 << 31);
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				a->d[i][j] = random() / T; 
	} 
	else {
		for (i=0;i<n/block;i++)
			for (j=0;j<n/block;j++)
				randomfill(block,a->p[i][j]);		
	}
}

*/

/* Fill the matrix a with random values between 0 and 1 */
void randomfill(int n, matrix a) {
	int i, j;
	double T = -(double)(1 << 31);

    	if (n <= block) {
		double **p=a->d;
		for (i = 0; i < n; i++)
	    		for (j = 0; j < n; j++)
				p[i][j] = random() / T;
    	} 
	else {
		for (i=0;i<n;i++)
			for (j=0;j<n;j++) 
				a->p[i/block][j/block]->d[i%block][j%block]=random() / T;
    	}
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
		int i,j;
		a->p = (matrix **)calloc(n/block, sizeof(matrix*));
		check(a->p != NULL,
			"newmatrix: out of space for submatrices");
		for (i=0;i<n/block;i++) {
			a->p[i] = (matrix*)calloc(n/block,sizeof(matrix));
			check(a->p[i] != NULL,
				"newmatrix: out of space for submatrices");
		}
		
		for (i=0;i<n/block;i++)
			for (j=0;j<n/block;j++) 
				a->p[i][j]=newmatrix(block);
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
		int i,j;
		for (i=0;i<n/block;i++)
			for (j=0;j<n/block;j++)
				freematrix(m->p[i][j],block);
	}
}


void print (matrix a, int n, FILE * f) {
	int i,j;
	if (n<=block) {
		double **p=a->d;
		for (i=0;i<n;i++) {
			for (j=0;j<n;j++)
				fprintf(f,"%lf ",p[i][j]);
			fprintf(f,"\n");
		}
	}
	else {
		for (i=0;i<n;i++) {
			for (j=0;j<n;j++) 
				fprintf(f,"%lf ", a->p[i/block][j/block]->d[i%block][j%block]);
			fprintf(f,"\n");
		}
	}
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







