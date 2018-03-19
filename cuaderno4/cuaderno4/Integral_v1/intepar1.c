/***********************************************************************
	intepar1.c
        Integral de una funcion mediante sumas de areas de trapecios
        Funciones punto a punto: bucle de send y receive
************************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <math.h>

#define M_PI 3.14159265358979323846

double   t0, t1;

void   Leer_datos (double* a, double* b, int* n);
double Integrar (double a_loc, double b_loc, int n_loc, double w);
double f (double x);


int main (int argc, char** argv)
{
  int         pid, npr;    		// Identificador y numero de proc.
  double      a, b, w, a_loc, b_loc;
  int         n, n_loc, resto, kont;
  double      resul, resul_loc;		// Resultado de la integral

  int         i,origen, destino, tag;
  MPI_Status  status;


  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);


// Lectura y distribucion de parametros a todos los procesadores

  if (pid == 0)
  {

    Leer_datos (&a, &b, &n);

    t0 = MPI_Wtime ();
    for (destino=1; destino<npr; destino++)
    {
      tag = 0;
	  // Arreglar por Isend
      MPI_Send (&a, 1, MPI_DOUBLE, destino, tag, MPI_COMM_WORLD);
      MPI_Send (&b, 1, MPI_DOUBLE, destino, tag, MPI_COMM_WORLD);
      MPI_Send (&n, 1, MPI_INT, destino, tag, MPI_COMM_WORLD);
    }
  }
  else
  {
    origen = 0; tag = 0;
    MPI_Recv (&a, 1, MPI_DOUBLE, origen, tag, MPI_COMM_WORLD, &status);
    MPI_Recv (&b, 1, MPI_DOUBLE, origen, tag, MPI_COMM_WORLD, &status);
    MPI_Recv (&n, 1, MPI_INT, origen, tag, MPI_COMM_WORLD, &status);
  }

  // Reparto, mirar si cada uno se lleva lo que le toca
  w = (b-a) / n;  	// cada uno calcula el trozo que tiene que sumar
  n_loc = n / npr;
  resto = n % npr;

  if (pid < resto) n_loc = n_loc + 1;

  a_loc = a + pid * n_loc * w;
  if (pid >= resto) a_loc = a_loc + resto * w;
  b_loc = a_loc + n_loc * w;

  if (pid==npr-1) n_loc=n_loc-1;

// Calculo de la integral local
  resul_loc = Integrar (a_loc, b_loc, n_loc, w);


// Suma de resultados parciales
	// Reduce mal hecho, cambiar por reduce sum
  if (pid == 0)
  {
    resul = resul_loc;
    tag = 0;
    kont = 1;
    while (kont < npr)		// se reciven n-1 mensajes en cualquier orden
    {
      MPI_Recv (&resul_loc, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
      resul += resul_loc;
      kont++;
    }
    resul = (resul  + ((f (a) + f (b)) / 2.0))*w;
  }
  else
  {
    destino = 0; tag = 0;
    MPI_Send (&resul_loc, 1, MPI_DOUBLE, destino, tag, MPI_COMM_WORLD);
  }


// Impresion de resultados

  if (pid == 0)
  {
    t1 = MPI_Wtime ();
    printf ("\n  Integral (= ln x+1 + atan x), de %1.1f a %1.1f, %d trap. = %1.12f\n", a,b,n,resul);
    printf ("\n  Integral intervalo [0..1] --> PI/4+ln(2)  %1.12f, dif = %1.12E\n",
            M_PI/4.0 + log(2.0),M_PI/4.0 + log(2.0)-resul);
    printf ("  Tiempo ejecucion (%d proc.) = %1.3f ms \n\n", npr, (t1-t0)*1000);
  }

  MPI_Finalize ();
  return (0);
} /*  main  */




// FUNCION Leer_datos
void Leer_datos (double* a, double* b, int* n)
{
  float  aux_a, aux_b;

  printf ("\n  Introduce a, b (limites) y n (num. de trap.)  \n");
  scanf ("%f %f %d", &aux_a, &aux_b, n);

  (*a) = (double) aux_a;
  (*b) = (double) aux_b;
} /* Leer_datos */



// FUNCION Integrar: calculo local de la integral
double Integrar (double a_loc, double b_loc, int n_loc, double w)
{
  double resul_loc, x;
  int    i;

// calculo de la integral
  resul_loc = 0.0;

  x = a_loc;

  for (i=0; i<n_loc;i++ )
  {
    x = x + w;
    resul_loc = resul_loc + f (x);
  }
  //resul_loc = resul_loc * w;
  return (resul_loc);

} /*  Integrar  */



// FUNCION f: funcion a integrar
double f (double x)
{
  double y;

  y = 1.0 /(x + 1.0) + 1.0 / (x*x + 1.0);
  return (y);
}

