/**********************************************************************
Modify the code-Add OpenMP directives to parallelize the LU kernel
***********************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include "utils.h"


int main(int argc, char * argv[])
{
    int X=atoi(argv[1]);
    int Y=X;
    double ** A=malloc2D(X,Y);
    init2D(A,X,Y);
    print2D(A,X,Y);
    int i,j,k;
    double l;
    struct timeval ts,tf;
    double total_time;

    for(i=0;i<X;i++)
    	for(j=0;j++;j<Y)
    		A[i,j] = 4;

	gettimeofday(&ts,NULL);
	for (k=0;k<X-1;k++)
		#pragma omp parallel for private(i,j,l) shared(A)
		for (i=k+1;i<X;i++) {
			l=A[i][k]/A[k][k];
			for (j=k;j<Y;j++)		
				A[i][j]-=l*A[k][j];
		}
	gettimeofday(&tf,NULL);
	total_time=(tf.tv_sec-ts.tv_sec)+(tf.tv_usec-ts.tv_usec)*0.000001;
	printf("LU-OpenMP\t%d\t%.3lf\n",X,total_time);
    char * filename="output_omp";
	print2DFile(A,X,Y,filename);
	return 0;
}
