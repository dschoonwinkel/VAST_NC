#!/usr/bin/env bash

sudo -s ./repeated_VASTreal_console.py

# for i in {1..1}
# do
# 	echo "Run $i"
# 	make
# 	label=$(./generate_label.py)
# 	echo $label
# 	sudo ../mininet_topos/Multinode_pox_codinghost.py
# 	./vaststat_replay
# 	make backup_results foo=$label
# 	echo "Run $i completed $HOSTNAME $label"
# 	echo "Run $i completed $HOSTNAME $label" | nc 10.10.11.237 12345 -q 1

# done
