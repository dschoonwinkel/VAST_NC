#!/bin/bash
for i in {1..5}
do
	echo "Run $i"
	make
	sudo ../mininet_topos/Multinode_pox_codinghost.py
	./vaststat_replay
	make backup_results foo=net_udpnc_10perc

done