#include <mpi.h>
#include <stdio.h>

void PasoMensajes_1 (MPI_Comm comm)
{
    int rank, numrank;
    int tiporank;
    int lnom;
    char  nombrepr[MPI_MAX_PROCESSOR_NAME];

    int rankdestino, rankfuente;
    int bufferenvio[20],bufferrecibo[20];

    MPI_Status  status;


    rank = que_rank_soy(comm);
    numrank = cuantos_somos(comm);

    bufferenvio[0] = rank; bufferenvio[1]=numrank;
    //rankdestino = modulo(rank+1,numrank)
    rankdestino = (rank+1) % numrank;
    //tiporank = modulo (rank,2)
    tiporank = rank%2;
    rankfuente = rank - 1;
    if (rankfuente<0) rankfuente=numrank-1;

    MPI_Get_processor_name (nombrepr, &lnom);
    if (tiporank == 0)   // para evitar deadlock pongo el if
    {
      // rank pares
      MPI_Send (bufferenvio,2,MPI_INT,
                     rankdestino,0,comm);

      MPI_Recv(bufferrecibo,2,MPI_INT,
                  rankfuente,MPI_ANY_TAG,comm,
                  &status);

      printf(" rank %d proc %s recibido %d %d\n",rank,nombrepr,
             bufferrecibo[0],bufferrecibo[1]);fflush(stdout);
    }

   else
   {
      // rank impares
      MPI_Recv(bufferrecibo,2,MPI_INT,
                  rankfuente,MPI_ANY_TAG,comm,
                  &status);

       printf(" rank %d proc %s recibido %d %d\n",rank,nombrepr,
             bufferrecibo[0],bufferrecibo[1]);fflush(stdout);
      MPI_Send (bufferenvio,2,MPI_INT,
                     rankdestino,0,comm);
   }

    return;
}

