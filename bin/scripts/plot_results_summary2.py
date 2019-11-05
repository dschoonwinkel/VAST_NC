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

# Only Mininet results
print("*******************\nMininet LOSS plot 50 NODES Mininet")
resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 0)
resultsSubset = subsetLessThanByColumnValue(resultsSubset, LOSS_PERC, 15)
resultsSubset = excludeByColumnValue(resultsSubset, LOSS_PERC, 1)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency_TCPUDP(resultsSubset, LOSS_PERC, "LOSS_PERC", "TCP vs UDP, 50 NODES, 0-10% LOSS_PERC, Mininet")
plot.savefig("results_summary_Mininet_LOSSX_50NODES_TCPUDP.pdf", dpi=1200, pad_inches=2)

resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 0)
resultsSubset = excludeByColumnValue(resultsSubset, LOSS_PERC, 1)
resultsSubset = excludeByColumnValue(resultsSubset, LOSS_PERC, 2)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency_UDPUDPNC(resultsSubset, LOSS_PERC, "LOSS_PERC", "UDP vs UDPNC, 50 NODES, 0-10% LOSS_PERC, Mininet")
plot.savefig("results_summary_Mininet_LOSSX_50NODES_UDPUDPNC.pdf", dpi=1200, pad_inches=2)

print("*******************\nMininet LOSS0, 50 NODES, DELAY0 Mininet")
resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 0)
resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 0)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency_TCPUDP(resultsSubset, DELAY_MS, "DELAY_MS", "TCP vs UDP, LOSS 0%, 50 NODES, 0 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS0_DELAY0_50NODES_TCPUDP.pdf", dpi=1200, pad_inches=2)
plot_TopoCon_Drift_BW_Latency_UDPUDPNC(resultsSubset, DELAY_MS, "DELAY_MS", "UDP vs UDPNC, LOSS 0%, 50 NODES, 0 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS0_DELAY0_50NODES_UDPUPDNC.pdf", dpi=1200, pad_inches=2)
plot_TopoCon_Drift_BW_Latency_allModels(resultsSubset, DELAY_MS, "DELAY_MS", "TCP vs UDP vs UDPNC, LOSS 0%, 50 NODES, 0 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS0_DELAY0_50NODES_allModels.pdf", dpi=1200, pad_inches=2)


print("*******************\nMininet DELAY plot, 10% LOSS 50 NODES Mininet")
resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 10)
resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
plot_TopoCon_Drift_BW_Latency_TCPUDP(resultsSubset, DELAY_MS, "DELAY_MS", "TCP vs UDP, LOSS 10%, 50 NODES, 0-200 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS10_DELAYX_50NODES_TCPUDP.pdf", dpi=1200, pad_inches=2)
plot_TopoCon_Drift_BW_Latency_UDPUDPNC(resultsSubset, DELAY_MS, "DELAY_MS", "UDP vs UDPNC, LOSS 10%, 50 NODES, 0-200 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS10_DELAYX_50NODES_UDPUPDNC.pdf", dpi=1200, pad_inches=2)


print("*******************\nMininet Scaling plot, 10% LOSS 20-60 NODES Mininet")
resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 10)
resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 0)
plot_TopoCon_Drift_BW_Latency_TCPUDP(resultsSubset, NODES_COUNT, "NODES_COUNT", "TCP vs UDP, LOSS 10%, 20-60 NODES, 0 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS10_DELAY0_NODESX_TCPUDP.pdf", dpi=1200, pad_inches=2)
plot_TopoCon_Drift_BW_Latency_UDPUDPNC(resultsSubset, NODES_COUNT, "NODES_COUNT", "UDP vs UDPNC, LOSS 10%, 20-60 NODES, 0 ms Delay, Mininet")
plot.savefig("results_summary_Mininet_LOSS10_DELAY0_NODESX_UDPUDPNC.pdf", dpi=1200, pad_inches=2)



# Need more delay results to make this work...
# print("*******************\nMininet LOSS vs DELAY plot 50 NODES Mininet")
# if hasMatplotlib:
#     plot.figure()

# resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 10)
# resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
# TCPsubset = subsetByColumnValue(resultsSubset, NET_MODEL, NET_MODEL_STRINGS.index('net_ace'))

# resultsSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# resultsSubset = subsetByColumnValue(resultsSubset, LOSS_PERC, 0)
# resultsSubset = subsetByColumnValue(resultsSubset, DELAY_MS, 20)
# resultsSubset = subsetByColumnValue(resultsSubset, NODES_COUNT, 50)
# TCP2subset = subsetByColumnValue(resultsSubset, NET_MODEL, NET_MODEL_STRINGS.index('net_ace'))

# ax1 = plot_TopoCon_Drift_BW_Latency(TCPsubset, 0, DELAY_MS, "DELAY_MS", "TCP, LOSS 10%, 50 NODES, DELAYX, Mininet")
# plot_TopoCon_Drift_BW_Latency(TCP2subset, 1, DELAY_MS, "DELAY_MS", "TCP, LOSS 0%, 50 NODES, 20ms Delay, Mininet")

# custom_lines = list()
# custom_lines_names = list()

# custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
# custom_lines_names.append("TCP 0 PERC LOSS, DELAY X")
# ax1.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})

# if hasMatplotlib:
#         plot.savefig("results_summary_Mininet_LOSS10_DELAYX_50NODES.pdf", dpi=1200, pad_inches=2)

if hasMatplotlib:
        plot.show()
