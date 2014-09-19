/*
 * strassen.h
 *
 * Copurtesy [Buhler 1993].
 * Header file for strassen matrix multiplication functions.
 */

/*
 * Square matrices of size <= block are handled with the ``classical
 * algorithms.  The shape of almost all functions is therefore
 * something like
 *
 *	if ( n <= block )
 *	    classical algorithms
 *	else
 *	    n/= 2
 *	    recursive call for 4 half-size submatrices
 */

typedef union _matrix {
    double **d;
    union _matrix **p;
} *matrix;


void StrassenMult(int,matrix,matrix,matrix);
void RecAdd(int, matrix, matrix, matrix);
void RecSub(int, matrix, matrix, matrix);

/*
 * Notational shorthand to access submatrices for matrices named
 * a,b,c,d 
 */

#define a11 a->p[0]
#define a12 a->p[1]
#define a21 a->p[2]
#define a22 a->p[3]
#define b11 b->p[0]
#define b12 b->p[1]
#define b21 b->p[2]
#define b22 b->p[3]
#define c11 c->p[0]
#define c12 c->p[1]
#define c21 c->p[2]
#define c22 c->p[3]
#define d11 d->p[0]
#define d12 d->p[1]
#define d21 d->p[2]
#define d22 d->p[3]
#define e11 e->p[0]
#define e12 e->p[1]
#define e21 e->p[2]
#define e22 e->p[3]
