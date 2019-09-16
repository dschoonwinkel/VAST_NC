#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
import os
from plot_result_utils import parseFilenameLabel
import re


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
RAW_MCRECVBYTES = 10
USED_MCRECVBYTES = 11

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
x_axis_interval = TIMESTEP_DURATION * 1000
# MAX_TIMESTAMP = 130000
# Keep all except the last bit, where gateway has probably disconnected and other nodes do not know what's going on
MAX_TIMESTAMP = (SIMULATION_STEPS * TIMESTEP_DURATION)
# Use this to see what happens past node 1 leave
# MAX_TIMESTAMP = (SIMULATION_STEPS * TIMESTEP_DURATION * 2)

TIMESTEP_DURATION_S = TIMESTEP_DURATION / 1000.0
print("TIMESTEP_DURATION_S", TIMESTEP_DURATION_S)
print("NODE_COUNT", NODE_COUNT)
TOTAL_SETUPTIME_S = ((1 + TIMESTEP_DURATION_S * 10)*NODE_COUNT + TIMESTEP_DURATION_S * 1000)
TOTAL_SETUPTIME = TOTAL_SETUPTIME_S * 1000
print("Total connection setup time: ", TOTAL_SETUPTIME, 'ms')
print("Total connection setup time: ", TOTAL_SETUPTIME / 1000, 's')

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
    results_summary_filename = '%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir
    if (os.path.isfile(results_summary_filename)):
        with open(results_summary_filename, 'r') as summary_file:
            data = summary_file.readlines()

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
    results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]),
     int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9]), int(row[10]), int(row[11])])

numpy_results = np.array(results)

timestamps = numpy_results[:,0]
timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
first_timestamp = int(results_text[0][0])


print("Timestamps:", timestamps)
after_setuptime_relative_timestamp = timestamps[0]+TOTAL_SETUPTIME
print("After setuptime relative timestamp", after_setuptime_relative_timestamp)
index_aftersetuptime = np.where(timestamps>after_setuptime_relative_timestamp)[0][0]
print("index in timestamps for after setuptime", index_aftersetuptime)
last_relative_timestamp = timestamps[-1]

if last_relative_timestamp < 0.9 * MAX_TIMESTAMP:
    print("Test was not completed, using last timestamp as endpoint, rescaling xticks")
    x_axis_interval = x_axis_interval * last_relative_timestamp / MAX_TIMESTAMP
    MAX_TIMESTAMP = last_relative_timestamp

active_nodes = numpy_results[:,ACTIVE_NODES]
max_nodes = np.max(active_nodes)
active_matchers = numpy_results[:,ACTIVE_MATCHERS]
max_matchers = np.max(active_matchers)


# TOPO CONSISTENCY
topo_consistency = (100* 1.0*numpy_results[:,AN_VISIBLE] / (1.0*numpy_results[:,AN_ACTUAL]))[:len(timestamps)]
where_is_finite = np.isfinite(topo_consistency)
print("len(where_is_finite)", len(where_is_finite))
mean_consistency = np.mean(topo_consistency[where_is_finite])
where_are_NaNs = np.isnan(topo_consistency)
topo_consistency[where_are_NaNs] = 100
print("Mean consistency:", mean_consistency)

mean_consistency_beforeloss = np.mean(topo_consistency[0:index_aftersetuptime])
mean_consistency_afterloss = np.mean(topo_consistency[index_aftersetuptime:])
print("mean_consistency_beforeloss:", mean_consistency_beforeloss)
print("mean_consistency_afterloss:", mean_consistency_afterloss)



# DRIFT DISTANCE
# Before loss
drift_nodes_beforeloss = numpy_results[:,DRIFT_NODES][:index_aftersetuptime]
where_is_finite = np.nonzero(drift_nodes_beforeloss)
drift_distance = numpy_results[:,TOTAL_DRIFT][:index_aftersetuptime]
normalised_drift_distance = drift_distance / drift_nodes_beforeloss

mean_drift_distance_beforeloss = np.mean(normalised_drift_distance[where_is_finite])
print("Mean normalized drift distance before loss:", mean_drift_distance_beforeloss)

# After loss
drift_nodes = numpy_results[:,DRIFT_NODES][index_aftersetuptime:len(timestamps)]
where_is_finite = np.nonzero(drift_nodes)
drift_distance = numpy_results[:,TOTAL_DRIFT][index_aftersetuptime:len(timestamps)]
normalised_drift_distance = drift_distance / drift_nodes

mean_drift_distance_afterloss = np.mean(normalised_drift_distance[where_is_finite])
print("Mean normalized drift distance after loss:", mean_drift_distance_afterloss)

drift_nodes = numpy_results[:,DRIFT_NODES][:len(timestamps)]
where_is_finite = np.nonzero(drift_nodes)
drift_distance = numpy_results[:,TOTAL_DRIFT][:len(timestamps)]
normalised_drift_distance = drift_distance / drift_nodes

mean_drift_distance = np.mean(normalised_drift_distance[where_is_finite])
print("Mean normalized drift distance:", mean_drift_distance)




#SEND / RECV STAT
# Show results in kBps -> 10 ticks per second (1000ms / 100ms per tick) / 1000 B per kB
CONVERSION_FACTOR = (1000 / TIMESTEP_DURATION) / 1000

send_stat = (numpy_results[:,WORLDSENDSTAT]*CONVERSION_FACTOR)[:len(timestamps)]
recv_stat = (numpy_results[:,WORLDRECVSTAT]*CONVERSION_FACTOR)[:len(timestamps)]
mean_sendstat = np.mean(send_stat)
mean_recvstat = np.mean(recv_stat)  

print("Mean send_stat:", mean_sendstat)
print("Mean send_recv:", mean_recvstat)

mean_sendstat_beforeloss = np.mean(send_stat[0:index_aftersetuptime])
mean_recvstat_beforeloss = np.mean(recv_stat[0:index_aftersetuptime])  

print("mean_sendstat_beforeloss:", mean_sendstat_beforeloss)
print("mean_recvstat_beforeloss:", mean_recvstat_beforeloss)

mean_sendstat_afterloss = np.mean(send_stat[index_aftersetuptime:])
mean_recvstat_afterloss = np.mean(recv_stat[index_aftersetuptime:])

print("mean_sendstat_afterloss:", mean_sendstat_afterloss)
print("mean_recvstat_afterloss:", mean_recvstat_afterloss)

raw_mcrecvbytes = (numpy_results[:,RAW_MCRECVBYTES]*CONVERSION_FACTOR)[:len(timestamps)]
used_mcrecvbytes = (numpy_results[:,USED_MCRECVBYTES]*CONVERSION_FACTOR)[:len(timestamps)]
mean_rawmcrecv_stat = np.mean(raw_mcrecvbytes)
mean_usedmcrecv_stat = np.mean(used_mcrecvbytes)

print("Mean raw_mcrecvbytes:", mean_rawmcrecv_stat)
print("Mean used_mcrecvbytes:", mean_usedmcrecv_stat)

mean_rawmcrecv_stat_beforeloss = np.mean(raw_mcrecvbytes[0:index_aftersetuptime])
mean_usedmcrecv_stat_beforeloss = np.mean(used_mcrecvbytes[0:index_aftersetuptime])

print("mean_rawmcrecv_stat_beforeloss:", mean_rawmcrecv_stat_beforeloss)
print("mean_usedmcrecv_stat_beforeloss:", mean_usedmcrecv_stat_beforeloss)

mean_rawmcrecv_stat_afterloss = np.mean(raw_mcrecvbytes[index_aftersetuptime:])
mean_usedmcrecv_stat_afterloss = np.mean(used_mcrecvbytes[index_aftersetuptime:])

print("mean_rawmcrecv_stat_afterloss:", mean_rawmcrecv_stat_afterloss)
print("mean_usedmcrecv_stat_afterloss:", mean_usedmcrecv_stat_afterloss)


resources_filename = re.sub(r"\.txt", "_resources.txt", input_file)
resources_fileexist = False
if (os.path.isfile(resources_filename)):
    resources_fileexist = True
    print("Resource file found")
    resources_text = list()
    with open(resources_filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            resources_text.append(row)
            # print(",".join(row))
    if len(resources_text) <= 1:
        resources_fileexist = False;


if resources_fileexist:
    header = resources_text[0]
    resources_text = resources_text[1:]

    resources = list()

    for row in resources_text:
        # print(row)
        resources.append([(int(row[0])*1000-first_timestamp), float(row[1]), float(row[2]), int(row[3])])
        # print(results[-1])
        # print(int(row[0])%10000)

    numpy_resources = np.array(resources)
    mean_CPU = np.mean(numpy_resources[:,1])
    mean_MemMB = np.mean(numpy_resources[:,2])
    mean_Nodes = np.mean(numpy_resources[:,3])
    median_CPU = np.median(numpy_resources[:,1])
    median_MemMB = np.median(numpy_resources[:,2])

    print("Mean CPU: %5.2f %%" % mean_CPU)
    print("Median CPU: %5.2f %%" % median_CPU)
    print("Mean Mem %5.2f MiB" % mean_MemMB)
    print("Median Mem %5.2f MiB" % median_MemMB)
    print("Mean Nodes %5.2f " % mean_Nodes)



# events_fileexist = False
# events_filename = re.sub(r"\.txt", "_events.txt", input_file)
# if (os.path.isfile(events_filename)):
#     events_fileexist = True
#     print("Events file found")
#     events_text = list()
#     with open(events_filename, 'r') as csvfile:
#         spamreader = csv.reader(csvfile, delimiter=",")
#         for row in spamreader:
#             events_text.append(row)
#             # print(",".join(row))

#     header = events_text[0]
#     events_text = events_text[1:]

#     events = list()

#     for row in events_text:
#         # print(row)
#         events.append([float(row[0]), row[1], row[2]])
#         # print(results[-1])
#         # print(int(row[0])%10000)

#     events_np = np.array(events)
#     unique_messages = np.unique(events_np[:,2])
#     for i in range(len(unique_messages)):
#         print(i, ": ", unique_messages[i])


mean_nicsendbytes_afterloss = 0
mean_nicrecvbytes_afterloss = 0

tsharksummary_filename = re.sub(r"\.txt", "_tshark_summary.txt", input_file)
tsharksummary_fileexists = False
if (os.path.isfile(tsharksummary_filename)):
    tsharksummary_fileexists = True
    print("Tshark summary file found")
    tsharksummary_text = list()
    with open(tsharksummary_filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            tsharksummary_text.append(row)
            # print(",".join(row))
    if len(tsharksummary_text) <= 1:
        tsharksummary_fileexists = False;


if tsharksummary_fileexists:
    header = tsharksummary_text[0]
    tsharksummary_text = tsharksummary_text[1:]

    tsharksummary = list()

    for row in tsharksummary_text:
        # print(row)
        tsharksummary.append([(int(row[0])-first_timestamp), int(row[1]), int(row[2])])
        # print(results[-1])
        # print(int(row[0])%10000)

    numpy_tsharksummary = np.array(tsharksummary)

    NIC_SEND_BYTES = 1
    NIC_RECV_BYTES = 2

    nic_sendbytes = (numpy_tsharksummary[:,NIC_SEND_BYTES]*CONVERSION_FACTOR)[:len(timestamps)]
    nic_recvbytes = (numpy_tsharksummary[:,NIC_RECV_BYTES]*CONVERSION_FACTOR)[:len(timestamps)]
    mean_nicsendbytes = np.mean(nic_sendbytes)
    mean_nicrecvbytes = np.mean(nic_recvbytes)

    print("Mean NIC send bytes", mean_nicsendbytes)
    print("Mean NIC recv bytes", mean_nicrecvbytes)

    mean_nicsendbytes_beforeloss = np.mean(nic_sendbytes[0:index_aftersetuptime])
    mean_nicrecvbytes_beforeloss = np.mean(nic_recvbytes[0:index_aftersetuptime])

    print("mean_nicsendbytes_beforeloss", mean_nicsendbytes_beforeloss)
    print("mean_nicrecvbytes_beforeloss", mean_nicrecvbytes_beforeloss)

    mean_nicsendbytes_afterloss = np.mean(nic_sendbytes[index_aftersetuptime:])
    mean_nicrecvbytes_afterloss = np.mean(nic_recvbytes[index_aftersetuptime:])

    print("mean_nicsendbytes_afterloss", mean_nicsendbytes_afterloss)
    print("mean_nicrecvbytes_afterloss", mean_nicrecvbytes_afterloss)



# Output to results summary, only if we know the LABEL
if LABEL_list:
    NODE_COUNT = LABEL_list[1]
    LABEL_list.insert(0, first_timestamp)
    LABEL_list.extend([np.max(active_nodes), np.max(active_matchers), 
                  mean_consistency_afterloss, 
                  mean_drift_distance_afterloss, 
                  mean_sendstat_afterloss, mean_recvstat_afterloss, 
                  mean_rawmcrecv_stat_afterloss, mean_usedmcrecv_stat_afterloss, 
                  mean_nicsendbytes_afterloss, mean_nicrecvbytes_afterloss])
    LABEL_list.append(DATESTAMP_str)

    #Check if the result is already in summary
    in_result_summary = False
    with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'r') as symmary_file:
        data = symmary_file.readlines()

        for line in data:
            if line.find(DATESTAMP_str) != -1:
                in_result_summary = True
                print("Result already in summmary: ", line)


    # print(LABEL_list)
    if not in_result_summary:
        with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'a') as outfile:
            outfile.write(("%s, %d, %d, %d, %d, %d, %d, %d, %3.2f, %3.2f, %f, %f, %f, %f, %f, %f, %f %f, %s\n") % 
                  tuple(LABEL_list))





