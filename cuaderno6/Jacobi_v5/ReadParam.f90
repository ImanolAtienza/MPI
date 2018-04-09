subroutine readParam(gconf, conv, nchip, chip, cooling, fans)
  implicit none

  real*8  gconf(4)
  real*8  conv(3)
  real*8 , allocatable ::  chip(:,:)
  real*8  cooling(5)
  real*8  fans(5)
  integer nchip
  integer ios,i

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

  read(5,*)
  read(5,*) nchip ! n chips
  allocate (chip(6,nchip))

  do i = 1,nchip
   read(5,*)
   read(5,*) chip(1,i) ! chip_coor_x
   read(5,*)
   read(5,*) chip(2,i) ! chip_coor_y
   read(5,*)
   read(5,*) chip(3,i) ! chip_size_x
   read(5,*)
   read(5,*) chip(4,i) ! chip_size_y
   read(5,*)
   read(5,*) chip(5,i) ! chip_temp_ini
   read(5,*)
   read(5,*) chip(6,i) ! chip_temp_max
  end do

  read(5,*)
  read(5,*) cooling(1) ! cool_coor_x
  read(5,*)
  read(5,*) cooling(2) ! cool_coor_y
  read(5,*)
  read(5,*) cooling(3) ! cool_size_x
  read(5,*)
  read(5,*) cooling(4) ! cool_size_y
  read(5,*)
  read(5,*) cooling(5) ! cool_temp_ini

  read(5,*)
  read(5,*) fans(1) ! fan_coor_x
  read(5,*)
  read(5,*) fans(2) ! fan_coor_y
  read(5,*)
  read(5,*) fans(3) ! fan_size_x
  read(5,*)
  read(5,*) fans(4) ! fan_size_y
  read(5,*)
  read(5,*) fans(5) ! fan_temp_ini

  close(5)
 end
