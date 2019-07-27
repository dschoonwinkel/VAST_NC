#!/usr/bin/env python3
import glob
import os

for name in glob.glob('logs_horus/*/*/output_dump/*.txt'):
	# print(name)
	# print(os.path.split(name))
	dirname = os.path.split(name)[0][11:]
	filename = os.path.split(name)[1]

	print(dirname)
	print(filename)

	with open(name, 'r') as f1:
		data = f1.readlines()
		data = data[-30:]
		# print(data)

		for line in data:
			if (line.find("total_packets_recvd:") != -1):
				print(line[:-1])
			if (line.find("total_not_meantforme_unicast") != -1):
				print(line[:-1])
			if (line.find("~net_udpNC_consumer: total_packets_processed") != -1):
				print(line[:-1])
			if (line.find("~net_udpNC_consumer: total_not_meantforme_unicast") != -1):
				print(line[:-1])
			if (line.find("~net_udpNC_consumer: total_not_meantforme_multicast") != -1):
				print(line[:-1])
			if (line.find("~net_udpNC_consumer: total_toolate_packets") != -1):
				print(line[:-1])
			if (line.find("~net_udpNC_consumer: total used packets") != -1):
				print(line[:-1])



