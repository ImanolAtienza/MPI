subroutine Sworker (b, manager, myid, N, M)
    use mpi
    implicit none

    double precision, intent(IN) , dimension(M):: b
    integer, intent(IN) :: manager,myid, N,M

    integer  filas,columnas, fila

    double precision buffer(M)
    double precision ans
    integer i,ierr
    integer status(MPI_STATUS_SIZE)

    filas = N
    columnas=M

    call MPI_BCAST(b, columnas, MPI_DOUBLE_PRECISION, manager, &
                      MPI_COMM_WORLD, ierr)
    if (myid .le. filas) then
      do
         call MPI_RECV(buffer, columnas, MPI_DOUBLE_PRECISION, &
                       manager, MPI_ANY_TAG, MPI_COMM_WORLD, &
                       status, ierr)
         if (status(MPI_TAG) .eq. 0) exit
         fila = status(MPI_TAG)
         ans = 0.0
         do i = 1,columnas
            ans = ans+buffer(i)*b(i)
         enddo
         call MPI_SEND(ans, 1, MPI_DOUBLE_PRECISION, manager, &
                       fila, MPI_COMM_WORLD, ierr)
      enddo
    endif
end
