

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
    parameter (MAX_ROWS = 4, MAX_COLS = 5)
    double precision a(MAX_ROWS,MAX_COLS)

    integer myid, numprocs, ierr,root
    integer hagotris
    integer estado
    double precision, dimension (:,:), allocatable :: t_a
    double precision, dimension (:), allocatable :: bvxm,cvxm

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)

    root=0  !!! funciona para 0; para otro habría que cambiar trasppuesta....
    if (myid .eq. root) then
      print *,'Triangular Inferior (0,no ; 1,si)'
      read (*,*),hagotris
    endif

    call MPI_BCAST(hagotris, 1, MPI_INT, root, &
                      MPI_COMM_WORLD, ierr)

    allocate (bvxm(MAX_ROWS),stat=estado)
    if (estado>0)then
          print *,'error allocate bvxm'
    endif
    allocate (cvxm(MAX_COLS),stat=estado)
    if (estado>0)then
          print *,'error allocate bvxm'
    endif


    if (hagotris .eq. 0) then
        if (myid .eq. root)  call Inicializacion_A_b(a,bvxm,MAX_ROWS,MAX_COLS)
        call VxM (a,bvxm,cvxm, myid, numprocs, MAX_ROWS,MAX_COLS,root)
    else
       call Inicializacion_A(a,MAX_ROWS,MAX_COLS)
       allocate (t_a(MAX_ROWS,MAX_COLS),stat=estado)
       if (estado>0)then
          print *,'error allocate t_a'
       endif
       if (myid.eq.0) call Inicializacion_b(bvxm,MAX_ROWS)

       call Triangular_Distri (a,t_a,MAX_ROWS,MAX_COLS)

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
             print '(100F7.0)',matriz(i,:)
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

subroutine Triangular(a,t_a, N,M)
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N,M)::t_a
    integer N,M

    integer i,j
    t_a = 0
    do i = 1,N
       do j = 1,i
              t_a(i,j) = a(i,j)
       enddo
    enddo

end

subroutine Triangular_Distri(a,t_a, N,M)

    use mpi
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(N,M)::t_a
    integer N,M,root

    integer i,j
    integer, dimension (:), allocatable ::  cuantos
    integer, dimension (:), allocatable ::  dist
    INTEGER(KIND=MPI_ADDRESS_KIND)  ::  LB, EXTENT
    integer clon, numprocs, ierr, estado
    integer filas,columnas,desde, reparto, iproc
    integer numcolumproc, wtrapecio, trapecio
    integer status(MPI_STATUS_SIZE), req

    call MPI_Comm_rank( MPI_Comm_world, clon, ierr)
    call MPI_Comm_size( MPI_Comm_world, numprocs, ierr)

    allocate (cuantos(numprocs),stat=estado)
    if (estado>0) then
          print *, 'error en allocate cuantos'
    endif
    allocate (dist(numprocs),stat=estado)
    if (estado>0) then
          print *, 'error en allocate dist'
    endif

    filas = N
    columnas = M
    reparto = floor(dble(columnas)/dble(numprocs))

    if (clon==0) then
        do iproc = 0,numprocs-1
         if (iproc==numprocs-1) then
            numcolumproc = columnas - ((numprocs-1)* reparto)
         else
            numcolumproc = reparto
         endif

         desde = iproc * reparto
         do i=1,numcolumproc
          cuantos (i) = filas - (desde) - (i-1)
          dist(i) = desde*(filas+1)  + (i-1)*(filas+1)
         enddo

         call MPI_Type_indexed (numcolumproc,cuantos,dist, MPI_DOUBLE, wtrapecio, ierr);
         call MPI_Type_create_resized (wtrapecio, lb, extent, trapecio, ierr)
         CALL MPI_TYPE_COMMIT (trapecio, ierr)
         if (iproc==0) then
            call MPI_ISend (a,1,trapecio,&
                        0,0,MPI_COMM_WORLD,req,ierr)
         endif

         CALL MPI_Recv (t_a, 1, trapecio, &
                          iproc,0, MPI_COMM_WORLD, status,ierr)
         call MPI_Type_free (trapecio,ierr)
        enddo
    else
       if (clon==numprocs-1) then
            numcolumproc = columnas - ((numprocs-1)* reparto)
         else
            numcolumproc = reparto
         endif

         desde = clon * reparto
         do i=1,numcolumproc
          cuantos (i) = filas - (desde) - (i-1)
          dist(i) = desde*(filas+1)  + (i-1)*(filas+1)
         enddo
         call MPI_Type_indexed (numcolumproc,cuantos,dist, MPI_DOUBLE, wtrapecio, ierr);
         call MPI_Type_create_resized (wtrapecio, lb, extent, trapecio, ierr)
         CALL MPI_TYPE_COMMIT (trapecio, ierr)

         call MPI_Send (a,1,trapecio,&
                        0,0,MPI_COMM_WORLD,ierr)
         call MPI_Type_free (trapecio,ierr)
    endif

    deallocate (cuantos); deallocate(dist)
end
