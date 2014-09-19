
/*
 * strassen.c
 *
 * Courtesy [Buhler 1993].
 * Routines to realize the Strassen recursive matrix multiplication.
 * StrassenMult n by n matrices a and b, putting the result in c, and
 * using the matrix d as scratch space.  The Strassen algorithm is: 
 *
 *      q7 = (a12-a22)(b21+b22)
 *      q6 = (-a11+a21)(b11+b12)
 *      q5 = (a11+a12)b22
 *      q4 = a22(-b11+b21)
 *      q3 = a11(b12-b22)
 *      q2 = (a21+a22)b11
 *      q1 = (a11+a22)(b11+b22)
 *      c11 = q1+q4-q5+q7
 *      c12 = q3+q5
 *      c21 = q2+q4
 *      c22 = q1+q3-q2+q6
 *
 * where the double indices refer to RecSubmatrices in an obvious way.
 * Each line of StrassenMult() that recursively calls itself computes one
 * of the q's.  Four scratch half-size matrices are required by the
 * sequence of computations here; with some rearrangement this
 * storage requirement can be reduced to three half-size matrices. 
 *
 * The small matrix computations (i.e., for n <= block) can be
 * optimized considerably from those given here; in particular, this
 * is important to do before the value of block is chosen optimally. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mm_strassen.h"

matrix newmatrix(int);		/* allocate storage */
void freematrix (matrix m, int n); /*free storage */
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
	matrix a, b,c;
	check(argc >= 3, "main: Need matrix size and block size on command line");
	n = atoi(argv[1]);
	block=atoi(argv[2]);

	a = newmatrix(n);
	b = newmatrix(n);
	c = newmatrix(n);

	randomfill(n, a);
	randomfill(n, b);
	gettimeofday(&ts,NULL);
	StrassenMult(n, a, b, c);	/* strassen algorithm */
	gettimeofday(&tf,NULL);
	tt=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;
	printf("Strassen Size %d Block %d Time %lf\n",n,block,tt);
	char *filename=malloc(30*sizeof(char));
	sprintf(filename,"res_mm_strassen_%d",n);
	FILE * f=fopen(filename,"w");
	print(n,c,f);
	fclose(f);

	freematrix(a,n);
	freematrix(b,n);
	freematrix(c,n);	
    	return 0;
}

/*Recursive Strassen Multiplication*/
void StrassenMult(int n, matrix a, matrix b, matrix c) {
	
	matrix t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,q1,q2,q3,q4,q5,q6,q7;

	
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
		n /= 2;


		t1=newmatrix(n);
		t2=newmatrix(n);
		t3=newmatrix(n);
		t4=newmatrix(n);
		t5=newmatrix(n);
		t6=newmatrix(n);
		t7=newmatrix(n);
		t8=newmatrix(n);
		t9=newmatrix(n);
		t10=newmatrix(n);
		q1=newmatrix(n);
		q2=newmatrix(n);
		q3=newmatrix(n);
		q4=newmatrix(n);
		q5=newmatrix(n);
		q6=newmatrix(n);
		q7=newmatrix(n);

		RecAdd(n,a11,a22,t1);
		RecAdd(n,b11,b22,t2);		
		RecAdd(n,a21,a22,t3);
		RecSub(n,b12,b22,t4);		
		RecSub(n,b21,b11,t5);		
		RecAdd(n,a11,a12,t6);		
		RecSub(n,a21,a11,t7);		
		RecAdd(n,b11,b12,t8);		
		RecSub(n,a12,a22,t9);		
		RecAdd(n,b21,b22,t10);
				
		StrassenMult(n,t1,t2,q1);		
		StrassenMult(n,t3,b11,q2);		
		StrassenMult(n,a11,t4,q3);		
		StrassenMult(n,a22,t5,q4);		
		StrassenMult(n,t6,b22,q5);		
		StrassenMult(n,t7,t8,q6);		
		StrassenMult(n,t9,t10,q7);
		
		RecAdd(n,q1,q4,c11);
		RecSub(n,c11,q5,c11);
		RecAdd(n,q7,c11,c11);
			
		RecAdd(n,q3,q5,c12);
		
		RecAdd(n,q2,q4,c21);
		
		RecAdd(n,q1,q3,c22);
		RecAdd(n,q6,c22,c22);
		RecSub(n,c22,q2,c22);
		
		freematrix(t1,n);
		freematrix(t2,n);
		freematrix(t3,n);
		freematrix(t4,n);
		freematrix(t5,n);
		freematrix(t6,n);
		freematrix(t7,n);
		freematrix(t8,n);
		freematrix(t9,n);
		freematrix(t10,n);
		freematrix(q1,n);
		freematrix(q2,n);
		freematrix(q3,n);
		freematrix(q4,n);
		freematrix(q5,n);
		freematrix(q6,n);
		freematrix(q7,n);

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
		RecAdd(n, a21, b21,c21);		
		RecAdd(n, a22, b22, c22);
    	}
}

/* c = a-b */
void RecSub(int n, matrix a, matrix b, matrix c) {
    	if (n <= block) {
		double **p = a->d, **q = b->d, **r = c->d;
		int i, j;

		for (i = 0; i < n; i++) 
		    	for (j = 0; j < n; j++) 
				r[i][j] = p[i][j] - q[i][j];
	    
    	} 	
	else {
		n /= 2;		
		RecSub(n, a11, b11, c11);		
		RecSub(n, a12, b12, c12);			
   		RecSub(n, a21, b21, c21);    	
		RecSub(n, a22, b22, c22);		
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
		check(a->p != NULL,
			"newmatrix: out of space for RecSubmatrices");
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




