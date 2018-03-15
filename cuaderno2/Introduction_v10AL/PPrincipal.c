/*************************************************************
	Introduccion_v10AL
************************************************************/

#include <mpi.h>
#include <stdio.h>
/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

extern void Ejemplo1_v10 (MPI_Comm comm, int n, int vec[n]);

int main (int argc, char *argv[])
{
    #define N  1000
     int vector[N];

     MPI_Init(&argc, &argv);

     // incializar vector a 2
      vector[0:N] = 2;
     Ejemplo1_v10(MPI_COMM_WORLD,N,vector);

     MPI_Finalize();
     return (0);
}

