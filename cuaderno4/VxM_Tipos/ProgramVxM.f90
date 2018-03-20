

    program pVxM
    use mpi
    implicit none

    integer MAX_ROWS, MAX_COLS
    parameter (MAX_ROWS = 10, MAX_COLS = 9)
    double precision a(MAX_ROWS,MAX_COLS)

    integer myid, numprocs, ierr,root
    integer hagotras
    integer estado
    double precision, dimension (:,:), allocatable :: t_a
    double precision, dimension (:), allocatable :: bvxm,cvxm

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)

    root=0  !!! funciona para 0; para otro habría que cambiar trasppuesta....
    if (myid .eq. root) then
      print *,'traspuesta (0,no ; 1,si)'
      read (*,*),hagotras
    endif

    call MPI_BCAST(hagotras, 1, MPI_INT, root, &
                      MPI_COMM_WORLD, ierr)

    if (hagotras .eq. 0) then
       allocate (bvxm(MAX_ROWS),stat=estado)
       if (estado>0)then
          print *,'error allocate bvxm'
       endif
      allocate (cvxm(MAX_COLS),stat=estado)
       if (estado>0)then
          print *,'error allocate bvxm'
       endif
        if (myid .eq. root)  call Inicializacion_A_b(a,bvxm,MAX_ROWS,MAX_COLS)
        call VxM (a,bvxm,cvxm, myid, numprocs, MAX_ROWS,MAX_COLS,root)
    else
       call Inicializacion_A(a,MAX_ROWS,MAX_COLS)
       allocate (t_a(MAX_COLS,MAX_ROWS),stat=estado)
       if (estado>0)then
          print *,'error allocate t_a'
       endif
       allocate (bvxm(MAX_COLS),stat=estado)
       if (estado>0)then
          print *,'error allocate bvxm'
       endif
       allocate (cvxm(MAX_ROWS),stat=estado)
       if (estado>0)then
          print *,'error allocate bvxm'
       endif
       if (myid.eq.0) call Inicializacion_b(bvxm,MAX_COLS)
        !! llamar a traspuesta
       !call Traspuesta (a,t_a,MAX_ROWS,MAX_COLS)
       call Traspuesta_Distri (a,t_a,MAX_ROWS,MAX_COLS)

       call VxM (t_a,bvxm,cvxm, myid, numprocs, MAX_COLS, MAX_ROWS, root)

       deallocate (t_a)
    endif


    if (myid .eq. root) then
     if (hagotras .eq. 0) then
       print *, cvxm(1:MAX_COLS)
     else
       print *, cvxm(1:MAX_ROWS)
     endif
    endif
    deallocate (bvxm); deallocate (cvxm)
    call MPI_FINALIZE(ierr)


   end program pVxM

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

subroutine Traspuesta(a,t_a, N,M)
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(M,N)::t_a
    integer N,M

    integer i,j

    do i = 1,N
       do j = 1,M
              t_a(j,i) = a(i,j)
       enddo
    enddo

end

subroutine Traspuesta_Distri(a,t_a, N,M)

    use mpi
    implicit none

    double precision, intent(IN) , dimension(N,M)::a
    double precision, intent(OUT) , dimension(M,N)::t_a
    integer N,M,root

    integer i,j
    integer, dimension (:), allocatable ::  vtam
    integer, dimension (:), allocatable ::  vdespl
    INTEGER(KIND=MPI_ADDRESS_KIND)  ::  LB, EXTENT
    integer clon, numprocs, ierr
    integer tipo_columna, tipo_fila, tipo_fila2
    integer filas,columnas,nfilas,resto,filaini,ngather,estado,donde,desde


    call MPI_Comm_rank( MPI_Comm_world, clon, ierr)
    call MPI_Comm_size( MPI_Comm_world, numprocs, ierr)
    allocate (vtam(numprocs),stat=estado)
    if (estado>0) then
          print *, 'error en allocate vtam'
    endif
    allocate (vdespl(numprocs),stat=estado)
    if (estado>0) then
          print *, 'error en allocate vdespl'
    endif

    filas = N
    columnas = M

    !! tipo vector columnas para mover a t_a (:j)
    call MPI_TYPE_CONTIGUOUS (columnas, MPI_DOUBLE, tipo_columna, ierr)
    CALL MPI_TYPE_COMMIT (tipo_columna, ierr)

    ! Reparto

    ngather = floor(dble(filas)/dble(numprocs))
    do i = 1, numprocs-1
         vtam (i) = ngather
         vdespl(i) = (i-1)*ngather
    end do
    vtam (numprocs) = filas - ((numprocs-1)* ngather)
    vdespl(numprocs) = (numprocs-1)*ngather
    desde = vdespl(clon+1)+1
    ngather = vtam(clon+1)

    call MPI_TYPE_VECTOR (columnas, 1, filas, MPI_DOUBLE, tipo_fila, ierr)
    call MPI_TYPE_GET_EXTENT(MPI_DOUBLE, lb, extent, ierr)
    call MPI_Type_create_resized (tipo_fila, lb, extent, tipo_fila2, ierr)
    CALL MPI_TYPE_COMMIT (tipo_fila2, ierr)

    !! se hace la traspuesta
    CALL MPI_Gatherv (a(desde,1), ngather, tipo_fila2, &
                          t_a, vtam, vdespl, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)

    deallocate (vtam); deallocate(vdespl)
end
