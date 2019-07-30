#!/usr/bin/env bash

cd logs/logs_backup

for output in $(find . | grep results1.txt)
do 

# echo $output 

NAME=$output
NAME2=${NAME:7}
NAME3=${NAME2/"results/"/}
NAME4=${NAME3/'/'/_}

echo $NAME4

mkdir results_extracted
cp $NAME results_extracted/$NAME4

done