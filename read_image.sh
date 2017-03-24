#!/bin/bash


path="/home/lucas/Polytech/S8/MRI/Projet/"

while IFS='' read -r line || [[ -n "$line" ]]; do
  echo "$line"
done < "$1"
 
echo "Finished"
