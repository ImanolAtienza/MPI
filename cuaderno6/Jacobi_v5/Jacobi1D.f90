     program main
!
      use mpi
      implicit none

      INTERFACE
       subroutine readParam(gconf, conv, nchip, chip, cooling, fans)
        implicit none
           real*8  gconf(4), conv(3)
           real*8, allocatable ::  chip(:,:)
           real*8 cooling(5), fans(5)
           integer nchip
       endsubroutine readParam
      END INTERFACE

      integer maxn
      double precision, allocatable :: a(:,:), b(:,:), fheat(:,:), fcool(:,:), w(:,:)
      integer nx, ny
      integer myid, numprocs, ierr
      integer comm1d, nbrbottom, nbrtop, s, e, it, ii, maxstep,i
      double precision diff, diffnorm, dwork, convTot
      double precision t1, t2
      external diff

      double precision gconf(4) , conv(3),  cooling(5), fans(5)
      double precision, allocatable :: chip(:,:)
      integer nchip
      double precision tmp_ambiente, factor_escala
      integer, dimension (:), allocatable   :: recvcnts, displs
      integer tipo_columna
      logical kdim(1)
      integer nnproc(1)

      call MPI_INIT( ierr )
      call MPI_COMM_RANK( MPI_COMM_WORLD, myid, ierr )
      call MPI_COMM_SIZE( MPI_COMM_WORLD, numprocs, ierr )

      if (myid .eq. 0) then
           call readParam(gconf, conv, nchip, chip, cooling, fans)
           nx = gconf(1); ny = gconf(2)
      endif

      call MPI_BCAST(gconf,4,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      nx   = gconf(1)
      ny   = gconf(2)
      factor_escala = gconf(3)
      tmp_ambiente = gconf(4)
      call MPI_BCAST(conv,3,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      maxStep = conv(1)/ 8
      convTot = conv(3)
      call MPI_BCAST(nchip,1,MPI_INT,0,MPI_COMM_WORLD,ierr)
      if (myid/=0) then
          allocate (chip(6,nchip))
      endif
      call MPI_BCAST(chip,nchip*6,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(cooling,5,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(fans,5,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)

      kdim(1) = .false.
      nnproc(1)=numprocs
      call MPI_CART_CREATE( MPI_COMM_WORLD, 1, nnproc, kdim, &
                            .true., comm1d, ierr )

      call MPI_COMM_RANK( comm1d, myid, ierr )
      call MPI_Cart_shift( comm1d, 0,  1, nbrbottom, nbrtop, ierr )

      call MPE_DECOMP1D( ny, numprocs, myid, s, e )

      allocate (a(0:nx+1,s-1:e+1))
      allocate (b(0:nx+1,s-1:e+1))
      allocate (fheat(0:nx+1,s-1:e+1))
      allocate (fcool(0:nx+1,s-1:e+1))
      allocate (w(0:nx+1,0:ny+1))
      print *,nx,ny,myid,s,e

      call onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente,nchip,chip, factor_escala,&
                     fcool,cooling, fans)

     call MPI_TYPE_CONTIGUOUS (nx+2, MPI_DOUBLE_PRECISION, tipo_columna, ierr)
     CALL MPI_TYPE_COMMIT (tipo_columna, ierr)

    allocate (recvcnts(numprocs))
    allocate (displs(numprocs))

    call MPI_Gather (e-s+1,1,MPI_INT, &
                     recvcnts,1, MPI_INT ,0, MPI_COMM_WORLD ,ierr)
    if (myid==0) then
        displs(1) = 1
        do i = 2, numprocs
         displs(i) = displs(i-1)+recvcnts(i-1)
        end do
        print *,recvcnts
        print *,displs
    endif

    call MPI_BARRIER( MPI_COMM_WORLD, ierr )
    CALL MPI_Gatherv (a(0,s), e-s+1, tipo_columna, &
                          w, recvcnts, displs, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)

    if (myid==0) call escribirMat(w,1,nx,ny)
     t1 = MPI_WTIME()
      do it=1, maxStep
       do ii=1,400
	     call NoBlockexchng1( a, nx, s, e, comm1d, myid, nbrbottom, nbrtop, it )
	     call sweep1d( a, fheat, nx, s, e, b, fcool )
	     call NoBlockexchng1( b, nx, s, e, comm1d, myid, nbrbottom, nbrtop, it )
  	     call sweep1d( b, fheat, nx, s, e, a, fcool )
       enddo
 	   dwork = diff( a, b, nx, s, e )
	   call MPI_Allreduce( dwork, diffnorm, 1, MPI_DOUBLE_PRECISION, &
                            MPI_SUM, comm1d, ierr )

       CALL MPI_Gatherv (a(0,s), e-s+1, tipo_columna, &
                          w, recvcnts, displs, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)

      if (myid==0) call escribirMat(w,it+1,nx,ny)
      if (diffnorm .lt. convTot) exit
      if (myid .eq. 0) print *, 400*it, ' Difference is ', diffnorm
      enddo

      if (myid .eq. 0 .and. it .gt. maxStep) print *, 'Failed to converge'
      t2 = MPI_WTIME()
      if (myid .eq. 0) then
         print *, 'Converged after ', 400*it, ' Iterations in ', t2 - t1, &
             ' secs '
         call SYSTEM('pause')
      endif
!
      deallocate (a)
      deallocate (b)
      deallocate (fheat)
      deallocate (w)
      deallocate (recvcnts)
      deallocate (displs)
      deallocate (chip)

      call MPI_FINALIZE(ierr)
    end
