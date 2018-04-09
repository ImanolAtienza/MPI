
     subroutine onedinit( a, b,  nx, ny, s, e, tmp_ambiente)
     implicit none
      integer nx, s, e, ny
      double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1)
                       
       
      double precision tmp_ambiente
!
      integer i, j
!
      a(0:nx+1,s-1:e+1) = tmp_ambiente
      b(0:nx+1,s-1:e+1) = tmp_ambiente
      
 
!
!    Handle boundary conditions
!
 
      a(0:1,s-1:e+1) = 10.0d0
      b(0:1,s-1:e+1) = 10.0d0
      a(nx:nx+1,s-1:e+1) = 10.0d0
      b(nx:nx+1,s-1:e+1) = 10.0d0
      
      if (s .eq. 1) then
         a(0:nx,0:1) = 10.0d0
         b(0:nx,0:1) = 10.0d0         
      endif
     if (e .eq. ny) then
         a(0:nx,ny:ny+1) = 10.0d0
         b(0:nx,ny:ny+1) = 10.0d0         
      endif
 
      return
    end
    

