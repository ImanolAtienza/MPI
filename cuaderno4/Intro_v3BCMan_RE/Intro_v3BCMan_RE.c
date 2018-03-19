#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define min(x,y) ((x)<(y)?(x):(y))

int rank, nprocs;

void MI_MPI_Bcast(int n, int vec[n], MPI_Datatype tipo, int root, MPI_Comm comm){
	MPI_Request *req;
    MPI_Status *vstatus;
    int i;

	MPI_Comm_size (comm, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    req = malloc(sizeof(MPI_Request)*nprocs);
    vstatus = malloc(sizeof(MPI_Status)*nprocs); // Ejemplo de acceso, vstatus[3].MPI_SOURCE

    if (rank == 0)  // root
    {
     for (i = 0; i <= nprocs-1; i++)
     {
      MPI_Isend (vec, n, tipo,
                 i, 2, comm, &req[i]);
     }
     MPI_Irecv(vec, n, tipo,
               root, 2, comm,
               &req[rank]); 
	
     MPI_Waitall(nprocs, req, vstatus);	
    } else  // resto procesos
    {
    	//printf("Sale aqui %d\n", vec[root]);
      MPI_Irecv(vec, n, tipo,
               root, 2, comm,
               &req[rank]);

      MPI_Wait(&req[rank],&vstatus[rank]);
    }
    free(req);
    free(vstatus);
}

void Intro_v3BCMan_RE (MPI_Comm comm, int n, int vec[n], int root)
{
    int ierr;
    MPI_Status status;
    int stride, start, fin, sum,  total, resto;
    int i, tmp;

	// Bcast(datos, cuantos_datos, tipo_datos, quien_manda, comunicador)
	// MPI_Bcast(vec, n, MPI_INT, root, comm);
	MI_MPI_Bcast(n, vec, MPI_INT, root, comm);
	
	// Reparto
	  stride = n / nprocs;
	  resto = n % nprocs;
      start= (rank < resto) ? rank * (stride+1) : rank * stride + resto;
      fin = (rank < resto) ? start + (stride+1) - 1 : start + stride-1;	
	
	// Calculo
    sum = 0;
    for (i = start;i <= fin; i++)
     sum = sum + vec[i];
 
	// Reduccion
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





