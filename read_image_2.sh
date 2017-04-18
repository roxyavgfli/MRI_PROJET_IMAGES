#!/bin/bash
[ -e "result_clust.bin" ] && rm result_clust.bin
while IFS='' read -r line || [[ -n "$line" ]]; do
    ./read_image -ic $line
    echo '.'
done < "$1"
