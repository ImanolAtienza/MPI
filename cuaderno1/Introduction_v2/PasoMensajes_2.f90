subroutine PasoMensajes_2 (comm)
    use mpi
    implicit none
    integer, intent(IN) :: comm
    include "definiciones.txt"

    integer rank, numrank, status(MPI_STATUS_SIZE), ierr
    integer tiporank

    integer rankdestino, rankfuente
    integer bufferenvio(20),bufferrecibo(20)
    integer cuantosrecibidos, recibidosource, recibidotag


    rank = que_rank_soy(comm)
    numrank = cuantos_somos(comm)

    rankdestino = modulo(rank+1,numrank)
    tiporank = modulo (rank,2)
    rankfuente = rank - 1
    if (rankfuente<0) rankfuente=numrank-1


    bufferenvio(1:rank+1) = rank

    if (tiporank == 0) then  !! para evitar deadlock pongo el if
     ! rank pares
       call MPI_Send (bufferenvio,rank+1,MPI_INT,&
                     rankdestino,0,comm,&
                     ierr)

      call MPI_Recv(bufferrecibo,20,MPI_INT,& ! maximo=20
                  rankfuente,MPI_ANY_TAG,comm,&
                  status,ierr)
      recibidosource = status (MPI_SOURCE)
      recibidotag = status(MPI_TAG)
      !! MPI_Get_count (status, datatype, count, ierr)
      !!      This routine returns in count the actual number of elements received by a call that returned status
      call MPI_Get_count (status, MPI_INT, cuantosrecibidos,ierr)
   else
      ! rank impares
      call MPI_Recv(bufferrecibo,20,MPI_INT,& !maximo=20
                  rankfuente,MPI_ANY_TAG,comm,&
                  status,ierr)
      recibidosource = status (MPI_SOURCE)
      recibidotag = status(MPI_TAG)
      call MPI_Get_count (status, MPI_INT, cuantosrecibidos,ierr)

      call MPI_Send (bufferenvio,rank+1,MPI_INT,&
                     rankdestino,0,comm,&
                     ierr)
    endif
    !
    print '(A, I2, A, 2I2, A, 20I2 )', ' rank ',rank, ' recibido source tag ',recibidosource,recibidotag,&
                               " buf",bufferrecibo(1:cuantosrecibidos)

     return
endsubroutine PasoMensajes_2

