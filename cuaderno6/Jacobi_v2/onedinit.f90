
!     subroutine onedinit( a, b, nx, ny, s, e, tmp_ambiente)
!     implicit none
!      integer nx, s, e
!      double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
!                       f(0:nx+1, s-1:e+1)
!       
!      double precision tmp_ambiente
!!
!      integer i, j
!!
!      a(0:nx+1,s-1:e+1) = tmp_ambiente
!      b(0:nx+1,s-1:e+1) = tmp_ambiente
! 
!!
!!    Handle boundary conditions
!!
! 
!      a(0:1,s-1:e+1) = 40.0d0
!      b(0:1,s-1:e+1) = 40.0d0
!      a(nx:nx+1,s-1:e+1) = 40.0d0
!      b(nx:nx+1,s-1:e+1) = 40.0d0
!      
!      if (s .eq. 1) then
!         a(0:nx,0:1) = 40.0d0
!         b(0:nx,0:1) = 40.0d0         
!      endif
!     if (e .eq. ny) then
!         a(0:nx,ny:ny+1) = 40.0d0
!         b(0:nx,ny:ny+1) = 40.0d0         
!      endif
! 
!      return
!    end
    
     subroutine onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente, chips, factor_escala)
     implicit none
      
     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1)
      double precision chips(6), factor_escala
       
      double precision tmp_ambiente, tempchip
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!
      a(0:nx+1,s-1:e+1) = tmp_ambiente
      b(0:nx+1,s-1:e+1) = tmp_ambiente
 
      coorsup_x = chips(1)*factor_escala
      coorsup_y = chips(2)*factor_escala
      coorinf_x = chips(3)*factor_escala + coorsup_x
      coorinf_y = chips(4)*factor_escala + coorsup_y
      tempchip = chips(5)
       fheat = tmp_ambiente
      if ((coorsup_y < (s-1)) .and. (coorinf_y < (s-1))) return
      if (coorsup_y > (e+1)) return
      if (coorsup_y < (s-1) ) then
          desde = s-1
      else
          desde = coorsup_y      
      endif
       if (coorinf_y > (e+1) ) then
          hasta = e+1
      else
          hasta = coorinf_y      
      endif
      !print *, 'chip ', coorsup_x, desde, coorinf_x, hasta
      do j = desde,hasta
        do i = coorsup_x, coorinf_x
            fheat(i,j) = tempchip
            a(i,j) = tempchip
        enddo
      enddo
      return
    end    
