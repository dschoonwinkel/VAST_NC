#!/usr/bin/env bash

for file in $(find $HOME/Development/VAST-0.4.6/bin/logs/logs_backup | grep results1.txt)
do
echo $file
plot_results_quick.py $file
done
