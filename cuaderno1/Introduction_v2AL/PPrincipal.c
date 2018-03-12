/*************************************************************
	Introduccion_v2
************************************************************/

#include <mpi.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
     MPI_Init(&argc, &argv);

     PasoMensajes_2(MPI_COMM_WORLD);

     MPI_Finalize();
     return (0);
}

