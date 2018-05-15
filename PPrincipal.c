/*************************************************************
	Introduccion_v3BCMan_RE
************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "defines.h"

/*  s (int vec[CONSTANTE])
    s ( int *vec)
*/

// Forma en C de decirle que vas a usar una funcion externa que no esta en el fichero o modulo
extern void PracticaBinaria (MPI_Comm comm, int n, int vec[n], int root);

int main (int argc, char *argv[]) {
	// En c las constantes se definen con define
    #define N  1000

	// Declarar variables primitivas
	int root = 0, rank, nprocs, i, j = 1,jj, w,l,n,k, tamUser, tamItems, tamVistas, tamJ, auxPosC = 0, pos, origen = 0, messagesize, tam, cuantos2, pos2, itemI, itemJ, itemJ_ant;
	float posF, posC;
	char buf[1024];
	int *pR_I, *pR_J, *auxpR_I, *auxpR_J; // Si ha valorado algo, usuarios
	long double suma, *aux2norma, *prod_interno, *prod2N, auxnorma, *arcocoseno;
	double *angulo;
	int userI, userJ, nbusqueda;
	
	int Nloc, cuantos, stride, resto;
	int *sendcounts_I, *displ_I, *sendcounts_J, *displ_J, *auxitem;
	
	Elemento *v_Elementos;

	// Declarar otro tipo variables
	FILE *file;

	// Declarar variables MPI
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
	//MPI_Request req;

    MPI_Init (&argc, &argv);
	
	// variables para crear el struct
	 int blocklengths[7];
	 MPI_Aint offsets[7];
	 MPI_Datatype types [7];
	 MPI_Datatype struct_elemento;
	 
	 types[0] = MPI_INT;
	 types[1] = MPI_INT;
	 types[2] = MPI_LONG_DOUBLE;
	 types[3] = MPI_LONG_DOUBLE;
	 types[4] = MPI_LONG_DOUBLE;
	 types[5] = MPI_LONG_DOUBLE;
	 types[6] = MPI_UB;
	 
	 blocklengths[0] = 1;
	 blocklengths[1] = 1;
	 blocklengths[2] = 1;
	 blocklengths[3] = 1;
	 blocklengths[4] = 1;
	 blocklengths[5] = 1;
	 blocklengths[6] = 1;
	 
	 offsets[0] = offsetof(Elemento, item_i);
	 offsets[1] = offsetof(Elemento, item_j);
	 offsets[2] = offsetof(Elemento, prod_interno);
	 offsets[3] = offsetof(Elemento, prod2N);
	 offsets[4] = offsetof(Elemento, acos);
	 offsets[5] = offsetof(Elemento, angulo);
	 offsets[6] = sizeof(Elemento);
 
	MPI_Type_create_struct(7, blocklengths, offsets, types, &struct_elemento);
	MPI_Type_commit(&struct_elemento);
	 
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &nprocs);
	
	sendcounts_I = malloc(nprocs*sizeof(int));
	displ_I = malloc(nprocs*sizeof(int));
	
	if (rank == root){
		sendcounts_J = malloc(nprocs*sizeof(int));
		displ_J = malloc(nprocs*sizeof(int));
	}
	
	nbusqueda =  atoi(argv[2]);
	printf("nbusqueda %d\n", nbusqueda);
	
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
	
//	MPI_Barrier (comm);
	
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
	}
	// Calculo prod_interno y prod2N
	v_Elementos = malloc((sendcounts_I[rank]*(tamItems-1))*sizeof(Elemento));
	j = i = w = itemJ = 0;
	while (i <= nprocs - 1) {
		cuantos = 0;
		if (rank == i) {
			itemI = displ_I[rank];	itemJ_ant = itemJ;
			for (j=0; j<=sendcounts_I[rank]-1; j++, itemI++){
				auxnorma = aux2norma[j];
				MPI_Bcast (&auxnorma, 1, MPI_LONG_DOUBLE, i, comm);				
				if (j==sendcounts_I[i]-1) {
					pos = cuantos; cuantos = auxpR_I[j + 1];
				} else {
					pos = cuantos; cuantos += auxpR_I[j + 1] - auxpR_I[j];
				}
				for(n = 0; n <= nprocs-1; n++) 
					if(n != rank)
						MPI_Send(&auxpR_J[pos], cuantos - pos, MPI_INT, n, 1, comm);
				cuantos2 = pos2 = 0; itemJ = itemJ_ant;
				for (jj=0; jj<=sendcounts_I[rank]-1; jj++, itemJ++){
					suma = 0;
					if (jj==sendcounts_I[rank]-1) {
							pos2 = cuantos2; cuantos2 = auxpR_I[jj + 1];
					} else {
						pos2 = cuantos2; cuantos2 += auxpR_I[jj + 1] - auxpR_I[jj];
					}
					if (j!=jj){
						v_Elementos[w].item_i = itemI;
						v_Elementos[w].item_j = itemJ;
						v_Elementos[w].prod2N = aux2norma[j]*aux2norma[jj];
						for (n=0; n<=cuantos-pos-1; n++){
							userI = auxpR_J[pos+n];
							for (k=0; k<=cuantos2-pos2-1; k++){
								userJ = auxpR_J[pos2+k];
								if (userI==userJ)
									suma += 1;		
							}
						}
						v_Elementos[w].prod_interno = (long double) suma;
						w++;
					}						
				}				
			}
			i++; j = 0;
		} else {
			itemI = displ_I[rank];
			MPI_Bcast (&auxnorma, 1, MPI_LONG_DOUBLE, i, comm);
			MPI_Probe(i, 1, comm, &status);
			MPI_Get_count(&status, MPI_INT, &messagesize);
			auxitem = malloc(sizeof(int) * messagesize);
			MPI_Recv(auxitem, messagesize, MPI_INT, i, 1, comm, &status);
			for (jj=0; jj<=sendcounts_I[rank]-1; jj++, itemI++){
				v_Elementos[w].item_i = itemI;
				v_Elementos[w].item_j = itemJ;
				v_Elementos[w].prod2N = auxnorma*aux2norma[jj];
				suma = 0;
				if (jj==sendcounts_I[rank]-1)
					tam = auxpR_I[jj+1]-cuantos;
				else {
					tam = auxpR_I[jj+1]-auxpR_I[jj];
					cuantos += tam;
				}
				for (n=0; n<=tam-1; n++){
					userI = auxpR_J[cuantos+n];
					for (k=0; k<=messagesize-1; k++){
						userJ = auxitem[k];
						if (userI==userJ)
							suma += 1;
					}
				}		
				v_Elementos[w].prod_interno = (long double) suma;
				w++;
			}
			free (auxitem);
			j++; itemJ++;
			if (sendcounts_I[i] == j) {
				i++; j = 0;
			} 	
		}	
	}

	// Calculo acos y angulo
	for (i=0; i<=sendcounts_I[rank]*(tamItems-1)-1; i++){
		v_Elementos[i].acos = (long double) acos((double) v_Elementos[i].prod_interno/(double) v_Elementos[i].prod2N);
		v_Elementos[i].angulo = ((double) v_Elementos[i].acos*(180.0/3.14159265));
	}
	
	MPI_Barrier(comm);
/*
	for (i=0; i<=nprocs-1; i++)
		if (rank == i)
			for (j=0; j<=sendcounts_I[rank]*(tamItems-1)-1; j++){
				printf("soy %d -> item_i: %d\n", rank, v_Elementos[j].item_i);
				printf("soy %d -> item_j: %d\n", rank, v_Elementos[j].item_j);
				printf("soy %d -> prod_interno: %Le\n", rank, v_Elementos[j].prod_interno);
				printf("soy %d -> prod2N: %Le\n", rank, v_Elementos[j].prod2N);
				printf("soy %d -> acos: %Le\n", rank, v_Elementos[j].acos);
				printf("soy %d -> angulo: %f\n", rank, v_Elementos[j].angulo);
			}
*/	
	// Busqueda
	long double *acos_menor, *busqueda, *aux_busqueda;
	busqueda = malloc((tamItems-1)*sizeof(long double));
	acos_menor = malloc(nbusqueda*sizeof(long double));
	Elemento *elem_menor;
	elem_menor = malloc(nbusqueda*sizeof(Elemento));
	itemI = displ_I[rank];

	for (i=0; i<=nprocs-1; i++){
		if (rank == i){
			printf("Soy %d y tengo %d items -> \n", rank, sendcounts_I[rank]);
			for (n=0; n<=sendcounts_I[rank]-1; n++, itemI++){			
				k=0;
				printf("\t Busqueda para item %d:\n", itemI);
				for (j=0; j<=sendcounts_I[rank]*(tamItems-1)-1; j++)			
					if (itemI == v_Elementos[j].item_i){
					//	printf("\t\t item j %d -> acos %Le ; angulo %f \n", v_Elementos[j].item_j, v_Elementos[j].acos, v_Elementos[j].angulo);
						busqueda[k] = v_Elementos[j].acos;
						k++;
					}
				aux_busqueda = malloc(((tamItems-1))*sizeof(long double));
				aux_busqueda = busqueda;
				for (j=0; j<=nbusqueda-1; j++){
					MPI_Reduce_local(aux_busqueda, &acos_menor[j], (tamItems-1)-j, MPI_LONG_DOUBLE, MPI_MIN);
					for (w=0; w<=(tamItems-1)-j; w++){
						if (aux_busqueda[w] == acos_menor[j])
							break;
					}
					free(aux_busqueda);
					aux_busqueda = malloc(((tamItems-1)-j)*sizeof(long double));
					aux_busqueda[w:(tamItems-1)-j] = aux_busqueda[w+1:(tamItems-1)];						
				}
				k=0;
				for (j=0; j<=sendcounts_I[rank]*(tamItems-1)-1; j++)			
					if (itemI == v_Elementos[j].item_i){
					//	printf("\t\t item j %d -> acos %Le ; angulo %f \n", v_Elementos[j].item_j, v_Elementos[j].acos, v_Elementos[j].angulo);
						for (w=0; w<=nbusqueda-1; w++){
							if (v_Elementos[j].acos==acos_menor[w]){
								elem_menor[k]=v_Elementos[j];
								k++;
								printf("\t\t item j %d -> acos %Le ; angulo %f \n", elem_menor[j].item_j, elem_menor[j].acos, elem_menor[j].angulo);
								break;
							}
						}
					}
			}
		}
	}

	
	if(rank == root) {
		free (pR_J); free (pR_I);
		free (displ_J); free (sendcounts_J);
	}

	free (displ_I);
    free (sendcounts_I);
	free (auxpR_I);
	free (auxpR_J);
	free (aux2norma);
/*
	free (angulo);
	free (prod_interno);
	free (prod2N);
*/
    MPI_Finalize();
	return (0);
}

