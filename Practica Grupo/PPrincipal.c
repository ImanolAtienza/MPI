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
	int root = 0, rank, nprocs, i, j = 1, tamUser, tamItems, tamVistas, auxPosC = 0;
	float posF, posC;
	char buf[1024];
	int *pR_I, *pR_J; // Si ha valorado algo, usuarios

	// Declarar otro tipo variables
	FILE *file;

	// Declarar variables MPI
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init (&argc, &argv);

	 
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &nprocs);
	
	if(rank == root) {
		
		if (file = fopen (argv[1], "r")) 
			fscanf (file, "%d", &tamUser);    
			for(i = 0; i < 3; i++) {	
       			switch (i) {
       				case 0:
       					// Obtener numeros de usuarios
       					printf("Numero de usuarios %d\n", tamUser);
       					fscanf (file, "%d", &tamItems); 
       					break;
       				case 1:
       					// Crear vector de items y obtener numero de items
       					pR_I = malloc(sizeof(int) * (tamItems + 1));
       					printf("Numero de items %d\n", tamItems);
       					fscanf (file, "%d", &tamVistas); 
       					break;
   					case 2:
   						// Crear vector de usuarios con respecto a items vistos/comprados
   						pR_J = malloc(sizeof(int) * tamVistas);
   						printf("Numero de items vistos %d\n", tamVistas);
   						break;
       			}
			}

			pR_I[0] = 0;
			for(i = 0; i < tamVistas; i++) {
				fscanf (file, "%f %f", &posF, &posC); 
				pR_J[i] = (int) (posF - 1);
				if(((int) (posC - 1)) != auxPosC) {
					printf("El oro del peru %d, %d\n", pR_I[j - 1], i);
					auxPosC = (int) (posC - 1);
					pR_I[j] = i;
					j++;
				}
				printf("Contenido en pR_J %d\n", pR_J[i]);
			}

			fclose (file); 
			pR_I[tamItems] = tamVistas;
			printf("\n"); 

			for(i = 0; i < (tamItems + 1); i++)
				printf("Contenido en pR_I %d\n", pR_I[i]); 

			printf("\n\n");     
	}


    MPI_Finalize();

    free(pR_J);
    free(pR_I);

    return (0);
}

