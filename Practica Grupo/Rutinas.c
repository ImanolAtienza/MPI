/********************************************************************
    Rutinas.c
*********************************************************************/

#include "defines.h"

int i, j = 1;
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

int lecturaFDatos(int *I, int *J, int tI, int tV, int *sendC_I, int n, int r) {
  int auxPosC = 0;
  int pos = 0;
  float posF, posC;

  I[0] = 0;
  if(r == root)
    for(i = 0; i < tV; i++) {
      for(j = 0; j <= n-1; j++) {
        for(k = 0; k < sendC_I[j]; k++) {
          fscanf (fp, "%f %f", &posF, &posC); 
          J[i] = (int) (posF - 1);
          if(((int) (posC - 1)) != auxPosC) {
            auxPosC = (int) (posC - 1);
            I[j] = i;
            j++;
          }
        }
        pos = 0;
        MPI_Pack(I, sendC_I[j], MPI_INT, buf, sizeof(buf), &pos, comm);
        MPI_Pack(J, sendC_J[j], MPI_INT, buf, sizeof(buf), &pos, comm);
        MPI_Send(buf, sizeof(buf), MPI_PACKED, j, 0, comm);
        free(I);
        free(J);
      }
    } 
  else {
    MPI_Recv(buf, sizeof(buf), MPI_PACKED, root, 0, comm, &status);
    MPI_Unpack(buf, sizeof(buf), &pos, I, sendC_I[r], MPI_INT, comm);
    MPI_Unpack(buf, sizeof(buf), &pos, J, sendC_J[r], MPI_INT, comm);
  }

      I[tI] = tV;

//  printf("Soy %d, sendcounts_I %d, displ_I %d\n", rank, sendcounts_I[rank], displ_I[rank]);
    auxpR_I = malloc((sendC_I[rank] + 1) * sizeof(int)); 

    MPI_Scatterv(pR_I, sendC_I, displ_I, MPI_INT, auxpR_I, sendC_I[rank], MPI_INT, root, comm); 

  // Reparto de pR_J, solo lo hace el root porque los demas procesos no conocen pR_I
    if (rank == root) {
      for (i=0; i<=nprocs-1; i++){
        sendcounts_J[i] = pR_I[sendC_I[i]+displ_I[i]]-pR_I[displ_I[i]]; 
        displ_J[i] = pR_I[displ_I[i]];
//      if (rank == root)
//        printf(" rank %d ->   displ_J: %d   sendcounts_J: %d\n", i, displ_J[i], sendcounts_J[i]);
      } 
    }

    MPI_Scatter(sendcounts_J, 1, MPI_INT, &tamJ, 1, MPI_INT, root, comm);
    printf("Soy %d, tamJ %d\n", rank, tamJ);
    auxpR_I[sendC_I[rank]] = tamJ;

    auxpR_J = malloc(tamJ*sizeof(int));
    MPI_Scatterv(pR_J, sendcounts_J, displ_J, MPI_INT, auxpR_J, tamJ, MPI_INT, root, comm);

    fclose (fp);
    return 0;
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

    return 0;
  }