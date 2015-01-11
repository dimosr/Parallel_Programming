/**************************************************
# Copyright (C) 2014 Raptis Dimos <raptis.dimos@yahoo.gr>
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
# **************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk_api.h>

/*
 * A matrix is defined to be a pointer to a ``union _matrix'', which
 * is (if the size is <= BREAK) a matrix of numbers, or else is an
 * array of four submatrices of half size. 
 */

typedef union _matrix {
    double **d;
    union _matrix **p;
} *matrix;

matrix newmatrix(int);      /* allocate storage */
void randomfill(int, matrix);   /* fill with srandom values in the the range [0,1) */
void freematrix(matrix, int);
void print (matrix, int, FILE *);
void SerialMult(int, matrix, matrix, matrix);   /* Serial Multiplication Algorithm */
double calculate_cell(int n, double **p, double **q, int i, int j); /* Calculate cell (i, j) value */
void check(int, char *);    /* check for error conditions */


int main(int argc, char **argv) {
    struct timeval ts,tf;
    double tt;
        int n;
        matrix a, b, c;

        check(argc >= 2, "main: Need matrix size on command line");
        n = atoi(argv[1]);

        a = newmatrix(n);
        b = newmatrix(n);
        c = newmatrix(n);
        randomfill(n, a);
        randomfill(n, b);

    gettimeofday(&ts,NULL);
    SerialMult(n, a, b, c); /* Serial Multiplication */
    gettimeofday(&tf,NULL);
    tt=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;

    printf("Serial Size %d Time %lf Workers %d\n", n, tt, __cilkrts_get_nworkers());
    char * filename=malloc(30*sizeof(char));
    sprintf(filename,"res_mm_serial_for_%d", n);
    FILE * f=fopen(filename,"w");
    print(c,n,f);
    fclose(f);

    freematrix(a,n);
    freematrix(b,n);
    freematrix(c,n);    
    return 0;
}

/*c=a*b*/
void SerialMult(int n, matrix a, matrix b, matrix c) {
    double **p = a->d, **q = b->d, **r = c->d;
    int i, j;
    for (i = 0; i < n; ++i) {
        cilk_for (j = 0; j < n; ++j) {
            r[i][j] = calculate_cell(n, p, q, i, j);
        }
    }
}

/* Calculate a cell value */
double calculate_cell(int n, double **p, double **q, int i, int j) {
    double sum;
    int k;
    for (sum = 0., k = 0; k < n; k++)
        sum += p[i][k] * q[k][j];
    return sum;
}
    
/* Fill the matrix a with srandom values between 0 and 1 */
void randomfill(int n, matrix a) {
    int i, j;
    double **p = a->d;
    double T = -(double)(1 << 31);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            p[i][j] = rand() / T;
}


/* return new square n by n matrix */
matrix newmatrix(int n) {
    
    matrix a;
    a = (matrix)malloc(sizeof(*a));
    check(a != NULL, "newmatrix: out of space for matrix");

    int i;

    a->d = (double **)calloc(n, sizeof(double *));
    check(a->d != NULL,"newmatrix: out of space for row pointers");
    for (i = 0; i < n; i++) {
        a->d[i] = (double *)calloc(n, sizeof(double));
        check(a != NULL, "newmatrix: out of space for rows");
    }
 
    return a;
}

void freematrix(matrix a, int n) {
    int i;
    for (i=0;i<n;i++)
        free(a->d[i]);
    free(a);
}

void print (matrix a, int n, FILE * f) {
    int i,j;
    double **p=a->d;
    for (i=0;i<n;i++) {
        for (j=0;j<n;j++)
            fprintf(f,"%lf ",p[i][j]);
        fprintf(f,"\n");
    }
}


void check(int e, char *s) {
    if (!e) {
        fprintf(stderr, "Fatal error -> %s\n", s);
        exit(1);
    }
}

