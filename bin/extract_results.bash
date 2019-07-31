#!/usr/bin/env bash

cd logs/logs_backup

for output in $(find . | grep results1.txt)
do 

# echo $output 

NAME=$output
echo $NAME
NAME2=${NAME:2}
NAME3=${NAME2/"results/"/}
NAME4=${NAME3/'/'/_}

echo $NAME4

mkdir ../../results_extracted &> /dev/null
cp $NAME ../../results_extracted/$NAME4

done
