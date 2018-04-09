     
     subroutine initchip( a, b, fheat, nx, ny, s, e, tmp_ambiente, chips, factor_escala)
     implicit none
      
     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1)
      double precision chips(6), factor_escala
       
      double precision tmp_ambiente, tempchip
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!
 
      coorsup_x = chips(1)*factor_escala
      coorsup_y = chips(2)*factor_escala
      coorinf_x = chips(3)*factor_escala + coorsup_x
      coorinf_y = chips(4)*factor_escala + coorsup_y
      tempchip = chips(5)
     
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
            if (fheat(i,j)> a(i,j)) then
                a(i,j) = a(i,j) + 0.05 * (fheat(i,j) - a(i,j))
                b(i,j) = b(i,j) + 0.05 * (fheat(i,j) - b(i,j))
            endif
        enddo
      enddo
 
      return
    end    
   subroutine initcooling( a, b, fcool, nx, ny, s, e,cooling , factor_escala )
     implicit none
      
     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                        fcool(0:nx+1, s-1:e+1)
      double precision  factor_escala,cooling(5)
       
      double precision  tempcooling
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!       
      coorsup_x = cooling(1)*factor_escala
      coorsup_y = cooling(2)*factor_escala
      coorinf_x = cooling(3)*factor_escala + coorsup_x
      coorinf_y = cooling(4)*factor_escala + coorsup_y
      tempcooling = cooling(5)
      fcool = -1000.0d0 !! valor no posible
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
            fcool(i,j) = tempcooling
            if  (fcool(i,j) < a(i,j)) then
              a(i,j) = a(i,j) - 0.01 * (a(i,j)-fcool(i,j))
              b(i,j) = b(i,j) - 0.01 * (b(i,j)-fcool(i,j))
            endif
        enddo
      enddo  
      return
    end  
    
    subroutine initfans( a, b, fcool, nx, ny, s, e, fans , factor_escala )
     implicit none
      
     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                        fcool(0:nx+1, s-1:e+1)
      double precision  factor_escala,fans(5)
       
      double precision  tempfans
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!       
      coorsup_x = fans(1)*factor_escala
      coorsup_y = fans(2)*factor_escala
      coorinf_x = fans(3)*factor_escala + coorsup_x
      coorinf_y = fans(4)*factor_escala + coorsup_y
      tempfans = fans(5)
 
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
            if (fcool(i,j) == -1000.0d0 .or. tempfans < fcool(i,j)) then
               fcool(i,j) = tempfans
               if  (fcool(i,j) < a(i,j)) then
                  a(i,j) = a(i,j) - 0.01 * (a(i,j)-fcool(i,j))
                  b(i,j) = b(i,j) - 0.01 * (b(i,j)-fcool(i,j))
               endif
            endif
        enddo
      enddo  
      return
    end    
    subroutine onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente, nchip, chips, factor_escala, fcool, cooling, fans)
     implicit none
      
     integer nx, ny, s, e, nchip
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1), fcool(0:nx+1, s-1:e+1)
      double precision  factor_escala,cooling(5),fans(5)
      double precision :: chips(6,nchip)
       
      double precision tmp_ambiente, tempchip, tempcooling
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!
      a(0:nx+1,s-1:e+1) = tmp_ambiente
      b(0:nx+1,s-1:e+1) = tmp_ambiente
      fheat = tmp_ambiente
      do i = 1,nchip
       call initchip    (a, b, fheat, nx, ny, s, e, tmp_ambiente, chips(:,i), factor_escala)
      end do
      
      call initcooling (a, b, fcool, nx, ny, s, e, cooling, factor_escala)
      call initfans    (a, b, fcool, nx, ny, s, e, fans, factor_escala)
 
    end    