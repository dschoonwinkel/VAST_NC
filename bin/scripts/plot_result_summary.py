#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser

hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
except ImportError:
    print("Matplotlib not available on this console")
    hasMatplotlib = False

def plotByColumn(results_matrix, xColumnIndex, yColumnIndex):
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

MININET = 1
DOCKER = 2

x_axis_interval = 2

results_text = list()

home_dir = expanduser("~")
print("Home Dir: ", home_dir)


with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'r') as csvfile:
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
# print(plotByColumn(results_nparray, NODES_COUNT, AVG_DRIFT))
# xColumnList, yColumnList = plotByColumn(results_nparray, NODES_COUNT, AVG_DRIFT)



# DockerSubset = subsetByColumnValue(results_nparray, PLATFORM, DOCKER)
# netUDPsubset = subsetByColumnValue(DockerSubset, NET_MODEL, 3)
# print("netUDPsubsetDocker", len(netUDPsubset))

# if hasMatplotlib:
#     font = {'family' : 'sans',
#             'weight' : 'normal', 
#             'size'   : 5}
#     plot.rc('font', **font)
#     ax = plot.subplot(2,2,1)
#     xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
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
#     xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
#     plot.boxplot(yColumnList, positions=xColumnList) 
#     ax.title.set_text('netUDPNC Mininet')
#     plot.xlabel('NODES_COUNT')
#     plot.ylim([0.5, 5])
#     plot.ylabel('Drift distance [units]')  
#     plot.grid()
#     plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_zoomedin.pdf" % home_dir, dpi=1200)













# Only Mininet results
print("Mininet UDP Subset")
MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
if hasMatplotlib:
    plot.figure()
    ax = plot.subplot(1,2,1)
    xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
    print("Mininet UDP avg:", np.mean(netUDPsubset[:,AVG_DRIFT]))
    plot.boxplot(yColumnList, positions=xColumnList) 
    ax.title.set_text('netUDP Mininet')
    plot.xlabel('NODES_COUNT')
    # plot.ylim([0.5, 5])
    plot.ylabel('Drift distance [units]')  
    plot.grid()


print("Mininet UDPNC Subset")
MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
if hasMatplotlib:
    ax = plot.subplot(1,2,2)
    xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
    plot.boxplot(yColumnList, positions=xColumnList) 
    ax.title.set_text('netUDPNC Mininet')
    plot.xlabel('NODES_COUNT')
    # plot.ylim([0.5, 5])
    plot.ylabel('Drift distance [units]')  
    plot.grid()
    plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet.pdf" % home_dir, dpi=1200)

# Only Mininet results
print("Mininet UDP Subset zoomed in")
MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
netUDPsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 3)
if hasMatplotlib:
    plot.figure()
    ax = plot.subplot(1,2,1)
    xColumnList, yColumnList = plotByColumn(netUDPsubset, NODES_COUNT, AVG_DRIFT)
    plot.boxplot(yColumnList, positions=xColumnList) 
    ax.title.set_text('netUDP Mininet')
    plot.xlabel('NODES_COUNT')
    plot.ylim([0.5, 5])
    plot.ylabel('Drift distance [units]')  
    plot.grid()


print("Mininet UDPNC Subset zoomed in")
MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
netUDPNCsubset = subsetByColumnValue(MininetSubset, NET_MODEL, 4)
if hasMatplotlib:
    ax = plot.subplot(1,2,2)
    xColumnList, yColumnList = plotByColumn(netUDPNCsubset, NODES_COUNT, AVG_DRIFT)
    plot.boxplot(yColumnList, positions=xColumnList) 
    ax.title.set_text('netUDPNC Mininet')
    plot.xlabel('NODES_COUNT')
    plot.ylim([0.5, 5])
    plot.ylabel('Drift distance [units]')  
    plot.grid()
    plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary_Mininet.pdf" % home_dir, dpi=1200)
    plot.show()