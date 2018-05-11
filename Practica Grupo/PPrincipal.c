/*************************************************************
	PPrincipal - Busqueda Binaria
************************************************************/

#include "defines.h"

int main (int argc, char *argv[]) {
	// Declaracion de variables primitivas de la aplicacion
	int root = 0, rank, nprocs, i, j = 1, jj, w , n, k, tamUser, tamItems, tamVistas, tamJ, auxPosC = 0, pos, messagesize, tam, Nloc, cuantos, stride, resto;
	int userI, userJ;
	int *sendcounts_I, *displ_I, *sendcounts_J, *displ_J, *auxitem, *pR_I, *pR_J, *auxpR_I, *auxpR_J; // Si ha valorado algo, usuarios

	float posF, posC;

	double *angulo;
	long double suma, *aux2norma, *prod_interno, *prod2N, auxnorma, *arcocoseno;

	char buf[N];

	// Declarar otro tipo variables
	FILE *file;

	// Declarar variables MPI
	MPI_Comm comm = MPI_COMM_WORLD;
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
	
//	printf("Soy %d, sendcounts_I %d, displ_I %d\n", rank, sendcounts_I[rank], displ_I[rank]);
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

