/***********************************************************************
   malla2d.c

************************************************************************/

#include <stdio.h>
#include "mpi.h"
#include <math.h>

int EncaMalla (int *encx, int *ency, int left, int rigth, int top, int bottom)
{
          if (*encx>0)
          {
              (*encx)--;
              printf (" send rigth %d \n",rigth); fflush(stdout);
              return rigth;
          }
          else
          if (*encx<0)
          {
              (*encx)++;
              printf (" send left %d \n",left); fflush(stdout);
              return left;
          }
          else
           if (*ency>0)
          {
              (*ency)--;
              printf (" send top %d \n",top); fflush(stdout);
              return top;
          }
          else
          if (*ency<0)
          {
              (*ency)++;
               printf (" send bottom %d \n",bottom); fflush(stdout);
               return bottom;
          }
          else
          {
             printf (" send nodo \n"); fflush(stdout);
             return -1; // ya ha llegado
          }

}

void NodoUnaRuta (int pidnodo, MPI_Comm c_intercom, int desde, int hasta, int n, int datos[n])
{
   int *message = malloc (sizeof(int)*(n+4));
   int i,nprouters;

   MPI_Status status;
   if (pidnodo==desde)
      {
         message[0]=1; //inyectado nodo
         message[1]=hasta; //inyectado nodo
         message[2]=0;
         message[3]=0; // saltos
         message[4:n]=datos[0:n];
         MPI_Send(message,n+4,MPI_INT,pidnodo,0,c_intercom);
         printf ("Proceso %d  pid_nodo = %d, ENVIA a %d el dato %d %d\n",
            pidnodo, pidnodo,hasta,datos[0],datos[1]);
        fflush(stdout);
      }
      if (pidnodo==hasta)
      {
         MPI_Recv(message,n+4,MPI_INT,pidnodo,0,c_intercom,&status);
         datos[0:n]=message[4:n];
         printf ("Proceso %d  pid_nodo = %d, RECIBE de %d el dato %d %d\n",
            pidnodo, pidnodo,desde,datos[0],datos[1]);
         fflush(stdout);

      }
   free(message);
}
void RouterUnaRuta (int pid_m, int pidnodo, MPI_Comm c_intercom,MPI_Comm c_malla2d,
                    int desde, int hasta, int n, int message[n],
                    int left,int rigth,int top,int bottom)
{
   MPI_Status status;
   int enc[2], coordesde[2], coorhasta[2];
   int siguiente,i, nprouters;

   if (pid_m == desde)
        MPI_Recv(message,n,MPI_INT,pid_m,0,c_intercom,&status);
   else
       MPI_Recv(message,n,MPI_INT,MPI_ANY_SOURCE,0,c_malla2d,&status);

   for (;;)
   {
    if (message[0]==1)
    { //inyectado del nodo
        MPI_Cart_coords (c_malla2d, pid_m, 2, coordesde);
        MPI_Cart_coords (c_malla2d, message[1], 2, coorhasta);
        message[1] =  coorhasta [0] - coordesde[0];
        message[2] =  coorhasta [1] - coordesde[1];
        message[0]=2;
        siguiente = EncaMalla(&message[1],&message[2],left,rigth,top,bottom);
        if (siguiente==-1) // ya ha llegado
        {
            MPI_Send(message,n,MPI_INT,pidnodo,0,c_intercom);
            MPI_Comm_size (c_malla2d, &nprouters);
            message[0]=0; // finalizar todos
            for (i=0;i<nprouters;i++)
                if (i!=pid_m)
                  MPI_Send(message,n,MPI_INT,i,0,c_malla2d);
            return;
        }
        else
            MPI_Send (message,n,MPI_INT,siguiente,0,c_malla2d);
    }
    else
    if (message[0]==2)
    {//inyectado por otro router
        siguiente = EncaMalla(&message[1],&message[2],left,rigth,top,bottom);
        if (siguiente==-1) // ya ha llegado
        {
            MPI_Send(message,n,MPI_INT,pidnodo,0,c_intercom);
            MPI_Comm_size (c_malla2d, &nprouters);
            message[0]=0; // finalizar todos
            for (i=0;i<nprouters;i++)
                if (i!=pid_m)
                  MPI_Send(message,n,MPI_INT,i,0,c_malla2d);
            return;
        }
        else
            MPI_Send (message,n,MPI_INT,siguiente,0,c_malla2d);
    }
    if (message[0]==1 || message[0]==2)
    {
      MPI_Recv(message,n,MPI_INT,MPI_ANY_SOURCE,0,c_malla2d,&status); // espero por finalizar
      if (message[0]==0)
        return;
      else // error
        return;
    }
    else // recibido el de finalizar
      return;
   }
}
int main (int argc, char* argv[])
{
  int       pid, pid_malla, pid_m;
  int       pidnodo,color;
  int       nprtot, nprnodos, k;
  int       kdim[2], anillo[2], opt = 1;
  int       coord[2];

  MPI_Comm  c_malla2d;
  MPI_Comm  comm, c_nodos, c_intercom;

  int result;
  int datos[10], message[10+4];
  int left,rigth,top,bottom;
  int desde,hasta;

  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprtot);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);

  if (pid==0)
  {
   printf ("desde hasta\n"); fflush(stdout);
   scanf ("%d %d",&desde,&hasta);
  }

  MPI_Bcast (&desde,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast (&hasta,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Comm_dup (MPI_COMM_WORLD, &comm);

    //MPI_Comm_dup (MPI_COMM_WORLD, &c_routers);

// datos de la topologia
  color = pid%2;
  MPI_Comm_split (comm, color, pid, &c_nodos);
  MPI_Comm_rank (c_nodos, &pidnodo);
  MPI_Comm_size (c_nodos, &nprnodos);

  if (pid%2==0)
  {
      //nodos
      MPI_Intercomm_create(c_nodos,0,MPI_COMM_WORLD,
                           1,99,&c_intercom);
      datos[0:10]=pidnodo+1;
      NodoUnaRuta(pidnodo,c_intercom,desde,hasta,10,&datos[0]);
   }
  else
  {
      // routers
      k = sqrt (nprnodos);
      kdim[0] = kdim[1] = k;	// numero de nodos en cada dimension
      anillo[0] = anillo[1] = 0; 	// anillo (1) o cadena (0) en cada dim.
      // creacion del nuevo comunicador con una malla asociada
      MPI_Cart_create (c_nodos, 2, kdim, anillo, 1, &c_malla2d);
      //coordenadas del procesador en la malla
      MPI_Comm_rank (c_malla2d, &pid_malla);
      MPI_Cart_coords (c_malla2d, pid_malla, 2, coord);
      //a la inversa, partiendo de las coordenadas, devuelve el valor del pid en la malla
      MPI_Cart_rank (c_malla2d, coord, &pid_m);
      MPI_Intercomm_create(c_nodos,0,MPI_COMM_WORLD,
                           0,99,&c_intercom);
      MPI_Cart_shift (c_malla2d,0,1,&left,&rigth);
      MPI_Cart_shift (c_malla2d,1,1,&bottom,&top);

      RouterUnaRuta(pid_m,pidnodo,c_intercom,c_malla2d,desde,hasta,14,message,
                    left,rigth,top,bottom);

  }


  MPI_Finalize ();
  return (0);
}  /* main */
