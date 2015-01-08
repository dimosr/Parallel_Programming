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
#include <omp.h>
#include "utils.h"


int main(int argc, char * argv[])
{
    int X=atoi(argv[1]);
    int Y=X;
    double ** A=malloc2D(X,Y);
    init2D(A,X,Y);
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
	return 0;
}
