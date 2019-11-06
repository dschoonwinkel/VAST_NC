#!/usr/bin/env python3
import matplotlib.pyplot as plot
import numpy as np
import csv
import sys

from os.path import expanduser

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

with open("../VASTreal.ini", 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])
    print(TIMESTEP_DURATION)
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\n')+1])
    print (SIMULATION_STEPS)

# x_axis_interval = 20000
x_axis_interval = TIMESTEP_DURATION * 2000
# MAX_TIMESTAMP = 130000
# Keep all except the last bit, where gateway has probably disconnected and other nodes do not know what's going on
MAX_TIMESTAMP = (SIMULATION_STEPS * TIMESTEP_DURATION)

results_text = list()

input_file = '%s/Development/VAST-0.4.6/bin/logs/results/individual_drift.txt' % home_dir

if (len(sys.argv) > 1):
    input_file = sys.argv[1]

print("Input file: ", input_file)

with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)
        # print(",".join(row))

header = results_text[0]
results_text = results_text[1:]

numpy_results = np.array(results_text, dtype=np.int64)
# print(numpy_results)
# print(numpy_results.shape)

# print(numpy_results[:,1])
# print(numpy_results[:,2])

first_timestamp = int(results_text[0][0])
timestamps = np.subtract(numpy_results[:,0], first_timestamp)
last_relative_timestamp = timestamps[-1]

timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
last_relative_timestamp = timestamps[-1]

if last_relative_timestamp < 0.9 * MAX_TIMESTAMP:
    print("Test was not completed, using last timestamp as endpoint, rescaling xticks")
    x_axis_interval = x_axis_interval * last_relative_timestamp / MAX_TIMESTAMP
    MAX_TIMESTAMP = last_relative_timestamp

# print(timestamps.shape)

plot.figure(1, figsize=(12, 10), dpi=80)

print(timestamps)
print(numpy_results.shape)
numpy_results = numpy_results[0:len(timestamps),]
print(numpy_results.shape)
print(np.max(numpy_results[:,1:]))
if np.max(numpy_results[:,1:]) > 10000 :
	print("Unusually large value detected: ", np.max(numpy_results[:,1:]))

# print(timestamps)
# print(numpy_results[:,1])

# plot.subplot(4,1,1)
for i in range(1,numpy_results.shape[1]):
	plot.plot(timestamps, numpy_results[:,i], label="Node%d" % (i-1))

plot.legend()
# plot.plot(timestamps, numpy_results[:,9], label="Node%d" % 2)

plot.ylabel("Drift distance")
plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.yticks(np.arange(min(active_nodes), max(active_nodes)+1, 1))
plot.xlim(0, MAX_TIMESTAMP)
plot.grid(True)

plot.show()
# active_nodes = numpy_results[:,1]
# plot.plot(timestamps, active_nodes[:len(timestamps)])

# plot.subplot(4,1,2)
# topo_consistency = (100* 1.0*numpy_results[:,3] / (1.0*numpy_results[:,2]))[:len(timestamps)]
# plot.plot(timestamps, topo_consistency)
# where_is_finite = np.isfinite(topo_consistency)
# print(len(where_is_finite))
# mean_consistency = np.mean(topo_consistency[where_is_finite])
# where_are_NaNs = np.isnan(topo_consistency)
# topo_consistency[where_are_NaNs] = 100
# plot.plot(timestamps[where_are_NaNs], topo_consistency[where_are_NaNs], 'r,')
# plot.plot([0,timestamps[-1]], [mean_consistency,mean_consistency], 'r')
# print("Mean consistency:", mean_consistency)
# plot.ylabel("Topo consistency [%]")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xlim(0, MAX_TIMESTAMP)
# plot.ylim(0, 110)
# plot.grid(True)

# plot.subplot(4,1,3)
# drift_nodes = numpy_results[:,DRIFT_NODES][:len(timestamps)]
# where_is_finite = np.nonzero(drift_nodes)
# drift_distance = numpy_results[:,TOTAL_DRIFT][:len(timestamps)]
# normalised_drift_distance = drift_distance / drift_nodes

# mean_drift_distance = np.mean(normalised_drift_distance[where_is_finite])

# plot.plot(timestamps, normalised_drift_distance)
# plot.plot([0,timestamps[-1]], [mean_drift_distance,mean_drift_distance], 'r')
# print("Mean normalized drift distance:", mean_drift_distance)
# plot.ylabel("Norm drift distance")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xlim(0, MAX_TIMESTAMP)
# plot.grid(True)

# plot.subplot(4,1,4)
# # Show results in kBps -> 100 * 10ms per second / 1000 B per kB
# send_stat = (numpy_results[:,7]*100/1000)[:len(timestamps)]
# recv_stat = (numpy_results[:,8]*100/1000)[:len(timestamps)]
# mean_sendstat = np.mean(send_stat)*100/1000
# plot.plot(timestamps, send_stat[:len(timestamps)], 'g',label='Send stat')
# plot.plot([0,timestamps[-1]], [mean_sendstat, mean_sendstat], 'r')
# print("Mean send_stat:", mean_sendstat)
# # plot.plot(timestamps, recv_stat*100/1000, 'b+', label='Recv stat')
# plot.ylabel("Send/recv stats [kBps]")
# plot.xlabel("Timestamp [ms]")
# # plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
# plot.legend()
# plot.grid(True)
# plot.xlim(0, MAX_TIMESTAMP)

# plot.savefig("../logs/results/active_nodes_topo_cons_.pdf", dpi=300)
# plot.savefig("../logs/results/active_nodes_topo_cons_.png", dpi=300)

# if (len(sys.argv) > 1):
# 	print("Saving test results in results_summary.txt with label " + sys.argv[1])

# 	with open('../logs/results/results_summary.txt', 'a') as outfile:
# 		outfile.write(("%s, " + sys.argv[1] + ", %f, %f, %f, %f, %f\n") % (first_timestamp, np.max(active_nodes), mean_consistency, mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)))

# plot.show()

