#!/usr/bin/env python3
import matplotlib.pyplot as plot
import numpy as np
import csv
import sys
from os.path import expanduser

FIRST_TIMESTAMP = 0
NET_MODEL = 1
NODES_COUNT = 2
BW_LIMIT = 3
DELAY_MS = 4
LOSS_PERC = 5
ACTIVE_NODES = 6
AVG_TOPO_CONS = 7
AVG_DRIFT = 8
AVG_WORLDSENDSTAT = 9
AVG_WORLDRECVSTAT = 10

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
NET_MODEL_keys = set()

# print(results_text)

for row in results_text:
    # print(row)
    current_loss_perc = int(row[LOSS_PERC])
    if current_loss_perc not in loss_percentages_keys:
        loss_percentages_keys.add(current_loss_perc)
        results[int(row[LOSS_PERC])] = dict()

    results[current_loss_perc][row[FIRST_TIMESTAMP]] = [row[NODES_COUNT], 
        row[BW_LIMIT], 
        float(row[DELAY_MS]), 
        float(row[ACTIVE_NODES]), 
        float(row[AVG_TOPO_CONS]), 
        float(row[AVG_DRIFT]), 
        float(row[AVG_WORLDSENDSTAT]),
        float(row[AVG_WORLDRECVSTAT])]



    # print(results[-1])
    # print(int(row[0])%10000)

# print(results)

# print("\n")

# # print(loss_percentages_keys)


# # After reading, the matrixes look a bit different
# NODES_COUNT = 0
# BW_LIMIT = 1
# DELAY_MS = 2
# ACTIVE_NODES = 3
# AVG_TOPO_CONS = 4
# AVG_DRIFT = 5
# AVG_WORLDSENDSTAT = 6
# AVG_WORLDRECVSTAT = 7

# # #####


# dict_of_numpy_results = dict()

# for key in loss_percentages_keys:
#     print(results[key])
#     this_perctage_results = results[key]
#     dict_of_numpy_results[key] = np.array([this_perctage_results[i] for i in this_perctage_results.keys()])

# print(dict_of_numpy_results)

# # print(results)
# # print(results.keys())
# # print(results.values())
# # numpy_results = np.array([results[i] for i in results.keys()])
# # print(numpy_results)

# # print(numpy_results[:,2])

# # # print("Timestamps: ", numpy_results[:,0])

# # timestamps = numpy_results[:,0]
# # first_timestamp = int(results_text[0][0])

# # loss_perctages = numpy_results[:,LOSS_PERC]

# plot.figure(1, figsize=(12, 10), dpi=80)


# ######## Topology consistency 
# plot.subplot(4,1,1)

# topo_consistency = list()
# keys = list()

# for key in loss_percentages_keys:
#     this_perctage_results = dict_of_numpy_results[key]
#     topo_consistency.append(np.array(this_perctage_results[:,AVG_TOPO_CONS], dtype=np.float))
#     keys.append(key)

# plot.boxplot(topo_consistency, positions=keys) 
# # plot.xlabel('Loss [%]')
# plot.ylabel('Topology consistency [%]')  
# plot.grid()






# ######## Drift distance
# plot.subplot(4,1,2)

# drift_distance = list()
# keys = list()

# for key in loss_percentages_keys:
#     this_perctage_results = dict_of_numpy_results[key]
#     drift_distance.append(np.array(this_perctage_results[:,AVG_DRIFT], dtype=np.float))
#     keys.append(key)

# plot.boxplot(drift_distance, positions=keys) 
# # plot.xlabel('Loss [%]')
# plot.ylabel('Drift distance [VE units]')  
# plot.grid()





# ######## Send bandwidth
# plot.subplot(4,1,3)

# send_bandwidth = list()
# keys = list()

# for key in loss_percentages_keys:
#     this_perctage_results = dict_of_numpy_results[key]
#     send_bandwidth.append(np.array(this_perctage_results[:,AVG_WORLDSENDSTAT], dtype=np.float))
#     keys.append(key)

# plot.boxplot(send_bandwidth, positions=keys) 
# # plot.xlabel('Loss [%]')
# plot.ylabel('Send BW [B]')  
# plot.grid()


# ######## Recv bandwidth
# plot.subplot(4,1,4)

# recv_bandwidth = list()
# keys = list()

# for key in loss_percentages_keys:
#     this_perctage_results = dict_of_numpy_results[key]
#     recv_bandwidth.append(np.array(this_perctage_results[:,AVG_WORLDRECVSTAT], dtype=np.float))
#     keys.append(key)

# plot.boxplot(recv_bandwidth, positions=keys) 
# plot.xlabel('Loss [%]')
# plot.ylabel('Recv BW [B]')  
# plot.grid()

# plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary.pdf" % home_dir, dpi=300)
# plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary.png" % home_dir, dpi=300)


results = dict()

for row in results_text:
    print(row)
    current_NET_MODEL = int(row[NET_MODEL])
    if current_NET_MODEL not in NET_MODEL_keys:
        NET_MODEL_keys.add(current_NET_MODEL)
        results[current_NET_MODEL] = dict()

    results[current_NET_MODEL][row[FIRST_TIMESTAMP]] = [row[NODES_COUNT], 
        row[BW_LIMIT], 
        float(row[DELAY_MS]), 
        float(row[ACTIVE_NODES]), 
        float(row[AVG_TOPO_CONS]), 
        float(row[AVG_DRIFT]), 
        float(row[AVG_WORLDSENDSTAT]),
        float(row[AVG_WORLDRECVSTAT])]

# print(results)

dict_of_numpy_results = dict()

for key in NET_MODEL_keys:
    # print(results[key])
    this_NET_MODEL_results = results[key]
    dict_of_numpy_results[key] = np.array([this_NET_MODEL_results[i] for i in this_NET_MODEL_results.keys()])

# print(dict_of_numpy_results)

# # print(results)
# print(results.keys())
# # print(results.values())
# print(numpy_results)

# print(numpy_results[:,2])

# print("Timestamps: ", numpy_results[:,0])

# timestamps = numpy_results[:,0]
# first_timestamp = int(results_text[0][0])

# loss_perctages = numpy_results[:,LOSS_PERC]

# plot.figure(1, figsize=(12, 10), dpi=80)


# ######## Topology consistency 
plot.subplot(2,1,1)

NET_MODEL_results = list()
keys = list()

for key in NET_MODEL_keys:
    this_perctage_results = dict_of_numpy_results[key]
    NET_MODEL_results.append(np.array(this_perctage_results[:,4], dtype=np.float))
    keys.append(key)

plot.boxplot(NET_MODEL_results, positions=keys) 
plot.xlabel('NET_MODEL [2=ace, 3=udp, 4=udpnc]')
plot.ylabel('Topology consistency [%]')  
plot.grid()

# ######## Topology consistency 
plot.subplot(2,1,2)

# NET_MODEL_results = list()
# keys = list()

# for key in NET_MODEL_keys:
#     this_perctage_results = dict_of_numpy_results[key]
#     NET_MODEL_results.append(np.array(this_perctage_results[:,5], dtype=np.float))
#     keys.append(key)

# plot.boxplot(NET_MODEL_results, positions=keys) 
# plot.xlabel('NET_MODEL [2=ace, 3=udp, 4=udpnc]')
# plot.ylabel('Drift distance [units]')  
# plot.grid()

# print(results_text)
# print(np.array(results_text, dtype=np.float))
results_nparray = np.array(results_text, dtype=np.float)
np.set_printoptions(linewidth=np.inf, formatter={'float': '{: 0.3f}'.format})
print(results_nparray[np.where(results_nparray[:,NET_MODEL] == 2)])
net_ace_results = results_nparray[np.where(results_nparray[:,NET_MODEL] == 2)]
net_udp_results = results_nparray[np.where(results_nparray[:,NET_MODEL] == 3)]
net_udpnc_results = results_nparray[np.where(results_nparray[:,NET_MODEL] == 4)]
NET_MODEL_results = [net_ace_results[:,AVG_DRIFT], net_udp_results[:,AVG_DRIFT], net_udpnc_results[:,AVG_DRIFT]]
print(NET_MODEL_results)

plot.boxplot(NET_MODEL_results, positions=keys) 
plot.xlabel('NET_MODEL [2=ace, 3=udp, 4=udpnc]')
plot.ylabel('Drift distance [units]')  
plot.grid()




plot.show()