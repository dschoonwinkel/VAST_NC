#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS
from plot_results_summary_utils import *
import re


x_axis_interval = 2


if len(sys.argv) >= 3:
    input_file_realOVS = sys.argv[1]
    input_file_Mininet = sys.argv[2]

else:
    print("Usage: compare_realOVS_Mininet_summary.py <realOVS_summary.txt> <Mininet_summary.txt>")
    exit(1)

np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})

realOVS_header, realOVS_results_nparray = readFileRetNumpyArray(input_file_realOVS)
# print(realOVS_results_nparray)

# Only Mininet results, latency results
print("*******************\nMininet LOSS0 plot 4 NODES REAL_OVS")
realOVSSubset = subsetByColumnValue(realOVS_results_nparray, PLATFORM, REAL_OVS)
realOVSSubset = subsetByColumnValue(realOVSSubset, LOSS_PERC, 0)
realOVSSubset = subsetByColumnValue(realOVSSubset, DELAY_MS, 0)
realOVSSubset = subsetByColumnValue(realOVSSubset, NODES_COUNT, 4)
colors = ['blue', 'red', 'green']
linestyles = [':', '-.', '--']
if hasMatplotlib:
    plot.figure()

custom_lines = list()
custom_lines_names = list()

# print(realOVSSubset)
DescriptionString = "0LOSS, 4nodes, realOVS"


# for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):
i = 4
if i == 4:

    print(NET_MODEL_STRINGS[i])
    NETMODEL_subset = subsetByColumnValue(realOVSSubset, NET_MODEL, i)
    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, DELAY_MS, AVG_TOPO_CONS, DescriptionString)
    print(xColumnList)
    print(len(xColumnList))
    if hasMatplotlib and len(xColumnList) > 0:
        

        ax = boxPlotHelper(511, i, xColumnList+0.3*(i-2), yColumnList, '', 'Topo Cons\n[%]', width=0.3, color=colors[i-2])
        ax.title.set_text("TCP vs UDP vs UDPNC. 0\% LOSS, 4 NODES, REAL_OVS")
        plot.xlim([np.min(xColumnList)-0.3, np.max(xColumnList)+0.3*(i-2)+0.3])
        ax.set_xticks(xColumnList)
        ax.tick_params(labelbottom=False)
        # ax.set_yticks(np.arange(99.2, 100.2, 0.2))


        from matplotlib.lines import Line2D
        custom_lines.append(Line2D([0], [0], color=colors[i-2], linestyle=linestyles[i-2], lw=1))
        if NET_MODEL_STRINGS[i] == 'net_ace':
            custom_lines_names.append("TCP")
        else:
            custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

        ax.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, AVG_DRIFT, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(512, i, xColumnList+0.3*(i-2), yColumnList, '', 'Drift dist\n[units]', width=0.3, color=colors[i-2])
        plot.xlim([np.min(xColumnList)-0.3, np.max(xColumnList)+0.3*(i-2)+0.3])
        ax.set_xticks(xColumnList)
        ax.tick_params(labelbottom=False)
        ax.set_yticks(np.arange(1.5, 4, 0.5))

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, NICSEND_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(513, i, xColumnList+0.3*(i-2), yColumnList, '', 'NIC Send\n[kBps]', width=0.3, color=colors[i-2])
        plot.xlim([np.min(xColumnList)-0.3, np.max(xColumnList)+0.3*(i-2)+0.3])
        ax.set_xticks(xColumnList)
        ax.tick_params(labelbottom=False)

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, NICRECV_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(514, i, xColumnList+0.3*(i-2), yColumnList, '', 'NIC Recv\n[kBps]', width=0.3, color=colors[i-2])
        plot.xlim([np.min(xColumnList)-0.3, np.max(xColumnList)+0.3*(i-2)+0.3])
        ax.set_xticks(xColumnList)
        ax.set_xticklabels(xColumnList)
        ax.tick_params(labelbottom=False)

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, LOSS_PERC, LATENCY, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(515, i, xColumnList+0.3*(i-2), yColumnList, 'LOSS_PERC', 'Latency\n[ms]', width=0.3, color=colors[i-2])
        plot.xlim([np.min(xColumnList)-0.3, np.max(xColumnList)+0.3*(i-2)+0.3])
        ax.set_xticks(xColumnList)
        ax.set_xticklabels(xColumnList)

if hasMatplotlib:
    plot.savefig("compare_realOVS_Mininet_LOSS0_4NODES.pdf", dpi=1200)
    plot.show()
