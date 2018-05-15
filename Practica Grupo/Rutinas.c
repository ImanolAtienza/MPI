/********************************************************************
    Rutinas.c
*********************************************************************/

#include "defines.h"

int i, j;
FILE *fp;
MPI_Status status;

int lecturaFichero(char *file, int *tU, int *tI, int *tV) {
  if(fp = fopen (file, "r")) {
    // Obtener numeros de usuarios
    fscanf (fp, "%d", tU);    
    printf("Numero de usuarios %d\n", *tU);

    // Obtener numero de items y crear vector de items
    fscanf (fp, "%d", tI); 
    printf("Numero de items %d\n", *tI);

    // Crear vector de usuarios con respecto a items vistos/comprados
    fscanf (fp, "%d", tV); 
    printf("Numero de items vistos %d\n\n", *tV);

    return 0;
  }
  return 1;
}

int empaquetadoEnvio(int *tI, int *tV, int *tU, int r) {
  int pos = 0;
  char buf[N], auxBuf[N];

  MPI_Pack(tU, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
  MPI_Pack(tI, 1, MPI_INT, buf, sizeof(buf), &pos, comm);
  MPI_Pack(tV, 1, MPI_INT, buf, sizeof(buf), &pos, comm);

  pos = 0;
  MPI_Scatter(buf, sizeof(buf), MPI_PACKED, auxBuf, sizeof(auxBuf), MPI_PACKED, root, comm);
  //MPI_Bcast(buf, sizeof(buf), MPI_PACKED, root, comm);
  MPI_Unpack(auxBuf, sizeof(buf), &pos, tU, 1, MPI_INT, comm);
  MPI_Unpack(auxBuf, sizeof(buf), &pos, tI, 1, MPI_INT, comm);
  MPI_Unpack(auxBuf, sizeof(buf), &pos, tV, 1, MPI_INT, comm);

  free(buf);
  free(auxBuf);
  return 0;
}