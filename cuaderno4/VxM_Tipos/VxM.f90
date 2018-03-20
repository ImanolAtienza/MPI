subroutine VxM (a,b,c, myid, numprocs, N,M,root)
    use mpi
    implicit none

    !!!!!!
    ! es VxM
    !!!!!!

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N):: b
    double precision, intent(OUT), dimension(M) ::c
    integer numprocs, myid,root
    integer N,M

    integer  filas,columnas

    double precision, allocatable :: wa(:,:)
    double precision ans
    integer numsent,i,j, sender, anstype,ierr
    integer status(MPI_STATUS_SIZE), size, desde, hasta, elementos, trozoprimero
    integer counts (numprocs), despl(numprocs)
    double precision wc(M)
    integer tipo_vector


      filas=N
      columnas=M

 !! reparto

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

       call MPI_Type_Contiguous (N,MPI_DOUBLE_PRECISION,tipo_vector,ierr)
       call MPI_Type_Commit (tipo_vector,ierr)

       call MPI_BCAST(b, 1, tipo_vector, root, &
                      MPI_COMM_WORLD, ierr)

       call MPI_Scatterv (a(1,1),counts,despl,MPI_DOUBLE_PRECISION,&
                           wa(1,1),counts(myid+1),MPI_DOUBLE_PRECISION,&
                           root,MPI_COMM_WORLD, ierr)

  !! trabajo local

        hasta = counts(myid+1)/filas
        do j = 1 , hasta
         ans = 0.0d0
         do i = 1,filas
            ans = ans+b(i)*wa(i,j)
         enddo
         wc(j) = ans
        enddo

  !!  envio tabajo local  (reduccion)

       counts(1) = trozoprimero
       despl(1)=0
       counts(2:numprocs) = size

       do i = 2,numprocs
         despl(i) = despl(i-1) + counts(i-1)
       enddo

       call MPI_Gatherv (wc,counts(myid+1),MPI_DOUBLE_PRECISION,&
                         c,counts,despl,MPI_DOUBLE_PRECISION,&
                         root,MPI_COMM_WORLD, ierr)

       call MPI_Type_free(tipo_vector, ierr)
       return
 end

