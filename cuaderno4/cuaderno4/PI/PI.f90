
    program NPI

     use mpi
     implicit none

     double precision  PI25DT
     parameter        (PI25DT = 3.141592653589793238462643d0)
     double precision  mypi, pi, h, sum, x, f, a
     integer n, myid, numprocs, i,j, ierr
     double precision starttime, endtime, acum
!                                function to integrate
     f(a) = 4.d0 / (1.d0 + a*a)

     call MPI_INIT(ierr)
     call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
     call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)

     !print *, "somos ", numprocs , " soy " , myid
     if (myid .eq. 0) then
           print *, 'Enter the number of intervals: (0 quits) '
           read(*,*) n
     endif

     acum=0
     do j = 1,21


        starttime = MPI_WTIME()
!                                broadcast n
        !! MPI_BCAST (buffer, count, datatype, root, comm, ierr)
         ! This routine copies count elements of type datatype from buffer on the process with rank root into the location
         ! specified by buffer on all other processes in the group referenced by the communicator comm
         ! All processes must supply the same value for the argument root
         ! A broadcast need not be synchronizing. Returning from a broadcast tells you nothing about the status of the other processes involved in a broadcast.
         ! Furthermore, though MPI does not require MPI_BCAST to be synchronizing, it neither prohibits synchronous behavior.
        call MPI_BCAST(n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD, ierr)
!                                check for quit signal
        if (n .le. 0) exit
!                                calculate the interval size
        h = 1.0d0/n
        sum  = 0.0d0
        do i = myid+1, n, numprocs
           x = h * (dble(i) - 0.5d0)
           sum = sum + f(x)
        enddo
        mypi = h * sum
!                                collect all the partial sums
        !! Global reduction across all members of a group
        !! Can us predefined operations or user defined operations
        !! Can be used on single elements or arrays of elements
        !! Counts and types must be the same on all processors
        !! Operations are assumed to be associative
        !! All pre-defined operations are commutative
        !! User defined operations can be different on each processor, but not recommended

        !! MPI_REDUCE (sendbuf, recvbuf, cnt, dtype, op, root, comm, ierr)
         ! recvbuf only meaningful on root
         ! Combines elements (on an element by element basis) in sendbuf according to op
         ! Results of the reduction are returned to root in recvbuf
         ! MPI_IN_PLACE can be used for sendbuf on root
        call MPI_REDUCE(mypi, pi, 1, MPI_DOUBLE_PRECISION, &
                        MPI_SUM, 0, MPI_COMM_WORLD, ierr)
!                                node 0 prints the answer.
       endtime = MPI_WTIME()
       acum = acum + ( endtime-starttime)
      enddo
       if (myid .eq. 0) then
         print *, 'pi is ', pi, ' Error is', abs(pi - PI25DT)
         print *, 'time is ', acum / 21, ' seconds'
     endif

     call MPI_FINALIZE(ierr)

    end program NPI

