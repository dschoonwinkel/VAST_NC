#!/usr/bin/env python3

import glob
import numpy as np
# import matplotlib.pyplot as plot
from os.path import expanduser
import os
import csv
from datetime import datetime, timedelta
import re

TIMESTAMP = 0
LEN = 1
SRC = 2
DEST = 3
TYPE = 4

SEND_BYTES = 14
RECV_BYTES = 15


tsharkpcaps_files = glob.glob("*pcapout.csv")
results_file = "../results/results1.txt"
# timestamped_tshark_filename = "../results/results1_tshark_summary.txt"
timestamped_tshark_filename = re.sub(r"\.txt", "_tshark_summary.txt", results_file)

if not os.path.isfile(results_file):
    print("Results file does not exist, exiting")
    exit(0)

if os.path.isfile(timestamped_tshark_filename):
    print("Timestamped Tshark file already exists, exiting")
    exit(0)


print(tsharkpcaps_files)
print(results_file)


# filename = tsharkpcaps_files[0]
# filename = 'h10_pcapout.csv'

results = list()
with open(results_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    header = next(spamreader, None)
    for row in spamreader:
        # if len(row) > SEND_BYTES:
        #     print("Results already include two extra columns, probably already parsed, exiting")
        #     exit(0)

        row.extend([0,0])
        results.append(row)

if len(results[0]) < 15:
    print("Results are too short, using SEND_BYTES == 12")
    SEND_BYTES = 12
    RECV_BYTES = 13




tsharkdata_dict = dict()
tshark_results_indexes = dict()

#Parse all tshark files
for filename in tsharkpcaps_files:

    tsharkdata_text = list()
    with open(filename, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=",")
        for row in spamreader:
            tsharkdata_text.append(row)

    # print(tsharkdata_text)
    tshark_results = list()

    for line in tsharkdata_text:
        #Malformed line, probably saved incorrectly
        if len(line) < 6:
            continue
        

        # print(line)
        datestr = re.search(r'\w+\s+\d+, \d{4} \d{2}:\d{2}:\d{2}.\d{6}', line[1])
        # print(datestr)
        if not datestr:
            break
        datestr = datestr.group(0)

        date = datetime.strptime(datestr, "%b %d, %Y %H:%M:%S.%f")
        date += timedelta(hours=2)

        # print(date)
        # print(datetime.timestamp(date))
        line = line[1:]
        #Convert to epoch timestamp, and milliseconds
        line[0] = datetime.timestamp(date)*1000

        if line[SRC] == '' or line[DEST] == '':
            continue

        #Remove IGMP packets
        if line[TYPE] == '2':
            # print(line)
            continue

        #Remove ICMP - destination unknown packets
        if line[TYPE] == '1,17':
            # print(line)
            # print(datestr)
            continue

        # print(line)
        tshark_results.append(line)

    for line in tshark_results:
        #Check if there is anything other than UDP or TCP
        if not (line[TYPE] == '17' or line[TYPE] == '6'):
            print(filename, line)

    tsharkdata_dict[filename] = tshark_results
    tshark_results_indexes[filename] = 0 


timestamped_tshark_list = list()
timestamped_tshark_header = ['timestamp', 'NIC send', 'NIC recv']

#Interlace tshark files in results
for result in results:
    # print(result)
    latest_timestamp = int(result[0])
    recv_bytes = 0
    send_bytes = 0

    #Run through every file
    for tshark_resultskey in tsharkdata_dict.keys():

        tshark_results = tsharkdata_dict[tshark_resultskey]
        tshark_results_index = tshark_results_indexes[tshark_resultskey]

        while tshark_results_index < len(tshark_results) and tshark_results[tshark_results_index][TIMESTAMP] < latest_timestamp:
            # print(tshark_results[tshark_results_index])
            this_ip_address = '10.0.0.' + re.search(r'\d+', tshark_resultskey).group(0)
            # print(this_ip_address)
            # print(latest_timestamp, tshark_results[tshark_results_index][TIMESTAMP])
            # print("\t\t", tshark_results[tshark_results_index])
            # print(datetime.fromtimestamp(latest_timestamp/1000), datetime.fromtimestamp(tshark_results[tshark_results_index][TIMESTAMP]/1000))
            # print(latest_timestamp, tshark_results[tshark_results_index][TIMESTAMP])
            if tshark_results[tshark_results_index][SRC] == this_ip_address:
                send_bytes += int(tshark_results[tshark_results_index][LEN])

            elif tshark_results[tshark_results_index][DEST] == this_ip_address:
                recv_bytes += int(tshark_results[tshark_results_index][LEN])

            tshark_results_index += 1

        tshark_results_indexes[tshark_resultskey] = tshark_results_index

    result[SEND_BYTES] += send_bytes
    result[RECV_BYTES] += recv_bytes
    timestamped_tshark_list.append([result[TIMESTAMP], send_bytes, recv_bytes])

    # print("send_bytes", send_bytes)
    # print("recv_bytes", recv_bytes)

    # print("Timestamp:\t ", latest_timestamp)

# for result in results:
#     print(result)

# with open(results_file, 'w') as csvfile:
#     spamwriter = csv.writer(csvfile, delimiter=",")
#     spamwriter.writerow(header)
#     for result in results:
#         spamwriter.writerow(result)

with open(timestamped_tshark_filename, 'w') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=",")
    spamwriter.writerow(timestamped_tshark_header)
    for line in timestamped_tshark_list:
        spamwriter.writerow(line)

