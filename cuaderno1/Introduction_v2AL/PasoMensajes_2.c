#include <mpi.h>
#include <stdio.h>
#include <string.h>

void PasoMensajes_2 (MPI_Comm comm)
 {
    int i;
    int rank, numrank;
    MPI_Status status;
    int tiporank;

    int rankdestino, rankfuente;
    int bufferenvio[20],bufferrecibo[20];
    int cuantosrecibidos, recibidosource, recibidotag;
    char texto [500],chnum[10];
    int cnt,cnt1;

    rank = que_rank_soy(comm);
    numrank = cuantos_somos(comm);

    rankdestino = (rank+1) % numrank;

    tiporank = rank%2;
    rankfuente = rank - 1;
    if (rankfuente<0) rankfuente=numrank-1;

    // bufferenvio(1:rank+1) = rank
	bufferenvio[0:rank+1:1] = rank;

    if (tiporank == 0)   // para evitar deadlock pongo el if
    {
     // rank pares
      MPI_Send (bufferenvio,rank+1,MPI_INT,
                     rankdestino,0,comm);

      MPI_Recv(bufferrecibo,20,MPI_INT, // maximo=20
                  rankfuente,MPI_ANY_TAG,comm,
                  &status);
      //recibidosource = status (MPI_SOURCE)
      //recibidotag = status(MPI_TAG)
      recibidosource = status.MPI_SOURCE;
      recibidotag = status.MPI_TAG;
      MPI_Get_count (&status, MPI_INT, &cuantosrecibidos);
    }

   else
   {
    // rank impares
      MPI_Recv(bufferrecibo,20,MPI_INT, // maximo=20
                  rankfuente,MPI_ANY_TAG,comm,
                  &status);
      //recibidosource = status (MPI_SOURCE)
      //recibidotag = status(MPI_TAG)
      recibidosource = status.MPI_SOURCE;
      recibidotag = status.MPI_TAG;
      MPI_Get_count (&status, MPI_INT, &cuantosrecibidos);
      MPI_Send (bufferenvio,rank+1,MPI_INT,
                     rankdestino,0,comm);
   }

 //   print '(A, I2, A, 2I2, A, 20I2 )', ' rank ',rank, ' recibido source tag ',recibidosource,recibidotag,&
//                            " buf",bufferrecibo(1:cuantosrecibidos)

     sprintf(texto," rank %d recibido source tag %d %d buf ",rank,recibidosource,recibidotag);

     for (i=0;i<cuantosrecibidos;i++)
     {
       sprintf (chnum,"%2d ",bufferrecibo[i]);
       strcat(texto,chnum);
     }
     printf ("%s\n",texto);
    return;
 }





