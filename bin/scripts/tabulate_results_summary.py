#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS, NET_ACE, NET_UDP, NET_UDPNC, NET_MODEL_STRINGS_THESIS
from plot_results_summary_utils import *
import re
import tabulate
from tabulate_results_utils import tabulate_TopoCon_Drift_BW_Latency, tabulate_TopoCon_Drift_BW_Latency_scaling, tabulate_TopoCon_Drift_BW_Latency_PLATFORMS,tabulate_TopoCon_Drift_BW_Latency_degradation


if len(sys.argv) > 1:
    input_file_results = sys.argv[1]

else:
    print("Usage: plot_results_summary2.py <results_summary.txt>")
    exit(1)

np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})

results_header, results_results_nparray = readFileRetNumpyArray(input_file_results)

# print(r"TCP vs UDP, 50 Nodes, 0-70\% Packet Loss, 0 Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# NETMODELs = [NET_ACE, NET_UDP]

# SUBSETNODE_COUNT = 50
# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")



# print(r"TCP vs UDP, 50 Nodes, 0-70\% Packet Loss, 0 Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# NETMODELs = [NET_ACE, NET_UDP]

# SUBSETNODE_COUNT = 50
# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")


# print("\n\n\n***************************************************\n\n\n")
# print(r"TCP vs UDP, 50 Nodes, 10\% Packet Loss, 0-200 ms Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

# NETMODELs = [NET_ACE, NET_UDP]

# SUBSETNODE_COUNT = 50
# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")


# print("\n\n\n***************************************************\n\n\n")
# print(r"TCP vs UDP, 50 Nodes, 10\% Packet Loss, 0-200 ms Network Delay [ms] Degradation")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

# NETMODELs = [NET_ACE, NET_UDP]

# SUBSETNODE_COUNT = 50
# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")



# print("\n\n\n***************************************************\n\n\n")
# print(r"TCP vs UDP, 20-100 Nodes, 10\% Packet Loss, 0 Network Delay [ms]")
# NETMODELs = [NET_ACE, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

# tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)

# print("\n\n\n***************************************************\n\n\n")
# print("TCP vs UDP, 50 Nodes, Packet Loss 0, Bandwidth limit [MBps]")
# NETMODELs = [NET_ACE, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# MininetSubset = excludeByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, 50, BW_LIMIT, XUnitsString=r"MBps", reverseXAxis=True)


# print("\n\n\n***************************************************\n\n\n")
# print("TCP vs UDP, 50 Nodes, Packet Loss 0, Bandwidth limit [MBps], Degradation")
# NETMODELs = [NET_ACE, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# MininetSubset = excludeByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, 50, BW_LIMIT, XUnitsString=r"MBps", reverseXAxis=True)










# print("\n\n\n***************************************************\n\n\n")
# print("TCP vs UDP vs UDPNC, 50 Nodes 0\% Packet Loss, 0 Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# SUBSETNODE_COUNT = 50
# NETMODELs = [NET_ACE, NET_UDP, NET_UDPNC]
# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")












# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes 0-70% Packet Loss, 0 Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# SUBSETNODE_COUNT = 50
# NETMODELs = [NET_UDP, NET_UDPNC]
# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")

# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes 0-70% Packet Loss, 0 Network Delay [ms], DEGRADATION")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# SUBSETNODE_COUNT = 50
# NETMODELs = [NET_UDP, NET_UDPNC]
# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")



# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes, 10% Packet Loss, 0-200 ms Network Delay [ms]")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# SUBSETNODE_COUNT = 50
# NETMODELs = [NET_UDP, NET_UDPNC]
# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")


# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes, 10% Packet Loss, 0-200 ms Network Delay [ms] Degradation")
# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# SUBSETNODE_COUNT = 50
# NETMODELs = [NET_UDP, NET_UDPNC]
# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, SUBSETNODE_COUNT, DELAY_MS, XUnitsString=r"ms")


print("\n\n\n***************************************************\n\n\n")
print("UDP vs UDPNC, 20-100 Nodes, Packet Loss 10, 0 Network Delay [ms]")
NETMODELs = [NET_UDP, NET_UDPNC]

MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)
MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)
MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)

tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)





# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes, Packet Loss 0, Bandwidth limit [MBps]")
# NETMODELs = [NET_UDP, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# MininetSubset = excludeByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency(MininetSubset, NETMODELs, 50, BW_LIMIT, XUnitsString=r"MBps", reverseXAxis=True)



# print("\n\n\n***************************************************\n\n\n")
# print("UDP vs UDPNC, 50 Nodes, Packet Loss 0, Bandwidth limit [MBps] Degradation")
# NETMODELs = [NET_UDP, NET_UDPNC]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = excludeByColumnValue(MininetSubset, NODES_COUNT, 4)
# MininetSubset = excludeByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency_degradation(MininetSubset, NETMODELs, 50, BW_LIMIT, XUnitsString=r"MBps", reverseXAxis=True)













# print(r"Mininet Hosts vs Physical Hosts, 4 Nodes, 0-20\% Packet Loss, 0 Network Delay [ms] TCP")
# Subset = subsetByColumnValue(results_results_nparray, DELAY_MS, 0)
# Subset = subsetByColumnValue(Subset, BW_LIMIT, 0)
# Subset = subsetByColumnValue(Subset, NODES_COUNT, 4)
# NETMODEL = [NET_ACE]

# SUBSETNODE_COUNT = 4
# PLATFORMS = [1,3]
# tabulate_TopoCon_Drift_BW_Latency_PLATFORMS(Subset, NETMODEL, PLATFORMS, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")


# print(r"\n\nMininet Hosts vs Physical Hosts, 4 Nodes, 0-20\% Packet Loss, 0 Network Delay [ms] UDP")
# NETMODEL = [NET_UDP]
# SUBSETNODE_COUNT = 4
# PLATFORMS = [1,3]
# tabulate_TopoCon_Drift_BW_Latency_PLATFORMS(Subset, NETMODEL, PLATFORMS, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")


# print(r"\n\nMininet Hosts vs Physical Hosts, 4 Nodes, 0-20\% Packet Loss, 0 Network Delay [ms] UDPNC")
# NETMODEL = [NET_UDPNC]
# SUBSETNODE_COUNT = 4
# PLATFORMS = [1,3]
# tabulate_TopoCon_Drift_BW_Latency_PLATFORMS(Subset, NETMODEL, PLATFORMS, SUBSETNODE_COUNT, LOSS_PERC, XUnitsString=r"\%")




# print("\n\n\n***************************************************\n\n\n")
# print("TCP vs UDP, 50 Nodes, Packet Loss 0 [MBps]")
# NETMODELs = [NET_ACE, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, NODES_COUNT, 50)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)



# print("\n\n\n***************************************************\n\n\n")
# print("TCP vs UDP, 100 Nodes, Packet Loss 0 [MBps]")
# NETMODELs = [NET_ACE, NET_UDP]

# MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
# MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, DELAY_MS, 0)
# MininetSubset = subsetByColumnValue(MininetSubset, NODES_COUNT, 100)
# MininetSubset = subsetByColumnValue(MininetSubset, BW_LIMIT, 0)

# tabulate_TopoCon_Drift_BW_Latency_scaling(MininetSubset, NETMODELs, NODES_COUNT)
