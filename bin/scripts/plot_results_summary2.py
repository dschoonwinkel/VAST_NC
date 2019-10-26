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
if hasMatplotlib:
    plot.figure()

custom_lines = list()
custom_lines_names = list()

# print(resultsSubset)
DescriptionString = "LOSS, 50nodes, Mininet"


for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

    print(NET_MODEL_STRINGS[i])
    NETMODEL_subset = subsetByColumnValue(resultsSubset, NET_MODEL, i)
    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, AVG_TOPO_CONS, DescriptionString)
    print(xColumnList)
    print(len(xColumnList))
    if hasMatplotlib and len(xColumnList) > 0:
        

        ax = boxPlotHelper(511, i, xColumnList, yColumnList, '', 'Topo Cons\n[%]', width=0.3)
        ax.title.set_text("TCP vs UDP vs UDPNC, 50 NODES, Mininet")
        # ax.set_yticks(np.arange(99.2, 100.2, 0.2))


        from matplotlib.lines import Line2D
        custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
        if NET_MODEL_STRINGS[i] == 'net_ace':
            custom_lines_names.append("TCP")
        else:
            custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

        ax.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, AVG_DRIFT, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(512, i, xColumnList, yColumnList, '', 'Drift dist\n[units]', width=0.3)
        # ax.set_yticks(np.arange(1.5, 4, 0.5))

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, NICSEND_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(513, i, xColumnList, yColumnList, '', 'NIC Send\n[kBps]', width=0.3)

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, NICRECV_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(514, i, xColumnList, yColumnList, '', 'NIC Recv\n[kBps]', width=0.3)

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, LATENCY, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(515, i, xColumnList, yColumnList, 'LOSS_PERC', 'Latency\n[ms]', width=0.3, setXTickLabel=True)

if hasMatplotlib:
    plot.savefig("results_summary_Mininet_LOSS_50NODES.pdf", dpi=1200)
    plot.show()
