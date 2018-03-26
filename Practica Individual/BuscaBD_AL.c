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
    Se analizara si escala bien el tiempo del programa, variando el tama�o de la BD,
       los elementos de busqueda y el numero de procesadores:
       1.- Lo tama�os de BD: 1.000, 100.000 y 10.000.000
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


  /**
     Repartir a los clones los parametros leidos por el root de pantalla.
     Si utilizas funciones colectivas, se valora minimizar el numero de ellas,
     ya que implica menor tiempo de ejecucion.
  **/



  /**
      Repartir a los clones el vector de busqueda
  **/


  /**
        Comprobar que se ha repartido bien, el vector de busqueda.
        Utiliza un numero de elementos a buscar peque�o para comprobar.

  **/

 /**  Reparto de la BD que posee el root entre los clones.
      Cada clon tendra localmente un trozo de la BD.
      El reparto de la BD debe ser lo mas equitativo posible entre lo clones
      por motivos de eficiencia.
      Se valorara utilizar tipos de datos para las trasferencias.
  **/

  /**
        Comprobar que se ha repartido bien la BD entre los clones,
        Utiliza una BD pequena para comprobar.
  **/

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
     Todos los clones enviar�n sus resultados al root.
     Depende de la soluci�n que encuentres, puede implicarte que el root necesite una estructura extra
      para recibir todas las estructuras resultados de los procesos.
      Se valorara el menor uso de memoria, y de transferencia de datos.
  **/

  /**
      Comprobar que la b�squeda distribuida se hace bien.
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
      desviaci�n con la que se ha inicializado el campo euros a partir
      de la distribucion gaussiana.
 **/



 /**
      Sacar por pantalla una tipificacion de los elementos encontrados
      a partir de la media y la desviaci�n del campo euros de la BD
 **/

 if (pid==root)
  {
    for (...)
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
        else strcpy (texto,"en el 50 por ciento m�s pobre");

       /***
        Sacar por pantalla
         1.- La informacion de cada elemento encontrado de la BD
         2.- Su tipificacion en franjas de pobres y ricos
       ***/
    }
  }



/** liberar memoria de variables dinamicas **/


  return (0);
} /*  main  */




