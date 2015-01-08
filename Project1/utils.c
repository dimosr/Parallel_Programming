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

double ** malloc2D(int X, int Y) {
 	int i,j;
	double **a;
	
	a=malloc(X*sizeof(double*));
    if (a==NULL) {
        fprintf(stderr,"Malloc failed!\n");
        exit(-1);
    }
	a[0]=calloc(X*Y,sizeof(double));
    if (a[0]==NULL) {
        fprintf(stderr,"Malloc failed!\n");
        exit(-1);
    }

	for (i=1;i<X;i++)
		a[i]=a[i-1]+Y;	
	return a;
}

void free2D(double ** a, int X, int Y) {
    int i;
    if (X>1) {
        for (i=1;i<X;i++)
            a[i]=NULL;
    }
    free(a[0]);
    free(a);
}

void init2D(double ** a, int X, int Y) {
    int i,j;
    for (i=0;i<X;i++)
        for (j=0;j<Y;j++)
            a[i][j]=(rand()%100000)/10000.0;
}

void print2D(double ** a, int X, int Y) {
    int i,j;
    for (i=0;i<X;i++) {
        for (j=0;j<Y;j++)
            printf("%lf ",a[i][j]);
        printf("\n");
    }
}

void print2DFile(double **a, int X, int Y, char * filename) {
    int i,j;
    FILE * f=fopen(filename,"a");
    for (i=0;i<X;i++) {
        for (j=0;j<Y;j++) 
            fprintf(f,"%lf ",a[i][j]);
        fprintf(f,"\n");
    }
    fclose(f);
}   
