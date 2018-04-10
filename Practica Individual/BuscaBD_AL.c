/********************************************************************

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

#include "defines.h"

/***
                    CONSIDERACIONES GENERALES
    Se desarrollaran las tres fases de forma progresiva.
    Se presentaran los programas de las tres fases, con sus comprobaciones
    de que funcionan.
    Se analizara si escala bien el tiempo del programa, variando el tamaño de la BD,
       los elementos de busqueda y el numero de procesadores:
       1.- Lo tamaños de BD: 1.000, 100.000 y 10.000.000
       2.- Los elementos de busqueda: un 1, 2, 10, 20
       3.- Numero procesadores: 1,2,8,32,128

    Se valorara:
     1.- El uso de empaquetado y tipos de datos derivados.
     2.- El uso minimo de mensajes de comunicacion.
     3.- El tiempo de ejecucion

    Un consejo:
     Empieza haciendo lo sencillo, aunque sea ineficiente, pero que te funcione.
     Depues vete complicando gradualmente el algoritmo para hacerlo mas eficiente, legible,
     elegante....
***/


/**
    Funcion que inicializa la BD y el vector con identificadores a buscar.
    Su especificacion esta en el fichero Rutinas.c
**/
extern   Leer_datos (Elemento *v_Elementos, int n_Elementos,
                 int n_iguales, int *elem_buscar,
                 unsigned semilla);

int main (int argc, char** argv)
{
  int i, rank, nprocs, sizeBD, elementS, root, pos, *vectorElementos, *numElemnt, *desp, bLength[4], stride, resto;
  int tamBuf = 1024;
  char buf[tamBuf];
  double t0, t1;
  time_t t;
  Elemento *vectorBD, *vectorBDAux;
  MPI_Aint offset[4];
  MPI_Datatype types[4], tipoStruct;
  MPI_Comm comm;
  //MPI_Request *req;
  //MPI_Status *vstatus;

  MPI_Init(&argc, &argv);

  root = 0;
  pos = 0;
  
  comm = MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size (comm, &nprocs);

  numElemnt = malloc(sizeof(int) * nprocs);
  desp = malloc(sizeof(int) * nprocs);
  //req = malloc(sizeof(MPI_Request) * nprocs);
  //vstatus = malloc(sizeof(MPI_Status) * nprocs);
  
  offset[0] = offsetof(Elemento, Producto);
  offset[1] = offsetof(Elemento, identificador);
  offset[2] = offsetof(Elemento, euros);
  offset[3] = sizeof(Elemento);  
  
  bLength[0] = 16;
  bLength[1:4] = 1;

  types[0] = MPI_CHAR;
  types[1] = MPI_INT;
  types[2] = MPI_FLOAT;
  types[3] = MPI_UB;

  MPI_Type_create_struct(4, bLength, offset, types, &tipoStruct);
  MPI_Type_commit(&tipoStruct);

/***********************/
/****    FASE 1     ****/
/***********************/

  /** El root debe pedir por pantalla:
       1.- El numero de elementos en la BD
       2.- La cantidad de elementos a Buscar aleatoriamente en la BD
       3.- Reservar espacio para la BD y el vector de busqueda. Las dimensiones
           de las estructuras las dan los parametros de los pasos 1 y 2.
       4.- Inicializar la BD y el vector de busqueda llamando a Leer_datos(...)
  **/

  if(rank == root) {
    printf("Inicio Fase 1\n\nIntroduce el numero de elementos en la BD:\n");
    scanf("%d", &sizeBD);
    printf("Cantidad de elementos a buscar aleatoriamente en la BD:\n");
    scanf("%d", &elementS);
    printf("\n");

    t0 = MPI_Wtime ();
    
    vectorBD = vectorBDAux = malloc(sizeof(Elemento) * sizeBD);
    vectorElementos = malloc(sizeof(int) * elementS);

    Leer_datos(vectorBD, sizeBD, elementS, vectorElementos, (unsigned) time(&t));

  /**
     Repartir a los clones los parametros leidos por el root de pantalla.
     Si utilizas funciones colectivas, se valora minimizar el numero de ellas,
     ya que implica menor tiempo de ejecucion.
  **/

    MPI_Pack(&sizeBD, 1, MPI_INT, buf, tamBuf, &pos, comm);
    MPI_Pack(&elementS, 1, MPI_INT, buf, tamBuf, &pos, comm);
    MPI_Bcast (buf, tamBuf, MPI_PACKED, root, comm);
    //MPI_Wait(&req[rank],&vstatus[rank]); 
  } else {
    MPI_Bcast (buf, tamBuf, MPI_PACKED, root, comm);
    //MPI_Wait(&req[rank],&vstatus[rank]);
    MPI_Unpack (buf, tamBuf, &pos, &sizeBD, 1, MPI_INT, comm);
    MPI_Unpack (buf, tamBuf, &pos, &elementS, 1, MPI_INT, comm);

    vectorBD = vectorBDAux = malloc(sizeof(Elemento) * sizeBD);
    vectorElementos = malloc(sizeof(int) * elementS);    
  }

  //printf("Soy %d y tengo tam_bd %d y elem_Bus %d\n", rank, sizeBD, elementS);

  /**
      Repartir a los clones el vector de busqueda
  **/

  MPI_Bcast (vectorElementos, elementS, MPI_INT, root, comm);
  //MPI_Wait(&req[rank],&vstatus[rank]);

  /**
        Comprobar que se ha repartido bien, el vector de busqueda.
        Utiliza un numero de elementos a buscar pequeño para comprobar.

  **/

  /*printf("Soy %d y en mi vector de busqueda hay: ", rank);
  for(i=0; i < elementS; i++)
    printf("%d ", vectorElementos[i]);

  printf("\n");*/

 /**  Reparto de la BD que posee el root entre los clones.
      Cada clon tendra localmente un trozo de la BD.
      El reparto de la BD debe ser lo mas equitativo posible entre lo clones
      por motivos de eficiencia.
      Se valorara utilizar tipos de datos para las trasferencias.
  **/

  i = 0;
  stride = sizeBD / nprocs;
  resto = sizeBD % nprocs;

  desp[0] = 0;
  numElemnt[0:nprocs] = stride;


  while(resto != 0) {
  	numElemnt[i] += 1;
  	resto--;
  	i++; 
  }

  for(i = 0; i < nprocs; i++)
  	desp[i] = desp[i-1] + numElemnt[i-1]; 

 /* if(rank == root) {
	  printf("Bloques\n");
	  for(i = 0; i < nprocs; i++) 
	  	printf("%d - ", numElemnt[i]);

	  printf("\n");

	  printf("Desp..\n");
	  for(i = 0; i < nprocs; i++)
	  	printf("%d - ", desp[i]);

	  printf("\n");
  }*/

  MPI_Scatterv(vectorBD, numElemnt, desp, tipoStruct, vectorBDAux, numElemnt[rank], tipoStruct, root, comm);

  /**
        Comprobar que se ha repartido bien la BD entre los clones,
        Utiliza una BD pequena para comprobar.
  **/

  /*for(i = 0; i < numElemnt[rank]; i++)
  	printf("Soy %d tengo en la bd %s, %d, %f\n", rank, vectorBDAux[i].Producto, vectorBDAux[i].identificador, vectorBDAux[i].euros);*/

  if(rank == root) {
  	t1 = MPI_Wtime ();
  	printf("\nTiempo de ejecucion en rank %d = %1.3f ms\nFin Fase 1\n\n", rank, (t1-t0) * 1000);
  }

  MPI_Barrier(comm);

/***********************/
/****    FASE 2     ****/
/***********************/

 /**
    Eliminar/liberar memoria de la estructura con toda la BD que poseia el root.
    Solo quedaran los BD locales distribuidas en los clones.
    Si en la version anterior la liberabas al final del programa,
     quita el free del final para que no te de error
 **/

 /**
     BUSQUEDA DISTRIBUIDA EN LA BD DE LOS IDENTIFICADORES DEL VECTOR DE BUSQUEDA.

     Cada clon busca localmente en la parte de BD que le ha tocado
        los identificadores que hay que buscar
     Reservar una estructura local con los elementos de la BD que el clon haya encontrado.
  */


  /**
     Todos los clones enviarán sus resultados al root.
     Depende de la solución que encuentres, puede implicarte que el root necesite una estructura extra
      para recibir todas las estructuras resultados de los procesos.
      Se valorara el menor uso de memoria, y de transferencia de datos.
  **/

  /**
      Comprobar que la búsqueda distribuida se hace bien.
      En las pruebas puedes comprobar/imprimir el rank de los que lo encuentran...
  **/

/***********************/
/****    FASE 3     ****/
/***********************/

 /**
      CALCULAR LA MEDIA DEL CAMPO EUROS DE LA BD, DE FORMA DISTRIBUIDA.
      CALCULAR LA DESVIACION DEL CAMPO EUROS DE LA BD, DE FORMA DISTRIBUIDA.
      Utilizar la Precision mpi_long_double.
      Todos los clones tienen que tener la media y la desviacion.
 **/


 /**
      Comprobar si se hace bien de forma distribuida.
      La media y desviacion te tienen que dar algo parecido a la media y
      desviación con la que se ha inicializado el campo euros a partir
      de la distribucion gaussiana.
 **/



 /**
      Sacar por pantalla una tipificacion de los elementos encontrados
      a partir de la media y la desviación del campo euros de la BD
 **/

 if (rank==root)
  {
    /*for (...)
    {
       ...
       tipo = (media - euros_del_elemento_encontrado)/desviacion;
       if (tipo >= 3.0) strcpy (texto," 1,5 por 1000 mas pobre");
       else
       if (tipo >= 2.0) strcpy (texto," 2,25 por 100 mas pobre");
       else
       if (tipo >= 1.0) strcpy (texto,"16 por 100 mas pobre");
       else
       if (tipo <= -3.0) strcpy (texto,"1,5 por 1000 mas rico");
       else
       if (tipo <= -2.0) strcpy (texto,"2,25 por 100 mas rico");
       else
       if (tipo <= -1.0) strcpy (texto,"16 por 100 mas rico");
       else
        if (tipo<0.0)strcpy (texto,"en el 50 por ciento mas rico");
        else strcpy (texto,"en el 50 por ciento más pobre");

       /***
        Sacar por pantalla
         1.- La informacion de cada elemento encontrado de la BD
         2.- Su tipificacion en franjas de pobres y ricos
       ***/
    //}
  }



/** liberar memoria de variables dinamicas **/
  free(vectorElementos);
  free(vectorBD);
  free(numElemnt);
  free(desp);
  MPI_Finalize();
  return (0);
} /*  main  */




