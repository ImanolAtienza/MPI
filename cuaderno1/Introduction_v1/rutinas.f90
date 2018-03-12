function que_rank_soy (comm)
    use mpi
    implicit none
    integer, intent(IN):: comm
    integer que_rank_soy, ierr

    !! MPI_Comm_rank(comm,rank,ierr)
    !! comm: Communicator (integer in Fortran, MPI_Comm in C)
    !! rank: Returned rank of calling process --> Between 0 and n-1 with n processes
    !! ierr: Integer error return code
    !! Returns the rank of the calling process within the group associated with comm
    call MPI_Comm_rank(comm,que_rank_soy,ierr)

endfunction que_rank_soy

function cuantos_somos (comm)
    use mpi
    implicit none
    integer, intent(IN):: comm
    integer cuantos_somos, ierr

    !! MPI_Comm_size (comm, size, ierr)
    !! Comm: Communicator handle
    !! Size: Upon return, the number of processes in the group associated with comm
    call MPI_Comm_size (comm, cuantos_somos, ierr)

endfunction cuantos_somos

 function que_group_soy (comm)
    use mpi
    implicit none
    integer, intent(IN):: comm
    integer que_group_soy, ierr

    !! MPI_Comm_group (comm, group, ierr)
    !! This routine returns in group the group associated with the communicator comm
    call MPI_Comm_group (comm, que_group_soy, ierr)

endfunction que_group_soy


