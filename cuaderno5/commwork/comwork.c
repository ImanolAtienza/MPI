/***********************************************************************
  comwork.c
  1 master / n-1 workers    
  un comnicador para los workers
************************************************************************/

#include <stdio.h>
#include <mpi.h>


int main (int argc, char* argv[]) 
{
  int       npr, pid, pidw=-1;
  int       A=0, B=-1;
  MPI_Group  grupototal, grupoworkers;
  MPI_Comm   c_workers;


  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);


  MPI_Comm_group (MPI_COMM_WORLD, &grupototal);
  int eliminar = 0;
  MPI_Group_excl (grupototal, 1, &eliminar, &grupoworkers);
  MPI_Comm_create (MPI_COMM_WORLD, grupoworkers, &c_workers);
  
  if (pid > 0) {
    MPI_Comm_rank (c_workers, &pidw);
    if (pidw == 0) A = 7;
    MPI_Bcast (&A, 1, MPI_INT, 0, c_workers);
    A = A + pidw;
    MPI_Allreduce (&A, &B, 1, MPI_INT, MPI_SUM, c_workers);
  }
  else sleep (6);

  printf ("\n pid %d  pidw %d   B %d \n", pid, pidw, B);


  MPI_Finalize ();
  return (0);
}  /* main */
