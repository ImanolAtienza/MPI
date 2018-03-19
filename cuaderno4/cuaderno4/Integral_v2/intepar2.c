/********************************************************************************
	intepar2.c
        Integral de una funcion mediante sumas de areas de trapecios
        Broadcast para el envio de datos y Reduce para la recepcion
*********************************************************************************/

#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define M_PI 3.14159265358979323846

double   t0, t1;

void   Leer_datos (double* a, double* b, int* n);
double Integrar (double a_loc, double b_loc, int n_loc, double w);
double f (double x);


int main (int argc, char** argv)
{
  int         pid, npr, root;    	// Identificador y numero de proc, y nodo raiz.
  double      a, b, w, a_loc, b_loc;
  int         n, n_loc, resto;
  double      resul, resul_loc;		// Resultado de la integral


  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);


// Lectura y distribucion de parametros a todos los procesadores

  if (pid == 0) {
    Leer_datos (&a, &b, &n);
    t0 = MPI_Wtime ();
  }
  // Cambiarlo por 1 bcast
  MPI_Bcast (&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast (&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

// Reparto
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


// Suma de resultados parciales con el reduce

  resul = 0.0; root = 0;
  MPI_Reduce (&resul_loc, &resul, 1, MPI_DOUBLE, MPI_SUM, root, MPI_COMM_WORLD);
  resul = (resul  + ((f (a) + f (b)) / 2.0))*w;

// Impresion de resultados

  if (pid == 0)
  {
    t1 = MPI_Wtime ();
    printf ("\n  Integral (= ln x+1 + atan x), de %1.1f s %1.1f, %d trap.) = %1.12f\n",a,b,n,resul);
    printf ("\n  Integral intervalo [0..1] --> PI/4+ln(2)  %1.12f, dif = %1.12E\n", M_PI/4.0 + log(2.0),M_PI/4.0 + log(2.0)-resul);
    printf ("  Tiempo ejecucion (%d proc.) = %1.3f ms \n\n", npr, (t1-t0)*1000);
  }

  MPI_Finalize ();
  return (0);
} /*  main  */




// FUNCION Leer_datos
void Leer_datos (double* a, double* b, int* n)
{
  float aux_a, aux_b;

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

  y = 1.0 / (x + 1.0) + 1.0 / (x*x + 1.0);
  return (y);
}
