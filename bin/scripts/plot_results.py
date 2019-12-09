#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
import os
from plot_result_utils import parseFilenameLabel, parseFilenameNodesTimesteps, NET_MODEL_STRINGS, finite_mean, finite_max, NET_UDPNC
import re
import glob

hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
    from matplotlib.ticker import MaxNLocator
except ImportError:
    print("Matplotlib not available on this console")
    hasMatplotlib = False


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
MAX_TOPOCONS = 12
MIN_TOPOCONS = 13

print("Usage: ./plot_results.py <input_file: default = results1.txt>\n\
                 <results label = \"NET_MODEL, nodecount, BW, delay, loss%\">")

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

VASTreal_file = "%s/Development/VAST-0.4.6/bin/VASTreal.ini" % home_dir
Mininet_file = "%s/Development/VAST-0.4.6/bin/Mininet.ini" % home_dir

if (os.path.isfile("../VASTreal.ini")):
     VASTreal_file = "../VASTreal.ini"

if (os.path.isfile("../Mininet.ini")):
     Mininet_file = "../Mininet.ini"

print(VASTreal_file, Mininet_file)

with open(VASTreal_file, 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])
    # print("TIMESTEP_DURATION", TIMESTEP_DURATION, "[ms]")
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\n')+1])
    # print ("SIMULATION_STEPS", SIMULATION_STEPS)
    NET_MODEL = (int)(data[data.index('#NET_MODEL;     // 1: Net emulated 2: Net emulated with bandwidth limitation\n')+1])
    # print ("NET_MODEL", NET_MODEL)


with open(Mininet_file, 'r') as config:
    data = config.readlines()
    NODE_COUNT = int(data[data.index('#NODE_COUNT;    // Nodes started in simulation\n')+1])
    # print("NODE_COUNT", NODE_COUNT)
    BW = (float)(data[data.index('#BW;            // Bandwidth limit [Mbps], 0 if inifinte\n')+1])
    # print ("BW", BW)
    DELAY = (int)(data[data.index('#DELAY;         // Delay in MS\n')+1])
    # print ("DELAY", DELAY)
    LOSS_PERC = (int)(data[data.index('#LOSS_PERC;     // Percentages of packets dropped on downstream link. Upstream link unaffected\n')+1])
    # print ("LOSS_PERC", LOSS_PERC)



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

    NODES_parsed, SIMULATION_STEPS_parsed = parseFilenameNodesTimesteps(LABEL_string)
    print("parseFilenameNodesTimesteps", (NODES_parsed, SIMULATION_STEPS_parsed))

    if (NODES_parsed != NODE_COUNT or SIMULATION_STEPS != SIMULATION_STEPS_parsed):
        print("******************************************")
        print("NODES_parsed or SIMULATION_STEPS_parsed different to Mininet.ini, using parsed values")
        print("******************************************")
        NODE_COUNT = NODES_parsed
        SIMULATION_STEPS = SIMULATION_STEPS_parsed

else:
    print('LABEL_start not found')




# x_axis_interval = 20000
x_axis_interval = TIMESTEP_DURATION * 1000
# MAX_TIMESTAMP = 130000
# Keep all except the last bit, where gateway has probably disconnected and other nodes do not know what's going on
MAX_TIMESTAMP = (SIMULATION_STEPS * TIMESTEP_DURATION)
print("MAX TIMESTAMP", MAX_TIMESTAMP)
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


plot_yes = False

if (len(sys.argv) > 2):
    print(sys.argv[2])
    print("Plotting")
    plot_yes = True

with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)
        # print(",".join(row))

results_header = results_text[0]
results_text = results_text[1:]

results = list()

hasTopoConsMinMax = False
if len(results_header) == MIN_TOPOCONS+1:
    hasTopoConsMinMax = True

for row in results_text:
    # print(row)
    if hasTopoConsMinMax:
        results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]),
                int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9]), int(row[10]), int(row[11]), float(row[12]), float(row[13])])
    else:
        results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]),
                int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9]), int(row[10]), int(row[11])])
    # print(results[-1])
    # print(int(row[0])%10000)

numpy_results = np.array(results)

# print("Timestamps: ", numpy_results[:,0])

timestamps = numpy_results[:,0]
timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
first_timestamp = int(results_text[0][0])


print("Timestamps:", timestamps)
after_setuptime_relative_timestamp = timestamps[0]+TOTAL_SETUPTIME
print("After setuptime relative timestamp", after_setuptime_relative_timestamp)
print("np.where", np.where(timestamps>after_setuptime_relative_timestamp)[0])
if len(np.where(timestamps>after_setuptime_relative_timestamp)[0]) != 0:
    index_aftersetuptime = np.where(timestamps>after_setuptime_relative_timestamp)[0][0]
    print("index in timestamps for after setuptime", index_aftersetuptime)
else:
    index_aftersetuptime = len(timestamps) -1
    print("index in timestamps for after setuptime not available, using last timestamp", index_aftersetuptime)

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
if hasTopoConsMinMax:
    max_topo_consistency = numpy_results[:,MAX_TOPOCONS][:len(timestamps)]
    min_topo_consistency = numpy_results[:,MIN_TOPOCONS][:len(timestamps)]

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


max_drift = numpy_results[:,MAX_DRIFT][:len(timestamps)]
# Thumb suck: if only one of the nodes have 
# min_drift = (drift_distance - max_drift) / (drift_nodes - 1)
min_drift = np.zeros(max_drift.shape)



#SEND / RECV STAT
# Show results in kBps -> 10 ticks per second (1000ms / 100ms per tick) / 1000 B per kB
CONVERSION_FACTOR = (1000 / TIMESTEP_DURATION) / 1000

send_stat = (numpy_results[:,WORLDSENDSTAT]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
recv_stat = (numpy_results[:,WORLDRECVSTAT]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
mean_sendstat = finite_mean(send_stat)
mean_recvstat = finite_mean(recv_stat)  

print("Mean send_stat:", mean_sendstat)
print("Mean send_recv:", mean_recvstat)

mean_sendstat_beforeloss = finite_mean(send_stat[0:index_aftersetuptime])
mean_recvstat_beforeloss = finite_mean(recv_stat[0:index_aftersetuptime])  

print("mean_sendstat_beforeloss:", mean_sendstat_beforeloss)
print("mean_recvstat_beforeloss:", mean_recvstat_beforeloss)

mean_sendstat_afterloss = finite_mean(send_stat[index_aftersetuptime:])
mean_recvstat_afterloss = finite_mean(recv_stat[index_aftersetuptime:])

print("mean_sendstat_afterloss:", mean_sendstat_afterloss)
print("mean_recvstat_afterloss:", mean_recvstat_afterloss)

raw_mcrecvbytes = (numpy_results[:,RAW_MCRECVBYTES]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
used_mcrecvbytes = (numpy_results[:,USED_MCRECVBYTES]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
mean_rawmcrecv_stat = finite_mean(raw_mcrecvbytes)
mean_usedmcrecv_stat = finite_mean(used_mcrecvbytes)

print("Mean raw_mcrecv kbytes per node:", mean_rawmcrecv_stat)
print("Mean used_mcrecv kbytes per node:", mean_usedmcrecv_stat)

mean_rawmcrecv_stat_beforeloss = finite_mean(raw_mcrecvbytes[0:index_aftersetuptime])
mean_usedmcrecv_stat_beforeloss = finite_mean(used_mcrecvbytes[0:index_aftersetuptime])

print("mean_rawmcrecv_stat_beforeloss kBytes/node:", mean_rawmcrecv_stat_beforeloss)
print("mean_usedmcrecv_stat_beforeloss kBytes/node:", mean_usedmcrecv_stat_beforeloss)

mean_rawmcrecv_stat_afterloss = finite_mean(raw_mcrecvbytes[index_aftersetuptime:])
mean_usedmcrecv_stat_afterloss = finite_mean(used_mcrecvbytes[index_aftersetuptime:])

print("mean_rawmcrecv_stat_afterloss kBytes/node:", mean_rawmcrecv_stat_afterloss)
print("mean_usedmcrecv_stat_afterloss kBytes/node:", mean_usedmcrecv_stat_afterloss)


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



events_fileexist = False
events_filename = re.sub(r"\.txt", "_events.txt", input_file)
if (os.path.isfile(events_filename)):
    events_fileexist = True
    print("Events file found")
    events_text = list()
    with open(events_filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            events_text.append(row)
            # print(",".join(row))

    header = events_text[0]
    events_text = events_text[1:]

    events = list()

    for row in events_text:
        # print(row)
        events.append([float(row[0]), row[1], row[2]])
        # print(results[-1])
        # print(int(row[0])%10000)

    events_np = np.array(events)
    unique_messages = np.unique(events_np[:,2])
    for i in range(len(unique_messages)):
        print(i, ": ", unique_messages[i])


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

    nic_sendbytes = (numpy_tsharksummary[:,NIC_SEND_BYTES]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
    nic_recvbytes = (numpy_tsharksummary[:,NIC_RECV_BYTES]*CONVERSION_FACTOR/active_nodes)[:len(timestamps)]
    mean_nicsendbytes = finite_mean(nic_sendbytes)
    mean_nicrecvbytes = finite_mean(nic_recvbytes)

    print("Mean NIC send kbytes per node", mean_nicsendbytes)
    print("Mean NIC recv kbytes per node", mean_nicrecvbytes)

    mean_nicsendbytes_beforeloss = finite_mean(nic_sendbytes[0:index_aftersetuptime])
    mean_nicrecvbytes_beforeloss = finite_mean(nic_recvbytes[0:index_aftersetuptime])

    print("mean_nicsendbytes_beforeloss kbytes per node", mean_nicsendbytes_beforeloss)
    print("mean_nicrecvbytes_beforeloss kbytes per node", mean_nicrecvbytes_beforeloss)

    mean_nicsendbytes_afterloss = finite_mean(nic_sendbytes[index_aftersetuptime:])
    mean_nicrecvbytes_afterloss = finite_mean(nic_recvbytes[index_aftersetuptime:])

    print("mean_nicsendbytes_afterloss kbytes per node", mean_nicsendbytes_afterloss)
    print("mean_nicrecvbytes_afterloss kbytes per node", mean_nicrecvbytes_afterloss)





# Latency results
mean_normalized_latency = 0
mean_normalized_move_latency_afterloss = 0

latency_filename = re.sub(r"\.txt", "_latency.txt", input_file)
latency_fileexists = False
if (os.path.isfile(latency_filename)):
    latency_fileexists = True
    print("Latency file found")
    latency_text = list()
    with open(latency_filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            latency_text.append(row)
            # print(",".join(row))
    if len(latency_text) <= 1:
        latency_fileexists = False;


if latency_fileexists:
    header = latency_text[0]
    latency_text = latency_text[1:]

    latency = list()

    ACTIVE_NODES_LATENCY = 1
    MOVE_LATENCY = 2
    MAX_LATENCY = 3
    MIN_LATENCY = 4

    hasLatencyMinMax = False
    if len(header) == MIN_LATENCY+1:
        hasLatencyMinMax = True

    for row in latency_text:
        # print(row)
        if hasLatencyMinMax:
            latency.append([(int(row[0])-first_timestamp), int(row[1]), int(row[2]), int(row[3]), int(row[4])])
        else:
            latency.append([(int(row[0])-first_timestamp), int(row[1]), int(row[2])])
        # print(results[-1])
        # print(int(row[0])%10000)

    numpy_latency = np.array(latency)

    latency_active_nodes = (numpy_latency[:,ACTIVE_NODES_LATENCY])[:len(timestamps)]
    move_latency = (numpy_latency[:,MOVE_LATENCY])[:len(timestamps)]
    if hasLatencyMinMax:
        max_latency = (numpy_latency[:,MAX_LATENCY])[:len(timestamps)]
        min_latency = (numpy_latency[:,MIN_LATENCY])[:len(timestamps)]

    normalized_move_latency = move_latency / latency_active_nodes
    latency_where_is_finite = np.isfinite(normalized_move_latency)
    mean_normalized_move_latency = np.mean(normalized_move_latency[latency_where_is_finite])
    print("Mean Normalized latency ", mean_normalized_move_latency)
    max_normalized_move_latency = np.max(normalized_move_latency[latency_where_is_finite])

    normalized_move_latency_beforeloss = normalized_move_latency[1:index_aftersetuptime]
    latency_where_is_finite = np.isfinite(normalized_move_latency_beforeloss)
    mean_normalized_move_latency_beforeloss = np.mean(normalized_move_latency_beforeloss[latency_where_is_finite])
    print("mean_normalized_move_latency_beforeloss", mean_normalized_move_latency_beforeloss)

    normalized_move_latency_afterloss = normalized_move_latency[index_aftersetuptime:]
    latency_where_is_finite = np.isfinite(normalized_move_latency_afterloss)
    mean_normalized_move_latency_afterloss = np.mean(normalized_move_latency_afterloss[latency_where_is_finite])
    print("mean_normalized_move_latency_afterloss", mean_normalized_move_latency_afterloss)











# Output to results summary, only if we know the LABEL
if LABEL_list:
    NODE_COUNT = LABEL_list[1]
    LABEL_list.insert(0, first_timestamp)
    LABEL_list.extend([np.max(active_nodes), np.max(active_matchers), 
                  mean_consistency_afterloss, 
                  mean_drift_distance_afterloss, 
                  mean_sendstat_afterloss, mean_recvstat_afterloss, 
                  mean_rawmcrecv_stat_afterloss, mean_usedmcrecv_stat_afterloss, 
                  mean_nicsendbytes_afterloss, mean_nicrecvbytes_afterloss, mean_normalized_move_latency_afterloss])
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
    if not plot_yes and not in_result_summary:
        with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'a') as outfile:
            outfile.write(("%s, %d, %d, %2.2f, %d, %d, %d, %d, %3.2f, %3.2f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %s\n") % 
                  tuple(LABEL_list))
        #         outfile.write("%s, %s, %f, %f, %f, %f, %f\n" 
    #             % (first_timestamp, input_file, np.max(active_nodes), mean_consistency, 
    #               mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)))
    #         print("Saving test results in results_summary.txt with label " + input_file)


# NET_MODEL_STRINGS = ['net_emu', 'net_ace', 'net_udp', 'net_udpNC']
# label = "%s_%d_%d_%d_loss%d_%s" % \
#             (NET_MODEL_STRINGS[NET_MODEL-1], NODE_COUNT, BW, DELAY, LOSS_PERC, first_timestamp)
# print(label)

# with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'a') as outfile:
#     # outfile.write(("%s, %d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %s\n") % 
#           # (first_timestamp, NET_MODEL, NODE_COUNT, BW, DELAY, LOSS_PERC, np.max(active_nodes), mean_consistency, 
#           #     mean_drift_distance, np.mean(send_stat), np.mean(recv_stat), sys.argv[2]))
#         outfile.write("%s, %s, %f, %f, %f, %f, %f\n" 
#             % (first_timestamp, input_file, np.max(active_nodes), mean_consistency, 
#               mean_drift_distance, np.mean(send_stat), np.mean(recv_stat)))
#         print("Saving test results in results_summary.txt with label " + input_file)

def plot_results(with_ylim=False): 

    if (hasMatplotlib and plot_yes):

        plot.figure(figsize=(12, 10), dpi=80)
        if (LABEL_list):
            plot.title(str(LABEL_list))

        subplot_count = 4

        if latency_fileexists:
            subplot_count += 1

        if mean_rawmcrecv_stat_afterloss > 0:
            subplot_count += 1


        ax1 = plot.subplot(subplot_count,1,1)
        plot.plot(timestamps, active_nodes[:len(timestamps)], 'b')
        plot.plot(timestamps, active_matchers[:len(timestamps)], 'r')
        plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, max(active_nodes)+1], 'k')
        plot.text(timestamps[-1], max_nodes, str(max_nodes), color='b', bbox=dict(facecolor='white', alpha=1))
        plot.text(timestamps[-1], max_matchers, str(max_matchers), color='r', bbox=dict(facecolor='white', alpha=1))
        plot.ylabel("# Active")
        plot.legend(['Nodes', 'Matchers'])
        ax1.get_xaxis().set_visible(False)
        plot.yticks(np.arange(min(active_nodes),max(active_nodes)+1, max(active_nodes)/5))
        plot.xlim(0, MAX_TIMESTAMP)
        plot.grid(True)


        ax2 = plot.subplot(subplot_count,1,2)
        plot.plot(timestamps, topo_consistency)
        if (hasTopoConsMinMax):
            plot.fill_between(timestamps, max_topo_consistency, min_topo_consistency, color='gray', alpha=0.2)

        plot.plot(timestamps[where_are_NaNs], topo_consistency[where_are_NaNs], 'r,')
        plot.plot([0,timestamps[index_aftersetuptime]], [mean_consistency_beforeloss,mean_consistency_beforeloss], 'r--')
        plot.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_consistency_afterloss,mean_consistency_afterloss], 'r--')
        # plot.plot([0,timestamps[-1]], [mean_consistency,mean_consistency], 'r--')
        plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[np.min(topo_consistency), 100], 'k')
        plot.text(timestamps[0], mean_consistency_afterloss, "%5.2f" % mean_consistency_beforeloss, bbox=dict(facecolor='white', alpha=1))
        plot.text(timestamps[-1], mean_consistency_afterloss, "%5.2f" % mean_consistency_afterloss, bbox=dict(facecolor='white', alpha=1))
        plot.ylabel("Topo consistency\n[%]")
        ax2.get_xaxis().set_visible(False)
        plot.xlim(0, MAX_TIMESTAMP)
        print("Consistency plot ylims: ", plot.ylim())
        if with_ylim:
            plot.ylim(96, 100.2)
            plot.yticks(np.arange(96, 100, 1))
        
        plot.grid(True)

        ax3 = plot.subplot(subplot_count,1,3)
        plot.plot(timestamps, normalised_drift_distance)
        plot.plot([0,timestamps[index_aftersetuptime]], [mean_drift_distance_beforeloss,mean_drift_distance_beforeloss], 'r--')
        plot.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_drift_distance_afterloss,mean_drift_distance_afterloss], 'r--')
        ylims = plot.ylim()

        # Plot variance...
        plot.fill_between(timestamps, max_drift, min_drift, color='gray', alpha=0.2)
        if with_ylim:
            plot.ylim(ylims)

        # plot.plot([0,timestamps[-1]], [mean_drift_distance,mean_drift_distance], 'r--')
        plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(normalised_drift_distance[where_is_finite])], 'k')
        plot.ylabel("Norm drift\ndist[VE units]")
        plot.text(timestamps[0], mean_drift_distance_beforeloss, "%5.2f" % mean_drift_distance_beforeloss, bbox=dict(facecolor='white', alpha=1))
        plot.text(timestamps[-1], mean_drift_distance_afterloss, "%5.2f" % mean_drift_distance_afterloss, bbox=dict(facecolor='white', alpha=1))
        plot.xlim(0, MAX_TIMESTAMP)
        ax3.get_xaxis().set_visible(False)
        plot.grid(True)

        if (latency_fileexists):
            ax5 = plot.subplot(subplot_count,1,4)
            plot.plot(timestamps, normalized_move_latency, color='b')
            if (hasLatencyMinMax):
                plot.fill_between(timestamps, max_latency, min_latency, color='gray', alpha=0.2)
            # plot.plot([0,timestamps[-1]], [mean_normalized_move_latency_stat, mean_normalized_move_latency_stat], 'r')
            plot.plot([0,timestamps[index_aftersetuptime]], [mean_normalized_move_latency_beforeloss, mean_normalized_move_latency_beforeloss], color='r', linestyle='--')
            plot.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_normalized_move_latency_afterloss, mean_normalized_move_latency_afterloss], color='r', linestyle='--')
            plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, max_normalized_move_latency], 'k')
            plot.text(timestamps[0], mean_normalized_move_latency_beforeloss, "%5.2f" % (mean_normalized_move_latency_beforeloss), color='b', bbox=dict(facecolor='white', alpha=1))
            plot.text(timestamps[-1], mean_normalized_move_latency_afterloss, "%5.2f" % (mean_normalized_move_latency_afterloss), color='b', bbox=dict(facecolor='white', alpha=1))
            plot.xlim(0, MAX_TIMESTAMP)
            if with_ylim:
                plot.ylim(0, 200)
            
            ax5.get_xaxis().set_visible(False)
            ax5.yaxis.set_major_locator(MaxNLocator(nbins=5))
            plot.ylabel("Latency")
        
        
        if mean_rawmcrecv_stat_afterloss > 0:
            ax4 = plot.subplot(subplot_count,1,subplot_count-1)
            ax4.get_xaxis().set_visible(False)
        else:
            ax4 = plot.subplot(subplot_count,1,subplot_count)
        # ax4.plot(timestamps, send_stat, 'g',label='Send stat')
        # ax4.plot(timestamps, recv_stat, 'b', label='Recv stat')
        # ax4.plot([0,timestamps[-1]], [mean_sendstat, mean_sendstat], 'g--')
        # Text added later
        # ax4.text(timestamps[0], mean_sendstat*0.9, "%5.2f" % (mean_sendstat), color='g', bbox=dict(facecolor='white', alpha=1))
        # ax4.plot([0,timestamps[-1]], [mean_recvstat, mean_recvstat], 'b')
        # ax4.text(timestamps[-1]*0.95, mean_recvstat*0.6, "%5.2f" % (mean_recvstat), color='b')
        ax4.set_ylabel("NIC Send/Recv\n[kBps/node]")
        ax4.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, finite_max(send_stat)], 'k')
        plot.xlim(0, MAX_TIMESTAMP)
        ax4.grid(True)

        # ax4_b = ax4.twinx()
        if (tsharksummary_fileexists):
            ax4.plot(timestamps, nic_sendbytes, 'b')
            ax4.plot([0, timestamps[index_aftersetuptime]], [mean_nicsendbytes_beforeloss, mean_nicsendbytes_beforeloss], 'b--')
            ax4.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_nicsendbytes_afterloss, mean_nicsendbytes_afterloss], 'b--')
            # ax4.plot([0,timestamps[-1]], [mean_nicsendbytes, mean_nicsendbytes], 'b--')
            # ax4.text(timestamps[0], mean_nicsendbytes_beforeloss*1.3, "%5.2f" % (mean_nicsendbytes_beforeloss), color='b', bbox=dict(facecolor='white', alpha=1))
                    
            ax4.plot(timestamps, nic_recvbytes, 'r')    
            ax4.plot([0,timestamps[index_aftersetuptime]], [mean_nicrecvbytes_beforeloss, mean_nicrecvbytes_beforeloss], 'r--')
            ax4.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_nicrecvbytes_afterloss, mean_nicrecvbytes_afterloss], 'r--')

            from matplotlib.lines import Line2D
            # custom_lines = [Line2D([0], [0], color='g', lw=1),
            #                 Line2D([0], [0], color='b', lw=1),
            #                 Line2D([0], [0], color='r', lw=1)]
            custom_lines = [Line2D([0], [0], color='b', lw=1),
                            Line2D([0], [0], color='r', lw=1)]
            # ax4.legend(custom_lines, ['Send VAST', 'Send NIC', 'Recv NIC'], loc='lower center')
            # ax4.legend(custom_lines, ['Send VAST', 'Send NIC', 'Recv NIC'], loc='upper left', mode="expand")
            ax4.legend(custom_lines, ['Send NIC', 'Recv NIC'], loc='upper left', ncol=2)


            # ax4.legend(['Send NIC', 'Recv NIC'])
            # ax4.set_ylabel("NIC Send\nRecv Unicast [kBps]")
            ylim = ax4.get_ylim()
            # ylim = (ylim[0], ylim[1]*1.3) # For UDPNC
            ylim = (ylim[0], ylim[1]*1.3) # For TCP
            print(ylim)
            ax4.set_ylim(ylim)
            

            ypos1 = ((ylim[1] - ylim[0]) + ylim[0])*0.6
            ypos2 = ((ylim[1] - ylim[0]) + ylim[0])*0.4

            if LABEL_list[1] == NET_UDPNC:
                ypos2 = ((ylim[1] - ylim[0]) + ylim[0])*0.5
                ypos1 = ((ylim[1] - ylim[0]) + ylim[0])*0.2

            ypos3 = ((ylim[1] - ylim[0]) + ylim[0])*0.2

            # UDPNC: nicrecv_bytes should be on top, but for other two should be in the middle.
            ax4.text(timestamps[0], ypos2, "%5.2f" % (mean_nicrecvbytes_beforeloss), color='r', bbox=dict(facecolor='white', alpha=1))
            ax4.text(timestamps[-1], ypos2, "%5.2f" % (mean_nicrecvbytes_afterloss), color='r', bbox=dict(facecolor='white', alpha=1))
            ax4.text(timestamps[0], ypos1, "%5.2f" % (mean_nicsendbytes_beforeloss), color='b', bbox=dict(facecolor='white', alpha=1))
            ax4.text(timestamps[-1], ypos1, "%5.2f" % (mean_nicsendbytes_afterloss), color='b', bbox=dict(facecolor='white', alpha=1))
            # ax4.text(timestamps[0], ypos3, "%5.2f" % (mean_sendstat_beforeloss), color='g', bbox=dict(facecolor='white', alpha=1))
            # ax4.text(timestamps[-1], ypos3, "%5.2f" % (mean_sendstat_afterloss), color='g', bbox=dict(facecolor='white', alpha=1))
        
        

        if mean_rawmcrecv_stat_afterloss > 0:
            ax5 = plot.subplot(subplot_count,1,subplot_count)
            # plot.plot(timestamps, raw_mcrecvbytes, 'r',label='Raw MC Recv')
            # plot.plot(timestamps, used_mcrecvbytes, 'm', label='MC Recv')
            plot.plot(timestamps, used_mcrecvbytes, color='m')
            # plot.plot([0,timestamps[-1]], [mean_rawmcrecv_stat, mean_rawmcrecv_stat], 'r')
            plot.plot([0,timestamps[index_aftersetuptime]], [mean_usedmcrecv_stat_beforeloss, mean_usedmcrecv_stat_beforeloss], color='r', linestyle='--')
            plot.plot([timestamps[index_aftersetuptime],timestamps[-1]], [mean_usedmcrecv_stat_afterloss, mean_usedmcrecv_stat_afterloss], color='r', linestyle='--')
            # plot.plot([0,timestamps[-1]], [mean_usedmcrecv_stat, mean_usedmcrecv_stat], color='m', linestyle='--')
            plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, finite_max(raw_mcrecvbytes)], 'k')
            # plot.text(timestamps[-1], mean_rawmcrecv_stat, "%5.2f" % (mean_rawmcrecv_stat), color='r')
            plot.text(timestamps[0], mean_usedmcrecv_stat_beforeloss, "%5.2f" % (mean_usedmcrecv_stat_beforeloss), color='m', bbox=dict(facecolor='white', alpha=1))
            plot.text(timestamps[-1], mean_usedmcrecv_stat_afterloss, "%5.2f" % (mean_usedmcrecv_stat_afterloss), color='m', bbox=dict(facecolor='white', alpha=1))
            plot.xlim(0, MAX_TIMESTAMP)
            plot.ylabel("Coded Recv\n[kBps/node]")




        if not resources_fileexist:
            plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
        else:
            ax5.get_xaxis().set_visible(False)
        # plot.legend()
        plot.grid(True)
        plot.xlim(0, MAX_TIMESTAMP)

        # if resources_fileexist:
        #     ax1 = plot.subplot(6,1,6)
        #     ax1.plot(numpy_resources[:,0], numpy_resources[:,1], 'r',label='CPU %')
        #     ax1.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(numpy_resources[:,1])], 'k')
        #     ax1.text(timestamps[-1000], mean_CPU, "%3.2f" % mean_CPU, color='r')
        #     color = 'tab:red'
        #     ax1.set_ylabel("CPU % ", color=color)

        #     ax2 = ax1.twinx()
        #     ax2.plot(numpy_resources[:,0], numpy_resources[:,2], 'b',label='Mem')
        #     ax2.text(timestamps[-1000], median_MemMB, "%3.2f" % median_MemMB, color='b')
        #     color = 'tab:blue'
        #     ax2.set_ylabel("Mem [MB]", color=color)
        #     plot.xlabel("Timestamp [ms]")
        #     plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
        #     plot.grid(True)
        #     plot.xlim(0, MAX_TIMESTAMP)

        if events_fileexist:
            ax1 = plot.subplot(6,1,3)
            for event in events:
                # print(event)
                # print(np.where(unique_messages==event[2])[0][0])
                plot.plot([event[0], event[0]], [0, np.max(normalised_drift_distance[where_is_finite])]) 
                plot.text(event[0], np.max(normalised_drift_distance[where_is_finite]), 
                    str(np.where(unique_messages==event[2])[0][0]), rotation=90)
                # plot.xlim(0, MAX_TIMESTAMP)
                # plot.ylim(0, 100)

        fig_filename = "VASTreal_results.pdf"

        if (LABEL_list):
            fig_filename = re.sub("logs_net_", "", LABEL_string)
            fig_filename = re.sub("/", "", fig_filename)
            print(fig_filename)
            
        if (with_ylim):
            plot.savefig("%s.pdf" % fig_filename, dpi=300)
        else:
            plot.savefig("%s_without_ylim.pdf" % fig_filename, dpi=300)  

        plot.xlabel("Timestamp [ms]")

plot_results()
plot_results(True)

if len(results_header) == MIN_TOPOCONS+1:
    print("It contains max, min Topo consistency")
else:
    print(results_header)
    print("Len of reesults_header: ", len(results_header))

plot.show()
