   subroutine allocate_matriz (matriz, filas,columnas)
    implicit none
    integer, intent(IN) :: filas,columnas
    integer,  dimension (:,:), allocatable, intent(OUT) :: matriz
    integer i,estado

    allocate (matriz(filas,columnas),stat=estado)
      if (estado>0) then
          print *, 'error en allocate matriz'
      endif
    end subroutine

    subroutine inicializar (myid,filas, columnas,matriz)
    use mpi
    implicit none

    integer , intent(IN) :: myid,filas,columnas
    integer , intent(OUT) :: matriz(filas,columnas)
    integer i,j,ierr

         ! inicializar
     !if (myid.eq.0) then
      do i = 1,filas
       do j = 1,columnas
          matriz(i,j) = 2*i+j
       enddo
      enddo
     !endif

    end subroutine

    subroutine imprimir_matriz (myid,texto,matriz)
    use mpi
    implicit none
    integer, intent(IN) :: myid
    integer , allocatable, intent(IN) :: matriz(:,:)
    character (len=*), intent(IN) :: texto
    integer i
     if (myid .eq. 0) then
          print *,texto
          do i = 1, size(matriz,1)
             print '(100I3)',matriz(i,:)
          enddo
    endif

    end subroutine

