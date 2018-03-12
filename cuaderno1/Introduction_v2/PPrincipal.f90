
    program Muchas

     use mpi
     implicit none

     !include "mpif.h"

     integer ierr

     call MPI_INIT(ierr)
     call PasoMensajes_2(MPI_COMM_WORLD)

     call MPI_FINALIZE(ierr)
    end program Muchas

