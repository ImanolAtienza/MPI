/*************************************************************
	PasoMensajes_1.c
************************************************************/

#include <mpi.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
     MPI_Init(&argc, &argv);

     PasoMensajes_1(MPI_COMM_WORLD);

     MPI_Finalize();
     return (0);
}

