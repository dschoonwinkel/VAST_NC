#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS, NET_ACE, NET_UDP, NET_UDPNC
from plot_results_summary_utils import *
import re
import tabulate

if len(sys.argv) > 1:
    input_file_results = sys.argv[1]

else:
    print("Usage: plot_results_summary2.py <results_summary.txt>")
    exit(1)

np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})

results_header, results_results_nparray = readFileRetNumpyArray(input_file_results)
# print(results_results_nparray)


MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)

for SUBSETNODE_COUNT in [10, 20, 30, 40, 50]:

    print("\nNode count: ", SUBSETNODE_COUNT, "\linebreak")
    print("Average topology consistency [\%]")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString="%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex')
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    print(tabulate_result)

    print("Average normalized drift distance [VE units]")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, AVG_DRIFT, "AVG_DRIFT", XUnitsString="%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex')
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    print(tabulate_result)


