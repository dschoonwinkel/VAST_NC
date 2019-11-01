#!/usr/bin/env python3

import sys
import subprocess
from generate_label import generate_label
import socket

NET_MODEL_ACE = 2
NET_MODEL_UDP = 3
NET_MODEL_UDPNC = 4

def runOnce():
	print("Run sucess:", 
		subprocess.call("./repeated_VASTrealconsole_realOVS.py", 
		shell=True), "\n")


def replace_NETMODEL(NET_MODEL):

	with open("VASTreal_original.ini", 'r') as config:
		data = config.readlines()

	for i in range(len(data)):
		if data[i].find("NET_MODEL") != -1:
			data[i+1] = "%s\n" % NET_MODEL

	with open("VASTreal.ini", 'w') as config:
		for line in data:
			config.write(line)


	with open("Mininet_original.ini", 'r') as config:
		data = config.readlines()

	for i in range(len(data)):
		if data[i].find("PLATFORM") != -1:
			data[i+1] = "3\n" #Set platform to realOVS

	with open("Mininet.ini", 'w') as config:
		for line in data:
			config.write(line)




def main():
	NET_MODEL_list = [NET_MODEL_ACE, NET_MODEL_UDP, NET_MODEL_UDPNC]
	#NET_MODEL_list = [NET_MODEL_UDP, NET_MODEL_UDPNC]
	NET_MODEL_list_str = ["", "NET_MODEL_EMU", "NET_MODEL_ACE", "NET_MODEL_UDP", "NET_MODEL_UDPNC"]

	#Run over all NODE_COUNT options
	for i in range(len(NET_MODEL_list)):
		replace_NETMODEL(NET_MODEL_list[i])
		runOnce()
		print("Run completed %s %s" % (socket.gethostname(), NET_MODEL_list_str[NET_MODEL_list[i]]))

if __name__ == '__main__':
	main()
