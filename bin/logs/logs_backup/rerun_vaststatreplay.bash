#!/usr/bin/env bash
for folder in $(ls)
do
echo $folder
cd $folder
mkdir logs
mkdir logs/results
mv *.txt logs
cp ../../../VASTreal.ini ./
cp ../../../Mininet.ini ./
vaststat_replay
cp logs/results/results1.txt ./results/
rm logs/results/results1.txt
cd ..
done
