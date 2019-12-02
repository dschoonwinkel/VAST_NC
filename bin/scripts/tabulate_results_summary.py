#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS, NET_ACE, NET_UDP, NET_UDPNC, NET_MODEL_STRINGS_THESIS
from plot_results_summary_utils import *
import re
import tabulate
from tabulate_results_utils import tabulate_TopoCon_Drift_BW_Latency, tabulate_TopoCon_Drift_BW_Latency_scaling


if len(sys.argv) > 1:
    input_file_results = sys.argv[1]

else:
    print("Usage: plot_results_summary2.py <results_summary.txt>")
    exit(1)

np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})

results_header, results_results_nparray = readFileRetNumpyArray(input_file_results)

print(r"TCP vs UDP, 50 Nodes, 0-70\% Packet Loss, 0 Network Delay [ms]")
MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
NETMODELs = [NET_ACE, NET_UDP]

SUBSETNODE_COUNT = 50
tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")

print("\n\n\n***************************************************\n\n\n")
print(r"TCP vs UDP, 50 Nodes, 10\% Packet Loss, 0-200 ms Network Delay [ms]")
MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

NETMODELs = [NET_ACE, NET_UDP]

SUBSETNODE_COUNT = 50
tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")




print("\n\n\n***************************************************\n\n\n")
print(r"TCP vs UDP, 20-100 Nodes, 10\% Packet Loss, 0 Network Delay [ms]")
NETMODELs = [NET_ACE, NET_UDP]

MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)



print("\n\n\n***************************************************\n\n\n")
print("TCP vs UDP vs UDPNC, 50 Nodes 0\% Packet Loss, 0 Network Delay [ms]")
MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
SUBSETNODE_COUNT = 50
NETMODELs = [NET_ACE, NET_UDP, NET_UDPNC]
tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")







print("\n\n\n***************************************************\n\n\n")
print("UDP vs UDPNC, 50 Nodes 0-70% Packet Loss, 0 Network Delay [ms]")
MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
SUBSETNODE_COUNT = 50
NETMODELs = [NET_UDP, NET_UDPNC]
tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")


print("\n\n\n***************************************************\n\n\n")
print("UDP vs UDPNC, 50 Nodes, 10% Packet Loss, 0-200 ms Network Delay [ms]")
MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
SUBSETNODE_COUNT = 50
NETMODELs = [NET_UDP, NET_UDPNC]
tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")


print("\n\n\n***************************************************\n\n\n")
print("UDP vs UDPNC, 20-100 Nodes, Packet Loss 10, 0 Network Delay [ms]")
NETMODELs = [NET_UDP, NET_UDPNC]

MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)