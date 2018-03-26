subroutine ejemplo2 (comm, n, stride)
use mpi
implicit none
integer, intent(IN) :: comm, n, stride

integer :: vec(n)
integer :: buffer(STRIDE)

integer rank, nprocs, ierr, status(MPI_STATUS_SIZE)
integer  sum,  total, master, cuantos, cuantosrecibidos
integer  i, j, k, tmp, vecessendreceive, current, aquienenvio


Call MPI_Comm_Rank (comm, rank, ierr)
Call MPI_Comm_Size (comm, nprocs, ierr)

vecessendreceive = CEILING (dble(n)/dble(STRIDE))

master = nprocs-1

If (rank .eq. master) then ! master
 vec = 2; current = 1; cuantos = STRIDE; total = 0; j=0
 do i = 1, vecessendreceive
    aquienenvio = modulo (i, master)
    call MPI_Send (vec(current), cuantos, MPI_INTEGER,&
                    aquienenvio, 1, comm,&
                    ierr)
    current = current + cuantos
    if ((current + cuantos - 1) > n) cuantos = n - current + 1
    j = j + 1
    if (j ==  nprocs-1) then ! he enviado a todos los slaves
     do k = 0, nprocs-2
         call MPI_Recv(tmp,1,MPI_INT, &
                       k,MPI_ANY_TAG,comm, &
                       status, ierr)
         total = total + tmp
     enddo
     j=0
    endif
 enddo
 do i = 1,j !! recibo de los  pendientes el ultimo trozo
    call MPI_Recv(tmp,1,MPI_INT, &
                  i,MPI_ANY_TAG,comm, &
                  status, ierr)
    total = total + tmp
 enddo
 do i = 0,nprocs-2 !! envio a todos el final
    call MPI_Send (0, 0, MPI_INTEGER,&
                   i, 0, comm,&
                   ierr)
 enddo

ELSE  ! slaves
    total = 0
    do
      call MPI_Recv(buffer,stride,MPI_INT, &
                       master,MPI_ANY_TAG,comm, &
                       status,ierr)
      call MPI_Get_count (status, MPI_INT, cuantosrecibidos,ierr)
      if (status(MPI_TAG)==0) exit
      sum=0
      DO i = 1, cuantosrecibidos
        sum = sum + buffer(i)
      ENDDO
      total = total + sum
      call MPI_Send (sum, 1, MPI_INTEGER,&
                  master, rank, comm, ierr)
   end do
ENDIF


 if (rank.eq.master)  then
    print '(2(A,I))', 'rank ',rank, ' total general ',total
 else
    print '(2(A,I))', 'rank ',rank, ' total parcial ',total
 endif

end subroutine ejemplo2
