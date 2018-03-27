/****************************************************
	t_allreduce.c
	medir tiempos de un allreduce
****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NREP1 10	// repeticiones para quedarno con el minimo
#define NREP2 1000	// repeticiones del bucle de allreduce para hacer la media

int main (int argc, char **argv)
{
  int  npr, pid, i, j;
  double  a, b;
  double  t0, t1, tmin;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  tmin = 100000;
  for (i=0; i<NREP1; i++) 
  {
    MPI_Barrier (MPI_COMM_WORLD);
    a = 1.0; b = 0.0;
    t0 = MPI_Wtime ();

    for (j=0; j<NREP2; j++) 
       MPI_Allreduce (&a, &b, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    t1 = (MPI_Wtime () - t0) / NREP2;
    if (t1 < tmin) tmin = t1;
  }

  if (pid == 0) printf ("Allreduce (%d pr.)  Tex = %1.3f ms\n", npr, tmin*1000); 
  MPI_Finalize ();
  return 0;
}
