#!/bin/bash
[ -e "result.bin" ] && rm result.bin
while IFS='' read -r line || [[ -n "$line" ]]; do
    ./read_image -i $line
    echo '.'
done < "$1"
