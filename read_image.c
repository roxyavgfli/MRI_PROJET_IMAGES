#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "rdjpeg.h"
#define REDBIN 4
#define BLUEBIN 4
#define GREENBIN 4


int main(int argc, char *argv[])
{
  int i,j,n,nx,ny,nb;
  CIMAGE cim;
  float hist[REDBIN * GREENBIN * BLUEBIN];
  int r,g,b;
  int indice;

  //Initialisation de des valeur de l'histogramme a 0
  for (i = 0; i<REDBIN * GREENBIN * BLUEBIN; i++){
    hist[i] = 0;
    //printf("%f ", hist[i]);
  }

  /*------------------------------------------------*/
  /* lecture d'une image requête                    */
  /*------------------------------------------------*/
  read_cimage(argv[1],&cim);
  /*------------------------------------------------*/
  /* affichage des valeurs pour le premier bloc 8x8 */
  /* comme exemple de traitement                    */
  /*------------------------------------------------*/
  /*------------------------------------------------*/

  //traitement des pixel un par un et remplissage de l'histogram
  for (j = 0; j < cim.ny; j++) {       /* ligne par ligne */
    for (i = 0; i < cim.nx; i++) {   /* pixel par pixel */
      r = cim.r[i][j]/64;
      b = cim.b[i][j]/64;
      g = cim.g[i][j]/64;
      indice = r + 4 * g + 16 * b;
      //printf("%d ", indice);
      hist[indice]++;
    }
  }
  for (i = 0; i<REDBIN * GREENBIN * BLUEBIN; i++){
    hist[i] = hist[i]/(cim.nx * cim.ny);
  }
  FILE * result = fopen("result.bin", "a");
  fwrite(hist, sizeof(float), REDBIN*BLUEBIN*GREENBIN, result);
  fclose(result);
  exit(0);
}
