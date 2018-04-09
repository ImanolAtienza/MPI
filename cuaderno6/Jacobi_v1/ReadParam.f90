subroutine readParam(gconf, conv)
  implicit none

  real*8  gconf(4)
  real*8  conv(3)

  integer ios

  open(unit=5, file='param.txt',&
        IOSTAT= ios, STATUS='OLD')
   if (ios.ne.0) then
     print *, 'Error opening param file - IOSTAT = ', ios
     call SYSTEM("pause")
     stop
   endif

  read(5,*)
  read(5,*) gconf(1) ! size_x
  read(5,*)
  read(5,*) gconf(2) ! size_y
  read(5,*)
  read(5,*) gconf(3) ! factor_escala
  read(5,*)
  read(5,*) gconf(4) ! temp_ambiente

  read(5,*)
  read(5,*) conv(1) ! max_iter
  read(5,*)
  read(5,*) conv(2) ! dt
  read(5,*)
  read(5,*) conv(3) ! cnv_tol

  close(5)
 end
