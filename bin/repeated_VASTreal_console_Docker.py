#!/usr/bin/env python3

import sys
import subprocess
from generate_label import generate_label
import socket

def runOnce():
	print("Make success: ", subprocess.call("make"), "\n")
	label = generate_label()
	print(label)
	print("Mininet success:", 
		subprocess.call("$HOME/Development/Docker/vastrealconsole_docker/run_multiple.py", 
		shell=True), "\n")
	print("VASTStatReplay success:", 
		subprocess.call("$HOME/Development/Docker/vaststatreplay_docker/run.bash.py", 
		shell=True), "\n")
	print("Make backup success:", 
		subprocess.call("make backup_results foo=%s" % label,
		shell=True), "\n")

	return label
	


def replace_NODECOUNT(node_count):

	with open("Mininet.ini", 'r') as config:
		data = config.readlines()

	for i in range(len(data)):
		if data[i].find("NODE_COUNT") != -1:
			data[i+1] = "%s\n" % node_count
		if data[i].find("PLATFORM") != -1:
			data[i+1] = "2\n" #Set platform to Docker

	with open("Mininet.ini", 'w') as config:
		for line in data:
			config.write(line)


NODE_COUNT_list = [5, 10, 11, 15]
iterations = 1

# Run over all NODE_COUNT options
# for i in range(len(NODE_COUNT_list)):
# 	replace_NODECOUNT(NODE_COUNT_list[i])
# 	#Run iterations
# 	for j in range(iterations):
# 		# label = runOnce()
# 		label = "1"
# 		print("Run %d completed %s %s" % (j, socket.gethostname(), label))

# 	subprocess.call("echo \"Runs completed on %s with label %s\" \
# 		| mail -s \"Test status\" daniel.schoonwinkel@gmail.com" % 
# 		(socket.gethostname(), label),
# 			shell=True)

replace_NODECOUNT(NODE_COUNT_list[0])
# print(runOnce())