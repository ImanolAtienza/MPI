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
	int root = 0, rank, nprocs, i = 1, j = 0, posF, posC, posFaux = 0, tamUser, tamItems, tamVistas;
	float auxF, auxC;
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
			fscanf (file, "%d", &posF);    
			while (!feof(file)) {	
       			switch (i) {
       				case 1:
       					// Obtener numeros de usuarios
       					tamUser = j;
       					printf("Numero de usuarios %d\n", posF);
       					fscanf (file, "%d", &posF); 
       					i++;
       					break;
       				case 2:
       					// Crear vector de items y obtener numero de items
       					tamItems = j;
       					pR_I = malloc(sizeof(int) * (posF + 1));
       					printf("Numero de items %d\n", posF);
       					fscanf (file, "%d", &posF); 
       					i++;
       					break;
   					case 3:
   						// Crear vector de usuarios con respecto a items vistos/comprados
   						tamVistas = j;
   						pR_J = malloc(sizeof(int) * posF);
   						printf("Numero de items vistos %d\n", tamVistas);
   						fscanf (file, "%f %f", &auxF, &auxC); 
   						printf("Numero de items vistos %f, %f\n", auxF, auxC);
   						i++;
   						break;
   					case 4:
   						// Rellenar vectores
   						/*if(posC != posFaux) {
   							posFaux = pR_I[j] = (posC - 1);
   							j++;
   						}*/
   						//pR_J[posC] = 1;
   						
   						fscanf (file, "%d %d", &auxF, &auxC); 
   						printf("Numero de items vistos %f, %f\n", auxF, auxC);
   						break;
       			}
       			if(i == 4)
       				break;
			}
			printf("\n\n");
			fclose (file);        
	}


    MPI_Finalize();
    return (0);
}

