
subroutine onedinit(tiempo, h,u, alpha)

     implicit none
      integer   tiempo, h
      double precision  u(0:tiempo,0:h), alpha

      double precision  dx, x
      real(8),  parameter :: PI_8  = 4 * atan (1.0)
      double precision F
      integer par1,par2
      !F(par1,par2) = sin(PI_8*dble(par1)/dble(par2))
      !F(par1,par2) = sin(2.0d0*PI_8*dble(par1)/dble(par2))
      F(par1,par2) = abs(sin(2.0d0*PI_8*dble(par1)/dble(par2)))
!
      integer j
!
!  Time step 0:
!  Use the initial condition for U.
!
      do j = 0,tiempo
          u(j,0) =0.0d0; u(j,h) = 0.0d0
      end do


    do j = 1, h-1
      u(0,j) = F(j,h)
   end do

!  Time step 1:
!  Use the initial condition for dUdT.
!

  do j = 1, h-1
    u(1,j) = &
        ( alpha / 2.0D+00 ) * u(0,j-1) &
      + ( 1.0D+00 - alpha ) * u(0,j)   &
      + ( alpha / 2.0D+00 ) * u(0,j+1) &
      + 0.0d0
  end do

  return
end


