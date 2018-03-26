

    program MxV
    use mpi
    implicit none

    integer MAX_ROWS, MAX_COLS, rows, cols
    parameter (MAX_ROWS = 1000, MAX_COLS = 500)
    double precision a(MAX_ROWS,MAX_COLS), b(MAX_COLS)
    double precision c(MAX_ROWS), buffer(MAX_COLS), ans

    integer myid, manager, numprocs, ierr, status(MPI_STATUS_SIZE)
    integer i, j, numsent, sender
    integer anstype, row

    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)
    manager = 0
    rows   = MAX_ROWS
    cols   = MAX_COLS

    if (myid .eq. manager) then
    !     manager initializes a
       do j = 1,cols
          b(j) = 1
          do i = 1,rows
             a(i,j) = i
          enddo
       enddo

      call Smanager(a,b,c,manager,numprocs,rows,cols)

    else
        call SWorker(b,manager,myid,rows,cols)
    endif

    call MPI_FINALIZE(ierr)


   end program MxV

