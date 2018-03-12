/*************************************************************
	dlock1.c
************************************************************/

#include <mpi.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
    int pid, origen, destino, tag;
    int A, B, C;

    MPI_Status info;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);

	// El orden original es pid0 Recv - Send y en el pid1 Recv - Send..hay que cambiar a pid0 Recv - Send y en el pid1 Send - Recv
	// Tambien sería valid Send - Recv y en el pid1 Send - Recv debido a que el send no es potencialmente bloqueante y no se supera 
	// el tamaño del buffer en los clones, debido solo se envia una variable int
    if (pid==0)
    {
        A = 5;
		// Los tag se han cambiado porque no corresponde el valor del tag de un recv con el tag del send
        origen = 1; tag=1;
        printf("\n    >> recibiendo datos de P%1d \n",origen);
        MPI_Recv (&B, 1,MPI_INT,
                  origen,tag,MPI_COMM_WORLD,
                  &info);
        destino = 1; tag=0;
        printf("\n    >> enviando datos a P%1d \n",destino);
        MPI_Send (&A, 1,MPI_INT,
                  destino,tag,MPI_COMM_WORLD);
        C = A*B;
        printf("\n    C es %d en proc %d\n",C,pid);
    }
    else if(pid==1) // Para que solo entre el clon 1
    {
        B = 6;
		destino = 0; tag=1;
        printf("\n    >> enviando datos a P%1d \n",destino);
        MPI_Send (&B, 1,MPI_INT,
                  destino,tag,MPI_COMM_WORLD);
        origen = 0; tag=0;
        printf("\n    >> recibiendo datos de P%1d \n",origen);
        MPI_Recv (&A, 1,MPI_INT,
                  origen,tag,MPI_COMM_WORLD,
                  &info);
        C = A*B;
        printf("\n    C es %d en proc %d\n",C,pid);
    }

     MPI_Finalize();
     return (0);
}

