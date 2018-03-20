

    program pVxM
    use mpi
    implicit none

    interface
      subroutine imprimir_matriz(myid,texto,matriz)
          integer, intent(IN) :: myid
          character (len=*), intent(IN) :: texto
          double precision , intent(IN) :: matriz(:,:)
       end subroutine imprimir_matriz
     end interface



    integer MAX_ROWS, MAX_COLS
    parameter (MAX_ROWS = 10, MAX_COLS = 9)
    double precision a(MAX_ROWS,MAX_COLS)

    integer myid, numprocs, ierr,root
    integer hagosub
    integer estado
    double precision, dimension (:,:), allocatable :: t_a
    double precision, dimension (:), allocatable :: bvxm,cvxm

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)

    root=0  !!! funciona para 0; para otro habría que cambiar trasppuesta....
    if (myid .eq. root) then
      print *,'Subarray (0,no ; 1,si)'
      read (*,*),hagosub
    endif

    call MPI_BCAST(hagosub, 1, MPI_INT, root, &
                      MPI_COMM_WORLD, ierr)

    allocate (bvxm(MAX_ROWS),stat=estado)
    if (estado>0)then
          print *,'error allocate bvxm'
    endif
    allocate (cvxm(MAX_COLS),stat=estado)
    if (estado>0)then
          print *,'error allocate bvxm'
    endif


    if (hagosub .eq. 0) then
        if (myid .eq. root)  call Inicializacion_A_b(a,bvxm,MAX_ROWS,MAX_COLS)
        call VxM (a,bvxm,cvxm, myid, numprocs, MAX_ROWS,MAX_COLS,root)
    else
       call Inicializacion_A(a,MAX_ROWS,MAX_COLS)
       allocate (t_a(MAX_ROWS,MAX_COLS),stat=estado)
       if (estado>0)then
          print *,'error allocate t_a'
       endif

       if (myid.eq.0) call Inicializacion_b(bvxm,MAX_ROWS)

       call Subarray_Distri (a,t_a,MAX_ROWS,MAX_COLS)

       if (myid .eq. 0) call imprimir_matriz (myid,"matriz ",a)
       if (myid .eq. 0) call imprimir_matriz (myid,"matriz triangular",t_a)

       call VxM (t_a,bvxm,cvxm, myid, numprocs, MAX_ROWS, MAX_COLS, root)

       deallocate (t_a)
    endif


    if (myid .eq. root) then
       print *, cvxm(1:MAX_COLS)
    endif
    deallocate (bvxm); deallocate (cvxm)
    call MPI_FINALIZE(ierr)


end program pVxM

subroutine imprimir_matriz (myid,texto,matriz)

    implicit none
    integer, intent(IN) :: myid
    double precision,  intent(IN) :: matriz(:,:)
    character (len=*), intent(IN) :: texto

    integer i
    print *,texto
    do i = 1, size(matriz,1)
             print '(100F4.0)',matriz(i,:)
    enddo


end subroutine



subroutine Inicializacion_A_b (a,b,N,M)

    implicit none

    double precision, intent(OUT) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N):: b
    integer N,M

    integer i,j

    do i = 1,N
       b(i) = 1
       do j = 1,M
              a(i,j) = j
       enddo
    enddo
end
subroutine Inicializacion_A (a,N,M)

    implicit none

    double precision, intent(OUT) , dimension(N,M)::a
    integer N,M

    integer i,j

    do i = 1,N
       do j = 1,M
              a(i,j) = j
       enddo
    enddo
end

subroutine Inicializacion_B (b,N)
    implicit none

    double precision, intent(OUT) , dimension(N):: b
    integer N

    integer i

    do i = 1,N
       b(i) = 1
    enddo
end


subroutine Subarray_Distri(a,t_a, N,M)

    use mpi
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N,M)::t_a
    integer N,M,root

    integer i,j, myid, numprocs, ierr, iproc,sarray,numfilasproc,numcolumproc
    integer repartofil, repartocol, filas,columnas, desdefil, desdecol
    integer :: sizes (2), subsizes(2), starts(2)
    integer status(MPI_STATUS_SIZE), req

     call MPI_COMM_RANK( MPI_COMM_WORLD, myid, ierr )
     call MPI_COMM_SIZE( MPI_COMM_WORLD, numprocs, ierr )

     t_a = 0
     filas = N
     columnas = M
     repartocol = floor(dble(columnas)/dble(numprocs))
     repartofil = floor(dble(filas)/dble(numprocs))

     if (myid==0) then
        do iproc = 0,numprocs-1
         if (iproc==numprocs-1) then
            numcolumproc = columnas - ((numprocs-1)* repartocol)
            numfilasproc = filas - ((numprocs-1)* repartofil)
         else
            numcolumproc = repartocol
            numfilasproc = repartofil
         endif

         desdecol = iproc * repartocol; desdefil = iproc * repartofil
         sizes = [filas, columnas]
         subsizes = [numfilasproc,numcolumproc]
         starts = [desdefil, desdecol]
         call MPI_TYPE_CREATE_SUBARRAY (2, sizes, subsizes, starts, &
                                        MPI_ORDER_FORTRAN, MPI_DOUBLE, sarray,ierr)
         CALL MPI_TYPE_COMMIT (sarray, ierr)
         if (iproc==0) then
            call MPI_ISend (a,1,sarray,&
                        0,0,MPI_COMM_WORLD,req,ierr)
         endif

         CALL MPI_Recv (t_a, 1, sarray, &
                          iproc,0, MPI_COMM_WORLD, status,ierr)
         call MPI_Type_free (sarray,ierr)
        enddo
    else
       if (myid==numprocs-1) then
            numcolumproc = columnas - ((numprocs-1)* repartocol)
            numfilasproc = filas - ((numprocs-1)* repartofil)
          else
            numcolumproc = repartocol
            numfilasproc = repartofil
       endif

         desdecol = myid * repartocol; desdefil = myid * repartofil
         sizes = [filas, columnas]
         subsizes = [numfilasproc,numcolumproc]
         starts = [desdefil, desdecol]
         call MPI_TYPE_CREATE_SUBARRAY (2, sizes, subsizes, starts, &
                                        MPI_ORDER_FORTRAN, MPI_DOUBLE, sarray,ierr)
         CALL MPI_TYPE_COMMIT (sarray, ierr)
         call MPI_Send (a,1,sarray,&
                        0,0,MPI_COMM_WORLD,ierr)
         call MPI_Type_free (sarray,ierr)
    endif
end
