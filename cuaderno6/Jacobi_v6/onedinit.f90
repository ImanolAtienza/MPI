
     subroutine initchip( a, b, fheat, nx, ny, s, e, chips, factor_escala,tipo)
     implicit none

     integer nx, ny, s, e, tipo
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1)
      double precision chips(6), factor_escala

      double precision  tempchip
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
      !print *, 'chip ', coorsup_x, desde, coorinf_x, hasta,tipo
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

      double precision  tempcooling, temp
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta
!
      coorsup_x = cooling(1)*factor_escala
      coorsup_y = cooling(2)*factor_escala
      coorinf_x = cooling(3)*factor_escala + coorsup_x
      coorinf_y = cooling(4)*factor_escala + coorsup_y
      tempcooling = cooling(5)
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
      !print *, 'cool ', coorsup_x, desde, coorinf_x, hasta
      do j = desde,hasta
        do i = coorsup_x, coorinf_x
            if (fcool(i,j) == -1000.0d0) then
                fcool(i,j) = tempcooling
                if  (fcool(i,j) < a(i,j)) then
                    a(i,j) = a(i,j) - 0.01 * (a(i,j)-fcool(i,j))
                    b(i,j) = b(i,j) - 0.01 * (b(i,j)-fcool(i,j))
                endif
            else
                if (tempcooling < fcool(i,j)) then
                    temp = (a(i,j) - 0.01*fcool(i,j))/(1.0-0.01)
                    fcool(i,j) = tempcooling
                    a(i,j) = temp - 0.01 * (temp-fcool(i,j))
                    b(i,j) = temp - 0.01 * (temp-fcool(i,j))
                endif
            endif
        enddo
      enddo
      return
    end

    subroutine initfans( a, b, fheat, fcool, nx, ny, s, e, fans , factor_escala )
     implicit none

     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                        fcool(0:nx+1, s-1:e+1),fheat(0:nx+1, s-1:e+1)
      double precision  factor_escala,fans(7)

      double precision  tempfans, temp
      double precision chip(6)
!
      integer i, j, coorsup_x, coorsup_y,coorinf_x, coorinf_y, desde, hasta

!

      chip(1) = fans(1)
      chip(2) = fans(2)
      chip(3) = fans(3)
      chip(4) = fans(6)
      chip(5) = fans(7)
      call initchip    (a, b, fheat, nx, ny, s, e, chip, factor_escala, 2)

      coorsup_x = fans(1)*factor_escala
      coorsup_y = fans(2)*factor_escala
      coorinf_x = fans(3)*factor_escala + coorsup_x
      coorinf_y = fans(4)*factor_escala + coorsup_y
      coorsup_y = coorsup_y + fans(6)*factor_escala

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
      !print *, 'fan ', coorsup_x, desde, coorinf_x, hasta
      do j = desde,hasta
        do i = coorsup_x, coorinf_x
           if (fcool(i,j) == -1000.0d0) then
                fcool(i,j) = tempfans
                if  (fcool(i,j) < a(i,j)) then
                    a(i,j) = a(i,j) - 0.01 * (a(i,j)-fcool(i,j))
                    b(i,j) = b(i,j) - 0.01 * (b(i,j)-fcool(i,j))
                endif
            else
                if (tempfans < fcool(i,j)) then
                    temp = (a(i,j) - 0.01*fcool(i,j))/(1.0-0.01)
                    fcool(i,j) = tempfans
                    a(i,j) = temp - 0.01 * (temp-fcool(i,j))
                    b(i,j) = temp - 0.01 * (temp-fcool(i,j))
                endif
            endif
        enddo
      enddo
      return
    end
    subroutine initAbertura( a, b, fcool, nx, ny, s, e,&
                             coorsup_x, coorinf_x, factor_escala,tempAbertura, deltat  )
     implicit none

     integer nx, ny, s, e
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                        fcool(0:nx+1, s-1:e+1)
      double precision  factor_escala, deltat, tempAbertura
      integer coorsup_x ,coorinf_x
      double precision   temp, tempdelta
      integer i, j, wcoorsup_x, wcoorinf_x, desde, hasta


      wcoorsup_x = coorsup_x * factor_escala
      wcoorinf_x = coorinf_x * factor_escala
      !print *,wcoorsup_x,wcoorinf_x,tempAbertura,deltat
      !print *, 'abertura ', wcoorsup_x, wcoorinf_x
!      if (s==1) then
!          do i = wcoorsup_x, wcoorinf_x
!            tempdelta = tempAbertura
!            if (fcool(i,0) == -1000.0d0) then
!                fcool(i,0) = tempdelta
!                if  (fcool(i,0) < a(i,0)) then
!                    a(i,0) = a(i,0) - 1.0 * (a(i,0)-fcool(i,0))
!                    b(i,0) = b(i,0) - 1.0 * (b(i,0)-fcool(i,0))
!                endif
!            else
!                if (tempdelta < fcool(i,j)) then
!                    temp = (a(i,0) - 0.01*fcool(i,0))/(1.0-0.01)
!                    fcool(i,0) = tempdelta
!                    a(i,0) = temp - 1.0 * (temp-fcool(i,0))
!                    b(i,0) = temp - 1.0 * (temp-fcool(i,0))
!                endif
!            endif
!          enddo
!      endif

      desde = s-1
      hasta = e+1
      do j = desde,hasta
        do i = wcoorsup_x, wcoorinf_x
            tempdelta = tempAbertura + j*deltat
            if (fcool(i,j) == -1000.0d0) then
                fcool(i,j) = tempdelta
                if  (fcool(i,j) < a(i,j)) then
                    a(i,j) = a(i,j) - 1.0 * (a(i,j)-fcool(i,j))
                    b(i,j) = b(i,j) - 1.0 * (b(i,j)-fcool(i,j))
                endif
            else
                if (tempdelta < fcool(i,j)) then
                    temp = (a(i,j) - 0.01*fcool(i,j))/(1.0-0.01)
                    fcool(i,j) = tempdelta
                    a(i,j) = temp - 1.0 * (temp-fcool(i,j))
                    b(i,j) = temp - 1.0 * (temp-fcool(i,j))
                endif
            endif
        enddo
      enddo
      return
    end

    subroutine onedinit( a, b, fheat, nx, ny, s, e, tmp_ambiente, nchip, chips, factor_escala,&
                        fcool, ncooling, cooling, nfans,fans, nairext, airext)
     implicit none

     integer nx, ny, s, e, nchip, ncooling, nfans, nairext
     double precision a(0:nx+1, s-1:e+1), b(0:nx+1, s-1:e+1), &
                       fheat(0:nx+1, s-1:e+1), fcool(0:nx+1, s-1:e+1)
      double precision  factor_escala
      double precision :: chips(6,nchip),cooling(5,ncooling),fans(7,nfans),airext(4,nairext)

      double precision tmp_ambiente, tempabertura, deltat
!
      integer i, j, coorsup_x,coorinf_x
!
      a(0:nx+1,s-1:e+1) = tmp_ambiente
      b(0:nx+1,s-1:e+1) = tmp_ambiente
      fheat = tmp_ambiente
     do i = 1,nchip
       call initchip    (a, b, fheat, nx, ny, s, e, chips(:,i), factor_escala,1)
      end do
      fcool = -1000.0d0 !! valor no posible
      do i = 1,nchip
        call initcooling (a, b, fcool, nx, ny, s, e, cooling(:,i), factor_escala)
      end do
      do i = 1, nfans
        call initfans    (a, b, fheat,fcool, nx, ny, s, e, fans(:,i), factor_escala)
      enddo

      do i = 1,nairext
       coorsup_x = airext(1,i)
       coorinf_x = coorsup_x + airext(2,i)
       tempabertura = airext(3,i)
       deltat = airext(4,i)
       call initAbertura (a,b,fcool,nx, ny, s, e,&
          coorsup_x, coorinf_x, factor_escala,tempabertura,deltat)
      enddo


    end
