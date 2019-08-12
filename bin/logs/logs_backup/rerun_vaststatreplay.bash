#!/usr/bin/env bash
for folder in $(ls)
do
echo $folder
if [ -d $folder ];
then
:
else
    echo "$folder is not a directory"
    continue
fi

cd ./$folder
mkdir logs
mkdir logs/results
mv *.txt logs
cp -n ../../../VASTreal.ini ./
cp -n ../../../Mininet.ini ./
vaststat_replay
cp logs/results/results1.txt ./results/
rm logs/results/results1.txt
cd ..
done
