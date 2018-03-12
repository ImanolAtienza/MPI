#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(x,y) ((x)<(y)?(x):(y))

void Ejemplo1 (MPI_Comm comm, int n, int vec[n])
{
    int rank, nprocs, ierr;
    MPI_Status status;
    int stride, start, fin, sum,  total, resto;
    int i, tmp;

    MPI_Comm_rank (comm, &rank);
    MPI_Comm_size (comm, &nprocs);

    // reparto de trabajo
    //stride = ceil ((float)n/(float)nprocs);
    //start = (stride * rank);
    //fin = min(n, (stride * (rank+1)));

	  stride = n / nprocs;
	  resto = n % nprocs;
      start= (rank < resto) ? rank * (stride+1) : rank * stride + resto;
      fin = (rank < resto) ? start + (stride+1) - 1 : start + stride-1;

    // computo, cada clon sumara los elementos del vector desde start hasta fin
    sum = 0;
    for (i = start;i <= fin; i++)
     sum = sum + vec[i];


    if (rank == 0)  // root, recibir de cada clon su resultado de su trozo del vector
    {
     total = sum;
     for (i = 1; i <= nprocs-1; i++)
     {
      MPI_Recv(&tmp, 1, MPI_INT,
                    i, 2, comm,
                    &status);
      total = total + tmp;
     }
    }

    else  // resto procesos
      MPI_Send (&sum, 1, MPI_INT,
                     0, 2, comm);

     //print '(4(A,I4))', 'rank ',rank, ' desde ',start, ' hasta ',fin, ' sum ', sum
     printf ("rank %d desde %d hasta %d sum %d\n",rank,start,fin,sum); fflush(stdout);
     if (rank == 0)
       // print *, ' total ',total
     {
        printf (" total %d\n",total); fflush(stdout);
     }

}





