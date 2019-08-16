#!/usr/bin/env python3
import glob
import sys
from os.path import expanduser
import os
import re
import csv
from tabulate import tabulate
import numpy as np


print("Usage: ./plot_events.py <results_file.txt>")

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

results_text = list()

input_file = '%s/Development/VAST-0.4.6/bin/logs/results/results1.txt' % home_dir

if (len(sys.argv) > 1):
    input_file = sys.argv[1]


results_text = list()

with open(input_file, 'r') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",")
    for row in spamreader:
        results_text.append(row)

header = results_text[0]
results_text = results_text[1:]
first_timestamp = int(results_text[0][0])

print(first_timestamp)

input_file = os.path.abspath(input_file)
print("Input_file", input_file)

dirname = os.path.split(input_file)[0] 
dirname = os.path.split(dirname)[0] + "/output_dump"

output_dump_file_exist = False
# print(dirname)

if (os.path.exists(dirname)):
    output_dump_file_exist = True
    print("output_dump_file_exist file found")
    print(dirname)

else:
    dirname = os.path.split(input_file)[0]
    # print(dirname)
    # dirname = os.path.split(dirname)[0] + "/output_dump"
    dirname = os.path.split(dirname)[0]
    # print(dirname)
    dirname = os.path.split(dirname)[0] + "/output_dump"
    if (os.path.exists(dirname)):
        output_dump_file_exist = True
        print("output_dump_file_exist file found")
        print(dirname)

events = list()

if output_dump_file_exist:
    files = glob.glob(dirname+"/*.txt")
    # print(files)

    for file in files:
        with open(file, 'r') as logfile:
            data = logfile.readlines()
            for line in data:
                if (re.search(r'\d{16}=\d+:\d+:\d+', line)):
                    node_label = os.path.split(file)[1]
                    # print(node_label, ": ", line)
                    timestamp = int(re.search(r'\d{16}', line)[0]) / 1000
                    message = re.search(r'\d{16}=.+?\s.*', line)[0][34:]
                    # print(timestamp, node_label, message)
                    events.append([timestamp-first_timestamp, node_label, message])

    events.sort(key=lambda x: x[0])

    output_filename = re.sub('results1.txt', 'results1_events.txt', input_file)
    print(output_filename)
    with open(output_filename, 'w') as events_file:
        # print(tabulate(events))
        # events_file.write(tabulate(events))
        spamwriter = csv.writer(events_file, delimiter=",")
        for event in events:
            spamwriter.writerow(event)

else:
    print("output_dump_folder does not exist")



