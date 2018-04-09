  subroutine escribirMat( a, numframes, nx, ny )
    implicit none
    double precision :: a(0:nx+1,0:ny+1)

    integer numframes,nx,ny
    integer i,j
    character nombrefichero*50
    integer escala
    integer ios

        escala=10
 !       write(nombrefichero, '(A, I3.3, A)') 'Salida\outputPar',numframes, '.txt'
        write(nombrefichero, '(A, I3.3 A)') 'Salida/outputPar',numframes, '.txt'

        open(5,file=nombrefichero,action="write",status="replace",&
             IOSTAT= ios)
        if (ios.ne.0) then
          print *, 'Error opening outPar file - IOSTAT = ', ios
          call SYSTEM("pause")
          stop
        endif

        do  i=0,nx+1,escala
         do  j=0,ny+1,escala
             write(5,'(1000(f7.2,1x))',advance="no") a(i,j)
         end do
         write (5,*)
        end do

        close(5)

    endsubroutine
