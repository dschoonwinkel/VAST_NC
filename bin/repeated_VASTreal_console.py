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
		subprocess.call("sudo ../mininet_topos/Multinode_pox_codinghost.py", 
		shell=True), "\n")
	print("VASTStatReplay success:", 
		subprocess.call("./vaststat_replay", 
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
			data[i+1] = "1\n" #Set platform to Mininet

	with open("Mininet.ini", 'w') as config:
		for line in data:
			config.write(line)

def main():
	#NODE_COUNT_list = [50, 40, 30, 20, 10]
	#NODE_COUNT_list = [40, 30, 20, 10]
	NODE_COUNT_list = [50]
	# NODE_COUNT_list = [15]

	with open("Mininet.ini", 'r') as config:
			data = config.readlines()

	for i in range(len(data)):
		if data[i].find("ITERATIONS") != -1:
			ITERATIONS = int(data[i+1])
			print("ITERATIONS: ", ITERATIONS)


	#Run over all NODE_COUNT options
	for i in range(len(NODE_COUNT_list)):
		replace_NODECOUNT(NODE_COUNT_list[i])
		#Run ITERATIONS
		for j in range(ITERATIONS):
			label = runOnce()
			print("Run %d completed %s %s" % (j, socket.gethostname(), label))
			with open("run_log.txt", 'a') as runlog:
				runlog.write("Run %d completed %s %s\n" % (j, socket.gethostname(), label))

		# subprocess.call("echo \"Runs completed on %s with label %s\" \
		# 	| mail -s \"Test status\" daniel.schoonwinkel@gmail.com" % 
		# 	(socket.gethostname(), label),
		# 		shell=True)

	with open("run_log.txt", 'a') as runlog:
		runlog.write("Runs completed\n\n")
	

	# replace_NODECOUNT(NODE_COUNT_list[0])
	# print(runOnce())

if __name__ == '__main__':
	main()
