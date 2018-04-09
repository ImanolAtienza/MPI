  subroutine escribirMat( a, numframes, nx, ny, factor_escala )
    implicit none
    double precision :: a(0:nx,0:ny)
    
    integer numframes,nx,ny
    integer i,j
    character nombrefichero*16
    integer factor_escala
   
      integer ( kind = 4 ) command_unit

      call get_unit ( command_unit )
      write(nombrefichero, '(A, I3.3, A)') 'outputPar',numframes, '.txt'
 
      open(command_unit,file=nombrefichero,action="write",status="replace")

       
        do  i=0,nx
         do  j=0,ny,factor_escala
             write(command_unit,'(1000(f7.2,1x))',advance="no") a(i,j) 
         end do 
         write (command_unit,*)
        end do
        
        close(command_unit)
        
    endsubroutine
    