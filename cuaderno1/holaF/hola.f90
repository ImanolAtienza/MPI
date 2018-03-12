!/*************************************************************
!hola.f90
!       programa MPI: activacion de procesos
!************************************************************/
! Todas las variables que van desde la I..N -> Integer, si no son Reales o Float
program pi
  use mpi ! Lo mismo que include en C
  implicit none ! Se definen todas las variables

  integer ierr,rank,npr,len
  integer ::   A = 21, lnom
  CHARACTER*(MPI_MAX_PROCESSOR_NAME)nombre ! Definir vector de caracteres, con variable constante propia de MPI
  ! call es para llamar una funcion
  ! Siempre el programa MPI debe ir entre un Init y un finalize
  call MPI_Init (ierr); ! Iniciar MPI, hace que todo el entorno MPI se ponga a funcionar, si haces una llamada fuera de init y finalize de mpi no funciona

  call MPI_Comm_size (MPI_COMM_WORLD, npr,ierr) ! Cuantos procesos hay cuando has mandado ejecutar el programa, en npr devuelve cuantos procesos hay
  call MPI_Comm_rank (MPI_COMM_WORLD, rank,ierr) ! De los npr procesos, rank tendra "quien eres tu" de los npr procesos, numero de clon

  call MPI_Get_processor_name (nombre, lnom, ierr) ! Dice el nombre del nodo en el que estas 

  A = A + 1; ! A es local para todos los clones...todos imprimen 22
 ! & para decirle a fortran que la instruccion continua en la siguiente linea
 ! print *," >> Proc", rank," de ",npr," nodo ",nombre," A =", A
  print '(A,I3,A,I3,A,A20,A,I3)'," >> Proc ",rank, " de ",npr,&
          " nodo ",nombre," A= ",a
 call  MPI_Finalize (ierr); ! Terminar MPI
endprogram pi

