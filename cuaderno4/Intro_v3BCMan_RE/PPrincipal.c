/*************************************************************
	Introduccion_v3BCMan_RE
************************************************************/

#include <mpi.h>
#include <stdio.h>
/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

// Forma en C de decirle que vas a usar una funcion externa que no esta en el fichero o modulo
extern void Intro_v3BCMan_RE (MPI_Comm comm, int n, int vec[n], int root);

int main (int argc, char *argv[])
{
	// En c las constantes se definen con define
    #define N  1000
     int vector[N];

     MPI_Init(&argc, &argv);

	 int root = 0;
	 int rank;
	 
	 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	 
     // incializar vector a 2 (solo el root), tambien vale vector[ : ] --> quiere decir por defecto, lo mismo que lo siguiente
	 if(rank==root)
      vector[:]=2;
  
     Intro_v3BCMan_RE(MPI_COMM_WORLD,N,vector,root);

     MPI_Finalize();
     return (0);
}

