!
! Perform a Jacobi sweep for a 1-d decomposition.
! Sweep from a into b
!
      subroutine sweep1d( a, fheat, nx, s, e, b, fcool )
      implicit none
      integer nx, s, e
      double precision a(0:nx+1,s-1:e+1), fheat(0:nx+1,s-1:e+1), &
                       b(0:nx+1,s-1:e+1), fcool(0:nx+1,s-1:e+1)
!
      integer i, j
      double precision h
!
      h = 1.0d0 / dble(nx+1)
      do j=s, e
         do i=1, nx
              b(i,j) =  a(i,j) + 0.1 * (a(i-1,j-1)+a(i-1,j)+a(i-1,j+1)+&
                                       a(i,j-1)+           a(i,j+1)+&
                                      a(i+1,j-1)+a(i+1,j)+a(i+1,j+1) - &
                                      8 * a(i,j))
            if (fheat(i,j) > b(i,j)) then
                b(i,j) = b(i,j) + 0.05 * (fheat(i,j) - b(i,j))
            endif
            
            if ((fcool(i,j) /= -1000.0d0).and. (fcool(i,j) < b(i,j))) then
                b(i,j) = b(i,j) - 0.01 * (b(i,j)-fcool(i,j))
            endif
         enddo
      enddo
      return
      end