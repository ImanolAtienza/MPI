

    program pVxM
    use mpi
    implicit none

    integer MAX_ROWS, MAX_COLS
    parameter (MAX_ROWS = 4, MAX_COLS = 5)
    double precision a(MAX_ROWS,MAX_COLS), bvxm (MAX_ROWS)
    double precision cvxm (MAX_COLS)

    integer myid, numprocs, ierr


    call MPI_INIT(ierr)
    call MPI_COMM_RANK(MPI_COMM_WORLD, myid, ierr)
    call MPI_COMM_SIZE(MPI_COMM_WORLD, numprocs, ierr)


    call VxM (a,bvxm,cvxm, myid, numprocs, MAX_ROWS,MAX_COLS)

    if (myid .eq. 0) then
       print *, cvxm(1:MAX_COLS) !VxM
    endif
    call MPI_FINALIZE(ierr)


   end program pVxM

