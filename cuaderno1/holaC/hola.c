/*************************************************************
	hola.c
        programa MPI: activacion de procesos
************************************************************/

#include <mpi.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
  int   lnom;
  char  nombrepr[MPI_MAX_PROCESSOR_NAME];

  int   pid, npr;      	// identificador y numero de procesos
  int   A = 21;

  MPI_Init (&argc, &argv);

  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  MPI_Get_processor_name (nombrepr, &lnom);

  A = A + 1;

  printf (" >> Proceso %2d de %2d activado en %s, A = %d\n", pid, npr, nombrepr, A);

  MPI_Finalize ();
  return (0);
}
