import csv
import numpy as np
import scipy.stats as st
from plot_result_utils import NET_MODEL_STRINGS, NET_MODEL_STRINGS_THESIS
hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
    from matplotlib.ticker import AutoMinorLocator, FormatStrFormatter, LinearLocator, MaxNLocator
except ImportError:
    print("Matplotlib not available on this console")
    hasMatplotlib = False

FIRST_TIMESTAMP = 0
NET_MODEL = 1
NODES_COUNT = 2
BW_LIMIT = 3
DELAY_MS = 4
LOSS_PERC = 5
STEPS = 6
PLATFORM = 7
ACTIVE_NODES = 8
ACTIVE_MATCHERS = 9
AVG_TOPO_CONS = 10
AVG_DRIFT = 11
AVG_WORLDSENDSTAT = 12
AVG_WORLDRECVSTAT = 13
RAW_MCRECVBYTES = 14
USED_MCRECVBYTES = 15
NICSEND_BYTES = 16
NICRECV_BYTES = 17
LATENCY = 18

MININET = 1
DOCKER = 2
REAL_OVS = 3


def readFileRetNumpyArray(filename):
    results_text = list()

    with open(filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            results_text.append(row)
            # print(",".join(row))

    header = results_text[0]
    results_text = results_text[1:]


    results_nparray = np.array(results_text)
    results_nparray = results_nparray[:, :-1]
    results_nparray = np.array(results_nparray, dtype=np.float)

    return header, results_nparray




def seperateByColumn(results_matrix, xColumnIndex, yColumnIndex, tag):
    # print(results_matrix[:,xColumnIndex])
    # print("seperateByColumn: ", tag)
    xColumnList = np.unique(results_matrix[:,xColumnIndex])
    # print(xColumnList)
    yColumnList = list()
    for point in xColumnList:
        # print(point)
        # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)])
        # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)][:,yColumnIndex])
        yColumnList.append(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex])
        # print(point, "Count:", len(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        # print(point, "Avg: ", np.mean(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        # print(point, "Median: ", np.median(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        # print(point, "Std: ", np.std(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]), "\n")

    return xColumnList, yColumnList

def subsetByColumnValue(results_matrix, xColumnIndex, value):
    subset = results_matrix[np.where(results_matrix[:,xColumnIndex] == value)]
    # print(subset)

    return subset

def subsetLessThanByColumnValue(results_matrix, xColumnIndex, value):
    subset = results_matrix[np.where(results_matrix[:,xColumnIndex] < value)]
    # print(subset)

    return subset

def excludeByColumnValue(results_matrix, xColumnIndex, value):
    subset = results_matrix[np.where(results_matrix[:,xColumnIndex] != value)]
    # print(subset)

    return subset

def tabulateByNETMODEL(results_matrix, yColumnIndex, tag):
    xColumnList = np.unique(results_matrix[:,NET_MODEL])
    # print(xColumnList)

    line_list = list()
    # print(tag, end="\t\t")
    line_list.append(tag)

    for j in range(len(xColumnList)):
        # print(NET_MODEL_STRINGS[int(xColumnList[j])])
        # print(xColumnList[i], end='\t')
#         # print(xColumnList[i], yColumnList[j])
#         # print()
        xSubset = results_matrix[np.where(results_matrix[:,NET_MODEL] == xColumnList[j])]
        # print(xSubset[:,i])
        # print(("%3.2f" % np.mean(xSubset[:,yColumnIndex]))[0:5], end="\t")

        samples = xSubset[:,yColumnIndex]
        mean = np.mean(samples)
        median = np.median(samples)
        conf_req = 0.90
        t_value = st.t.ppf(1 - conf_req/2, len(samples)-1)
        std_err = np.std(samples)/np.sqrt(len(samples))
        margin = t_value * std_err
        # conf_int = st.t.interval(0.95, len(xSubset[:,yColumnIndex])-1, loc=np.median(xSubset[:,yColumnIndex]), scale=st.sem(xSubset[:,yColumnIndex]))

        line_list.append("%3.2f pm %3.2f" % (median, margin))
#         ySubset = xSubset[np.where(xSubset[:,yColumnIndex] == yColumnList[j])]
#         # print(ySubset)
#         # print(len(ySubset))
#         countMatrix[i,j] = len(ySubset)
#         print(countMatrix[i,j], end='\t')
    # print("")
    return line_list

default_colors = ['blue', 'red', 'green']
default_linestyles = [':', '-.', '--']
default_spacing = 0.35
default_width = 0.3

def boxPlotHelper(subplotLayout, BoxIndex, xColumnList, yColumnList, xlabel, ylabel, width=0, color='', setXTickLabel=False, offset=0, scaleX=True, other_ax=None):
    if hasMatplotlib:
        ax = plot.subplot(subplotLayout)
        if (len(xColumnList) > 0):
            
            if color == '':
                color = default_colors[(BoxIndex-2) % len(default_colors)]

            if (width == 0):
                width = 0.03*np.max(xColumnList)  
            # if (width < default_width):
            #     width = default_width

            spacing = 0.033*np.max(xColumnList)
            # if spacing < default_spacing:
            #     print("Using default_spacing")
            #     spacing = default_spacing 

            if len(xColumnList) == 1:
                width = default_width
                spacing = default_spacing 

            xColumnListOffset = xColumnList+spacing*(BoxIndex-2+offset) 

            if other_ax is not None:
                ax_twiny = other_ax.twiny()
                print("boxPlotHelper: twinning y")

            box = plot.boxplot(yColumnList, positions=xColumnListOffset, widths=width) 
            plot.setp(box['boxes'], color=color)
            plot.xlabel(xlabel)
            plot.ylabel(ylabel)  
            plot.grid(True, alpha=0.5)

            # print("scaleX", scaleX)
            if scaleX:
                plot.xlim([np.min(xColumnList)-spacing, np.max(xColumnList)+spacing*(BoxIndex-2)+spacing])
                plot.xticks(xColumnList, xColumnList)
                plot.tick_params(labelbottom=setXTickLabel)

            medians = list()
            for arr in yColumnList:
                medians.append(np.median(arr))
            if (len(medians) > 0):
                plot.plot(xColumnListOffset, medians, color=color, linestyle=default_linestyles[(BoxIndex-2) % len(default_linestyles)], linewidth=1)

        else:
            print("No data")

        return ax
    else:
        print("Matplotlib not installed")

def plot_TopoCon_Drift_BW_Latency(NETMODEL_subset, BoxIndex, XAxisProp, PropName, Title, DescriptionString="", offset=0, color='', ShowActiveNodes=False, ReverseAxis=False):

    if ShowActiveNodes:
        subplot_base = 601
        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, ACTIVE_NODES, DescriptionString)
        print(xColumnList)
        print(len(xColumnList))
        if hasMatplotlib and len(xColumnList) > 0:

            ax1 = boxPlotHelper(611, BoxIndex, xColumnList, yColumnList, '', 'Active Nodes\n[%]', offset=offset, color=color)
            ax1.title.set_text(Title)
            ax1.yaxis.set_major_locator(MaxNLocator(nbins=5))

            if ReverseAxis:
                ax1.invert_xaxis()

    else:
        subplot_base = 500

    
    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_TOPO_CONS, DescriptionString)
    print(xColumnList)
    print(len(xColumnList))
    if hasMatplotlib and len(xColumnList) > 0:

        ax = boxPlotHelper(subplot_base + 11, BoxIndex, xColumnList, yColumnList, '', 'Topo Cons\n[%]', offset=offset, color=color)
        if not ShowActiveNodes:
            ax.title.set_text(Title)
            ax1 = ax
        ax.yaxis.set_major_locator(MaxNLocator(nbins=5))

        if ReverseAxis:
                ax.invert_xaxis()


    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_DRIFT, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(subplot_base + 12, BoxIndex, xColumnList, yColumnList, '', 'Drift dist\n[units]', offset=offset, color=color)
        ax.yaxis.set_major_locator(MaxNLocator(nbins=5))

        if ReverseAxis:
                ax.invert_xaxis()

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, LATENCY, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax = boxPlotHelper(subplot_base + 13, BoxIndex, xColumnList, yColumnList, '', 'Latency\n[ms]', offset=offset, color=color)
        ax.yaxis.set_major_locator(MaxNLocator(nbins=5))

        if ReverseAxis:
                ax.invert_xaxis()


    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICSEND_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax3 = boxPlotHelper(subplot_base + 14, BoxIndex, xColumnList, yColumnList, '', 'NIC Send\n[kBps/node]', offset=offset, color=color)
        ax3.yaxis.set_major_locator(MaxNLocator(nbins=5))
        ax3.relim()
        ax3.autoscale()
        ylims_3 = ax3.get_ylim()
        print("ylims_3", ylims_3)
        ax3.set_xlim(ax1.get_xlim())

        if ReverseAxis:
                ax3.invert_xaxis()

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICRECV_BYTES, DescriptionString)
    if hasMatplotlib and len(xColumnList) > 0:
        
        ax4 = boxPlotHelper(subplot_base + 15, BoxIndex, xColumnList, yColumnList, PropName, 'NIC Recv\n[kBps/node]', setXTickLabel=True, offset=offset, color=color)
        ax4.yaxis.set_major_locator(MaxNLocator(nbins=5))
        ax4.relim()
        ax4.autoscale()

        ylims_4 = ax4.get_ylim()
        print("ylims_4", ylims_4)
        ylims = [np.min([ylims_3[0], ylims_4[0]]), np.max([ylims_3[1], ylims_4[1]])]
        print("ylims", ylims)
        ax3.set_ylim(ylims)
        print("After set ylims:", ax3.get_ylim())
        ax4.set_ylim(ylims)
        print("After set ylims:", ax4.get_ylim())
        ax4.set_xlim(ax1.get_xlim())

        if ReverseAxis:
                ax4.invert_xaxis()


    return ax1

def plot_TopoCon_Drift_BW_Latency_twiny(NETMODEL_subset, BoxIndex, XAxisProp, PropName, Title, DescriptionString="", offset=0, color=''):

    xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_TOPO_CONS, DescriptionString)
    print(xColumnList)
    print(len(xColumnList))
    if hasMatplotlib and len(xColumnList) > 0:
        pass
        ax1 = plot.subplot(511)
        xlims = ax1.get_xlim()

        boxPlotHelper(511, BoxIndex, xColumnList, yColumnList, '', 'Topo Cons\n[%]', offset=offset, color=color, other_ax=ax1)
        ax1.set_xlim(xlims)
        ax1.title.set_text(Title)
        # ax1.yaxis.set_major_locator(MaxNLocator(nbins=5))

    # xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_DRIFT, DescriptionString)
    # if hasMatplotlib and len(xColumnList) > 0:
        
    #     ax = boxPlotHelper(512, BoxIndex, xColumnList, yColumnList, '', 'Drift dist\n[units]', offset=offset, color=color)
    #     ax.yaxis.set_major_locator(MaxNLocator(nbins=5))

    # xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, LATENCY, DescriptionString)
    # if hasMatplotlib and len(xColumnList) > 0:
        
    #     ax = boxPlotHelper(513, BoxIndex, xColumnList, yColumnList, '', 'Latency\n[ms]', offset=offset, color=color)
    #     ax.yaxis.set_major_locator(MaxNLocator(nbins=5))


    # xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICSEND_BYTES, DescriptionString)
    # if hasMatplotlib and len(xColumnList) > 0:
        
    #     ax3 = boxPlotHelper(514, BoxIndex, xColumnList, yColumnList, '', 'NIC Send\n[kBps/node]', offset=offset, color=color)
    #     ax3.yaxis.set_major_locator(MaxNLocator(nbins=5))
    #     ax3.relim()
    #     ax3.autoscale()
    #     ylims_3 = ax3.get_ylim()
    #     print("ylims_3", ylims_3)
    #     ax3.set_xlim(ax1.get_xlim())

    # xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICRECV_BYTES, DescriptionString)
    # if hasMatplotlib and len(xColumnList) > 0:
        
    #     ax4 = boxPlotHelper(515, BoxIndex, xColumnList, yColumnList, PropName, 'NIC Recv\n[kBps/node]', setXTickLabel=True, offset=offset, color=color)
    #     ax4.yaxis.set_major_locator(MaxNLocator(nbins=5))
    #     ax4.relim()
    #     ax4.autoscale()

    #     ylims_4 = ax4.get_ylim()
    #     print("ylims_4", ylims_4)
    #     ylims = [np.min([ylims_3[0], ylims_4[0]]), np.max([ylims_3[1], ylims_4[1]])]
    #     print("ylims", ylims)
    #     ax3.set_ylim(ylims)
    #     print("After set ylims:", ax3.get_ylim())
    #     ax4.set_ylim(ylims)
    #     print("After set ylims:", ax4.get_ylim())
    #     ax4.set_xlim(ax1.get_xlim())


    # return ax1


def plot_TopoCon_Drift_BW_Latency_allModels(resultsSubset, XAxisProp, PropName, Title, DescriptionString="", ShowActiveNodes=False, ReverseAxis=False):
    if hasMatplotlib:
        plot.figure()

    custom_lines = list()
    custom_lines_names = list()


    for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

        print(NET_MODEL_STRINGS[i])
        NETMODEL_subset = subsetByColumnValue(resultsSubset, NET_MODEL, i)
        ax1 = plot_TopoCon_Drift_BW_Latency(NETMODEL_subset, i, XAxisProp, PropName, Title, DescriptionString="", ShowActiveNodes=ShowActiveNodes, ReverseAxis=ReverseAxis)

        from matplotlib.lines import Line2D
        custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
        if NET_MODEL_STRINGS[i] == 'net_ace':
            custom_lines_names.append("TCP")
        else:
            custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

        ax1.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})

def plot_TopoCon_Drift_BW_Latency_TCPUDP(resultsSubset, XAxisProp, PropName, Title, DescriptionString="", ShowActiveNodes=False, ReverseAxis=False):
    if hasMatplotlib:
        plot.figure()

    custom_lines = list()
    custom_lines_names = list()


    for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udp') + 1):

        print(NET_MODEL_STRINGS[i])
        NETMODEL_subset = subsetByColumnValue(resultsSubset, NET_MODEL, i)
        ax1 = plot_TopoCon_Drift_BW_Latency(NETMODEL_subset, i, XAxisProp, PropName, Title, DescriptionString="", ShowActiveNodes=ShowActiveNodes, ReverseAxis=ReverseAxis)

        from matplotlib.lines import Line2D
        custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
        if NET_MODEL_STRINGS[i] == 'net_ace':
            custom_lines_names.append("TCP")
        else:
            custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

        ax1.legend(custom_lines, custom_lines_names, loc='left', prop={'size':7}, ncol=2)
        # ax1.legend(custom_lines, custom_lines_names, prop={'size':7}, ncol=2)

def plot_TopoCon_Drift_BW_Latency_UDPUDPNC(resultsSubset, XAxisProp, PropName, Title, DescriptionString="", ShowActiveNodes=False, ReverseAxis=False):
    if hasMatplotlib:
        plot.figure()

    custom_lines = list()
    custom_lines_names = list()


    for i in range(NET_MODEL_STRINGS.index('net_udp'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

        print(NET_MODEL_STRINGS[i])
        NETMODEL_subset = subsetByColumnValue(resultsSubset, NET_MODEL, i)
        ax1 = plot_TopoCon_Drift_BW_Latency(NETMODEL_subset, i, XAxisProp, PropName, Title, DescriptionString="", offset=-1, ShowActiveNodes=ShowActiveNodes, ReverseAxis=ReverseAxis)

        from matplotlib.lines import Line2D
        custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
        if NET_MODEL_STRINGS[i] == 'net_ace':
            custom_lines_names.append("TCP")
        else:
            custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

        ax1.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})


# def tabulateByNETMODEL(results_matrix, yColumnIndex, tag):
#     xColumnList = np.unique(results_matrix[:,NET_MODEL])
#     # print(xColumnList)

#     line_list = list()
#     # print(tag, end="\t\t")
#     line_list.append(tag)

#     for j in range(len(xColumnList)):
#         # print(NET_MODEL_STRINGS[int(xColumnList[j])])
#         # print(xColumnList[i], end='\t')
# #         # print(xColumnList[i], yColumnList[j])
# #         # print()
#         xSubset = results_matrix[np.where(results_matrix[:,NET_MODEL] == xColumnList[j])]
#         # print(xSubset[:,i])
#         # print(("%3.2f" % np.mean(xSubset[:,yColumnIndex]))[0:5], end="\t")

#         samples = xSubset[:,yColumnIndex]
#         mean = np.mean(samples)
#         median = np.median(samples)
#         conf_req = 0.90
#         t_value = st.t.ppf(1 - conf_req/2, len(samples)-1)
#         std_err = np.std(samples)/np.sqrt(len(samples))
#         margin = t_value * std_err
#         # conf_int = st.t.interval(0.95, len(xSubset[:,yColumnIndex])-1, loc=np.median(xSubset[:,yColumnIndex]), scale=st.sem(xSubset[:,yColumnIndex]))

#         line_list.append("%3.2f pm %3.2f" % (median, margin))
# #         ySubset = xSubset[np.where(xSubset[:,yColumnIndex] == yColumnList[j])]
# #         # print(ySubset)
# #         # print(len(ySubset))
# #         countMatrix[i,j] = len(ySubset)
# #         print(countMatrix[i,j], end='\t')
#     # print("")
#     return line_list

def tabulateNETMODELs(results_matrix, NETMODELs, xColumnIndex, yColumnIndex, PropName, XUnitsString="", YUnitsString=""):
    # print(PropName)
    # print(NETMODEL_subset[:, [NET_MODEL, DELAY_MS, xColumnIndex, yColumnIndex]])

    xColumnList = np.zeros([0])
    header = list()
    line_list = list()

    header = ["~"]
    for NETMODEL in NETMODELs:
        NETMODEL_subset = subsetByColumnValue(results_matrix, NET_MODEL, NETMODEL)
        xColumnList = np.unique(np.append(xColumnList, NETMODEL_subset[:,xColumnIndex]))
        header.append(NET_MODEL_STRINGS_THESIS[NETMODEL])
        # print("results_matrix[:,xColumnIndex]", np.unique(NETMODEL_subset[:,xColumnIndex]))
    header.append("pm \%")
    line_list.append(header)
    
    # print("xColumnList", xColumnList)

    yColumnList = list()
    for item in xColumnList:
        medians = list()
        item_subset = subsetByColumnValue(results_matrix, xColumnIndex, item)
        line = [str(item) + " " + XUnitsString]
        for NETMODEL in NETMODELs:
            NETMODEL_subset = subsetByColumnValue(item_subset, NET_MODEL, NETMODEL)
            samples = NETMODEL_subset[:,yColumnIndex]
            if len(samples) > 0:
                mean = np.mean(samples)
                median = np.median(samples)
                median = np.median(samples)
                medians.append(median)
                conf_req = 0.90
                t_value = st.t.ppf(1 - conf_req/2, len(samples)-1)
                std_err = np.std(samples)/np.sqrt(len(samples))
                margin = t_value * std_err
                line.append("%3.2f pm %3.2f %s" % (median, margin, YUnitsString))
            else:
                line.append(None)

        if (len(medians) == 2):
            perc_improvement = (medians[1] - medians[0]) / medians[0] * 100
            line.append("%3.2f \%%" % (perc_improvement))
        else:
            line.append("~")

        line_list.append(line)

    return header, line_list, xColumnList


def tabulateNETMODELsNICSendReceive(results_matrix, NETMODELs, NODES_COUNT, xColumnIndex, yColumnIndex, PropName, XUnitsString="", YUnitsString=""):
    # print(PropName)
    # print(NETMODEL_subset[:, [NET_MODEL, DELAY_MS, xColumnIndex, yColumnIndex]])

    xColumnList = np.zeros([0])
    header = list()
    line_list = list()

    header = ["~"]
    for NETMODEL in NETMODELs:
        NETMODEL_subset = subsetByColumnValue(results_matrix, NET_MODEL, NETMODEL)
        xColumnList = np.unique(np.append(xColumnList, NETMODEL_subset[:,xColumnIndex]))
        header.append(NET_MODEL_STRINGS_THESIS[NETMODEL])
        header.append("total")
        # print("results_matrix[:,xColumnIndex]", np.unique(NETMODEL_subset[:,xColumnIndex]))
    header.append("pm \%")
    line_list.append(header)
    
    # print("xColumnList", xColumnList)

    yColumnList = list()
    for item in xColumnList:
        medians = list()
        item_subset = subsetByColumnValue(results_matrix, xColumnIndex, item)
        line = [str(item) + " " + XUnitsString]
        for NETMODEL in NETMODELs:
            NETMODEL_subset = subsetByColumnValue(item_subset, NET_MODEL, NETMODEL)
            samples = NETMODEL_subset[:,yColumnIndex]
            if len(samples) > 0:
                mean = np.mean(samples)
                median = np.median(samples)
                median = np.median(samples)
                medians.append(median)
                conf_req = 0.90
                t_value = st.t.ppf(1 - conf_req/2, len(samples)-1)
                std_err = np.std(samples)/np.sqrt(len(samples))
                margin = t_value * std_err
                line.append("%.2f pm %.2f %s" % (median, margin, YUnitsString))
                line.append("%.2f %s" % (median * NODES_COUNT, YUnitsString))
            else:
                line.append(None)
                line.append(None)

        line_list.append(line)

        if (len(medians) == 2):
            perc_improvement = (medians[1] - medians[0]) / medians[0] * 100
            line.append("%3.2f \%%" % (perc_improvement))
        else:
            line.append("~")

    return header, line_list, xColumnList

def tabulateNETMODELsNICSendReceive_scaling(results_matrix, NETMODELs, xColumnIndex, yColumnIndex, PropName, XUnitsString="", YUnitsString=""):
    # print(PropName)
    # print(NETMODEL_subset[:, [NET_MODEL, DELAY_MS, xColumnIndex, yColumnIndex]])
    if xColumnIndex != NODES_COUNT:
        print("Not selcting NODES_COUNT, you might have made a mistake")
        exit(0)

    xColumnList = np.zeros([0])
    header = list()
    line_list = list()

    header = ["~"]
    for NETMODEL in NETMODELs:
        NETMODEL_subset = subsetByColumnValue(results_matrix, NET_MODEL, NETMODEL)
        xColumnList = np.unique(np.append(xColumnList, NETMODEL_subset[:,xColumnIndex]))
        header.append(NET_MODEL_STRINGS_THESIS[NETMODEL])
        header.append("total")
        # print("results_matrix[:,xColumnIndex]", np.unique(NETMODEL_subset[:,xColumnIndex]))
    header.append("pm \%")
    line_list.append(header)
    
    # print("xColumnList", xColumnList)

    yColumnList = list()
    for item in xColumnList:
        medians = list()
        item_subset = subsetByColumnValue(results_matrix, xColumnIndex, item)
        line = [str(item) + " " + XUnitsString]
        for NETMODEL in NETMODELs:
            NETMODEL_subset = subsetByColumnValue(item_subset, NET_MODEL, NETMODEL)
            samples = NETMODEL_subset[:,yColumnIndex]
            if len(samples) > 0:
                mean = np.mean(samples)
                median = np.median(samples)
                median = np.median(samples)
                medians.append(median)
                conf_req = 0.90
                t_value = st.t.ppf(1 - conf_req/2, len(samples)-1)
                std_err = np.std(samples)/np.sqrt(len(samples))
                margin = t_value * std_err
                line.append("%.2f pm %.2f %s" % (median, margin, YUnitsString))
                line.append("%.2f %s" % (median * item, YUnitsString))
            else:
                line.append(None)

        if (len(medians) == 2):
            perc_improvement = (medians[1] - medians[0]) / medians[0] * 100
            line.append("%3.2f \%%" % (perc_improvement))
        else:
            line.append("~")

        line_list.append(line)

    return header, line_list, xColumnList
