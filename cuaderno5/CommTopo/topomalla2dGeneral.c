/***********************************************************************
   topomalla.c
   Creacion de una topologia en malla y sus comunicadores
        filas,columnas y planos
************************************************************************/

#include <stdio.h>
#include "mpi.h"
#include <math.h>

int main (int argc, char* argv[])
{
  int       pid, pid_malla, pid_m, pid_fila, pid_col, pid_plano;
  int       i,npr, k, dimen;
  int       *kdim, *anillo, opt = 1;
  int       *coord, *dim;
  int       pruebaf, pruebac;
  MPI_Comm  c_malla, c_fila, c_col, c_plano;
  int derecha,izquierda,arriba,abajo,antes,despues;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  if (pid==0)
  {
      do
      {
        printf ("dimensiones >0 && <=3\n");
        scanf ("%d",&dimen);
      }
      while (dimen<1 || dimen>3);

      kdim = malloc(sizeof(int)*dimen);
      anillo = malloc(sizeof(int)*dimen);
      coord = malloc(sizeof(int)*dimen);
      dim = malloc(sizeof(int)*dimen);
      for (i=0;i<dimen;i++)
      {
        printf ("rango dimension %d\n",i+1);
        scanf ("%d",&kdim[i]);
        printf ("anillo (0 no; 1 si)\n");
        scanf ("%d",&anillo[i]);
     }
  }
  MPI_Bcast (&dimen,1,MPI_INT,0,MPI_COMM_WORLD);
  if (pid!=0)
  {
      kdim = malloc(sizeof(int)*dimen);
      anillo = malloc(sizeof(int)*dimen);
      coord = malloc(sizeof(int)*dimen);
      dim = malloc(sizeof(int)*dimen);
  }
  MPI_Bcast (kdim,dimen,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast (anillo,dimen,MPI_INT,0,MPI_COMM_WORLD);

// datos de la topologia

// creacion del nuevo comunicador con una malla-toro asociada
  MPI_Cart_create (MPI_COMM_WORLD, dimen, kdim, anillo, opt, &c_malla);

//coordenadas del procesador en la malla-toro
  MPI_Comm_rank (c_malla, &pid_malla);
  MPI_Cart_coords (c_malla, pid_malla, dimen, coord);

//a la inversa, partiendo de las coordenadas, devuelve el valor del pid en la malla-toro
  MPI_Cart_rank (c_malla, coord, &pid_m);

  if (pid == 0) printf ("\n ---- coordenadas en la malla-toro \n");
sleep(1);

 if (dimen==1)
  {
     /**Displacement ( > 0: upward shift, < 0: downward shift) (integer)**/
     MPI_Cart_shift (c_malla,0,1,&izquierda,&derecha);
      printf ("Proceso %d > pid_mt = %d, coords = (%d) pid_m = %d\n  , de_iz(%d %d)\n",
        pid, pid_malla, coord[0],pid_m, derecha,izquierda);
  }
 else
 if (dimen==2)
  {
     /**Displacement ( > 0: upward shift, < 0: downward shift) (integer)**/
     MPI_Cart_shift (c_malla,0,1,&izquierda,&derecha);
     MPI_Cart_shift (c_malla,1,-1,&abajo,&arriba);
     printf ("Proceso %d > pid_mt = %d, coords = (%d, %d) pid_m = %d\n    de_iz_ar_ab(%d %d %d %d)\n",
        pid, pid_malla, coord[0],coord[1], pid_m,derecha,izquierda,arriba,abajo);
  }
 else
 {
     /**Displacement ( > 0: upward shift, < 0: downward shift) (integer)**/
     MPI_Cart_shift (c_malla,0,1,&izquierda,&derecha);
     MPI_Cart_shift (c_malla,1,-1,&abajo,&arriba);
     MPI_Cart_shift (c_malla,2,1,&antes,&despues);
     printf ("Proceso %d > pid_mt = %d, coords = (%d, %d, %d), pid_m = %d\n    de_iz_ar_ab_an_de(%d %d %d %d %d %d)\n",
        pid, pid_malla, coord[0],coord[1],coord[2], pid_m,derecha,izquierda,arriba,abajo,antes,despues);
 }

sleep(2);

//creacion de comunicadores para las filas de la malla-toro
 switch (dimen)
 {
     case 1: dim[0]=1; break;
     case 2: dim[0]=1; dim[1]=0; break;
     case 3: dim[0]=1; dim[1]=0; dim[2]=0; break;
 }
  MPI_Cart_sub (c_malla, dim, &c_fila);
  MPI_Comm_rank (c_fila, &pid_fila);

// prueba de comunicacion: el primer nodo de cada fila envia su pid al resto
  if (pid_fila == 0) pruebaf = pid;
  MPI_Bcast (&pruebaf, 1, MPI_INT, 0, c_fila);

  if (pid == 0) printf ("\n ---- prueba de BC por filas \n");
sleep(1);

if (dimen==1)
  printf ("Proceso %d >  coords = (%d), pid_primero_fila = %d, pid_fila: %d\n",
        pid,  coord[0], pruebaf, pid_fila);
 else
 if (dimen==2)
  printf ("Proceso %d >  coords = (%d, %d), pid_primero_fila = %d, pid_fila: %d\n",
        pid,  coord[0],coord[1], pruebaf, pid_fila);
 else
  printf ("Proceso %d > coords = (%d, %d, %d), pid_primero_fila = %d, pid_fila: %d\n",
        pid,  coord[0],coord[1],coord[2], pruebaf, pid_fila);


sleep(2);
//creacion de comunicadores para las columnas de la malla-toro
 switch (dimen)
 {
     case 1: dim[0]=1; break;
     case 2: dim[0]=0; dim[1]=1; break;
     case 3: dim[0]=0; dim[1]=1; dim[2]=0; break;
 }
  MPI_Cart_sub (c_malla, dim, &c_col);
  MPI_Comm_rank (c_col, &pid_col);

// prueba de comunicacion: el primer nodo de cada fila envia su pid al resto
  if (pid_col == 0) pruebaf = pid;
  MPI_Bcast (&pruebaf, 1, MPI_INT, 0, c_col);

  if (pid == 0) printf ("\n ---- prueba de BC por col \n");
sleep(1);

if (dimen==1)
  printf ("Proceso %d >  coords = (%d), pid_primero_col = %d, pid_col: %d\n",
        pid,  coord[0], pruebaf, pid_col);
 else
 if (dimen==2)
  printf ("Proceso %d >  coords = (%d, %d), pid_primero_col = %d, pid_col: %d\n",
        pid,  coord[0],coord[1], pruebaf, pid_col);
 else
  printf ("Proceso %d > coords = (%d, %d, %d), pid_primero_col = %d, pid_col: %d\n",
        pid,  coord[0],coord[1],coord[2], pruebaf, pid_col);


sleep(2);
if (dimen==3)
{
   dim[0]=1; dim[1]=1; dim[2]=0;
   MPI_Cart_sub (c_malla, dim, &c_plano);
   MPI_Comm_rank (c_plano, &pid_plano);

  if (pid_plano == 0) pruebaf = pid;
  MPI_Bcast (&pruebaf, 1, MPI_INT, 0, c_plano);

  if (pid == 0) printf ("\n ---- prueba de BC por planos \n");
  sleep(1);

  printf ("Proceso %d > coords = (%d, %d, %d), pid_primero_plano = %d, pid_plano: %d\n",
        pid,  coord[0],coord[1],coord[2], pruebaf, pid_plano);
}

  MPI_Finalize ();
  return (0);
}  /* main */
