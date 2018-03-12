/************************************************************************
    repartos.c
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>


int main (int argc, char **argv)
{
  int  N, Nloc, npr, resto, i, cuantos;

  printf ("\n Datos para el reparto:  N y npr\n");
  scanf ("%d %d" , &N, &npr);

  int  tam[npr], desp[npr];
  Nloc = ceil ((float)N / (float)npr);

//  REPARTO
  cuantos=0;
  for (i=0; i<npr; i++)
  {
    if (cuantos + Nloc <= N)
    {
     desp[i] = Nloc * i; tam[i] = Nloc;  cuantos += Nloc;
    }
    else
    {
      desp[i] = Nloc * i; tam[i] = N -  cuantos;
      break;
    }
  }
  for (i++; i<npr; i++)
    {
      desp[i] = N; tam[i] = 0;
    }
  printf ("\n  REPARTO \n");
  for (i=0; i<npr; i++) printf ("\n %d %d   %d  \n", i, tam[i], desp[i]);

 // Primer REPARTO -> El último se queda con el resto

	resto = N % npr;
	Nloc = (N-resto) /npr;
	for(i=0; i<npr; i++)
	{
		desp[i] = Nloc * i;
		tam[i] = Nloc;
	}
	tam[npr-1] += resto;

  printf ("\n PRIMER REPARTO \n");
  for (i=0; i<npr; i++) printf ("\n %d %d   %d  \n", i, tam[i], desp[i]);

// SEGUNDO REPARTO -> Los primeros se reparten el resto

        resto = N % npr;
        Nloc = (N-resto) / npr;
	desp[0]=0;
        for(i=0; i<npr; i++)
        {
		tam[i] = Nloc;
		if(resto>0){
			tam[i] += 1;
			resto--;
		}
		if(i>0)desp[i] = desp[i-1]+tam[i-1];
	}

  printf ("\n SEGUNDO REPARTO \n");
  for (i=0; i<npr; i++) printf ("\n %d %d   %d  \n", i, tam[i], desp[i]);

// TERCER REPARTO
  for (i=0; i<npr; i++)
  {
    tam[i] = (i+1)*N/npr - i*N/npr;
    desp[i] = i*N/npr;
  }

  printf ("\n TERCER REPARTO \n");
  for (i=0; i<npr; i++) printf ("\n %d %d   %d  \n", i, tam[i], desp[i]);

  return 0;
}

