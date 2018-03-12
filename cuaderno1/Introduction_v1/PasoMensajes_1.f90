subroutine PasoMensajes_1 (comm)
    use mpi
    implicit none
    integer, intent(IN) :: comm
    include "definiciones.txt"

    integer rank, numrank, status(MPI_STATUS_SIZE), ierr
    integer tiporank

    integer rankdestino, rankfuente
    integer bufferenvio(20),bufferrecibo(20)


    !! Message passing is the transfer of data from one process to another
    !! This transfer requires cooperation of the sender and the receiver, but is initiated by the sender
    !! There must be a way to “describe” the data
    !! There must be a way to identify specific processes
    !! There must be a way to identify kinds of messages

    !! Data is described by a triple
    !!     1. Address: Where is the data stored
    !!     2. Datatype: What is the type of the data
    !!       Basic types (integers, reals, etc)
    !!       Derived types (good for non-contiguous data access)
    !!     3. Count: How many elements make up the message
    !! Processes are specified by a double
    !!     1. Communicator:  MPI_COMM_WORLD...
    !!     2. Rank: The relative rank of the specified process within the group associated with the communicator
    !! Messages are identified by a single tag
    !!     This can be used to differentiate between different kinds of messages

    !! Point-to-Point Communication
     ! Standard Mode:
     !    blocking:

     ! Generally ONLY use if you cannot call earlier AND there is no other work that can be done!
     ! Standard ONLY states that buffers can be used once calls return.
     ! It is implementation dependant on when blocking calls return.
     ! Blocking sends MAY block until a matching receive is posted. This is not required behavior,
     !  but the standard does not prohibit this behavior either.
     ! Further, a blocking send may have to wait for system resources such as system managed message buffers.
     ! Be VERY careful of deadlock when using blocking calls!


    !! MPI_Send (buf,cnt,dtype,dest,tag,comm,ierr)
    !!     buf: The address of the beginning of the data to be sent
    !!     cnt: The number of elements to be sent
    !!     dtype: datatype of each element
    !!     dest: The rank of the destination
    !!     tag: Integer message tag 0 >= tag >= MPI_TAG_UB (at least 32767)
    !!     comm: The communicator
    rank = que_rank_soy(comm)
    numrank = cuantos_somos(comm)
    bufferenvio(1) = rank; bufferenvio(2)=numrank
    rankdestino = modulo(rank+1,numrank)
    tiporank = modulo (rank,2)
    rankfuente = rank - 1
    if (rankfuente<0) rankfuente=numrank-1

    !print *,rank, 'r-e',rankfuente, rankdestino, 't-d', tiporank, bufferenvio(1:2)
    if (tiporank == 0) then  !! para evitar deadlock pongo el if
      ! rank pares
      call MPI_Send (bufferenvio,2,MPI_INT,&
                     rankdestino,0,comm,&
                     ierr)
    !! MPI_Recv (buf,cnt,dtype,source,tag,comm,status ierr)
    !!     buf: The address of the beginning of the data to be sent
    !!     cnt: specifies the maximum number of elements to receive
    !!     dtype: datatype of each element
    !!     source: The rank of the source (MPI_ANY_SOURCE)
    !!     tag: Integer message tag 0 >= tag >= MPI_TAG_UB (at least 32767) (MPI_ANY_TAG)
    !!     comm: The communicator
    !!     status: vector   (MPI_STATUS_IGNORE)
    !!       MPI_SOURCE --> source
    !!       MPI_TAG    --> tag

      call MPI_Recv(bufferrecibo,2,MPI_INT,&
                  rankfuente,MPI_ANY_TAG,comm,&
                  status,ierr)
      print *, ' rank ',rank, ' recibido ',bufferrecibo(1:2)
   else
      ! rank impares
      call MPI_Recv(bufferrecibo,2,MPI_INT,&
                  rankfuente,MPI_ANY_TAG,comm,&
                  status,ierr)
      print *, ' rank ',rank, ' recibido ',bufferrecibo(1:2)
      call MPI_Send (bufferenvio,2,MPI_INT,&
                     rankdestino,0,comm,&
                     ierr)
    endif


     return
endsubroutine PasoMensajes_1

