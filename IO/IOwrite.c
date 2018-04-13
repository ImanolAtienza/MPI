/********************************************************************

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

int main (int argc, char** argv)
{
  int pid, npr,root,pos,opcionnumesc,opcionescritura;
  MPI_Offset disp;
  MPI_File fh;
  MPI_Comm comm;

  int *submat,n_fil,n_col;
  #define TAMBUF 12
  char buffer[TAMBUF];

  int i, n_iguales, n_Elementos;
  int Nloc, resto;
  int  *tam, *desp;
  MPI_Datatype tipo_fila;

  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &pid);
  MPI_Comm_size (MPI_COMM_WORLD, &npr);

  root = 0;
  if (pid==root)
  {
      printf ("Numero filas\n"); fflush(stdout);
      scanf ("%d",&n_fil);
      printf ("Numero columnas\n"); fflush(stdout);
      scanf ("%d",&n_col);
      printf ("OpNumeroEscritores (0: root, 1: todos)\n"); fflush(stdout);
      scanf ("%d",&opcionnumesc);
    }

  pos=0;
  MPI_Pack (&n_fil,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
  MPI_Pack (&n_col,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
  MPI_Pack (&opcionnumesc,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
   MPI_Bcast (buffer,TAMBUF,MPI_PACKED,
             0,MPI_COMM_WORLD);

  pos=0;
  MPI_Unpack (buffer,TAMBUF,&pos,
            &n_fil,1,MPI_INT,
            MPI_COMM_WORLD);
  MPI_Unpack (buffer,TAMBUF,&pos,
            &n_col,1,MPI_INT,
            MPI_COMM_WORLD);
  MPI_Unpack (buffer,TAMBUF,&pos,
            &opcionnumesc,1,MPI_INT,
            MPI_COMM_WORLD);

  Nloc = n_fil / npr;
  resto = n_fil % npr;

  if (pid<resto) Nloc ++;

  if (opcionnumesc==0) // solo el root
  {
    if (pid==root)
    {
     submat  = malloc(sizeof(int)*n_fil*n_col);
     submat[0:n_fil*n_col]=pid+1;
    }
    else
    {
     submat  = malloc(sizeof(int)*1);
     submat[0:1]=pid+1;
    }
  }
  else
  {
    submat  = malloc(sizeof(int)*Nloc*n_col);
    submat[0:Nloc*n_col]=pid+1;
  }

  tam = malloc(sizeof(int)*npr);
  desp = malloc(sizeof(int)*npr);

  Nloc = n_fil / npr;
  resto = n_fil % npr;
  for (i=0; i<npr; i++)
  {
    tam[i] = Nloc;
    if (i < resto) tam[i]++;
    desp[i] = tam[i] * i;
    if (i>=resto) desp[i] += resto;
  }

  MPI_Type_contiguous (n_col,MPI_INT,&tipo_fila);
  MPI_Type_commit(&tipo_fila);

  if (opcionnumesc==1) // todos escriben
  {
   printf ("Escribe el clon %d\n",pid); fflush(stdout);
   comm = MPI_COMM_WORLD;
   MPI_File_open(comm,argv[1],MPI_MODE_CREATE|MPI_MODE_WRONLY,
                MPI_INFO_NULL,&fh);
   disp = desp[pid]*n_col*sizeof(int);
   MPI_File_seek  (fh, disp, MPI_SEEK_SET);
   MPI_File_write (fh, submat, tam[pid], tipo_fila,
                        MPI_STATUS_IGNORE);
   MPI_File_close(&fh);
  }
  else
  {
        if (pid==root)
        {
          printf ("Escribe el root %d\n",pid); fflush(stdout);
          comm = MPI_COMM_SELF;
          MPI_File_open(comm,argv[1],MPI_MODE_CREATE|MPI_MODE_WRONLY,
                MPI_INFO_NULL,&fh);
          disp = 0;
          MPI_File_write_at(fh,disp,submat,n_fil,tipo_fila,
                     MPI_STATUS_IGNORE);
          MPI_File_close(&fh);
        }
   }

  free(submat); free(tam); free(desp);
  MPI_Finalize ();
  return (0);
} /*  main  */




