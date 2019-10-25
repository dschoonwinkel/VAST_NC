#!/usr/bin/env python3
import glob
from os.path import expanduser
import os
import subprocess
import re
from datetime import datetime
import sys

home_dir = expanduser("~")
# print("Home Dir: ", home_dir)

# finddir = "%s/Development/VAST-0.4.6/bin/logs/logs_backup" % home_dir

if len(sys.argv) < 2:
    print("Usage: all_parsetshark_indirectory.py <folder>")
    finddir = os.getcwd()
else:
    finddir = sys.argv[1]

abspath = os.path.abspath(finddir)

print(finddir)

result_folders = glob.glob(finddir+"/*/pcaps")
print(result_folders)
if len(result_folders) == 0:
	result_folders = glob.glob(finddir+"pcaps")
	print(result_folders)

working_directory = os.getcwd()

for folder in result_folders:
    print(folder)
    os.chdir(folder)
    print(subprocess.call("parse_tsharkpcaps.py", shell=True))
    os.chdir(working_directory)

