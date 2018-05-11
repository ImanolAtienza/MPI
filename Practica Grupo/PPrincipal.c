/*************************************************************
	PPrincipal - Busqueda Binaria
************************************************************/

#include "defines.h"

int main (int argc, char *argv[]) {
	// Declaracion de variables primitivas de la aplicacion
	int rank, nprocs, jj, w , n, k, tamUser, tamItems, tamVistas, tamJ, pos, messagesize, tam, Nloc, cuantos, stride, resto;
	int userI, userJ;
	int i;
	int j = 1;
	int *sendcounts_I, *displ_I, *sendcounts_J, *displ_J, *auxitem, *pR_I, *pR_J, *auxpR_I, *auxpR_J; // Si ha valorado algo, usuarios

	double *angulo;
	long double suma, *aux2norma, *prod_interno, *prod2N, auxnorma, *arcocoseno;

	char buf[N];

	// Declarar variables MPI
	MPI_Status status;

	// Inicio de programa MPI
	MPI_Init(&argc, &argv);

	// Obtencion de identificador del clon, clones en el grupo general e inicializacion de vector de reparto, desplazamiento y, para el root,
	// vector de reparto y deslplazamiento del vector J
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &nprocs);
	
	sendcounts_I = malloc(nprocs * sizeof(int));
	displ_I = malloc(nprocs * sizeof(int));
	
	if (rank == root){
		sendcounts_J = malloc(nprocs*sizeof(int));
		displ_J = malloc(nprocs*sizeof(int));

		if(lecturaFichero(argv[1], &tamUser, &tamItems, &tamVistas) != 0) 
			return 1;

		/*pR_I = malloc(sizeof(int) * (tamItems + 1));
		pR_J = malloc(sizeof(int) * (tamVistas));

		if(lecturaFDatos(pR_I, pR_J, tamItems, tamVistas) != 0)
			return 1;
		
		for(i = 0; i < tamVistas; i++) 
			printf("Contenido en pR_J %d\n", pR_J[i]);

		printf("\n\n");

		for(i = 0; i < (tamItems + 1); i++)
			printf("Contenido en pR_I %d\n", pR_I[i]); 
		
		printf("\n\n");*/
	}

	empaquetadoEnvio(&tamItems, &tamVistas, &tamUser, rank);

	// Reparto de pR_I
	Nloc = ceil ((float)tamItems / (float)nprocs);
	cuantos = 0;

	for (i = 0; i <= nprocs-1; i++){
		if (((tamItems-cuantos) % (nprocs-i)) == 0)
			Nloc = (tamItems-cuantos) / (nprocs-i);

		displ_I[i] = cuantos; sendcounts_I[i] = Nloc; cuantos += Nloc;
//		if (rank == root)
//			printf(" rank %d ->   displ_I: %d	  sendcounts_I: %d\n", i, displ_I[i], sendcounts_I[i]);
	}

	pR_I = malloc(sizeof(int) * (sendcounts_I[rank] + 1));
	pR_J = malloc(sizeof(int) * (sendcounts_J[rank]));

	if(lecturaFDatos(pR_I, pR_J, tamItems, tamVistas, sendcounts_I, nprocs) != 0)
		return 1;

	for(i = 0; i < tamVistas; i++) 
		printf("soy %d mi contenido en pR_J %d\n", rank, pR_J[i]);

	printf("\n\n");

	for(i = 0; i < (tamItems + 1); i++)
		printf("soy %d mi contenido en pR_I %d\n", rank, pR_I[i]); 

		printf("\n\n");

	aux2norma = malloc(sendcounts_I[rank] * sizeof(long double));

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

	// Calculo prod_interno
	prod_interno = malloc((sendcounts_I[rank]*(tamItems-1))*sizeof(long double));
	prod2N = malloc ((sendcounts_I[rank]*(tamItems-1))*sizeof(long double));
	angulo = malloc ((sendcounts_I[rank]*(tamItems-1))*sizeof(long double));
	arcocoseno = malloc ((sendcounts_I[rank]*(tamItems-1))*sizeof(long double));
	prod_interno[0:(sendcounts_I[rank]*(tamItems-1))-1] = (long double) 0;
	j = i = 0;
	w = 0;
	while (i <= nprocs - 1) {
		cuantos = 0;
		if (rank == i) {
			for (j=0; j<=sendcounts_I[i]-1; j++){
				//printf ("soy %d -> aux2norma[%d] = %Le\n", rank,j, aux2norma[j]);
				auxnorma = aux2norma[j];
				MPI_Bcast (&auxnorma, 1, MPI_LONG_DOUBLE, i, comm);				
				if (j==sendcounts_I[i]-1) {
					pos = cuantos;
					cuantos += auxpR_I[j + 1] - cuantos;
				} else {
					pos = cuantos;
					cuantos += auxpR_I[j + 1] - auxpR_I[j];
				}
				for(n = 0; n <= nprocs-1; n++) 
					if(n != rank)
						MPI_Send(&auxpR_J[pos], cuantos - pos, MPI_INT, n, 1, comm);
					printf("auxpR_I[%d] = %d ; auxpR_I[%d] = %d\n",j+1, auxpR_I[j+1],j, auxpR_I[j]);
					printf("auxpR_I[%d] = %d ; auxpR_I[%d] = %d\n",jj+1, auxpR_I[jj+1],jj, auxpR_I[jj]);
					for (jj=0; jj<=sendcounts_I[rank]-1; jj++){
						suma = 0;
						if (j==jj)
							continue;
						prod2N[w] = aux2norma[j]*aux2norma[jj];
						if (jj==sendcounts_I[rank]-1)
							tam = auxpR_I[jj+1]-cuantos;
						else {
							tam = auxpR_I[jj+1]-auxpR_I[jj];
							cuantos += tam;
						}
						for (n=0; n<=auxpR_I[jj+1]-auxpR_I[jj]-1; n++){
							userI = auxpR_J[auxpR_I[jj]+n];
							for (k=0; k<=sendcounts_I[rank]-1; k++){
								userJ = auxpR_J[auxpR_I[j]+k];
								if (userI==userJ)
									suma += 1;
							}
						}
					//printf("soy %d -> item %d con item %d suma %Le\n",rank, j,jj,suma);
						prod_interno[w] = suma;
						w++;
					}									
				}
				i++;
				j = 0;
			} else {
				MPI_Bcast (&auxnorma, 1, MPI_LONG_DOUBLE, i, comm);
			//printf ("soy %d -> auxnorma de rank %d = %Le\n", rank,i, auxnorma);
				MPI_Probe(i, 1, comm, &status);
				MPI_Get_count(&status, MPI_INT, &messagesize);
				auxitem = malloc(sizeof(int) * messagesize);
				MPI_Recv(auxitem, messagesize, MPI_INT, i, 1, comm, &status);
				for (jj=0; jj<=sendcounts_I[rank]-1; jj++){
					prod2N[w] = auxnorma*aux2norma[jj];
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
				//printf("soy %d -> item %d con item %d suma %Le\n",rank,jj ,w ,suma);
					prod_interno[w] = suma;
					w++;
				}
				free (auxitem);
				j++;
				if (sendcounts_I[i] == j) {
					i++;
					j = 0;
				} 
			}	
		}
	// acos en double
		for (i=0; i<=sendcounts_I[rank]*(tamItems-1)-1; i++){
			arcocoseno[i] = (long double) acos((double) prod_interno[i]/(double) prod2N[i]);
			angulo[i] = ((double) arcocoseno[i]*(180.0/3.14159265));
		}

		MPI_Barrier(comm);

		for (i=0 ; i<=nprocs-1; i++)
			if (rank == i) 
				for (j=0; j<=sendcounts_I[rank]*(tamItems-1)-1; j++){
					printf("soy %d -> prod_interno: pos %d res %Le\n", rank, j, prod_interno[j]);
					printf("soy %d -> prod2N: pos %d res %Le\n", rank, j, prod2N[j]);
					printf("soy %d -> acos: pos %d res %Le\n", rank, j, arcocoseno[j]);
					printf("soy %d -> angulo: pos %d res %f\n", rank, j, angulo[j]);
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
				free (angulo);
				free (prod_interno);
				free (prod2N);

				MPI_Finalize();
				return (0);
			}

