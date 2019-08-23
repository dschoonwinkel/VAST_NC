#!/usr/bin/env python3
import glob
from os.path import expanduser
import os
import subprocess
import re
from datetime import datetime
import shutil

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

finddir = "%s/Development/VAST-0.4.6/bin/logs/logs_backup" % home_dir
results_extracted_dir = "%s/Development/VAST-0.4.6/bin/results_extracted" % home_dir

if not os.path.exists(results_extracted_dir):
	print("Making dir %s" % results_extracted_dir)
	os.mkdir(results_extracted_dir)

result_files = glob.glob(finddir+"/*/*/results1.txt")

for file in result_files:
	print(file)
	extracted_filename = re.sub(r"/results/", "_", file)
	extracted_filename = os.path.split(extracted_filename)[1]
	# print(extracted_filename)

	if (os.path.isfile(results_extracted_dir+"/" + extracted_filename)):
		print("File extracted already")

	else:
		shutil.copy2(file, results_extracted_dir+"/" + extracted_filename)

result_files = glob.glob(finddir+"/*/*/results1_resources.txt")

for file in result_files:
	print(file)
	extracted_filename = re.sub(r"/results/", "_", file)
	extracted_filename = os.path.split(extracted_filename)[1]
	# print(extracted_filename)

	if (os.path.isfile(results_extracted_dir+"/" + extracted_filename)):
		print("File extracted already")

	else:
		shutil.copy2(file, results_extracted_dir+"/" + extracted_filename)
