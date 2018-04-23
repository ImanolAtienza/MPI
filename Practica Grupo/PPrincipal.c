/*************************************************************
	Introduccion_v3BCMan_RE
************************************************************/

#include <mpi.h>
#include <stdio.h>
/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

// Forma en C de decirle que vas a usar una funcion externa que no esta en el fichero o modulo
extern void PracticaBinaria (MPI_Comm comm, int n, int vec[n], int root);

int main (int argc, char *argv[]) {
	// En c las constantes se definen con define
    #define N  1000

	// Declarar variables primitivas
	int root = 0, rank, nprocs, i = 0;
	char buf[1024];
	int *pR_I, *pC_J; // Si ha valorado algo, usuarios

	// Declarar otro tipo variables
	FILE *file;

	// Declarar variables MPI
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init (&argc, &argv);

	 
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &nprocs);
	
	if(rank == root) {
		
		if (file = fopen (argv[1], "r")) 
			while ((fread(buf, 1, sizeof buf, file)) > 0) {	
				printf();
       			switch (i) {
       				case 1:
       					pR_I = malloc(sizeof(int) * atoi(buf));
       					break;
   					case 3:
   						pR_J = malloc(sizeof(int) * atoi(buf));
       			}
       			i++;
			}
	}


    MPI_Finalize();
    return (0);
}

