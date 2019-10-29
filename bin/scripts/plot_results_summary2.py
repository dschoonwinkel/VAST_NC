#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS
from plot_results_summary_utils import *
import re
from os.path import expanduser

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

input_file_results = '%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir

if len(sys.argv) > 1:
    input_file_results = sys.argv[1]

else:
    print("Usage: plot_results_summary2.py <results_summary.txt>")
    exit(1)

np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})

results_header, results_results_nparray = readFileRetNumpyArray(input_file_results)
# print(results_results_nparray)

# Only Mininet results, latency results
print("*******************\nMininet LOSS0 plot 50 NODES Mininet")
resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 0)
resultsSubset = subsetLessThanByColumnValue(resultsSubset, LOSS_PERC, 15)
resultsSubset = excludeByColumnValue(resultsSubset, LOSS_PERC, 1)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency(resultsSubset, LOSS_PERC, "LOSS_PERC", "TCP vs UDP vs UDPNC, 50 NODES, 0-10\% LOSS_PERC, Mininet")


resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 10)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency(resultsSubset, DELAY_MS, "DELAY_MS", "TCP vs UDP vs UDPNC, LOSS 10\%, 50 NODES, 0-200 ms Delay, Mininet")

if hasMatplotlib:
        plot.show()
