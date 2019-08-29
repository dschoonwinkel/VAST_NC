#!/usr/bin/env python3

import numpy as np
import csv
import sys
from os.path import expanduser
from plot_result_utils import NET_MODEL_STRINGS

# def seperateByColumn(results_matrix, xColumnIndex, yColumnIndex):
#     # print(results_matrix[:,xColumnIndex])
#     xColumnList = np.unique(results_matrix[:,xColumnIndex])
#     # print(xColumnList)
#     yColumnList = list()
#     for point in xColumnList:
#         # print(point)
#         # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)])
#         # print(results_matrix[np.where(results_matrix[:,NET_MODEL] == point)][:,yColumnIndex])
#         yColumnList.append(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex])
#         print(point, "Count:", len(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
#         print(point, "Avg: ", np.mean(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
#         print(point, "Median: ", np.median(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]))
#         print(point, "Std: ", np.std(results_matrix[np.where(results_matrix[:,xColumnIndex] == point)][:,yColumnIndex]), "\n")

#     return xColumnList, yColumnList

def countByColumn(results_matrix, xColumnIndex, yColumnIndex):
    xColumnList = np.unique(results_matrix[:,xColumnIndex])
    yColumnList = np.unique(results_matrix[:,yColumnIndex])


    countMatrix = np.zeros([len(xColumnList), len(yColumnList)])

    #Header
    print("", end='\t')
    for y in yColumnList:
        print(y, end='\t')

    print("")

    # print(xColumnList)
    for i in range(len(xColumnList)):
        print(xColumnList[i], end='\t')
        for j in range(len(yColumnList)):
            # print(xColumnList[i], yColumnList[j])
            # print()
            xSubset = results_matrix[np.where(results_matrix[:,xColumnIndex] == xColumnList[i])]
            ySubset = xSubset[np.where(xSubset[:,yColumnIndex] == yColumnList[j])]
            # print(ySubset)
            # print(len(ySubset))
            countMatrix[i,j] = len(ySubset)
            print(countMatrix[i,j], end='\t')

        print("")

    



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


# print(results_text)
results_nparray = np.array(results_text)
results_nparray = results_nparray[:, :-1]

results_nparray = np.array(results_nparray, dtype=np.float)
np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})


MininetSubset = subsetByColumnValue(results_nparray, PLATFORM, MININET)
MininetSubset = subsetByColumnValue(MininetSubset, STEPS, 5000)

countByColumn(netUDPsubset, NODES_COUNT, LOSS_PERC)

for i in range(NET_MODEL_STRINGS.index('net_ace'),NET_MODEL_STRINGS.index('net_udpNC') + 1):

    print(i)
    print(NET_MODEL_STRINGS[i])
    subset = subsetByColumnValue(MininetSubset, NET_MODEL, i)
    countByColumn(subset, NODES_COUNT, LOSS_PERC)

