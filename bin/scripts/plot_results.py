#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
import os
from plot_result_utils import parseFilenameLabel, NET_MODEL_STRINGS
import re
import glob

hasMatplotlib = True
try:
    import matplotlib.pyplot as plot
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

print("Usage: ./plot_results.py <input_file: default = results1.txt>\n\
                 <results label = \"NET_MODEL, nodecount, BW, delay, loss%\">")

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

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

results_text = list()

input_file = '%s/Development/VAST-0.4.6/bin/logs/results/results1.txt' % home_dir

if (len(sys.argv) > 1):
    input_file = sys.argv[1]

plot_yes = False

if (len(sys.argv) > 2):
    print(sys.argv[2])
    print("Plotting")
    plot_yes = True


# print(input_file)

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
else:
    print('LABEL_start not found')









with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)
        # print(",".join(row))

header = results_text[0]
results_text = results_text[1:]

results = list()

for row in results_text:
    # print(row)
    results.append([(int(row[0])-int(results_text[0][0])), int(row[1]), int(row[2]), int(row[3]),
     int(row[4]), int(row[5]), int(row[6]), int(row[7]), int(row[8]), int(row[9]), int(row[10]), int(row[11])])
    # print(results[-1])
    # print(int(row[0])%10000)

numpy_results = np.array(results)

# print("Timestamps: ", numpy_results[:,0])

timestamps = numpy_results[:,0]
timestamps = timestamps[np.where(timestamps < MAX_TIMESTAMP)]
first_timestamp = int(results_text[0][0])
last_relative_timestamp = timestamps[-1]

if last_relative_timestamp < 0.9 * MAX_TIMESTAMP:
    print("Test was not completed, using last timestamp as endpoint, rescaling xticks")
    x_axis_interval = x_axis_interval * last_relative_timestamp / MAX_TIMESTAMP
    MAX_TIMESTAMP = last_relative_timestamp

active_nodes = numpy_results[:,ACTIVE_NODES]
max_nodes = np.max(active_nodes)
active_matchers = numpy_results[:,ACTIVE_MATCHERS]
max_matchers = np.max(active_matchers)

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

# Show results in kBps -> 10 ticks per second (1000ms / 100ms per tick) / 1000 B per kB
CONVERSION_FACTOR = (1000 / TIMESTEP_DURATION) / 1000


send_stat = (numpy_results[:,WORLDSENDSTAT]*CONVERSION_FACTOR)[:len(timestamps)]
recv_stat = (numpy_results[:,WORLDRECVSTAT]*CONVERSION_FACTOR)[:len(timestamps)]
mean_sendstat = np.mean(send_stat)
mean_recvstat = np.mean(recv_stat)

print("Mean send_stat:", mean_sendstat)
print("Mean send_recv:", mean_recvstat)

raw_mcrecvbytes = (numpy_results[:,RAW_MCRECVBYTES]*CONVERSION_FACTOR)[:len(timestamps)]
used_mcrecvbytes = (numpy_results[:,USED_MCRECVBYTES]*CONVERSION_FACTOR)[:len(timestamps)]
mean_rawmcrecv_stat = np.mean(raw_mcrecvbytes)
mean_usedmcrecv_stat = np.mean(used_mcrecvbytes)

print("Mean raw_mcrecvbytes:", mean_rawmcrecv_stat)
print("Mean used_mcrecvbytes:", mean_usedmcrecv_stat)




# Output to results summary, only if we know the LABEL
if LABEL_list:
    NODE_COUNT = LABEL_list[1]
    LABEL_list.insert(0, first_timestamp)
    LABEL_list.extend([np.max(active_nodes), np.max(active_matchers), mean_consistency, 
                  mean_drift_distance, mean_sendstat, mean_recvstat, 
                  mean_rawmcrecv_stat, mean_usedmcrecv_stat])
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
            outfile.write(("%s, %d, %d, %d, %d, %d, %d, %d, %3.2f, %3.2f, %f, %f, %f, %f, %f, %f, %s\n") % 
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







TOTAL_SETUPTIME = TIMESTEP_DURATION * 1000 + (1 + TIMESTEP_DURATION * 10)*NODE_COUNT
print("Total connection setup time: ", TOTAL_SETUPTIME, 'ms')
print("Total connection setup time: ", TOTAL_SETUPTIME / 1000, 's')


if (hasMatplotlib and plot_yes):

    plot.figure(1, figsize=(12, 10), dpi=80)
    if (LABEL_list):
        plot.title(str(LABEL_list))

    ax1 = plot.subplot(6,1,1)
    plot.plot(timestamps, active_nodes[:len(timestamps)])
    plot.plot(timestamps, active_matchers[:len(timestamps)])
    plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, max(active_nodes)+1], 'k')
    plot.text(timestamps[-1], max_nodes, str(max_nodes))
    plot.text(timestamps[-1], max_matchers, str(max_matchers))
    plot.ylabel("# Active")
    plot.legend(['Nodes', 'Matchers'])
    ax1.get_xaxis().set_visible(False)
    plot.yticks(np.arange(min(active_nodes),max(active_nodes)+1, max(active_nodes)/5))
    plot.xlim(0, MAX_TIMESTAMP)
    plot.grid(True)

    ax2 = plot.subplot(6,1,2)
    plot.plot(timestamps, topo_consistency)
    plot.plot(timestamps[where_are_NaNs], topo_consistency[where_are_NaNs], 'r,')
    plot.plot([0,timestamps[-1]], [mean_consistency,mean_consistency], 'r')
    plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, 100], 'k')
    plot.text(timestamps[-1], mean_consistency, "%5.2f" % mean_consistency)
    plot.ylabel("Topo consistency\n[%]")
    ax2.get_xaxis().set_visible(False)
    plot.xlim(0, MAX_TIMESTAMP)
    plot.ylim(0, 110)
    plot.grid(True)
    
    ax3 = plot.subplot(6,1,3)
    plot.plot(timestamps, normalised_drift_distance)
    plot.plot([0,timestamps[-1]], [mean_drift_distance,mean_drift_distance], 'r')
    plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(normalised_drift_distance[where_is_finite])], 'k')
    plot.ylabel("Norm drift\ndist[VE units]")
    plot.text(timestamps[-1], mean_drift_distance, "%5.2f" % mean_drift_distance)
    plot.xlim(0, MAX_TIMESTAMP)
    ax3.get_xaxis().set_visible(False)
    plot.grid(True)
    
    ax4 = plot.subplot(6,1,4)
    ax4.plot(timestamps, send_stat, 'g',label='Send stat')
    ax4.plot(timestamps, recv_stat, 'b', label='Recv stat')
    ax4.plot([0,timestamps[-1]], [mean_sendstat, mean_sendstat], 'g')
    ax4.text(timestamps[0], mean_sendstat*1.1, "%5.2f" % (mean_sendstat), color='g')
    ax4.set_ylabel("Send\nUnicast [kBps]", color='g')
    ax4.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(send_stat)], 'k')
    ax4.get_xaxis().set_visible(False)
    ylim = ax4.get_ylim()
    plot.xlim(0, MAX_TIMESTAMP)
    ax4.grid(True)

    ax4_b = ax4.twinx()    
    ax4_b.plot([0,timestamps[-1]], [mean_recvstat, mean_recvstat], 'b')
    ax4_b.set_ylabel("Recv\nUnicast [kBps]", color='b')
    ax4.text(timestamps[-1]*0.95, mean_recvstat*0.6, "%5.2f" % (mean_recvstat), color='b')
    ax4_b.set_ylim(ylim)
    
    
    

    ax5 = plot.subplot(6,1,5)
    plot.plot(timestamps, raw_mcrecvbytes, 'r',label='Raw MC Recv')
    plot.plot(timestamps, used_mcrecvbytes, 'm', label='Used MC Recv')
    plot.plot([0,timestamps[-1]], [mean_rawmcrecv_stat, mean_rawmcrecv_stat], 'r')
    plot.plot([0,timestamps[-1]], [mean_usedmcrecv_stat, mean_usedmcrecv_stat], 'm')
    plot.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(raw_mcrecvbytes)], 'k')
    plot.text(timestamps[-1], mean_rawmcrecv_stat, "%5.2f" % (mean_rawmcrecv_stat), color='r')
    plot.text(timestamps[-1], mean_usedmcrecv_stat, "%5.2f" % (mean_usedmcrecv_stat), color='m')
    
    plot.ylabel("Raw/Used\nMC [kBps]")
    if not resources_fileexist:
        plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
    else:
        ax5.get_xaxis().set_visible(False)
    plot.legend()
    plot.grid(True)
    plot.xlim(0, MAX_TIMESTAMP)

    if resources_fileexist:
        ax1 = plot.subplot(6,1,6)
        ax1.plot(numpy_resources[:,0], numpy_resources[:,1], 'r',label='CPU %')
        ax1.plot([TOTAL_SETUPTIME, TOTAL_SETUPTIME],[0, np.max(numpy_resources[:,1])], 'k')
        ax1.text(timestamps[-1000], mean_CPU, "%3.2f" % mean_CPU, color='r')
        color = 'tab:red'
        ax1.set_ylabel("CPU % ", color=color)

        ax2 = ax1.twinx()
        ax2.plot(numpy_resources[:,0], numpy_resources[:,2], 'b',label='Mem')
        ax2.text(timestamps[-1000], median_MemMB, "%3.2f" % median_MemMB, color='b')
        color = 'tab:blue'
        ax2.set_ylabel("Mem [MB]", color=color)
        plot.xlabel("Timestamp [ms]")
        plot.xticks(np.arange(min(timestamps), max(timestamps)+1, x_axis_interval))
        plot.grid(True)
        plot.xlim(0, MAX_TIMESTAMP)

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

        
    
    plot.savefig("VASTreal_results_%s.pdf" % input_file, dpi=300)
    # plot.savefig("VASTreal_results_%s.png" % label, dpi=300)

    plot.xlabel("Timestamp [ms]")
    
    plot.show()
