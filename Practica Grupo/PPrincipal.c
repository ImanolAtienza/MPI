/*************************************************************
	Busqueda Binaria
************************************************************/
#include "defines.h"

int main (int argc, char *argv[]) {
	// Declarar variables primitivas
	// Para recoger los float que contiene el fichero
	float posF, posC;
	
	// Contiene el clon y numero de clones
	int rank, nprocs;

	// Variables indice de bucles
	int i, j = 1, jj, w, l, n, k;

	// Diferentes variables para uso en momentos puntuales
	int tamUser, tamItems, tamVistas, tamJ, auxPosC = 0, pos, origen = 0, messagesize, tam, cuantos2, pos2, itemI, itemJ, itemJ_ant;

	// Son los vectores que contendran en primera instancia todo el fichero y despues, el trozo para cada clon
	int *pR_I, *pR_J, *auxpR_I, *auxpR_J;

	// Para identifciar al usuario y el número de minimas distancias que se desea buscar
	int userI, userJ, nbusqueda;
	
	// Variables para obtener los trozos que le toca a cada uno, así como los vectores con los trozos y desplazamientos para cada clon
	int Nloc, cuantos, stride, resto;
	int *sendcounts_I, *displ_I, *sendcounts_J, *displ_J, *auxitem;
	
	// Es necesario que sea puntero a double por que contendra los diferentes angulos del coseno y la función necesita una presición double
	double *angulo;

	// Es nesario la mayor precisión posible, por eso long double, todos son punteros para poder alojar las tamItems - 1 asociaciones con los items para el calculo
	// de la distancia entre ellos
	long double suma, *aux2norma, *prod_interno, *prod2N, auxnorma, *arcocoseno;

	// Buffer para empaquetado, N es una constante definida en defines.h de 1024
	char buf[N];

	// Puntero que contiendrá ordenados por items los calculos de cada uno y coordenadas para asociarlos entre ellos, se encuentra en defines.h
	Elemento *v_Elementos;

	// Variable para recorrer el fichero
	FILE *file;

	// Declarar variables MPI, status es requerido por que hacemos uso de algun Send y Recv
    MPI_Status status;
	//MPI_Request req;

    // Inicio de programa MPI
    MPI_Init (&argc, &argv);
	 
	MPI_Comm_rank (comm, &rank);
	MPI_Comm_size (comm, &nprocs);
	
	// Crear vector de cuanto le toca y desplazamientos para el vector pR_I
	sendcounts_I = malloc(nprocs*sizeof(int));
	displ_I = malloc(nprocs*sizeof(int));
	
	// Se obtiene el parametro de número de busqueda para el final	
	nbusqueda =  atoi(argv[2]);
	
	if(rank == root) {
		// Crear vector de cuanto le toca y desplazamientos para el vector pR_J, solo el root podrá realizar esta tareá al leer el solo el fichero
		sendcounts_J = malloc(nprocs*sizeof(int));
		displ_J = malloc(nprocs*sizeof(int));
		// Se abre el fichero y se obtienen los primeros parametros del fichero
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

			// Obtiene el resto del fichero, es decir, el contenido de los vectores pR_I y pR_J y los va rellenando
			pR_I[0] = 0;
			for(i = 0; i < tamVistas; i++) {
				fscanf (file, "%f %f", &posF, &posC); 
				pR_J[i] = (int) (posF - 1);
				if(((int) (posC - 1)) != auxPosC) {
					auxPosC = (int) (posC - 1);
					pR_I[j] = i;
					j++;
				}
			}

			fclose (file); 
			// El último elemento de pR_I corresponde al Upper Bound
			pR_I[tamItems] = tamVistas;
			printf("\n"); 
	
			// Para enviar un solo dato, se empaquetan los tamaños para los demás clones
			pos = 0;
			MPI_Pack (&tamUser, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
			MPI_Pack (&tamItems, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
			MPI_Pack (&tamVistas, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
				
	}
	// Se envía a todos lo empaquetado y todos lo desempaquetan
	pos = 0;
	MPI_Bcast (buf, sizeof(buf), MPI_PACKED, root, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamUser, 1, MPI_INT, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamItems, 1, MPI_INT, comm);
	MPI_Unpack (buf, sizeof(buf), &pos, &tamVistas, 1, MPI_INT, comm);
	
	// Reparto de pR_I, para que cada uno solo tenga con lo que va a trabajar
	Nloc = ceil ((float)tamItems / (float)nprocs);
	cuantos = 0;
	for (i=0; i<=nprocs-1; i++){
		if (((tamItems-cuantos)%(nprocs-i))==0)
			Nloc = (tamItems-cuantos)/(nprocs-i);
		displ_I[i] = cuantos; sendcounts_I[i] = Nloc; cuantos += Nloc;
	}
	
	// Se reserva el espacio para cada clon y el root le envía su trozo con el desplazamiento
	auxpR_I = malloc((sendcounts_I[rank] + 1) * sizeof(int));	
	aux2norma = malloc(sendcounts_I[rank] * sizeof(long double));
	MPI_Scatterv(pR_I, sendcounts_I, displ_I, MPI_INT, auxpR_I, 
						sendcounts_I[rank], MPI_INT, root, comm);	
	
	// Reparto de pR_J, solo lo hace el root porque los demas procesos no conocen pR_I al completo, debido a que es necesario restar pR_I menos pR_I más uno
	// para saber lo que le toca a cada clon
	if (rank == root) {
		for (i=0; i<=nprocs-1; i++){
			sendcounts_J[i] = pR_I[sendcounts_I[i]+displ_I[i]]-pR_I[displ_I[i]]; 
			displ_J[i] = pR_I[displ_I[i]];
		}	
	}
	
	// Primero hay que enviar para cada uno que le toca de pR_J y despues con ello se reserva dicho espacio y se obtienen los datos con una comunicacion colectiva
	MPI_Scatter(sendcounts_J, 1, MPI_INT, &tamJ, 1, MPI_INT, root, comm);	
	auxpR_I[sendcounts_I[rank]] = tamJ;
	
	auxpR_J = malloc(tamJ*sizeof(int));
	MPI_Scatterv(pR_J, sendcounts_J, displ_J, MPI_INT, auxpR_J, 
						tamJ, MPI_INT, root, comm);
	
	if(rank == root) {
		free (pR_J); free (pR_I);
		free (displ_J); free (sendcounts_J);
	}

	// Calculo 2norma para cada clon con lo que tiene
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
		// Si le toca al clon en orden, sera el encargado de establecer cuantos items tiene, cuantos tiene que enviar para que los demás opere, operar lo suyo y enviar
		// el item al resto
		if (rank == i) {
			itemI = displ_I[rank];	itemJ_ant = itemJ;
			for (j=0; j<=sendcounts_I[rank]-1; j++, itemI++){
				auxnorma = aux2norma[j];
				MPI_Bcast (&auxnorma, 1, MPI_LONG_DOUBLE, i, comm);			
				// Como el los vectores estan repartido hay que tener en cuenta las siguientes condiciones para evaluar cuantos datos tiene por cada item
				// como no se almacenan tamItems siempre, hay que observar la diferencia puesto que los que si se contiene quiere decir que han sido evaluados por un
				// usuario	
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
		// Si no le toca a el clon enviar lo que el tiene, tendrá que esperar a recibir lo que le falta para poder calcular
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

	free(aux2norma);
	free(auxpR_I);
	free(auxpR_J);

	// Calculo arcoseno y angulo del coseno para cada clon
	for (i=0; i<=sendcounts_I[rank]*(tamItems-1)-1; i++){
		v_Elementos[i].acos = (double) acos((double) v_Elementos[i].prod_interno/(double) v_Elementos[i].prod2N);
		v_Elementos[i].angulo = ((double) v_Elementos[i].acos*(180.0/3.14159265));
	}
	
	// Busqueda de los elementos con menor distancia de cada item
	Elemento *elem_menor, *elem_busqueda;
	elem_menor = malloc(nbusqueda*sizeof(Elemento));
	elem_busqueda = malloc((tamItems-1)*sizeof(Elemento));
	itemI = displ_I[rank];
	i=0;
	while (i <= nprocs - 1){
		if (rank == i){
			printf("Soy %d y tengo %d items -> \n", rank, sendcounts_I[rank]);
			for (n=0; n<=sendcounts_I[rank]-1; n++, itemI++){			
				k=0;
				printf("\t Busqueda para item %d:\n", itemI+1);
				// Busca en v_Elementos todos los elementos relacionados con itemI y los guarda en elem_busqueda
				for (j=0; j<=sendcounts_I[rank]*(tamItems-1)-1; j++)			
					if (itemI == v_Elementos[j].item_i){
						elem_busqueda[k] = v_Elementos[j];
						k++;
					}
				// Busca en elem_busqueda los nbusqueda menores y los guarda en elem_menor en orden
				for (j=0; j<=nbusqueda-1; j++){
					elem_menor[j] = elem_busqueda[0];
					pos=0;
					for (w=1; w<(tamItems-1)-j; w++){
						// Compara los acos y busca el menor de todos los elementos de elem_busqueda
						if (elem_busqueda[w].acos < elem_menor[j].acos){
							elem_menor[j] = elem_busqueda[w];
							// Guarda la posicion para modificar elem_busqueda eliminando el menor encontrado
							pos = w;
						}	
					}
					printf("\t\t menor %d -> item = %d ; acos = %Le ; angulo %f \n", j+1,elem_menor[j].item_j+1, elem_menor[j].acos, elem_menor[j].angulo);
					// Modifica elem_busqueda para que no vuelva a encontrar el mismo elemento
					elem_busqueda[pos:(tamItems-1)-j] = elem_busqueda[pos+1:(tamItems-1)-j];						
				}
			}
		}
		i++;
	}

	free (displ_I);
    free (sendcounts_I);

    MPI_Finalize();
	return (0);
}

