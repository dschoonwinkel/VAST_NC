#!/usr/bin/env bash
for i in {1..5}
do
	echo "Run $i"
	make
	label=$(./generate_label.py)
	echo $label
	sudo ../mininet_topos/Multinode_pox_codinghost.py
	./vaststat_replay
	make backup_results foo=$label
	echo "Run $i completed $HOSTNAME $label"
	echo "Run $i completed $HOSTNAME $label" | nc -N horus.local 12345 

done
