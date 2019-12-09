from plot_result_utils import NET_MODEL_STRINGS, NET_ACE, NET_UDP, NET_UDPNC, NET_MODEL_STRINGS_THESIS
from plot_results_summary_utils import *
import re
import tabulate

def tabulate_TopoCon_Drift_BW_Latency(results_subset, NETMODELs, subset_count, xProp, XUnitsString=r"\%", reverseXAxis=False):
    print(r"\begin{flushleft}")
    print("Node count: ", subset_count, r"\linebreak")
    print(r"Median of average topology consistency [\%]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, NETMODELs, xProp, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalized drift distances [VE units]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, NETMODELs, xProp, AVG_DRIFT, "AVG_DRIFT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of average latencies [ms]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs(NODES_subset, NETMODELs, xProp, LATENCY, "LATENCY", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised VAST send bandwidth [kBps/node]")
    print(r"\end{flushleft}")

    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised NIC send bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICSEND_BYTES, "NICSEND_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised NIC receive bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICRECV_BYTES, "NICRECV_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

def tabulate_TopoCon_Drift_BW_Latency_degradation(results_subset, NETMODELs, subset_count, xProp, XUnitsString=r"\%", reverseXAxis=False):
    print(r"\begin{flushleft}")
    print("Node count: ", subset_count, r"\linebreak")
    print(r"Median of average topology consistency [\%]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_degradation(NODES_subset, NETMODELs, xProp, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalized drift distances [VE units]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_degradation(NODES_subset, NETMODELs, xProp, AVG_DRIFT, "AVG_DRIFT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of average latencies [ms]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_degradation(NODES_subset, NETMODELs, xProp, LATENCY, "LATENCY", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised VAST send bandwidth [kBps/node]")
    print(r"\end{flushleft}")

    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised NIC send bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICSEND_BYTES, "NICSEND_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalised NIC receive bandwidth [kBps/node]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICRECV_BYTES, "NICRECV_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

def tabulate_TopoCon_Drift_BW_Latency_scaling(results_subset, NETMODELs, xProp, reverseXAxis=False):
    print(r"\begin{flushleft}")
    print(r"Median of average topology consistency [\%]")
    print(r"\end{flushleft}")
    header, line_list, xColumnList = tabulateNETMODELs(results_subset, NETMODELs, xProp, AVG_TOPO_CONS, "AVG_TOPO_CONS", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalized drift distances [VE units]")
    print(r"\end{flushleft}")
    header, line_list, xColumnList = tabulateNETMODELs(results_subset, NETMODELs, xProp, AVG_DRIFT, "AVG_DRIFT", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of average latencies [ms]")
    print(r"\end{flushleft}")
    header, line_list, xColumnList = tabulateNETMODELs(results_subset, NETMODELs, xProp, LATENCY, "LATENCY", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of average VAST send bandwidth [kBps/node] and total bandwidth of network")
    print(r"\end{flushleft}")

    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive_scaling(results_subset, NETMODELs, xProp, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Send Bandwidth [kBps/node] and total bandwidth of network")
    print(r"\end{flushleft}")
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive_scaling(results_subset, NETMODELs, xProp, NICSEND_BYTES, "NICSEND_BYTES", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Average NIC Receive Bandwidth [kBps/node] and total bandwidth of network")
    print(r"\end{flushleft}")
    header, line_list, xColumnList = tabulateNETMODELsNICSendReceive_scaling(results_subset, NETMODELs, xProp, NICRECV_BYTES, "NICRECV_BYTES", reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{rlrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    print(tabulate_result)
    

def tabulate_TopoCon_Drift_BW_Latency_PLATFORMS(results_subset, NETMODEL, PLATFORMS, subset_count, xProp, XUnitsString=r"\%", reverseXAxis=False):
    print(r"\begin{flushleft}")
    print("Node count: ", subset_count, r"\linebreak")
    print(r"Median of average topology consistency [\%]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_PLATFORM(NODES_subset, NETMODEL, PLATFORMS, xProp, AVG_TOPO_CONS, "AVG_TOPO_CONS", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of normalized drift distances [VE units]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_PLATFORM(NODES_subset, NETMODEL, PLATFORMS, xProp, AVG_DRIFT, "AVG_DRIFT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    print(r"\begin{flushleft}")
    print("Median of average latencies [ms]")
    print(r"\end{flushleft}")
    NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    header, line_list, xColumnList = tabulateNETMODELs_PLATFORM(NODES_subset, NETMODEL, PLATFORMS, xProp, LATENCY, "LATENCY", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    tabulate_result = re.sub("{llll}", r"{|c|cc|c|}", tabulate_result)
    print(tabulate_result)

    # print(r"\begin{flushleft}")
    # print("Average VAST Send Bandwidth [kBps/node]")
    # print(r"\end{flushleft}")

    # NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    # header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, AVG_WORLDSENDSTAT, "AVG_WORLDSENDSTAT", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    # tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    # tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    # tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    # print(tabulate_result)

    # print(r"\begin{flushleft}")
    # print("Average NIC Send Bandwidth [kBps/node]")
    # print(r"\end{flushleft}")
    # NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    # header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICSEND_BYTES, "NICSEND_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    # tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    # tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    # tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    # print(tabulate_result)

    # print(r"\begin{flushleft}")
    # print("Average NIC Receive Bandwidth [kBps/node]")
    # print(r"\end{flushleft}")
    # NODES_subset = subsetByColumnValue(results_subset, NODES_COUNT, subset_count)
    # header, line_list, xColumnList = tabulateNETMODELsNICSendReceive(NODES_subset, NETMODELs, subset_count, xProp, NICRECV_BYTES, "NICRECV_BYTES", XUnitsString=XUnitsString, reverseXAxis=reverseXAxis)
    # tabulate_result = tabulate.tabulate(line_list, tablefmt='latex_raw', headers="firstrow")
    # tabulate_result = re.sub("pm", r"$\\pm$", tabulate_result)
    # tabulate_result = re.sub("{llrlrl}", r"{|c|cc|cc|c|}", tabulate_result)
    # print(tabulate_result)
