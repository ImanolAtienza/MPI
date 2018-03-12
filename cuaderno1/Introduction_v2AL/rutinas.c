#include <mpi.h>
#include <stdio.h>

int que_rank_soy (MPI_Comm comm)
{
    int rank_soy;

    MPI_Comm_rank(comm,&rank_soy);
    return (rank_soy);
}


int cuantos_somos (MPI_Comm comm)
{
    int somos;

    MPI_Comm_size (comm, &somos);
    return (somos);
}


 MPI_Group que_group_soy (MPI_Comm comm)
{
    MPI_Group group_soy;

    MPI_Comm_group (comm, &group_soy);
    return (group_soy);
}


