import matplotlib.pyplot as plot
import numpy as np
import csv
import sys

TIMESTAMP = 0
ACTIVE_NODES = 1
AN_ACTUAL = 2
AN_VISIBLE = 3
TOTAL_DRIFT = 4
MAX_DRIFT = 5
DRIFT_NODES = 6
WORLDSENDSTAT = 7
WORLDRECVSTAT = 8

x_axis_interval = 20000
MAX_TIMESTAMP = 130000

results_text = list()

with open('../logs/results/results1.txt', 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)
        # print(",".join(row))

header = results_text[0]
results_text = results_text[1:]

results = list()

for row in results_text:
    # print(row)
    results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]), int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8])])
    # print(results[-1])
    # print(int(row[0])%10000)

numpy_results = np.array(results)

# print("Timestamps: ", numpy_results[:,0])

timestamps = numpy_results[:,0]
timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
first_timestamp = int(results_text[0][0])


plot.figure(1, figsize=(12, 10), dpi=80)

plot.subplot(4,1,1)
active_nodes = numpy_results[:,1]
plot.plot(timestamps, active_nodes[:len(timestamps)])
plot.ylabel("Active nodes")
plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
plot.yticks(np.arange(min(active_nodes), max(active_nodes)+1, 1))
plot.xlim(0, MAX_TIMESTAMP)

plot.grid(True)

plot.subplot(4,1,2)
topo_consistency = (100* 1.0*numpy_results[:,3] / (1.0*numpy_results[:,2]))[:len(timestamps)]
plot.plot(timestamps, topo_consistency)
where_is_finite = np.isfinite(topo_consistency)
print(len(where_is_finite))
mean_consistency = np.mean(topo_consistency[where_is_finite])
where_are_NaNs = np.isnan(topo_consistency)
topo_consistency[where_are_NaNs] = 100
plot.plot(timestamps[where_are_NaNs], topo_consistency[where_are_NaNs], 'r,')
plot.plot([0,timestamps[-1]], [mean_consistency,mean_consistency], 'r')
print("Mean consistency:", mean_consistency)
plot.ylabel("Topo consistency [%]")
plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
plot.xlim(0, MAX_TIMESTAMP)
plot.ylim(0, 110)
plot.grid(True)

plot.subplot(4,1,3)
drift_nodes = numpy_results[:,DRIFT_NODES][:len(timestamps)]
where_is_finite = np.nonzero(drift_nodes)
drift_distance = numpy_results[:,TOTAL_DRIFT][:len(timestamps)]
normalised_drift_distance = drift_distance / drift_nodes

mean_drift_distance = np.mean(normalised_drift_distance[where_is_finite])

plot.plot(timestamps, normalised_drift_distance)
plot.plot([0,timestamps[-1]], [mean_drift_distance,mean_drift_distance], 'r')
print("Mean normalized drift distance:", mean_drift_distance)
plot.ylabel("Norm drift distance")
plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
plot.xlim(0, MAX_TIMESTAMP)
plot.grid(True)

plot.subplot(4,1,4)
# Show results in kBps -> 100 * 10ms per second / 1000 B per kB
send_stat = (numpy_results[:,7]*100/1000)[:len(timestamps)]
recv_stat = (numpy_results[:,8]*100/1000)[:len(timestamps)]
mean_sendstat = np.mean(send_stat)*100/1000
plot.plot(timestamps, send_stat[:len(timestamps)], 'g',label='Send stat')
plot.plot([0,timestamps[-1]], [mean_sendstat, mean_sendstat], 'r')
print("Mean send_stat:", mean_sendstat)
# plot.plot(timestamps, recv_stat*100/1000, 'b+', label='Recv stat')
plot.ylabel("Send/recv stats [kBps]")
plot.xlabel("Timestamp [ms]")
# plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
plot.legend()
plot.grid(True)
plot.xlim(0, MAX_TIMESTAMP)

plot.savefig("../logs/results/active_nodes_topo_cons_.pdf", dpi=300)
plot.savefig("../logs/results/active_nodes_topo_cons_.png", dpi=300)

if (len(sys.argv) > 1):
	print("Saving test results in results_summary.txt with label " + sys.argv[1])

	with open('../logs/results/results_summary.txt', 'a') as outfile:
		outfile.write(("%s, " + sys.argv[1] + ", %f, %f, %f, %f, %f\n") % (first_timestamp, np.max(active_nodes), mean_consistency, mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)))

plot.show()

