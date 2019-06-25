#!/bin/bash
for i in {1..5}
do
	echo "Run $i"
	make
	sudo ../mininet_topos/Multinode_pox_codinghost.py
	./vaststat_replay

done