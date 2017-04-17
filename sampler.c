#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

//Affiche l'utilisation de l'éxécutable
void affiche_utilisation(){
	printf("./sampler <nom_du_fichier_a_sampler> \n");
}


int main (int argc,char *argv[]){
	int compteur = 150;
	int i;
	char* in_filename;
	char ligne[512];


	if (argc != 2){
		affiche_utilisation();
		exit(1);
	}

	in_filename = argv[1];
	FILE * in_file = fopen(in_filename, "r");

	for (i = 0; i < 3; i++){
		fgets(ligne, sizeof(ligne), in_file);
	}

	while(fgets(ligne, sizeof(ligne), in_file) != NULL){
		if(compteur == 150){
			printf("%s", ligne);
			compteur = 0;
		} else {
			compteur++;
		}
	}
}