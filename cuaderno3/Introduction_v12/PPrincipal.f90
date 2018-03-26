
    program Muchas

     use mpi
     implicit none

     !include "mpif.h"

     integer ierr
!ejemplo1
     integer, parameter :: N = 1000
     integer vector(N)

     call MPI_INIT(ierr)
 !ejemplo1
      vector = 2
      call ejemplo2_v2(MPI_COMM_WORLD,N,3)

     call MPI_FINALIZE(ierr)
    end program Muchas

