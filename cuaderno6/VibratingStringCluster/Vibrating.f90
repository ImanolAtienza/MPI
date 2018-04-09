     program main
!
      implicit none

      integer maxn
      double precision, allocatable :: u(:,:)
      integer tiempo, h

      integer  it, ii, maxstep,i, seguidas
      double precision  diffnorm, dwork, convTot
      integer*8 t1, t2, clock_rate, temp

      double precision gconf(2)

      real ( kind = 8 ) :: dt, dx, alpha
      real ( kind = 8 ) :: c =  5.0D+00

      integer nthreads

      call readParam(gconf)
      tiempo = gconf(1); h = gconf(2)
      print *,'tiempo ',tiempo, ' h ',h
      allocate (u(0:tiempo,0:h))

      dx = 1.0d0 / real ( h, kind = 8 )
      dt =  1.0d0 / real ( tiempo, kind = 8 )
      alpha = ( c * dt / dx ) ** 2
      write ( *, '(a,g14.6)' ) '  ALPHA = ( C * dT / dX )^2 = ', alpha
      !
      !  Warn the user if ALPHA will cause an unstable computation.
      !
      if ( 1.0D+00 < alpha ) then
         write ( *, '(a)' ) ''
         write ( *, '(a)' ) '  Warning!'
         write ( *, '(a)' ) '  ALPHA is greater than 1.'
         write ( *, '(a)' ) '  The computation is unstable.'
      end if

      call onedinit(tiempo, h, u, alpha)


      call SYSTEM_CLOCK(t1)

      do it=2, tiempo
	     call sweep1d( it, tiempo, h, u, alpha)
      enddo
      call SYSTEM_CLOCK(t2, clock_rate)
      call escribirMat(u,0,tiempo,h,1)

      print *, ' Time ', real(t2 - t1)/real(clock_rate), &
             ' secs '
!

      deallocate (u)

    end
