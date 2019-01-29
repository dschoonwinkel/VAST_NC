import matplotlib.pyplot as plot
import numpy as np
import csv
import sys

FIRST_TIMESTAMP = 0
NODES_COUNT = 1
BW_LIMIT = 2
DELAY_MS = 3
LOSS_PERC = 4
ACTIVE_NODES = 5
AVG_TOPO_CONS = 6
AVG_DRIFT = 7
AVG_WORLDSENDSTAT = 8
AVG_WORLDRECVSTAT = 9

x_axis_interval = 2

results_text = list()

with open('../logs/results/results_summary.txt', 'r') as csvfile:
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




plot.show()

# topo_consistency = numpy_results[:,AVG_TOPO_CONS]
# plot.boxplot(topo_consistency, positions=[0])
# plot.ylabel("Active nodes")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.yticks(np.arange(min(active_nodes), max(active_nodes)+1, 1))
# plot.xlim(0, max(timestamps)+1)

# plot.grid(True)

# plot.subplot(4,1,2)
# topo_consistency = 100* 1.0*numpy_results[:,3] / (1.0*numpy_results[:,2])
# plot.plot(timestamps, topo_consistency)
# where_is_finite = np.isfinite(topo_consistency)
# mean_consistency = np.mean(topo_consistency[where_is_finite])
# where_are_NaNs = np.isnan(topo_consistency)
# topo_consistency[where_are_NaNs] = 100
# plot.plot(timestamps[where_are_NaNs], topo_consistency[where_are_NaNs], 'r,')
# plot.ylabel("Topo consistency [%]")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xlim(0, max(timestamps)+1)
# plot.grid(True)

# plot.subplot(4,1,3)
# drift_nodes = numpy_results[:,DRIFT_NODES]
# drift_distance = numpy_results[:,TOTAL_DRIFT]
# normalised_drift_distance = drift_distance / drift_nodes

# mean_drift_distance = np.mean(normalised_drift_distance[where_is_finite])

# plot.plot(timestamps, normalised_drift_distance)
# plot.ylabel("Norm drift distance")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xlim(0, max(timestamps)+1)
# plot.grid(True)

# plot.subplot(4,1,4)
# send_stat = numpy_results[:,7]
# recv_stat = numpy_results[:,8]
# plot.plot(timestamps, send_stat, 'g',label='Send stat')
# # plot.plot(timestamps, recv_stat, 'b', label='Recv stat')
# plot.ylabel("Send/recv stats [B]")
# plot.xlabel("Timestamp [ms]")
# # plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.legend()
# plot.grid(True)
# plot.xlim(0, max(timestamps)+1)

# plot.savefig("../logs/results/active_nodes_topo_cons_.pdf", dpi=300)
# plot.savefig("../logs/results/active_nodes_topo_cons_.png", dpi=300)

# if (len(sys.argv) > 1):
# 	print("Saving test results in results_summary.txt with label " + sys.argv[1])

# 	with open('../logs/results/results_summary.txt', 'a') as outfile:
# 		outfile.write(("%s, " + sys.argv[1] + ", %f %f %f %f %f\n") % (first_timestamp, np.max(active_nodes), mean_consistency, mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)))

# plot.show()

