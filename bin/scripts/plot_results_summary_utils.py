import csv
import numpy as np
import scipy.stats as st
from plot_result_utils import NET_MODEL_STRINGS
hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
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

def boxPlotHelper(subplotLayout, BoxIndex, xColumnList, yColumnList, xlabel, ylabel, width=0, color='', setXTickLabel=False):
    if hasMatplotlib:
        ax = plot.subplot(subplotLayout)
        if (len(xColumnList) > 0):
            
            if color == '':
                color = default_colors[(BoxIndex-2) % len(default_colors)]


            if (width == 0):
                width = 0.03*np.max(xColumnList)  
            if (width < default_width):
                width = default_width

            spacing = 0.033*np.max(xColumnList)
            if spacing < default_spacing:
                print("Using default_spacing")
                spacing = default_spacing 

            xColumnListOffset = xColumnList+spacing*(BoxIndex-2) 

            box = plot.boxplot(yColumnList, positions=xColumnListOffset, widths=width) 
            plot.setp(box['boxes'], color=color)
            plot.xlabel(xlabel)
            plot.ylabel(ylabel)  
            plot.grid(True, alpha=0.5)

            plot.xlim([np.min(xColumnList)-spacing, np.max(xColumnList)+spacing*(BoxIndex-2)+spacing])
            plot.xticks(xColumnList, xColumnList)
            plot.tick_params(labelbottom=setXTickLabel)

            medians = list()
            for arr in yColumnList:
                medians.append(np.median(arr))
            if (len(medians) > 0):
                ax.plot(xColumnListOffset, medians, color=color, linestyle=default_linestyles[(BoxIndex-2) % len(default_linestyles)], linewidth=1)

        else:
            print("No data")

        return ax
    else:
        print("Matplotlib not installed")



def plot_TopoCon_Drift_BW_Latency(resultsSubset, XAxisProp, PropName, Title, DescriptionString=""):
    if hasMatplotlib:
        plot.figure()

    custom_lines = list()
    custom_lines_names = list()


    for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

        print(NET_MODEL_STRINGS[i])
        NETMODEL_subset = subsetByColumnValue(resultsSubset, NET_MODEL, i)
        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_TOPO_CONS, DescriptionString)
        print(xColumnList)
        print(len(xColumnList))
        if hasMatplotlib and len(xColumnList) > 0:
            

            ax = boxPlotHelper(511, i, xColumnList, yColumnList, '', 'Topo Cons\n[%]')
            ax.title.set_text(Title)
            # ax.set_yticks(np.arange(99.2, 100.2, 0.2))


            from matplotlib.lines import Line2D
            custom_lines.append(Line2D([0], [0], color=default_colors[i-2], linestyle=default_linestyles[i-2], lw=1))
            if NET_MODEL_STRINGS[i] == 'net_ace':
                custom_lines_names.append("TCP")
            else:
                custom_lines_names.append(NET_MODEL_STRINGS[i][4:].upper())

            ax.legend(custom_lines, custom_lines_names, loc='lower left', prop={'size':7})

        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, AVG_DRIFT, DescriptionString)
        if hasMatplotlib and len(xColumnList) > 0:
            
            ax = boxPlotHelper(512, i, xColumnList, yColumnList, '', 'Drift dist\n[units]')
            # ax.set_yticks(np.arange(1.5, 4, 0.5))

        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICSEND_BYTES, DescriptionString)
        if hasMatplotlib and len(xColumnList) > 0:
            
            ax = boxPlotHelper(513, i, xColumnList, yColumnList, '', 'NIC Send\n[kBps]')

        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, NICRECV_BYTES, DescriptionString)
        if hasMatplotlib and len(xColumnList) > 0:
            
            ax = boxPlotHelper(514, i, xColumnList, yColumnList, '', 'NIC Recv\n[kBps]')

        xColumnList, yColumnList = seperateByColumn(NETMODEL_subset, XAxisProp, LATENCY, DescriptionString)
        if hasMatplotlib and len(xColumnList) > 0:
            
            ax = boxPlotHelper(515, i, xColumnList, yColumnList, PropName, 'Latency\n[ms]', setXTickLabel=True)
