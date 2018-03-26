    program Cadena

     use mpi
     implicit none

     integer myid,numprocs, ierr

     call MPI_INIT(ierr)
     call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
     call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)

     call cadena_v1(MPI_COMM_WORLD)

     call MPI_FINALIZE(ierr)

    end program Cadena

