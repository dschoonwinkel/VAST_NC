#!/usr/bin/env python3
import glob
from os.path import expanduser
import os
import subprocess
import re
from datetime import datetime
import sys

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

finddir = "%s/Development/VAST-0.4.6/bin/logs/logs_backup" % home_dir

if len(sys.argv) > 1:
    finddir = sys.argv[1]

result_files = glob.glob(finddir+"/*/*/results1.txt")

results_summary_filename = '%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir
if (os.path.isfile(results_summary_filename)):
    with open(results_summary_filename, 'r') as summary_file:
        data = summary_file.readlines()

# print(result_files)

datelist = list()
for file in result_files:
    datestr = re.search(r'\d{2}_\d{2}_\d{4}-\d{2}:\d{2}', file).group(0)
    # print(datestr)
    datelist.append(datetime.strptime(datestr, "%d_%m_%Y-%H:%M"))

# print(datelist)
sorted_result_files = [result_files for _, result_files in sorted(zip(datelist, result_files))]

for file in sorted_result_files:
    print(file)
    already_in_result_summary = False

    datestr = re.search(r'\d{2}_\d{2}_\d{4}-\d{2}:\d{2}', file).group(0)
    for line in data:
        if line.find(datestr) != -1:
            print("parse_all_backupresults: Result already in summmary: \n", line)
            already_in_result_summary = True
            break
            
    if already_in_result_summary:
        continue

    print(subprocess.call("plot_results_quick.py %s" % file, shell=True))

