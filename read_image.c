#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "rdjpeg.h"
#include "proc.h"
#define REDBIN 4
#define BLUEBIN 4
#define GREENBIN 4

float abso (float a){
  if(a<0){a = -a;}
  return a;
}

void histo (float* hist, CIMAGE cim, int taille){
  int i, j, r, g, b, indice;
  //Initialisation de des valeur de l'histogramme a 0
  for (i = 0; i<taille; i++){
    hist[i] = 0;
  }
  //Remplissage avec l'image Pixel par Pixel.
  for (j = 0; j < cim.ny; j++) {       /* ligne par ligne */
    for (i = 0; i < cim.nx; i++) {   /* pixel par pixel */
      r = cim.r[i][j]/taille;
      b = cim.b[i][j]/taille;
      g = cim.g[i][j]/taille;
      indice = r + REDBIN * g + REDBIN * GREENBIN * b;
      hist[indice]++;
    }
  }
  for (i = 0; i<taille; i++){
    hist[i] = hist[i]/(cim.nx * cim.ny);
  }
}

int main(int argc, char *argv[])
{
  if(argc != 3){
    printf("Veuillez utilisez le programme de cette manière :\n ./read_image <option> <url>\noption : \n-i pour indexer\n-r pour la recherche\n");
    exit(1);
  }

  if(strcmp(argv[1], "-i") != 0 && strcmp(argv[1], "-r") != 0 ){
    printf("option : \n-i pour indexer\n-r pour la recherche\n");
    exit(1);
  }

  int i,j,n,nx,ny,nb;
  CIMAGE cim;
  int taille = REDBIN * BLUEBIN * GREENBIN;
  float hist[taille];

  /*------------------------------------------------*/
  /* lecture d'une image requête                    */
  /*------------------------------------------------*/
  read_cimage(argv[2],&cim);

  //traitement de l'histogram
  histo(hist, cim, taille);
  
  if(strcmp(argv[1], "-i") == 0){
    printf("Indexation\n");
    //FILE * result = fopen("result.bin", "a");
    //fwrite(hist, sizeof(float), taille, result);
    //fclose(result);
  } else if (strcmp(argv[1], "-r") == 0){
    printf("Comparaison\n");
    float distance = 0;
    
    // initialisation du tableau de Key avec les urls
    int n;
    char ** tab_urls = readList("urls.txt" , &n);
    KEY tab_key[n];
    for (i = 0; i<n; i++){
      tab_key[i].k=i;//Sinon toute les valeur de k valent 0.
    }

    float hist_to_compare[taille];
    FILE * hist_file = fopen("result.bin", "r");
       

    for(i = 0; i<n; i++){
      //Récupération de histogramme dans le ficier result.bin
      fread(hist_to_compare, sizeof(float), taille, hist_file);

      //calcul de la distance
      for(j = 0; j<taille; j++){
        distance = distance + abso(hist[j] * hist[j] - hist_to_compare[j] * hist_to_compare[j]); // distance euclidienne au carré
      }
      //On met la distance calculer a sa place dans le tableau
      tab_key[i].d = distance;
      distance = 0;
    }
    //Tri du tableau de KEY
    qsort(tab_key, n, sizeof(KEY), keyCompare);
    for (i = 0; i<10; i++){
      printf("<IMG SRC=\"%s\"'>\n",tab_urls[tab_key[i].k]);
    }
    fclose(hist_file); // On oublie pas de fermer ce qu'on a ouvert
  }
  exit(0);
}
