     program main
!
      use mpi
      implicit none

      INTERFACE
       subroutine readParam(gconf, conv, nchip, chip, ncooling, cooling, nfans, fans, nairext, airext )
        implicit none
           real*8  gconf(4), conv(3)
           real*8, allocatable ::  chip(:,:)
           real*8, allocatable ::  cooling(:,:)
           real*8, allocatable ::  fans(:,:)
           real*8, allocatable ::  airext(:,:)
           integer nchip, ncooling, nfans, nairext
       endsubroutine readParam
       subroutine onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente, nchip, chips, factor_escala,&
                        fcool, ncooling, cooling, nfans,fans, nairext, airext)
          implicit none

        integer nx, ny, s, e, nchip, ncooling, nfans, nairext
        double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1), fcool(0:nx+1, s-1:e+1)
        double precision  factor_escala
        double precision :: chips(6,nchip),cooling(5,ncooling),fans(7,nfans),airext(4,nairext)

        double precision tmp_ambiente
       endsubroutine onedinit

      END INTERFACE

      integer maxn
      double precision, allocatable :: a(:,:), b(:,:), fheat(:,:), fcool(:,:), w(:,:)
      integer nx, ny
      integer myid, numprocs, ierr
      integer comm1d, nbrbottom, nbrtop, s, e, it, ii, maxstep,i
      double precision dif, difnorm, dwork, convTot, temp, tempsum
      double precision t1, t2

      double precision gconf(4) , conv(3)
      double precision, allocatable :: chip(:,:)
      real*8, allocatable ::  cooling(:,:)
      real*8, allocatable ::  fans(:,:)
      real*8, allocatable ::  airext(:,:)
      integer nchip, ncooling, nfans, nairext
      double precision tmp_ambiente, factor_escala
      integer, dimension (:), allocatable   :: recvcnts, displs
      integer tipo_columna, indframe
      integer dims(1)
      logical isperiodic(1), reorder

      call MPI_INIT( ierr )
      call MPI_COMM_RANK( MPI_COMM_WORLD, myid, ierr )
      call MPI_COMM_SIZE( MPI_COMM_WORLD, numprocs, ierr )

      allocate (recvcnts(numprocs))
      allocate (displs(numprocs))

      if (myid .eq. 0) then
           call readParam(gconf, conv, nchip, chip, ncooling, cooling, nfans, fans, nairext, airext)
           print *,'chips ',nchip,' coolers ',ncooling,' fans ',nfans,' ranuras ',nairext
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
      call MPI_BCAST(ncooling,1,MPI_INT,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(nfans,1,MPI_INT,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(nairext,1,MPI_INT,0,MPI_COMM_WORLD,ierr)
      if (myid/=0) then
          allocate (chip(6,nchip))
          allocate (cooling(5,ncooling))
          allocate (fans(7,nfans))
          allocate (airext(4,nairext))
      endif

     ! print *,myid,nchip,ncooling,nfans,nairext

      call MPI_BCAST(chip,nchip*6,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(cooling,ncooling*5,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(fans,nfans*7,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)
      call MPI_BCAST(airext,nairext*4,MPI_DOUBLE_PRECISION,0,MPI_COMM_WORLD,ierr)

      dims(1) = numprocs
      isperiodic(1) = .false.
      reorder = .true.

      call MPI_CART_CREATE( MPI_COMM_WORLD, 1, dims, isperiodic, &
                            reorder, comm1d, ierr )

      call MPI_COMM_RANK( comm1d, myid, ierr )
      call MPI_Cart_shift( comm1d, 0,  1, nbrbottom, nbrtop, ierr )

      call MPE_DECOMP1D( ny, numprocs, myid, s, e )

      !print *,myid,nx,ny,s,e

      allocate (a(0:nx+1,s-1:e+1))
      allocate (b(0:nx+1,s-1:e+1))
      allocate (fheat(0:nx+1,s-1:e+1))
      allocate (fcool(0:nx+1,s-1:e+1))
      allocate (w(0:nx+1,0:ny+1))

      call onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente,nchip,chip, factor_escala,&
                     fcool,ncooling, cooling, nfans, fans, nairext, airext)

     call MPI_TYPE_CONTIGUOUS (nx+2, MPI_DOUBLE_PRECISION, tipo_columna, ierr)
     CALL MPI_TYPE_COMMIT (tipo_columna, ierr)

    call MPI_BARRIER(MPI_COMM_WORLD,ierr)
    call MPI_Gather (e-s+1,1,MPI_INT, &
                     recvcnts,1, MPI_INT ,0, MPI_COMM_WORLD ,ierr)
    if (myid==0) then
        displs(1) = 1
        do i = 2, numprocs
         displs(i) = displs(i-1)+recvcnts(i-1)
        end do
     endif

    call MPI_BARRIER( comm1d, ierr )
    !!!

               CALL MPI_Gatherv (fheat(0,s), e-s+1, tipo_columna, &
                          w, recvcnts, displs, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)
             if (myid==0) then
                 call escribirMat(w,0,nx,ny)
             endif
    !!!
    indframe=1
     t1 = MPI_WTIME()
      do it=1, maxStep
       do ii=1,200

	     call NoBlockexchng1( a, nx, s, e, comm1d, myid, nbrbottom, nbrtop, it )
	     call sweep1d( a, fheat, nx, s, e, b, fcool )

	     call NoBlockexchng1( b, nx, s, e, comm1d, myid, nbrbottom, nbrtop, it )
  	     call sweep1d( b, fheat, nx, s, e, a, fcool )
            if (it==1 .and. ii<10) then
             CALL MPI_Gatherv (a(0,s), e-s+1, tipo_columna, &
                          w, recvcnts, displs, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)

             if (myid==0) then
                 call escribirMat(w,indframe,nx,ny)
                 indframe = indframe+1
             endif

           endif

       enddo
       call diff(a,b,nx,s,e,dwork,temp)
 	   call MPI_Allreduce( dwork, difnorm, 1, MPI_DOUBLE_PRECISION, &
                            MPI_SUM, comm1d, ierr )
 	   call MPI_Allreduce( temp, tempsum, 1, MPI_DOUBLE_PRECISION, &
                            MPI_SUM, comm1d, ierr )
       tempsum = tempsum /(nx*ny)

       CALL MPI_Gatherv (a(0,s), e-s+1, tipo_columna, &
                          w, recvcnts, displs, tipo_columna, &
                          0, MPI_COMM_WORLD, ierr)

       if (myid==0) then
                 call escribirMat(w,indframe,nx,ny)
                 indframe = indframe+1
       endif
       if (difnorm .lt. convTot) exit
       if (myid .eq. 0) print *, 200*it, ' Difference is ', difnorm, 'temp ',tempsum
      enddo

      if (myid .eq. 0 .and. it .gt. maxStep) print *, 'Failed to converge'
      t2 = MPI_WTIME()
      if (myid .eq. 0) then
         print *, 'Converged after ', 200*it, ' Iterations in ', t2 - t1, &
             ' secs '
         print *, 'temperatura media ', tempsum
      endif
!
      deallocate (a)
      deallocate (b)
      deallocate (fheat)
      deallocate (w)
      deallocate (recvcnts)
      deallocate (displs)
      deallocate (chip)
      deallocate (cooling)
      deallocate (fans)

      call MPI_FINALIZE(ierr)
    end
