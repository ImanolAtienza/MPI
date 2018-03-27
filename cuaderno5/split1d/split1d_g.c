/***********************************************************************
   spli1d_g.c   SOLUCION
   un split de kdim x kdim procesos en columnas
   luego un BC desde el primero de cada columna

   COMPLETADO
   una reduccion en cada columna
   generacion de los comunicadores fila
   una reduccion en la primera fila

   SOLO PARA UN NUMERO CUADRADO DE PROCESOS
************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define N 5

int main (int argc, char* argv[])
{
  int       pid, pidcol, pidfil;
  int       npr, kdim;
  int       color, columna;
  int	    i, A[N], B[N], C[N];
  MPI_Comm  c_col, c_fil;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  for (i=0; i<N; i++) A[i] = -1;

  kdim = sqrt (npr);
  columna = pid % kdim;
  color = columna;

//  generacion de los comunicadores columnas
  MPI_Comm_split (MPI_COMM_WORLD, color, 0, &c_col);
  MPI_Comm_rank (c_col, &pidcol);

  if (pidcol == 0) for (i=0; i<N; i++)  A[i] = 100*pid + i;

  MPI_Bcast (A, N, MPI_INT, 0, c_col);
  printf ("\n BC color %d pid %d (pidcol %d) A = %d %d %d %d %d \n",color, pid, pidcol, A[0],A[1],A[2],A[3],A[4]);


// cada columna procesa su vector + reduce en el primero de la columna.

  for (i=0; i<N; i++) A[i] = A[i] + pid;

  MPI_Reduce (A, B, N, MPI_INT, MPI_SUM, 0, c_col);
  if (pidcol == 0) printf ("\n RC pid %d (pidcol %d) B = %d %d %d %d %d \n", pid, pidcol, B[0],B[1],B[2],B[3],B[4]);



// ahora generacion de los comunicadores fila

  MPI_Comm_split (MPI_COMM_WORLD, pid/kdim, 0, &c_fil);
  MPI_Comm_rank (c_fil, &pidfil);


// una reduccion producto en la primera fila

   MPI_Reduce (B, C, N, MPI_INT, MPI_PROD, 0, c_fil);
  if (pid == 0) printf ("\n  >>>  C = %d %d %d %d %d \n",  C[0],C[1],C[2],C[3],C[4]);


  MPI_Finalize ();
  return (0);
}  /* main */
