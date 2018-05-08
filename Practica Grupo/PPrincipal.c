/*************************************************************
	Introduccion_v3BCMan_RE
************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

// Forma en C de decirle que vas a usar una funcion externa que no esta en el fichero o modulo
extern void PracticaBinaria (MPI_Comm comm, int n, int vec[n], int root);

int main (int argc, char *argv[]) {
	// En c las constantes se definen con define
    #define N  1000

	// Declarar variables primitivas
	int root = 0, rank, nprocs, i, j = 1, l,n,k, tamUser, tamItems, tamVistas, tamJ, auxPosC = 0, pos, origen = 0, messagesize;
	float posF, posC;
	char buf[1024];
	int *pR_I, *pR_J, *auxpR_I, *auxpR_J; // Si ha valorado algo, usuarios
	long double suma, *aux2norma, *prod_interno;
	int userI, userJ;
	
	int Nloc, cuantos, stride, resto;
	int *sendcounts_I, *displ_I, *sendcounts_J, *displ_J, *auxitem;

	// Declarar otro tipo variables
	FILE *file;

	// Declarar variables MPI
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
	//MPI_Request req;

    MPI_Init (&argc, &argv);
	 
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &nprocs);
	
	sendcounts_I = malloc(nprocs*sizeof(int));
	displ_I = malloc(nprocs*sizeof(int));
	
	if (rank == root){
		sendcounts_J = malloc(nprocs*sizeof(int));
		displ_J = malloc(nprocs*sizeof(int));
	}
	
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
			
			printf("\n\n");

			pR_I[0] = 0;
			for(i = 0; i < tamVistas; i++) {
				fscanf (file, "%f %f", &posF, &posC); 
				pR_J[i] = (int) (posF - 1);
				if(((int) (posC - 1)) != auxPosC) {
					auxPosC = (int) (posC - 1);
					pR_I[j] = i;
					j++;
				}
				printf("Contenido en pR_J %d\n", pR_J[i]);
			}
			
			printf("\n\n");

			fclose (file); 
			pR_I[tamItems] = tamVistas;
			printf("\n"); 

			for(i = 0; i < (tamItems + 1); i++)
				printf("Contenido en pR_I %d\n", pR_I[i]); 

			printf("\n\n"); 
			
			pos = 0;
			MPI_Pack (&tamUser, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
			MPI_Pack (&tamItems, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
			MPI_Pack (&tamVistas, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
				
	}
	pos = 0;
	MPI_Bcast (buf, sizeof(buf), MPI_PACKED, root, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamUser, 1, MPI_INT, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamItems, 1, MPI_INT, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamVistas, 1, MPI_INT, comm);
	
	// Reparto de pR_I
	Nloc = ceil ((float)tamItems / (float)nprocs);
	cuantos = 0;
	for (i=0; i<=nprocs-1; i++){
		if (((tamItems-cuantos)%(nprocs-i))==0)
			Nloc = (tamItems-cuantos)/(nprocs-i);
		displ_I[i] = cuantos; sendcounts_I[i] = Nloc; cuantos += Nloc;
//		if (rank == root)
//			printf(" rank %d ->   displ_I: %d	  sendcounts_I: %d\n", i, displ_I[i], sendcounts_I[i]);
	}
	
	printf("Soy %d, sendcounts_I %d, displ_I %d\n", rank, sendcounts_I[rank], displ_I[rank]);
	auxpR_I = malloc((sendcounts_I[rank] + 1) * sizeof(int));	
	aux2norma = malloc(sendcounts_I[rank] * sizeof(long double));
	MPI_Scatterv(pR_I, sendcounts_I, displ_I, MPI_INT, auxpR_I, 
						sendcounts_I[rank], MPI_INT, root, comm);	
	
	MPI_Barrier (comm);
	
	// Reparto de pR_J, solo lo hace el root porque los demas procesos no conocen pR_I
	if (rank == root) {
		for (i=0; i<=nprocs-1; i++){
			sendcounts_J[i] = pR_I[sendcounts_I[i]+displ_I[i]]-pR_I[displ_I[i]]; 
			displ_J[i] = pR_I[displ_I[i]];
//			if (rank == root)
//				printf(" rank %d ->   displ_J: %d	  sendcounts_J: %d\n", i, displ_J[i], sendcounts_J[i]);
		}	
	}
		
	MPI_Scatter(sendcounts_J, 1, MPI_INT, &tamJ, 1, MPI_INT, root, comm);
	printf("Soy %d, tamJ %d\n", rank, tamJ);
	auxpR_I[sendcounts_I[rank]] = tamJ;
	
	auxpR_J = malloc(tamJ*sizeof(int));
	MPI_Scatterv(pR_J, sendcounts_J, displ_J, MPI_INT, auxpR_J, 
						tamJ, MPI_INT, root, comm);
	
	// Calculo 2norma
	n = 0;
	for (i=0; i<=sendcounts_I[rank]-1; i++){
		suma = 0;
		for (j=n; j<= auxpR_I[i+1]-1; j++){
			suma += (long double) 1;
			n++;
		}
						
		aux2norma[i] = sqrtl(suma);
		//printf("soy %d -> item %d -> 2norma: %Le\n",rank, i, aux2norma[i]);
	}

	j = i = 0;
	while (i <= nprocs - 1) {
		if (rank == i) {
			k = 0;
			for (j=0; j<=sendcounts_I[i]-1; j++){
				//printf("rank %d entra en el for\n", rank);
				if (sendcounts_I[i]==1){
					pos = k;
					k += auxpR_I[j + 1];
				} else if ((sendcounts_I[i]>1)&&(j==sendcounts_I[i]-1)) {
					//printf("rank %d entra en else if raro teniendo auxpR_I[j] = %d y auxpR_I[j+1] = %d\n", rank, auxpR_I[j], auxpR_I[j+1]);
					pos = k;
					k += auxpR_I[j + 1] - k;
				} else if ((sendcounts_I[i]>1)&&(j!=sendcounts_I[i]-1)) {
					pos = k;
					k += auxpR_I[j + 1] - auxpR_I[j];
				}
				//printf("Soy %d, tengo %d sendcounts, estoy en el item %d cuya pos1 = %d y tam %d\n", rank, sendcounts_I[rank], j, pos1, k-pos1);
				for(n = 0; n <= nprocs-1; n++) {
					if(n != rank)
						MPI_Send(&auxpR_J[pos], k - pos, MPI_INT, n, 0, comm);
				}
			}
			i++;
		} else {
			//MPI_Bcast(auxitem, tamItems, MPI_INT, i, comm);
			//printf("soy %d y estoy esperando a recibir de %d\n", rank, i);
			MPI_Probe(i, 0, comm, &status);
	  		MPI_Get_count(&status, MPI_INT, &messagesize);
	  		auxitem = malloc(sizeof(int) * messagesize);
			MPI_Recv(auxitem, messagesize, MPI_INT, i, 0, comm, &status);
			printf("soy %d me ha llegado el item %d y tiene ", rank, j);
			for(k = 0; k < messagesize; k++)
				printf("%d ", auxitem[k]);
			printf("\n\n");
			j++;
			if (sendcounts_I[i] == j) {
				i++;
				j = 0;
			}	
		}
		printf("soy %d y voy por la ite %d\n", rank, i);
	}

	/*for(i = 0; i <= sendcounts_I[rank] - 1; i++) {
		MPI_Bcast (&auxpR_J[tamJ - auxpR_I[i]], auxpR_I[i+1] - auxpR_I[i], MPI_INT, rank, comm);
	}*/
	
	if(rank == root) {
		free (pR_J); free (pR_I);
		free (displ_J); free (sendcounts_J);
	}

	free (displ_I);
    free (sendcounts_I);
	free (auxpR_I);
	free (auxpR_J);

    MPI_Finalize();
	return (0);
}

