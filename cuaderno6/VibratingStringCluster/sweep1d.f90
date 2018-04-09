
 subroutine sweep1d(i, tiempo,h, u,alpha )

      implicit none
      integer   i, tiempo, h
      double precision  u(0:tiempo,0:h), alpha
!
      integer j


      do j = 1, h-1
       u(i,j) = &
                                alpha   * u(i-1,j-1) &
        + 2.0D+00 * ( 1.0D+00 - alpha ) * u(i-1,j)   &
        +                       alpha   * u(i-1,j+1) &
        -                                 u(i-2,j)
      end do

      return
 end
