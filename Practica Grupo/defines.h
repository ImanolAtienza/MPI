/********************************************************************
 defines de la aplicacion
*********************************************************************/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// En c las constantes se definen con define
#define N  1024
#define root 0
#define comm MPI_COMM_WORLD

typedef struct  {
   int item_i;
   int item_j;
   long double prod_interno;
   long double prod2N;
   long double acos;
   float angulo;
} Elemento;

