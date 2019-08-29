#!/usr/bin/env bash
echo "Rsyncing results_summary.txt"
rsync -v ~/Development/VAST-0.4.6/bin/results_summary/results_summary.txt schoonwi@10.10.11.81:~/Development/VAST-0.4.6/bin/logs/logs_$(hostname)/
extract_results.py
echo "Rsyncing results_extracted"
rsync --progress -vr ~/Development/VAST-0.4.6/bin/results_extracted/* schoonwi@10.10.11.81:~/Development/VAST-0.4.6/bin/logs/logs_$(hostname)/results_extracted/
