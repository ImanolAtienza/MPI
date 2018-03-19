#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(x,y) ((x)<(y)?(x):(y))

void Introduccion_v3BC_RE (MPI_Comm comm, int n, int vec[n], int root)
{
    int rank, nprocs, ierr;
    MPI_Status status;
    int stride, start, fin, sum,  total, resto;
    int i, tmp;

	// Bcast(datos, cuantos_datos, tipo_datos, quien_manda, comunicador)
	MPI_Bcast(vec, n, MPI_INT, root, comm);
	
	// Reparto
	  MPI_Comm_size (comm, &nprocs);
	  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	  
	  stride = n / nprocs;
	  resto = n % nprocs;
      start= (rank < resto) ? rank * (stride+1) : rank * stride + resto;
      fin = (rank < resto) ? start + (stride+1) - 1 : start + stride-1;	
	
	// Calculo
    sum = 0;
    for (i = start;i <= fin; i++)
     sum = sum + vec[i];
 
	// Reduccion
    //if (rank == 0)  // root, recibir de cada clon su resultado de su trozo del vector
    //{
     //total = sum;
     /*for (i = 1; i <= nprocs-1; i++)
     {
      MPI_Recv(&tmp, 1, MPI_INT,
                    i, 2, comm,
                    &status);
      total = total + tmp;
     }
    }
    else  // resto procesos
      MPI_Send (&sum, 1, MPI_INT,
                     0, 2, comm);*/

	// Reduce(resultado_local_cadaclon, donde_guarda_res, cuantos_hay_que_enviar, tipo_envio, tipo_operacion, a_quien, comunicador)
	MPI_Reduce(&sum, &total, 1, MPI_INT, MPI_SUM, root, comm);
					 
     //print '(4(A,I4))', 'rank ',rank, ' desde ',start, ' hasta ',fin, ' sum ', sum
     printf ("rank %d desde %d hasta %d sum %d\n",rank,start,fin,sum); fflush(stdout);
     if (rank == 0)
       // print *, ' total ',total
     {
        printf (" total %d\n",total); fflush(stdout);
     }

}





