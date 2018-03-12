/*************************************************************
	Introduccion_v3
************************************************************/

#include <mpi.h>
#include <stdio.h>
/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

// Forma en C de decirle que vas a usar una funcion externa que no esta en el fichero o modulo
extern void Ejemplo1 (MPI_Comm comm, int n, int vec[n]);

int main (int argc, char *argv[])
{
	// En c las constantes se definen con define
    #define N  1000
     int vector[N];

     MPI_Init(&argc, &argv);

     // incializar vector a 2, tambien vale vector[ : ] --> quiere decir por defecto, lo mismo que lo siguiente
      vector[0:N]=2;
     Ejemplo1(MPI_COMM_WORLD,N,vector);

     MPI_Finalize();
     return (0);
}

