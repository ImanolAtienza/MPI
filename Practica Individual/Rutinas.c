/********************************************************************

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>

/**
     En el fichero esta definido el typedef de un elemento/registro de la BD
**/
#include "defines.h"

/***
 Obtenido de "Generating random numbers from Normal distribution in C"
 https://phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c/
***/

double
randn (double mu, double sigma)
{
  double U1, U2, W, mult;
  static double X1, X2;
  static int call = 0;

  if (call == 1)
    {
      call = !call;
      return (mu + sigma * (double) X2);
    }

  do
    {
      U1 = -1 + ((double) rand () / RAND_MAX) * 2;
      U2 = -1 + ((double) rand () / RAND_MAX) * 2;
      W = pow (U1, 2) + pow (U2, 2);
    }
  while (W >= 1 || W == 0);

  mult = sqrt ((-2 * log (W)) / W);
  X1 = U1 * mult;
  X2 = U2 * mult;

  call = !call;

  return (mu + sigma * (double) X1);
}
/**
    Funcion que inicializa la BD y el vector con identificadores a buscar.
    La debe llamar el root.
    El parametro "v_Elementos" es un puntero a la BD.
    Se ha debido crear espacio para la BD y v_elementos apunta a ese espacio.
    "n_Elementos" especifica el numero de registros/elementos de la BD que se ha reservado.
    El parametro "elem_buscar" apunta a un vector ya creado,
     donde se va a dejar los identificadores de los elementos a buscar.
    "n_iguales" especifica el numero de identificadores elegidos al azar que se van a buscar.
**/
void Leer_datos (Elemento *v_Elementos, int n_Elementos,
                 int n_iguales, int *elem_buscar,
                 unsigned semilla)  // time_t t; (unsigned) time(&t)
{
    Elemento *p_Elemento;
    int *donde_estan;
    int i,j, quien, limite, esta;
    char texto[20];

    donde_estan = malloc(n_iguales*sizeof(int));

    srand(semilla);

    for (i=0;i<n_Elementos;i++)
    {
        /**
            El campo "texto" es Prodxxxxxxxxxxx
            xxxxxxxxxxx tomo los valores 0..n_Elementos-1
        **/
        sprintf (texto,"Prod%11d",i);
        strcpy (v_Elementos[i].Producto,texto);
        /**
            El identificador toma los valores 0..n_Elementos-1
            coincide con el valor xxxxxxxxxxx del campo texto
        **/
        v_Elementos[i].identificador=i;
        /**
            El campo "euros" sigue una distribución gaussiana:
               en este caso, media 10000 y desviación 100
            La función randn(media,desviacion) obtiene los valores.
            El mínimo valor es 0.0.
        **/
        v_Elementos[i].euros=(float)randn(10000,100); /* media 10000, desv 100 */
        if (v_Elementos[i].euros < 0.0) v_Elementos[i].euros = 0.0;
    }

    /**
       Se van a seleccionar los identificadores a buscar de los RAND_MAX primeros
    **/
    if (n_Elementos > RAND_MAX) limite = RAND_MAX;
    else limite = n_Elementos;

    /**
        Seleción aleatoria de unos identificadores a buscar en la BD.
        Se seleccionan "n_iguales" identificadores, todos distintos.
        A los seleccionados se les cambia el identificador a -identificador.
        Los seleccionados se colocan en el vector elem_buscar.
    **/
    for (i=0;i<n_iguales;i++)
        donde_estan[i]=-1;
    for (i=0;i<n_iguales;i++)
    {
        quien = rand() % limite;
        esta =0;
        for (j=0;j<i;j++)
            if (donde_estan[j]==quien)
            {
              esta=1; break;
            }
        if (esta==1)
        {
            i--; continue;
        }
        v_Elementos[quien].identificador *= -1;
        elem_buscar[i] = v_Elementos[quien].identificador;
        donde_estan[i]= quien;
    }
    free (donde_estan);
}


