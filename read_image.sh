#!/bin/bash
[ -e "result.bin" ] && rm result.bin
while IFS='' read -r line || [[ -n "$line" ]]; do
    ./read_image $line
    echo '.'
done < "$1"
