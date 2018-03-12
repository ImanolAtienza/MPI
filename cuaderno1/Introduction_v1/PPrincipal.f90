
    program Muchas

     use mpi
     implicit none

     !include "mpif.h"


     integer ierr

     !!MPI_INIT(ierr)
     !! ierr: Integer error return value. 0 on success, non-zero on failure.
     !! This MUST be the first MPI routine called in any program.
     !! Can only be called once
     !! Sets up the environment to enable message passing
     call MPI_INIT(ierr)
     call PasoMensajes_1(MPI_COMM_WORLD)

     !! MPI_FINALIZE(ierr)
     !! ierr: Integer error return value. 0 on success, non-zero on failure.
     !! This routine must be called by each process before it exits
     !! This call cleans up all MPI state
     !! No other MPI routines may be called after MPI_FINALIZE
     !! All pending communication must be completed (locally) before a call to MPI_FINALIZE
     !! Only process with rank 0 is guaranteed to return
     call MPI_FINALIZE(ierr)
    end program Muchas

