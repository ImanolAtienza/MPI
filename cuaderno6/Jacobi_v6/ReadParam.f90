subroutine readParam(gconf, conv, nchip, chip, ncooling, cooling, nfans, fans,nairext,airext)
  implicit none

  real*8  gconf(4)
  real*8  conv(3)
  real*8 , allocatable ::  chip(:,:)
  real*8 , allocatable ::  cooling(:,:)
  real*8 , allocatable ::  fans(:,:)
  real*8 , allocatable ::  airext(:,:)
  integer nchip, ncooling, nfans, nairext
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
  read(5,*) ncooling ! n cooling
  allocate (cooling (5,ncooling))
  do i = 1,ncooling
   read(5,*)
   read(5,*) cooling(1,i) ! cool_coor_x
   read(5,*)
   read(5,*) cooling(2,i) ! cool_coor_y
   read(5,*)
   read(5,*) cooling(3,i) ! cool_size_x
   read(5,*)
   read(5,*) cooling(4,i) ! cool_size_y
   read(5,*)
   read(5,*) cooling(5,i) ! cool_temp_ini
  enddo

  read(5,*)
  read(5,*) nfans ! n nfans
 allocate (fans (7,nfans))
  do i = 1,nfans
   read(5,*)
   read(5,*) fans(1,i) ! fan_coor_x
   read(5,*)
   read(5,*) fans(2,i) ! fan_coor_y
   read(5,*)
   read(5,*) fans(3,i) ! fan_size_x
   read(5,*)
   read(5,*) fans(4,i) ! fan_size_y
   read(5,*)
   read(5,*) fans(5,i) ! fan_temp_ini
   read(5,*)
   read(5,*) fans(6,i) ! ancho_y_disipa
   read(5,*)
   read(5,*) fans(7,i) ! temp_disipa
  enddo

  read(5,*)
  read(5,*) nairext ! n airext
  allocate (airext (4,nairext))
  do i = 1,nairext
   read(5,*)
   read(5,*) airext(1,i) ! airext_coor_x
   read(5,*)
   read(5,*) airext(2,i) ! airext_size_x
   read(5,*)
   read(5,*) airext(3,i) ! airext_temp_ini
   read(5,*)
   read(5,*) airext(4,i) ! airext_delta
  enddo

  close(5)
 end
