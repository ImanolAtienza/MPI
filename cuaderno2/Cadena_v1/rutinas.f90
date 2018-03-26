subroutine ObtenerParametros (longitud, rep)
     implicit none
     integer, intent(OUT):: longitud, rep

    do
        print *, 'Longitud máxima a enviar, potencia de 2 (>0 && <= 2**22) '
        read(*,*) longitud
        if ( (longitud>0) .and. (longitud<23)) exit
    enddo
    do
       print *, 'numero repeticiones (>0) '
       read(*,*) rep
       if (rep>0) exit
    enddo

end subroutine

