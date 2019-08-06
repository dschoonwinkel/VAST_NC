#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
import os
from plot_result_utils import parseFilenameLabel


TIMESTAMP = 0
ACTIVE_NODES = 1
ACTIVE_MATCHERS = 2
AN_ACTUAL = 3
AN_VISIBLE = 4
TOTAL_DRIFT = 5
MAX_DRIFT = 6
DRIFT_NODES = 7
WORLDSENDSTAT = 8
WORLDRECVSTAT = 9

home_dir = expanduser("~")

with open("%s/Development/VAST-0.4.6/bin/VASTreal.ini" % home_dir, 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])
    # print("TIMESTEP_DURATION", TIMESTEP_DURATION, "[ms]")
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\n')+1])
    # print ("SIMULATION_STEPS", SIMULATION_STEPS)
    NET_MODEL = (int)(data[data.index('#NET_MODEL;     // 1: Net emulated 2: Net emulated with bandwidth limitation\n')+1])
    # print ("NET_MODEL", NET_MODEL)


with open("%s/Development/VAST-0.4.6/bin/Mininet.ini" % home_dir, 'r') as config:
    data = config.readlines()
    NODE_COUNT = int(data[data.index('#NODE_COUNT;    // Nodes started in simulation\n')+1])
    # print("NODE_COUNT", NODE_COUNT)
    BW = (int)(data[data.index('#BW;            // Bandwidth limit [Mbps], 0 if inifinte\n')+1])
    # print ("BW", BW)
    DELAY = (int)(data[data.index('#DELAY;         // Delay in MS\n')+1])
    # print ("DELAY", DELAY)
    LOSS_PERC = (int)(data[data.index('#LOSS_PERC;     // Percentages of packets dropped on downstream link. Upstream link unaffected\n')+1])
    # print ("LOSS_PERC", LOSS_PERC)

# x_axis_interval = 20000
x_axis_interval = TIMESTEP_DURATION * 2000
# MAX_TIMESTAMP = 130000
# Keep all except the last bit, where gateway has probably disconnected and other nodes do not know what's going on
MAX_TIMESTAMP = (SIMULATION_STEPS * TIMESTEP_DURATION)

results_text = list()

input_file = '%s/Development/VAST-0.4.6/bin/logs/results/results1.txt' % home_dir

if (len(sys.argv) > 1):
    input_file = sys.argv[1]

LABEL_list = None
abspath = os.path.abspath(input_file)
# print("Absolute path: ", abspath)
LABEL_start = str(abspath).find("logs_net")
LABEL_string = str(abspath)[LABEL_start:]
LABEL_end = LABEL_string.find("results")
LABEL_string = LABEL_string[:LABEL_end]

if LABEL_start != -1:
    # print(LABEL_string)
    LABEL_list, DATESTAMP_str = parseFilenameLabel(LABEL_string)
    print(LABEL_list, DATESTAMP_str)

    #Check if the result is already in summary
    in_result_summary = False
    with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'r') as symmary_file:
        data = symmary_file.readlines()

        for line in data:
            if line.find(DATESTAMP_str) != -1:
                in_result_summary = True
                print("Result already in summmary: ", line)
                exit(0)

else:
    print('LABEL_start not found')







with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)

header = results_text[0]
results_text = results_text[1:]

results = list()

for row in results_text:
    results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]), int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9])])

numpy_results = np.array(results)

timestamps = numpy_results[:,0]
timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
first_timestamp = int(results_text[0][0])
last_relative_timestamp = timestamps[-1]

if last_relative_timestamp < 0.9 * MAX_TIMESTAMP:
    print("Test was not completed, using last timestamp as endpoint, rescaling xticks")
    x_axis_interval = x_axis_interval * last_relative_timestamp / MAX_TIMESTAMP
    MAX_TIMESTAMP = last_relative_timestamp

active_nodes = numpy_results[:,ACTIVE_NODES]
active_matchers = numpy_results[:,ACTIVE_MATCHERS]

topo_consistency = (100* 1.0*numpy_results[:,AN_VISIBLE] / (1.0*numpy_results[:,AN_ACTUAL]))[:len(timestamps)]

where_is_finite = np.isfinite(topo_consistency)
print(len(where_is_finite))
mean_consistency = np.mean(topo_consistency[where_is_finite])
where_are_NaNs = np.isnan(topo_consistency)
topo_consistency[where_are_NaNs] = 100

print("Mean consistency:", mean_consistency)

drift_nodes = numpy_results[:,DRIFT_NODES][:len(timestamps)]
where_is_finite = np.nonzero(drift_nodes)
drift_distance = numpy_results[:,TOTAL_DRIFT][:len(timestamps)]
normalised_drift_distance = drift_distance / drift_nodes

mean_drift_distance = np.mean(normalised_drift_distance[where_is_finite])

print("Mean normalized drift distance:", mean_drift_distance)

# Show results in kBps -> 100 * 10ms per second / 1000 B per kB
send_stat = (numpy_results[:,WORLDSENDSTAT]*100/1000)[:len(timestamps)]
recv_stat = (numpy_results[:,WORLDRECVSTAT]*100/1000)[:len(timestamps)]
mean_sendstat = np.mean(send_stat)

print("Mean send_stat:", mean_sendstat)




# Output to results summary, only if we know the LABEL
if LABEL_list:
    NODE_COUNT = LABEL_list[1]
    LABEL_list.insert(0, first_timestamp)
    LABEL_list.extend([np.max(active_nodes), mean_consistency, 
                  mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)])
    LABEL_list.append(DATESTAMP_str)


    # print(LABEL_list)
    if not in_result_summary:
        with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'a') as outfile:
            outfile.write(("%s, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %s\n") % 
                  tuple(LABEL_list))




TOTAL_SETUPTIME = TIMESTEP_DURATION * 1000 + (1 + TIMESTEP_DURATION * 10)*NODE_COUNT
print("Total connection setup time: ", TOTAL_SETUPTIME, 'ms')
print("Total connection setup time: ", TOTAL_SETUPTIME / 1000, 's')