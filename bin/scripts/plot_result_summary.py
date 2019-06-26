#!/usr/bin/env python3
import matplotlib.pyplot as plot
import numpy as np
import csv
import sys
from os.path import expanduser

FIRST_TIMESTAMP = 0
NETWORK_TYPE = 1
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

# print(results_text)

for row in results_text:
    print(row)
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

print("\n")

# print(loss_percentages_keys)


# After reading, the matrixes look a bit different
NODES_COUNT = 0
BW_LIMIT = 1
DELAY_MS = 2
ACTIVE_NODES = 3
AVG_TOPO_CONS = 4
AVG_DRIFT = 5
AVG_WORLDSENDSTAT = 6
AVG_WORLDRECVSTAT = 7

# #####


dict_of_numpy_results = dict()

for key in loss_percentages_keys:
    print(results[key])
    this_perctage_results = results[key]
    dict_of_numpy_results[key] = np.array([this_perctage_results[i] for i in this_perctage_results.keys()])

print(dict_of_numpy_results)

# print(results)
# print(results.keys())
# print(results.values())
# numpy_results = np.array([results[i] for i in results.keys()])
# print(numpy_results)

# print(numpy_results[:,2])

# # print("Timestamps: ", numpy_results[:,0])

# timestamps = numpy_results[:,0]
# first_timestamp = int(results_text[0][0])

# loss_perctages = numpy_results[:,LOSS_PERC]

plot.figure(1, figsize=(12, 10), dpi=80)


######## Topology consistency 
plot.subplot(4,1,1)

topo_consistency = list()
keys = list()

for key in loss_percentages_keys:
    this_perctage_results = dict_of_numpy_results[key]
    topo_consistency.append(np.array(this_perctage_results[:,AVG_TOPO_CONS], dtype=np.float))
    keys.append(key)

plot.boxplot(topo_consistency, positions=keys) 
# plot.xlabel('Loss [%]')
plot.ylabel('Topology consistency [%]')  
plot.grid()






######## Drift distance
plot.subplot(4,1,2)

drift_distance = list()
keys = list()

for key in loss_percentages_keys:
    this_perctage_results = dict_of_numpy_results[key]
    drift_distance.append(np.array(this_perctage_results[:,AVG_DRIFT], dtype=np.float))
    keys.append(key)

plot.boxplot(drift_distance, positions=keys) 
# plot.xlabel('Loss [%]')
plot.ylabel('Drift distance [VE units]')  
plot.grid()





######## Send bandwidth
plot.subplot(4,1,3)

send_bandwidth = list()
keys = list()

for key in loss_percentages_keys:
    this_perctage_results = dict_of_numpy_results[key]
    send_bandwidth.append(np.array(this_perctage_results[:,AVG_WORLDSENDSTAT], dtype=np.float))
    keys.append(key)

plot.boxplot(send_bandwidth, positions=keys) 
# plot.xlabel('Loss [%]')
plot.ylabel('Send BW [B]')  
plot.grid()


######## Recv bandwidth
plot.subplot(4,1,4)

recv_bandwidth = list()
keys = list()

for key in loss_percentages_keys:
    this_perctage_results = dict_of_numpy_results[key]
    recv_bandwidth.append(np.array(this_perctage_results[:,AVG_WORLDRECVSTAT], dtype=np.float))
    keys.append(key)

plot.boxplot(recv_bandwidth, positions=keys) 
plot.xlabel('Loss [%]')
plot.ylabel('Recv BW [B]')  
plot.grid()

plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary.pdf" % home_dir, dpi=300)
plot.savefig("%s/Development/VAST-0.4.6/bin/results_summary/results_summary.png" % home_dir, dpi=300)


plot.show()