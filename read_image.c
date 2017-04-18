#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>

#include "rdjpeg.h"
#include "proc.h"
#define REDBIN 4
#define BLUEBIN 4
#define GREENBIN 4

float abso (float a){
  if(a<0){a = -a;}
  return a;
}

/* fonction utilisateur de comparaison fournie a qsort() */
static int compare_val (void const *a, void const *b)
{
   KEY const *pa = a;
   KEY const *pb = b;

   return pa->d - pb->d;
}

/*char* siftName (char* urlName) {
  char* toReturn;
	char copy[300];
	char part1[300];
	char part2[100];
	strcpy(part1, "./sift/test/1nn/");
	strcpy(copy, urlName);
	toReturn = strtok(copy,"http://mrim.imag.fr/voc10/images/");
	printf("token 1 : %s", toReturn);
	toReturn = strtok(NULL,".jpg");
	return (strcat(part1, strcat(toReturn,".sift")));
}*/

int getNumberUrl (char* url) {
	int n, i;
	char ** tab_urls = readList("urls.txt" , &n);
  KEY tab_key[n];
  for (i = 0; i<n; i++){
    tab_key[i].k=i;//Sinon toute les valeur de k valent 0.
    if (strcmp(url, tab_urls[tab_key[i].k]) == 0){
    	// printf("%s \n", tab_urls[tab_key[i].k]);
    	// printf("%d \n", i);
    	return i;
    }
  }
  return -1;
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

void histo_clust(float* hist, char* filename, int taille){

  int val, i;
  //Initialisation de des valeur de l'histogramme a 0
  for (i = 0; i<taille; i++){
    hist[i] = 0;
  }

  FILE * file = fopen(filename, "r");
  while( !feof (file)){
    fscanf(file ,"%d", &val);
    hist[val]++;
  }
  fclose(file);
}


int main(int argc, char *argv[])
{
  if(argc != 3){
    printf("Veuillez utilisez le programme de cette manière :\n ./read_image <option> <url>\noption : \n-i pour indexer\n-r pour la recherche\n -ic pour l'Indexation des cluster\n -rc pour la rechecher via cluster\n");
    exit(1);
  }

  if(strcmp(argv[1], "-i") != 0 && strcmp(argv[1], "-r") != 0 && strcmp(argv[1], "-rc") != 0 && strcmp(argv[1], "-ic") != 0 && strcmp(argv[1], "-tst") != 0){
    printf("option : \n-i pour indexer\n-r pour la recherche\n -ic pour l'Indexation des cluster\n -rc pour la rechecher via cluster\n");
    exit(1);
  }  
  
  if(strcmp(argv[1], "-i") == 0){
    printf("Indexation\n");
    
    /*------------------------------------------------*/
    /* lecture d'une image requête                    */
    /*------------------------------------------------*/
    CIMAGE cim;
    read_cimage(argv[2],&cim);

    //traitement de l'histogram
    int taille = REDBIN * BLUEBIN * GREENBIN;
    float hist[taille];
    histo(hist, cim, taille);

    FILE * result = fopen("result.bin", "a");
    fwrite(hist, sizeof(float), taille, result);
    fclose(result);

  } else if (strcmp(argv[1], "-r") == 0){
    printf("Comparaison\n");

    int i,j,n;
    float distance = 0;

    /*------------------------------------------------*/
    /* lecture d'une image requête                    */
    /*------------------------------------------------*/
    CIMAGE cim;
    read_cimage(argv[2],&cim);

    //traitement de l'histogram
    int taille = REDBIN * BLUEBIN * GREENBIN;
    float hist[taille];
    histo(hist, cim, taille);
    
    // initialisation du tableau de Key avec les urls
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
  } else if (strcmp(argv[1], "-rc") == 0){
    
    printf("recherche par cluster\n");
    int number_in_list = getNumberUrl(argv[2]);
    
    int i,j,n;
    
    char ** tab_urls = readList("urls.txt" , &n);
    KEY tab_key[n];
    for (i = 0; i<n; i++){
      tab_key[i].k=i;//Sinon toute les valeur de k valent 0.
    }
    float distance = 0;
    
    //traitement de l'histogram
    int taille = 256;
    float hist_osef[taille];
    float hist_sift_reference [taille];
    float hist_sift_compare [taille];
    
    FILE * hist_file = fopen("result_clust.bin", "r");
       

    for(i = 0; i<n; i++){
      //Récupération de histogramme dans le ficier result.bin
      fread(hist_osef, sizeof(float), taille, hist_file);
      if (i == number_in_list - 1){
      	// printf("on est là : %d", number_in_list);
      	// printf("on est ici : %d", i);
      	fread(hist_sift_reference, sizeof(float), taille, hist_file);
      }
    }
    
    fclose(hist_file); // On oublie pas de fermer ce qu'on a ouvert
    hist_file = fopen("result_clust.bin", "r");
    
    
    //for(j = 0; j < taille; j++)
    	//printf("%f \n", hist_sift_reference[j]);
    
    for(i = 0; i<n; i++){
      //Récupération de histogramme dans le ficier result.bin
      fread(hist_sift_compare, sizeof(float), taille, hist_file);
      //calcul de la distance
      for(j = 0; j<taille; j++){
        distance = distance + abso(hist_sift_reference[j] * hist_sift_reference[j] - hist_sift_compare[j] * hist_sift_compare[j]); // distance euclidienne au carré
      }
      //On met la distance calculer a sa place dans le tableau
      // printf("distance : %f	", distance);
      tab_key[i].d = distance;
      distance = 0;
    }
    //Tri du tableau de KEY
    qsort(tab_key, n, sizeof(KEY), compare_val);
    for (i = 0; i<10; i++){
      printf("<IMG style='width: 300px;' SRC=\"%s\"'>\n",tab_urls[tab_key[i].k]);
      printf("distance : %f",tab_key[i].d);
    }
    fclose(hist_file); // On oublie pas de fermer ce qu'on a ouvert
    
    
    
    
    
    
    /**********
    		FIN
    **********/
    

  } else if (strcmp(argv[1], "-ic") == 0){
    printf("Indexation par cluster\n");
    
    int taille = 256;
    
    //création de l'histogramme du cluster
    float hist_clust[taille];
    histo_clust(hist_clust, argv[2], taille);

    //Ecriture du fichier.
    FILE * result = fopen("result_clust.bin", "a");
    fwrite(hist_clust, sizeof(float), taille, result);
    fclose(result);
  } else if (strcmp(argv[1], "-tst") == 0){
  	// printf("name : %s \n", siftName(argv[2]));
  	printf("Recherche combinée\n");
  	
  	
  	int number_in_list = getNumberUrl(argv[2]);
    
    int i,j,n;
    int i2,j2,n2;
    
    char ** tab_urls = readList("urls.txt" , &n);
    KEY tab_key[n];
    for (i = 0; i<n; i++){
      tab_key[i].k=i;//Sinon toute les valeur de k valent 0.
    }
    float distance = 0;
    float distance2 = 0;
    
    //traitement de l'histogram
    int taille = 256;
    CIMAGE cim;
    read_cimage(argv[2],&cim);

    //traitement de l'histogram
    int taille2 = REDBIN * BLUEBIN * GREENBIN;
    float hist_ref2[taille];
    histo(hist_ref2, cim, taille2);
    
    float hist_osef[taille];
    float hist_sift_reference [taille];
    float hist_sift_compare [taille];
    
    float hist_to_compare[taille];
    FILE * hist_file1 = fopen("result.bin", "r");
    
    FILE * hist_file = fopen("result_clust.bin", "r");
       

    for(i = 0; i<n; i++){
      //Récupération de histogramme dans le ficier result.bin
      fread(hist_osef, sizeof(float), taille, hist_file);
      if (i == number_in_list - 1){
      	// printf("on est là : %d", number_in_list);
      	// printf("on est ici : %d", i);
      	fread(hist_sift_reference, sizeof(float), taille, hist_file);
      }
    }
    
    fclose(hist_file); // On oublie pas de fermer ce qu'on a ouvert
    hist_file = fopen("result_clust.bin", "r");
    
    
    //for(j = 0; j < taille; j++)
    	//printf("%f \n", hist_sift_reference[j]);
    
    for(i = 0; i<n; i++){
      //Récupération de histogramme dans le ficier result.bin
      fread(hist_sift_compare, sizeof(float), taille, hist_file);
      fread(hist_to_compare, sizeof(float), taille2, hist_file1);
      //calcul de la distance
      for(j = 0; j<taille; j++){
        distance = distance + abso(hist_sift_reference[j] * hist_sift_reference[j] - hist_sift_compare[j] * hist_sift_compare[j]);
      }
      //printf("taille2 %d", taille2);
      for(j = 0; j<taille2; j++){
      	//printf("s \n");
      	// printf("val : %f", hist_to_compare[j]);
      	distance = distance + 1000 * abso(hist_ref2[j] * hist_ref2[j] - hist_to_compare[j] * hist_to_compare[j]);
      }
      //On met la distance calculer a sa place dans le tableau
      // printf("distance : %f	", distance);
      tab_key[i].d = distance;
      distance = 0;
    }
    //Tri du tableau de KEY
    qsort(tab_key, n, sizeof(KEY), compare_val);
    for (i = 0; i<10; i++){
      printf("<IMG style='width: 300px;' SRC=\"%s\"'>\n",tab_urls[tab_key[i].k]);
      printf("distance : %f",tab_key[i].d);
    }
    fclose(hist_file); // On oublie pas de fermer ce qu'on a ouvert
  }
  exit(0);
}
