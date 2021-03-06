/****************************************************
	t_bcast.c
	medir tiempos de una bcast
****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NREP1 10	// repeticiones para quedarnos con el minimo
#define NREP2 1000	// repeticiones del bucle de barrier para hacer la media

int main (int argc, char **argv)
{
  int  npr, pid, i, j, k;
  int dato = 10;
  double  t0, t1, tmin;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  tmin = 100000;

   for (i=0; i<NREP1; i++)
   {
    MPI_Barrier (MPI_COMM_WORLD);
    t0 = MPI_Wtime ();

    for (j=0; j<NREP2; j++)
       MPI_Bcast (&dato,1,MPI_INT,
                  0,MPI_COMM_WORLD);

    t1 = (MPI_Wtime () - t0) / NREP2;
    if (t1 < tmin) tmin = t1;
   }

  if (pid == 0) printf ("%d %1.6f\n", npr, tmin*1000);

  MPI_Finalize ();
  return 0;
}
