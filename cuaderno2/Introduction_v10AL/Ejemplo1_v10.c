#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define min(x,y) ((x)<(y)?(x):(y))

void Ejemplo1_v10 (MPI_Comm comm, int n, int vec[n])
{
    int rank, nprocs;
    int stride, start, fin, sum,  total, resto;
    int i, flag;
    MPI_Request *req;
    int *tmp;
    MPI_Status *vstatus, status;
	MPI_Message mensaje;


    MPI_Comm_rank (comm, &rank);
    MPI_Comm_size (comm, &nprocs);

    // allocate (req(nprocs))
    req = malloc(sizeof(MPI_Request)*nprocs);
    // allocate (tmp(nprocs))
    tmp = malloc(sizeof(int)*nprocs);
    // allocate (status(MPI_STATUS_SIZE,nprocs))
    vstatus = malloc(sizeof(MPI_Status)*nprocs); // Ejemplo de acceso, vstatus[3].MPI_SOURCE

    /*reparto*/
    //stride = ceil ((float)n/(float)nprocs);
    //start = (stride * rank);
    //fin = min(n, (stride * (rank+1)));
     stride = n / nprocs;
     resto = n % nprocs;
     start = (rank < resto) ? rank * (stride+1) : rank * stride + resto;
      fin = (rank < resto) ? start + (stride+1) - 1 : start + stride - 1;

    /*trabajo*/
    sum = 0;
    for (i = start;i <= fin; i++)
     sum = sum + vec[i];


    if (rank == 0)  // root
    {
     total = sum;
	 i = 2;
     while (i <= nprocs-1)
     {
	  MPI_Improbe(i, 2, comm, &flag, &mensaje, &vstatus[1]); 
	  if(flag==1) {
		  MPI_Imrecv(&tmp[i], 1, MPI_INT, &mensaje, &req[i]);
		  //print '(4(A,I4))', 'rank ',rank, ' desde ',start, ' hasta ',fin, ' sum ', sum
			printf ("rank %d sum %d\n",i,tmp[i]);
		  i+=2;
	  }
     }
     i = 1;
     while (i <= nprocs-1)
     {
	  MPI_Improbe(i, 2, comm, &flag, &mensaje, &vstatus[1]); 
	  if(flag==1) {
		  MPI_Imrecv(&tmp[i], 1, MPI_INT, &mensaje, &req[i]);
		  //print '(4(A,I4))', 'rank ',rank, ' desde ',start, ' hasta ',fin, ' sum ', sum
			printf ("rank %d sum %d\n",i,tmp[i]);
		  i+=2;
	  }
     }
     // Espera a todos los que pertenezcan a un comunicador
     MPI_Waitall (nprocs-1,&req[1],&vstatus[1]);

    for (i = 1; i <= nprocs-1; i++)
      total = total + tmp[i];
    }
    else  // resto procesos
    {
      MPI_Isend (&sum, 1, MPI_INT,
                 0, 2, comm, &req[rank]);

      MPI_Wait(&req[rank],&vstatus[rank]);
    }


     
     if (rank == 0)
       // print *, ' total ',total
       printf (" total %d\n",total);


    free(req);
    free(vstatus);
    free(tmp);
}
