/*************************************************************
	send-dead.c
************************************************************/

#include <mpi.h>
#include <stdio.h>
#define N 140000

int main (int argc, char *argv[])
{
    int pid, kont;
    int a[N], b[N], c[N], d[N];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);

    a[0:N]=1;
    c[0:N]=2;

    for (kont=100; kont<=N; kont+=100)
    {
        if (pid==0)
        {
            MPI_Send (&a[0], kont,MPI_INT,
                      1,0,MPI_COMM_WORLD);
            MPI_Recv (&b[0], kont,MPI_INT,
                      1,0,MPI_COMM_WORLD,
                      &status);
            printf(" emisor %d dat %d\n",kont, b[0]);
        }
        else if(pid==1)
        {
            MPI_Send (&c[0], kont,MPI_INT,
                      0,0,MPI_COMM_WORLD);
            MPI_Recv (&d[0], kont,MPI_INT,
                      0,0,MPI_COMM_WORLD,
                      &status);
           printf("            receptor %d dat %d\n",kont, d[0]);
        }
    }

     MPI_Finalize();
     return (0);
}

