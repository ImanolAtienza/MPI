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
  int pid, npr,root,pos,opcion,opcionnumlec;
  MPI_Offset disp;
  MPI_File fh;

  int *mat,*wmat,n_fil,n_col;
  #define TAMBUF 16
  char buffer[TAMBUF];

  int i,j,k, n_iguales, n_Elementos;
  int Nloc, resto;
  int  *tam, *desp;
  MPI_Datatype tipo_fila;
  MPI_Comm comm;

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
      printf ("OpNumeroLectores (0: root, 1: todos)\n");
      scanf ("%d",&opcionnumlec);
      printf ("Opcion (sacan por pantalla lo leido 1: nproc; 2: root)\n"); fflush(stdout);
      scanf ("%d",&opcion);
   }

  pos=0;
  MPI_Pack (&n_fil,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
  MPI_Pack (&n_col,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
  MPI_Pack (&opcionnumlec,1,MPI_INT,
            buffer,TAMBUF,&pos,MPI_COMM_WORLD);
  MPI_Pack (&opcion,1,MPI_INT,
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
            &opcionnumlec,1,MPI_INT,
            MPI_COMM_WORLD);
  MPI_Unpack (buffer,TAMBUF,&pos,
            &opcion,1,MPI_INT,
            MPI_COMM_WORLD);

  Nloc = n_fil / npr;
  resto = n_fil % npr;

  if (pid<resto) Nloc ++;

  mat  = malloc(sizeof(int)*n_fil*n_col);

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

   if (opcionnumlec==0)
   {
       if (pid==root)
       {
         wmat  = malloc(sizeof(int)*n_fil*n_col);
         printf ("Lee el root %d\n",pid); fflush(stdout);
         comm = comm = MPI_COMM_SELF;
         MPI_File_open(comm,argv[1],MPI_MODE_RDONLY,
                MPI_INFO_NULL,&fh);
         disp = 0;
         MPI_File_seek  (fh, disp, MPI_SEEK_SET);
         MPI_File_read(fh,wmat,n_fil,tipo_fila,
                     MPI_STATUS_IGNORE);
         MPI_File_close(&fh);
       }
       MPI_Scatterv(wmat,tam,desp,tipo_fila,
                   &mat[desp[pid]*n_col],tam[pid],tipo_fila,
                   root,MPI_COMM_WORLD);
       free(wmat);
   }
   else
   {
      printf ("Lee el clon %d\n",pid); fflush(stdout);
      comm = MPI_COMM_WORLD;
      MPI_File_open(MPI_COMM_WORLD,argv[1],MPI_MODE_RDONLY,
                MPI_INFO_NULL,&fh);
      disp = desp[pid]*n_col*sizeof(int);
      MPI_File_read_at(fh,disp,&mat[desp[pid]*n_col],tam[pid],tipo_fila,
                     MPI_STATUS_IGNORE);
      MPI_File_close(&fh);
   }

 if (opcion ==1) // escribe cada clon lo suyo
 {
  for (k=0;k<npr;k++)
   if (pid==k)
   {
    for (i=desp[k];i<desp[k]+tam[k];i++, printf ("\n"))
    {
     printf("%d ",i);
     for (j=0;j<n_col;j++)
       printf ("%4d ",mat[i*n_col+j]);
    }

   }
 }
 else  // todos reciben lo que tienen cada uno
 {
  MPI_Allgatherv(MPI_IN_PLACE,0, MPI_DATATYPE_NULL,
                 mat,tam,desp,tipo_fila,
                 MPI_COMM_WORLD);

   if (pid==root)
   {
    for (i=0;i<n_fil;i++, printf ("\n"))
    {
     for (j=0;j<n_col;j++)
       printf ("%4d ",mat[i*n_col+j]);
    }
   }
 }



  free(mat); free(tam); free(desp);
  MPI_Finalize ();
  return (0);
} /*  main  */




