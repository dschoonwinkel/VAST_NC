#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
from plot_result_utils import NET_MODEL_STRINGS

FIRST_TIMESTAMP = 0
NET_MODEL = 1
NODES_COUNT = 2
BW_LIMIT = 3
DELAY_MS = 4
LOSS_PERC = 5
STEPS = 6
PLATFORM = 7
ACTIVE_NODES = 8
AVG_TOPO_CONS = 9
AVG_DRIFT = 10
AVG_WORLDSENDSTAT = 11
AVG_WORLDRECVSTAT = 12
RAW_MCRECVBYTES = 13
USED_MCRECVBYTES = 14

MININET = 1
DOCKER = 2

hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
except ImportError:
    print("Matplotlib not available on this console")
    hasMatplotlib = False

def seperateByColumn(results_matrix, xColumnIndex, yColumnIndex):
    # print(results_matrix[:,xColumnIndex])
    xColumnList = np.unique(results_matrix[:,xColumnIndex])
    # print(xColumnList)
    yColumnList = list()
    for point in xColumnList:
        # print(point)
        # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)])
        # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)][:,yColumnIndex])
        yColumnList.append(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex])
        print(point, "Count:", len(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        print(point, "Avg: ", np.mean(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        print(point, "Median: ", np.median(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
        print(point, "Std: ", np.std(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]), "\n")

    return xColumnList, yColumnList

def subsetByColumnValue(results_matrix, xColumnIndex, value):
    subset = results_matrix[np.where(results_matrix[:,xColumnIndex] == value)]
    # print(subset)

    return subset

def boxPlotHelper(subplotLayout, xColumnList, yColumnList, color, xlabel, ylabel):
    ax = plot.subplot(subplotLayout)
    if (len(xColumnList) > 0):
        
        box = plot.boxplot(yColumnList, positions=xColumnList) 
        plot.setp(box['boxes'], color=color)
        plot.xlabel(xlabel)
        plot.ylabel(ylabel)  
        plot.grid(True, alpha=0.5)

    else:
        print("No data")

    return ax

def tabulateByNETMODEL(results_matrix, tag):
    xColumnList = np.unique(results_matrix[:,NET_MODEL])

    print(tag, end="\t")

    for i in range(AVG_TOPO_CONS, AVG_WORLDRECVSTAT+1):
        # print(i)
        for j in range(len(xColumnList)):
            # print(xColumnList[i], end='\t')
    #         # print(xColumnList[i], yColumnList[j])
    #         # print()
            xSubset = results_matrix[np.where(results_matrix[:,NET_MODEL] == xColumnList[j])]
            # print(xSubset[:,i])
            print(("%3.2f" % np.mean(xSubset[:,i]))[0:5], end="\t")
    #         ySubset = xSubset[np.where(xSubset[:,yColumnIndex] == yColumnList[j])]
    #         # print(ySubset)
    #         # print(len(ySubset))
    #         countMatrix[i,j] = len(ySubset)
    #         print(countMatrix[i,j], end='\t')
        print("|", end="")

    print("\n")

x_axis_interval = 2

results_text = list()

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

input_file = '%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir

if len(sys.argv) > 1:
    input_file = sys.argv[1]

with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)
        # print(",".join(row))

header = results_text[0]
results_text = results_text[1:]

results = dict()
loss_percentages_keys = set()


# print(results_text)
# print(np.array(results_text, dtype=np.float))
results_nparray = np.array(results_text)
# print(results_nparray)
results_nparray = results_nparray[:, :-1]
# print(results_nparray)
results_nparray = np.array(results_nparray, dtype=np.float)
np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})
# print(results_nparray)
# print(results_nparray[:,NET_MODEL])
# print(seperateByColumn(results_nparray, NODES_COUNT, AVG_DRIFT))
# xColumnList, yColumnList = seperateByColumn(results_nparray, NODES_COUNT, AVG_DRIFT)



#Header
print("", end='~\t')
for i in range(AVG_TOPO_CONS, len(header)):
    print(header[i], end='\t\t')
print("")
print("~", end='')
for i in range(AVG_TOPO_CONS, len(header)):
    print(end='\t|~\t|~\t|~')
print("")

MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
NODES50Subset = subsetByColumnValue(MininetSubset, NODES_COUNT, 50)
LOSS5Subset = subsetByColumnValue(NODES50Subset, LOSS_PERC, 5)
tabulateByNETMODEL(LOSS5Subset, "5%")

MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
NODES50Subset = subsetByColumnValue(MininetSubset, NODES_COUNT, 50)
LOSS10Subset = subsetByColumnValue(NODES50Subset, LOSS_PERC, 10)
tabulateByNETMODEL(LOSS10Subset, "10%")






















# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 3)
# print("netUDPsubsetDocker", len(netUDPsubset))

# if hasMatplotlib:
#     font = {'family' : 'sans',
#             'weight' : 'normal', 
#             'size'   : 5}
#     plot.rc('font', **font)
#     ax = plot.subplot(2,2,1)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPNCsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 4)
# print("netUDPNCsubsetDocker", len(netUDPNCsubset))
# if hasMatplotlib:
#     ax = plot.subplot(2,2,2)
#     xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()

# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
# print("netUDPsubsetMininet", len(netUDPsubset))
# if hasMatplotlib:
#     ax = plot.subplot(2,2,3)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
# print("netUDPNCsubsetMininet", len(netUDPNCsubset))
# if hasMatplotlib:
#     ax = plot.subplot(2,2,4)
#     xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary.pdf" % home_dir, dpi=1200)






# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 3)

# if hasMatplotlib:
#     plot.figure()
#     ax = plot.subplot(2,2,1)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([1.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPNCsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 4)
# if hasMatplotlib:
#     ax = plot.subplot(2,2,2)
#     xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([1.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()

# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
# if hasMatplotlib:
#     ax = plot.subplot(2,2,3)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
# if hasMatplotlib:
#     ax = plot.subplot(2,2,4)
#     xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_zoomedin.pdf" % home_dir, dpi=1200)




# Only Mininet results
# print("*******************\nMininet LOSS_PERC plot 20 NODES")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# colors = ['blue', 'red', 'green']
# if hasMatplotlib:
#     plot.figure()

# for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

#     print(i)
#     print(NET_MODEL_STRINGS[i])
#     netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, i)
#     netUDPsubset_10NODES = subsetByColumnValue(netUDPsubset, NODES_COUNT, 20)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_10NODES, LOSS_PERC, AVG_TOPO_CONS)
#     print(xColumnList)
#     print(len(xColumnList))
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(411, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Topo Cons[%]')
#         ax.title.set_text("TCP vs UDP vs UDPNC. 20 NODES")
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)

#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_10NODES, LOSS_PERC, AVG_DRIFT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(412, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Drift dist [units]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)


#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_10NODES, LOSS_PERC, AVG_WORLDSENDSTAT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(413, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Send [kBps]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)

#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_10NODES, LOSS_PERC, AVG_WORLDRECVSTAT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(414, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Recv [kBps]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.set_xticks(xColumnList)
#         ax.set_xticklabels(xColumnList)

# if hasMatplotlib:
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet_allResults.pdf" % home_dir, dpi=1200)







# # 50 NODES
# # Only Mininet results
# print("*******************\nMininet LOSS_PERC plot 50 NODES")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# colors = ['blue', 'red', 'green']
# if hasMatplotlib:
#     plot.figure()

# for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

#     print(i)
#     print(NET_MODEL_STRINGS[i])
#     netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, i)
#     netUDPsubset_50NODES = subsetByColumnValue(netUDPsubset, NODES_COUNT, 50)
#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_50NODES, LOSS_PERC, AVG_TOPO_CONS)
#     print(xColumnList)
#     print(len(xColumnList))
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(411, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Topo Cons[%]')
#         ax.title.set_text("TCP vs UDP vs UDPNC. 50 NODES")
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)

#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_50NODES, LOSS_PERC, AVG_DRIFT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(412, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Drift dist [units]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)


#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_50NODES, LOSS_PERC, AVG_WORLDSENDSTAT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(413, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Send [kBps]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.get_xaxis().set_visible(False)

#     xColumnList, yColumnList = seperateByColumn(netUDPsubset_50NODES, LOSS_PERC, AVG_WORLDRECVSTAT)
#     if hasMatplotlib and len(xColumnList) > 0:
#         ax = boxPlotHelper(414, xColumnList+0.5*(i-2), yColumnList, colors[i-2], 'LOSS_PERC', 'Recv [kBps]')
#         plot.xlim([np.min(xColumnList)-0.5, np.max(xColumnList)+0.5*(i-2)+0.5])
#         ax.set_xticks(xColumnList)
#         ax.set_xticklabels(xColumnList)

# if hasMatplotlib:
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet_allResults_50NODES.pdf" % home_dir, dpi=1200)






# # Only Mininet results
# print("*******************\nMininet UDP Subset")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
# xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     plot.figure()
#     ax = plot.subplot(1,2,1)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 35])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# print("*******************\nMininet UDPNC Subset")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
# xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     ax = plot.subplot(1,2,2)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 35])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet.pdf" % home_dir, dpi=1200)

# # Only Mininet results
# print("*******************\nMininet UDP Subset zoomed in")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
# xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     plot.figure()
#     ax = plot.subplot(1,2,1)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# print("*******************\nMininet UDPNC Subset zoomed in")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
# netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
# xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     ax = plot.subplot(1,2,2)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet_zoomedin.pdf" % home_dir, dpi=1200)





















# # Only Docker results
# print("*******************\nDocker UDP Subset")
# MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
# xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     plot.figure()
#     ax = plot.subplot(1,2,1)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 35])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# print("*******************\nDocker UDPNC Subset")
# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPNCsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 4)
# xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     ax = plot.subplot(1,2,2)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 35])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Docker.pdf" % home_dir, dpi=1200)

# # Only Docker results
# print("*******************\nDocker UDP Subset zoomed in")
# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 3)
# xColumnList, yColumnList = seperateByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     plot.figure()
#     ax = plot.subplot(1,2,1)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDP Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()


# print("*******************\nDocker UDPNC Subset zoomed in")
# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPNCsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 4)
# xColumnList, yColumnList = seperateByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
# if hasMatplotlib:
#     ax = plot.subplot(1,2,2)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Docker')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.show()