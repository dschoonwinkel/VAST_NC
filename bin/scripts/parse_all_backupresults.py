#!/usr/bin/env python3
import glob
from os.path import expanduser
import subprocess
import re
from datetime import datetime

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

finddir = "%s/Development/VAST-0.4.6/bin/logs/logs_backup" % home_dir

result_files = glob.glob(finddir+"/*/*/results1.txt")

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
	print(subprocess.call("plot_results_quick.py %s" % file, shell=True))

