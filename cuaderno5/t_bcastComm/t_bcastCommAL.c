/****************************************************
	t_bcastComm.c
	medir tiempos de una bcast desde 2 a npr procesadores
****************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NREP1 10	// repeticiones para quedarnos con el minimo
#define NREP2 1000	// repeticiones del bucle de barrier para hacer la media

int main (int argc, char **argv)
{
  int  npr, pid, i, j, k, comS;
  int dato = 10;

  double  t0, t1, tmin;
  MPI_Group  grupoT, grupo;
  MPI_Comm   comGrupo;


  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  int vpid[1][3];
  vpid[0][2] = 1;

  for (k=2;k<npr;k++)
  {
  	vpid[0][0] = k;
  	vpid[0][1] = npr-1;
  	
   MPI_Comm_group (MPI_COMM_WORLD, &grupoT);
   MPI_Group_range_excl(grupoT, 1, vpid, &grupo);
   MPI_Comm_create (MPI_COMM_WORLD, grupo, &comGrupo);

   if (MPI_COMM_NULL != comGrupo)   /** xx solo entran los k procesos primeros **/
   {
    tmin = 100000;

    for (i=0; i<NREP1; i++)
    {
     MPI_Barrier (comGrupo);
     t0 = MPI_Wtime ();

     for (j=0; j<NREP2; j++)
       MPI_Bcast (&dato,1,MPI_INT,
                  0,comGrupo);  /** xxxxx solo a los k procesos primeros**/

     t1 = (MPI_Wtime () - t0) / NREP2;
     if (t1 < tmin) tmin = t1;
    }

    MPI_Comm_size(comGrupo, &comS);
    if (pid == 0) printf ("proc %d %1.6f\n", comS, tmin*1000); /** XXXX size nuevo del comunicador **/

    MPI_Comm_free(&comGrupo);
    MPI_Group_free(&grupo);

   }
  }


  MPI_Finalize ();
  return 0;
}
