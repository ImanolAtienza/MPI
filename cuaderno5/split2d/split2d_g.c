/***********************************************************************
   split2d_g.c	SOLUCION
   un split en 2D y un allreduce en cada nuevo comunicador
   SOLO PARA 16 PROCESADORES
************************************************************************/


#include <stdio.h>
#include <mpi.h>
#include <math.h>

int main (int argc, char* argv[]) 
{
  int       pid, pidbloque;
  int       npr, i, max;
  int       color, fila, columna, k, kbl, filabl, columnabl;
  MPI_Comm  bloque;


  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  k = 4;  // 4 x4 procesos
  fila = pid / k;
  columna = pid % k;

  kbl = 2;	// 2x2 bloques
  filabl = fila / kbl;
  columnabl = columna / kbl;

  color = filabl * kbl + columnabl;		// 4 colores, 4 bloques

/* este trozo de codigo es equivalente al anterior   
  if ((fila<2) && (columna<2)) color = 0;
  if ((fila < 2) && (columna >= 2)) color = 1;
  if ((fila >= 2) && (columna < 2)) color = 2;
  if ((fila >= 2) && (columna >= 2)) color = 3;
*/

  MPI_Comm_split (MPI_COMM_WORLD, color, 0, &bloque);
  MPI_Comm_rank (bloque, &pidbloque);		

  MPI_Allreduce (&pid, &max, 1, MPI_INT, MPI_MAX, bloque);
  printf ("\n en pid %d (pidbloque %d) el maximo es %d\n", pid, pidbloque, max);


  MPI_Finalize ();
  return (0);
}  /* main */
