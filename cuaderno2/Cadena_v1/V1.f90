subroutine cadena_v1 (comm)
    use mpi
    implicit none
    integer :: comm

     integer, parameter :: MAX_POT = 22
     integer, parameter :: MAX_LONG = 2**MAX_POT
     integer longitud, rep,i,j,k, tam0,tami, siguiente,nada
     integer myid, numprocs, ierr, status(MPI_STATUS_SIZE)
     integer datos(MAX_LONG)
     double precision resultados(MAX_POT+1), acum
     double precision starttime, endtime


     call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
     call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)


    if (myid == 0) then
      call ObtenerParametros (longitud, rep)
      do j = 0,longitud
          resultados(j+1)=0
          datos(1:2**j) = j
          acum = 0
          tam0 = 2**j
          do k = 1,rep
                starttime = MPI_WTIME()
                !print *,'myid ',myid,'send ', tam0
                call MPI_SEND(datos,tam0,MPI_INTEGER, &
                              1,myid,MPI_COMM_WORLD,&
                              ierr)
               !print *,'myid ',myid,'recv '
                call MPI_RECV(datos,tam0,MPI_INTEGER, &
                              numprocs-1,MPI_ANY_TAG,MPI_COMM_WORLD,&
                              status,ierr)
                endtime = MPI_WTIME()
                acum = acum + ( endtime-starttime)
          enddo
          resultados(j+1) = acum/dble(rep)
      enddo
      print *, resultados/numprocs
      !! final
      call MPI_SEND(0,0,MPI_INTEGER, &
                  1,myid,MPI_COMM_WORLD,&
                  ierr)
    else
      do
         call MPI_RECV(datos,MAX_LONG,MPI_INTEGER, &
                   myid-1,MPI_ANY_TAG,MPI_COMM_WORLD,&
                   status,ierr)
        call MPI_GET_COUNT(status,MPI_INTEGER,tami,ierr)
        !print *,'myid ',myid,'recibido ',tami, ' bytes'
        siguiente = modulo (myid+1, numprocs)
        if (tami == 0 .and. siguiente==0) exit
        call MPI_SEND(datos,tami,MPI_INTEGER, &
                   siguiente ,myid,MPI_COMM_WORLD,&
                   ierr)
        if (tami == 0) exit
        !print *,'myid ',myid,'enviado a',siguiente, ' ', tami, ' elementos'
     enddo
   endif

end subroutine cadena_v1
