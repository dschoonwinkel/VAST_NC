#!/usr/bin/env python3
import numpy as np
import csv
import sys
from os.path import expanduser
import os
from plot_result_utils import parseFilenameLabel, NET_MODEL_STRINGS, finite_mean, finite_max

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

