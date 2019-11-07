#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS, NET_ACE, NET_UDP, NET_UDPNC, NET_MODEL_STRINGS_THESIS
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

    print(r"\begin{flushleft}")
    print("\nNode count: ", SUBSETNODE_COUNT, r"\linebreak")
    print(r"Average topology consistency [\%]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average normalized drift distance [VE units]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, AVG_DRIFT, "AVG_DRIFT", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average VAST Send Bandwidth [kBps/node]")
    print(r"\end{flushleft}")

    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Send Bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, NICSEND_BYTES, "NICSEND_BYTES", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Receive Bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, NICRECV_BYTES, "NICRECV_BYTES", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average Latency [ms]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], LOSS_PERC, LATENCY, "LATENCY", XUnitsString=r"\%")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)









MininetSubset = subsetByColumnValue(results_results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, LOSS_PERC, 10)

for SUBSETNODE_COUNT in [10, 20, 30, 40, 50]:

    print(r"\begin{flushleft}")
    print("\nNode count: ", SUBSETNODE_COUNT, r"\linebreak")
    print(r"Average topology consistency [\%]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average normalized drift distance [VE units]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, AVG_DRIFT, "AVG_DRIFT", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average VAST Send Bandwidth [kBps/node]")
    print(r"\end{flushleft}")

    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Send Bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, NICSEND_BYTES, "NICSEND_BYTES", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Receive Bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, NICRECV_BYTES, "NICRECV_BYTES", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average Latency [ms]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(MininetSubset, NODES_COUNT, SUBSETNODE_COUNT)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, [NET_ACE, NET_UDP], DELAY_MS, LATENCY, "LATENCY", XUnitsString=r"ms")
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{lll}", r"{c|cc}", tabulate_result)
    print(tabulate_result)




