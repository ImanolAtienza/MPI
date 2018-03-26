subroutine Smanager (a,b,c, manager, numprocs, N,M)
    use mpi
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(IN) , dimension(M):: b
    double precision, intent(OUT), dimension(N) ::c
    integer manager,numprocs
    integer N,M
    integer  filas,columnas
    double precision buffer(M)
    double precision ans, suma
    integer numsent,i,j, sender, anstype,ierr
    integer status(MPI_STATUS_SIZE)

    filas=N
    columnas=M
    numsent = 0
    call MPI_BCAST(b, columnas, MPI_DOUBLE_PRECISION, manager, &
                   MPI_COMM_WORLD, ierr)
    do i = 1,min(numprocs-1,filas)
       buffer(1:columnas)=a(i,:)
       call MPI_SEND(buffer, columnas, MPI_DOUBLE_PRECISION, i, &
                     i, MPI_COMM_WORLD, ierr)
       numsent = numsent+1
    enddo
    do i = 1,filas
       call MPI_RECV(ans, 1, MPI_DOUBLE_PRECISION, &
                     MPI_ANY_SOURCE, MPI_ANY_TAG, &
                     MPI_COMM_WORLD, status, ierr)
       sender     = status(MPI_SOURCE)
       anstype    = status(MPI_TAG)       ! row is tag value
       c(anstype) = ans
       if (numsent .lt. filas) then        ! send another row
          buffer(1:columnas) = a(numsent+1,:)
          call MPI_SEND(buffer, columnas, MPI_DOUBLE_PRECISION, &
                        sender, numsent+1, MPI_COMM_WORLD, ierr)
          numsent = numsent+1
       else      ! Tell sender that there is no more work
          call MPI_SEND(MPI_BOTTOM, 0, MPI_DOUBLE_PRECISION, &
               sender, 0, MPI_COMM_WORLD, ierr)
       endif
    enddo
    suma=0
    do i = 1,filas
        suma = suma + c(i)
    end do
    print *,'Suma vector c ',suma
    return
 end
