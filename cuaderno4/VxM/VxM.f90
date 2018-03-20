subroutine VxM (a,b,c, myid, numprocs, N,M)
    use mpi
    implicit none

    !!!!!!
    ! es VxM
    !!!!!!

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
    !     manager initializes and then dispatches
    !     initialize a and b  (arbitrary)
          do i = 1,filas
            b(i) = 1
            do j = 1,columnas
             a(i,j) = j
            enddo
         enddo
       endif

       size = floor(dble(columnas)/dble(numprocs))
       !print *,size

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
       if (myid==0) then
        print *, counts
        print *, despl
       endif
       !
     !call SYSTEM('pause')

       !     send b to each worker process
       call MPI_BCAST(b, filas, MPI_DOUBLE_PRECISION, 0, &
                      MPI_COMM_WORLD, ierr)
       !  send size rows to each worker process

       !! MPI_SCATTERV (sendbuf, scnts, displs, sendtype,recvbuf, recvcount, recvtype, root,comm,ierr)
        ! Send arguments only meaningful at root
        ! Root can use MPI_IN_PLACE for recvbuf
        ! No location of the sendbuf can be read more than once

        call MPI_Scatterv (a(1,1),counts,despl,MPI_DOUBLE_PRECISION,&
                           wa(1,1),counts(myid+1),MPI_DOUBLE_PRECISION,&
                           0,MPI_COMM_WORLD, ierr)
       !call SYSTEM('pause')

        !desde = despl(myid+1)+1
        !hasta = desde + counts(myid+1)
        hasta = counts(myid+1)/filas
        do j = 1 , hasta
         ans = 0.0d0
         do i = 1,filas
            ans = ans+b(i)*wa(i,j)
         enddo
         wc(j) = ans
        enddo
       !! MPI_GATHERV (sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm,ierr)
        ! Vector variant of MPI_GATHER
        ! Allows a varying amount of data from each proc
        ! allows root to specify where data from each proc goes
        ! No portion of the receive buffer may be written more than once
        ! The amount of data specified to be received at the root must match amount of data sent by non-roots
        ! Displacements are in terms of recvtype
        ! MPI_IN_PLACE may be used by root.

       counts(1) = trozoprimero
       despl(1)=0
       counts(2:numprocs) = size

       do i = 2,numprocs
         despl(i) = despl(i-1) + counts(i-1)
       enddo
       if (myid==0) then
        print *, counts
        print *, despl
       endif
       !print *,myid,wc(1:hasta)
       !call SYSTEM('pause')
       call MPI_Gatherv (wc,counts(myid+1),MPI_DOUBLE_PRECISION,&
                         c,counts,despl,MPI_DOUBLE_PRECISION,&
                         0,MPI_COMM_WORLD, ierr)
        !call SYSTEM('pause')
       return
 end
