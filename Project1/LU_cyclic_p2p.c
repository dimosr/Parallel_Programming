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
    FILE *fp;
    char * filename="output_cyclic_p2p";
    MPI_Status status;
    int X,Y,x,y,X_ext,i,j,k,thread,initial,t,count,help;    
    double **A, **localA,*temp_line,l;
    X=atoi(argv[1]);
    Y=X;

    temp_line =malloc(Y*sizeof(double));


    //Extend dimension X with ghost cells if X%size!=0
    if (X%size!=0)
        X_ext=X+size-X%size;
    else
        X_ext=X;
      

    if (rank==0) {
        //Allocate and init matrix A
        A=malloc2D(X_ext,Y);
        init2D(A,X,Y);
        fp = fopen("output_cyclic_p2p","w");
        fprintf(fp,"\n****Initial Array***\n");
        fclose(fp);
        print2DFile(A,X,Y,filename);

    }

    //Local dimensions x,y
    x=X_ext/size;
    y=Y;

    //Allocate local matrix and scatter global matrix
    localA=malloc2D(x,y);
    double * idx;
    for (i=0;i<x;i++) {
        if (rank==0)
            idx=&A[i*size][0];            
        MPI_Scatter(idx,Y,MPI_DOUBLE,&localA[i][0],y,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    if (rank==0)
        free2D(A,X_ext,Y);
 
    //Timers   
    struct timeval ts,tf,time1,time2;
    double total_time=0,computation_time=0,communication_time=0;
        
	MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&ts,NULL);        

    /******************************************************************************
     The matrix A is distributed in a round-robin fashion to the local matrices localA
     You have to use point-to-point communication routines.
     Don't forget the timers for computation and communication!
        
    ******************************************************************************/
        for (k=0;k<X-1;k++)
        {
        	if ( rank == (k % size)  ){             //an einai o pirinas pou prepei na steilei
                	for(thread=0;thread < size;thread++){           //sto variable thread o pirinas ston opoio tha steilei
                        	if(thread != (k % size) ){
					gettimeofday(&time1,NULL);              //an den eimai egw o idios (pirinas),steile
                                	MPI_Send(&(localA[k / size][0]),Y,MPI_DOUBLE,thread,55,MPI_COMM_WORLD);
					gettimeofday(&time2,NULL);
                                }
                        }
                }
                else {          //edw mpainoun oloi oi ipoloipoi gia na lavoun
                                gettimeofday(&time1,NULL);

                                MPI_Recv(&(temp_line[0]),Y,MPI_DOUBLE,(k % size),55,MPI_COMM_WORLD,&status);
				gettimeofday(&time2,NULL);
                }
		communication_time+=time2.tv_sec-time1.tv_sec+(time2.tv_usec-time1.tv_usec)*0.000001;
                if(k < ((X-size)+rank) ){       //edw mpainoune osa threads einai "akoma sto paixnidi"|| (X-size) -->1 block prin to telos

                        if(k < rank ){             //vrisketai prin tin 1i grammi tou
                                        initial = 0;
                        }
                        else{                                   //exei perasei tin 1i grammi tou

                                if(rank <= (k % size) ){
                                        help = 1;
                                }
                                else{
                                        help = 0;
                                }
                                initial = ((k / size ) + help);
                        }
                        for(i= initial ;i<x ;i++){

                                if( rank != (k % size) ){               //an auto to thread exei lavei, xrisimopoiei to temp_line
                                        l = localA[i][k] / temp_line[k];

                                        for(j=k;j<X;j++){
                                        	localA[i][j] = localA[i][j]-l*temp_line[j];
                                        }
                                }
                                else{           //alliws xrisimopoiei tin diki tou grammi
                                        l = localA[i][k] / localA[k / size][k];
                                        for(j=k;j<X;j++){
                                        	localA[i][j] = localA[i][j]-l*localA[k / size][j];

                                        }
                                }
                        }
                }
        }


    gettimeofday(&tf,NULL);
    total_time=tf.tv_sec-ts.tv_sec+(tf.tv_usec-ts.tv_usec)*0.000001;
    computation_time=total_time-communication_time;

    //Gather local matrices back to the global matrix
    if (rank==0) 
        A=malloc2D(X_ext,Y);
    
    for(i=0;i<x;i++) {
            if (rank==0)
                idx=&A[i*size][0];
            MPI_Gather(&localA[i][0],y,MPI_DOUBLE,idx,Y,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    
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
        printf("LU-Cyclic-p2p\tSize\t%d\tProcesses\t%d\n",X,size);
        printf("Max times:\tTotal\t%lf\tComp\t%lf\tComm\t%lf\n",max_total,max_comp,max_comm);
        printf("Avg times:\tTotal\t%lf\tComp\t%lf\tComm\t%lf\n",avg_total,avg_comp,avg_comm);
    }

    //Print triangular matrix U to file
    if (rank==0) {
        fp = fopen("output_cyclic_p2p","a");
        fprintf(fp,"\n****Final Array****\n");
        fclose(fp);
        print2DFile(A,X,Y,filename);
    }


    MPI_Finalize();

    return 0;
}
