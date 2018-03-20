subroutine VxM (a,b,c, myid, numprocs, N,M)
    use mpi
    implicit none

    double precision, intent(OUT) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N):: b
    double precision, intent(OUT), dimension(M) ::c
    integer numprocs, myid
    integer N,M

    integer  filas,columnas

    double precision, allocatable :: wa(:,:)
    double precision ans
    integer numsent,i,j, sender, anstype,ierr
    integer status(MPI_STATUS_SIZE), size, desde, hasta, elementos, trozoprimero
    integer counts (numprocs), despl(numprocs)
    double precision wc(M)

       filas=N
       columnas=M
       if (myid .eq. 0) then
           do i = 1,filas
            b(i) = 1
            do j = 1,columnas
             a(i,j) = j
            enddo
         enddo
       endif

       size = floor(dble(columnas)/dble(numprocs))
       desde = (numprocs-1)*size
       hasta = columnas
       trozoprimero =hasta-desde
       allocate (wa(filas,max(size,trozoprimero)))
       counts(1) = trozoprimero * filas
       despl(1)=0
       counts(2:numprocs) = size * filas
       do i = 2,numprocs
         despl(i) = despl(i-1) + counts(i-1)
       enddo
        call MPI_BCAST(b, filas, MPI_DOUBLE_PRECISION, 0, &
                      MPI_COMM_WORLD, ierr)

       call MPI_Scatterv (a(1,1),counts,despl,MPI_DOUBLE_PRECISION,&
                           wa(1,1),counts(myid+1),MPI_DOUBLE_PRECISION,&
                           0,MPI_COMM_WORLD, ierr)
        hasta = counts(myid+1)/filas
        do j = 1 , hasta
         ans = 0.0d0
         do i = 1,filas
            ans = ans+b(i)*wa(i,j)
         enddo
         wc(j) = ans
        enddo

       counts(1) = trozoprimero
       despl(1)=0
       counts(2:numprocs) = size
       do i = 2,numprocs
         despl(i) = despl(i-1) + counts(i-1)
       enddo
        call MPI_Gatherv (wc,counts(myid+1),MPI_DOUBLE_PRECISION,&
                         c,counts,despl,MPI_DOUBLE_PRECISION,&
                         0,MPI_COMM_WORLD, ierr)
       return
 end
