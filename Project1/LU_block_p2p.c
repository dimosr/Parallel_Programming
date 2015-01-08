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
#include <mpi.h>
#include <sys/time.h>
#include "utils.h"


int main (int argc, char * argv[]) {
    int rank,size;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int X,Y,x,y,X_ext,i,j,k,thread,t,count,initial;
    double ** A, ** localA,*temp_line,l;
    X=atoi(argv[1]);
    Y=X;
    MPI_Status status;
    FILE *fp;
    char * filename="output_block_p2p";
   
    temp_line =(double *)malloc(Y*sizeof(double));
    //Extend dimension X with ghost cells if X%size!=0
    if (X%size!=0)
        X_ext=X+size-X%size;
    else
        X_ext=X;

    if (rank==0) {
        //Allocate and init matrix A
        A=malloc2D(X_ext,Y);
        init2D(A,X,Y);
        fp = fopen("output_block_p2p","w");
        fprintf(fp,"\n****Initial Array****\n");
        fclose(fp);
        print2DFile(A,X,Y,filename);

    }
      
    //Local dimensions x,y
    x=X_ext/size;
    y=Y;
    

    //Allocate local matrix and scatter global matrix
    localA=malloc2D(x,y);
    double * idx;
    if (rank==0) 
        idx=&A[0][0];
    MPI_Scatter(idx,x*y,MPI_DOUBLE,&localA[0][0],x*y,MPI_DOUBLE,0,MPI_COMM_WORLD);
 
   if (rank==0) {
        free2D(A,X_ext,Y);
    }

    //Timers   
    struct timeval ts,tf,time1,time2;
    double total_time=0,computation_time=0,communication_time=0;

	MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&ts,NULL);        

    for (k=0;k<X-1;k++){
        if ( rank == ( k / x)  ){              
            for(thread=0;thread < size;thread++){           
                if(thread != ( k / x) ){                
                    gettimeofday(&time1,NULL);
				   MPI_Send(&(localA[k % x][0]),Y,MPI_DOUBLE,thread,55,MPI_COMM_WORLD);   
                }
			    gettimeofday(&time2,NULL);
            }
        }
        else {        
            gettimeofday(&time1,NULL);
            MPI_Recv(&(temp_line[0]),Y,MPI_DOUBLE,(k / x),55,MPI_COMM_WORLD,&status);
            gettimeofday(&time2,NULL);
        }
        communication_time+=time2.tv_sec-time1.tv_sec+(time2.tv_usec-time1.tv_usec)*0.000001;
        if(k < (((rank+1)*x)-1) ){ 

            if(k <= (((rank)*x)-1) ){            
                initial = 0;
            }
            else{                                 
                initial = ((k % (x) ) + 1);
            }
            for(i= initial ;i<x ;i++){

                if( rank != ( k / x) ){               
                    l = localA[i][k] / temp_line[k];
                    for(j=k;j<X;j++){
                        localA[i][j] = localA[i][j]-l*temp_line[j];
                    }
                }
                else{
                    l = localA[i][k] / localA[k % x][k];
                    for(j=k;j<X;j++){
                        localA[i][j] = localA[i][j]-l*localA[k % x][j];
                    }
                }
            }
        }
    }

    gettimeofday(&tf,NULL);
    total_time=tf.tv_sec-ts.tv_sec+(tf.tv_usec-ts.tv_usec)*0.000001;
    computation_time=total_time-communication_time;

    //Gather local matrices back to the global matrix
    if (rank==0) {
        A=malloc2D(X_ext,Y);    
        idx=&A[0][0];
    }
    MPI_Gather(&localA[0][0],x*y,MPI_DOUBLE,idx,x*y,MPI_DOUBLE,0,MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);

    double avg_total,avg_comp,avg_comm,max_total,max_comp,max_comm;
    MPI_Reduce(&total_time,&max_total,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    MPI_Reduce(&computation_time,&max_comp,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    MPI_Reduce(&communication_time,&max_comm,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    MPI_Reduce(&total_time,&avg_total,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&computation_time,&avg_comp,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&communication_time,&avg_comm,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);

    avg_total/=size;
    avg_comp/=size;
    avg_comm/=size;

    if (rank==0) {
        printf("LU-Block-p2p\tSize\t%d\tProcesses\t%d\n",X,size);
        printf("Max time:\tTotal\t%lf\tComputation\t%lf\tCommunication\t%lf\n",max_total,max_comp,max_comm);
        printf("Avg time:\tTotal\t%lf\tComputation\t%lf\tCommunication\t%lf\n",avg_total,avg_comp,avg_comm);
    }

    //Print triangular matrix U to file
    if (rank==0) {
        fp = fopen("output_block_p2p","a");
        fprintf(fp,"\n****Final Array****\n");
        fclose(fp);
 
        print2DFile(A,X,Y,filename);
    }


    MPI_Finalize();

    return 0;
}
