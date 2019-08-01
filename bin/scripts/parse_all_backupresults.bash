#!/usr/bin/env bash

for file in $(find $HOME/Development/VAST-0.4.6/bin/logs/logs_horus | grep results1.txt)
do
echo $file
plot_results.py $file
done
