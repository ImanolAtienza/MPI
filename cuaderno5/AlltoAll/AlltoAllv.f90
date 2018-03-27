     program main
!
      use mpi
      implicit none

      interface
      subroutine allocate_matriz (matriz, filas,columnas)
          integer, intent(IN) :: filas,columnas
          integer,  dimension (:,:), allocatable, intent(OUT) :: matriz
      end subroutine allocate_matriz
      subroutine imprimir_matriz(myid,texto,matriz)
          integer, intent(IN) :: myid
          character (len=*), intent(IN) :: texto
          integer , allocatable, intent(IN) :: matriz(:,:)
       end subroutine imprimir_matriz
     end interface

      integer filas,columnas,i,j,tipo_fila, tipo_columna, tipo_fila2
      parameter (filas = 11, columnas=11)
      integer myid, numprocs, ierr, status(MPI_STATUS_SIZE)
      integer comm
      integer nfilas,resto,filaini,ngather,estado,donde,desde

      integer, dimension (:,:), allocatable :: matriz, t_matriz
      integer, dimension (:), allocatable ::  vtam,vtamr
      integer, dimension (:), allocatable ::  vdespl,vdesplr
      INTEGER(KIND=MPI_ADDRESS_KIND)    LB, EXTENT


      call MPI_INIT( ierr )
      call MPI_COMM_RANK( MPI_COMM_WORLD, myid, ierr )
      call MPI_COMM_SIZE( MPI_COMM_WORLD, numprocs, ierr )
     ! numservidores,servidor,root,comm
      comm = MPI_COMM_WORLD

      !
     call allocate_matriz(matriz, filas,columnas)
     call allocate_matriz(t_matriz, columnas, filas)
           ! inicializar
     call inicializar (myid,filas, columnas,matriz)
     call imprimir_matriz(myid,"matriz", matriz)

      allocate (vtam(numprocs),stat=estado)
      if (estado>0) then
          print *, 'error en allocate vtam'
      endif
      allocate (vdespl(numprocs),stat=estado)
      if (estado>0) then
          print *, 'error en allocate vdespl'
      endif
     allocate (vtamr(numprocs),stat=estado)
      if (estado>0) then
          print *, 'error en allocate vtam'
      endif
      allocate (vdesplr(numprocs),stat=estado)
      if (estado>0) then
          print *, 'error en allocate vdespl'
      endif

       ngather = floor(dble(columnas)/dble(numprocs)) * filas
        do i = 1, numprocs
        if (myid /= numprocs-1) then
         vtam (i) = ngather
         vdespl(i) = (myid)*ngather
        else
         vtam (i) = (columnas*filas) - ((numprocs-1)* ngather)
         vdespl(i) = (myid)*ngather
        end if
        if (i < numprocs) then
         vtamr (i) = ngather
         vdesplr(i) = (i-1)*ngather
        else
         vtamr (numprocs) = (columnas*filas) - ((numprocs-1)* ngather)
         vdesplr(numprocs) = (numprocs-1)*ngather
        end if

       end do
       CALL MPI_AlltoAllv (matriz, vtam, vdespl,MPI_INT, &
                          t_matriz, vtamr, vdesplr, MPI_INT, &
                          MPI_COMM_WORLD, ierr)
       if (myid .eq. 0) then
         call imprimir_matriz(0, 't_matriz p0',t_matriz)
       endif
       if (myid .eq. numprocs-1) then
         call imprimir_matriz(0, 't_matriz numprocs-1',t_matriz)
       endif
!
       deallocate (matriz)
       deallocate (t_matriz)
       deallocate (vtam)
       deallocate (vtamr)
       deallocate (vdespl)
       deallocate (vdesplr)
       call MPI_FINALIZE(ierr)

 end
