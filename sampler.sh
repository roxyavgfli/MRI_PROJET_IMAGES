#!/bin/bash

make -b
rm samples.txt
for file in `ls /media/lucas/DATA/MRI_RESSOURCE/sift/`
do
	echo "Traitement de $file"
	./sampler "/media/lucas/DATA/MRI_RESSOURCE/sift/$file"  | sed -re 's/; /;/g' | cut -d';' -f2- | cut -d';' -f1 >> samples.txt 
done
