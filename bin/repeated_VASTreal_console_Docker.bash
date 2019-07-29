#!/usr/bin/env bash
for i in {1..1}
do
	echo "Run $i"
	sudo make
	label=$(./generate_label.py)
	echo $label
	$HOME/Development/Docker/vastrealconsole_docker/run_multiple.py
	$HOME/Development/Docker/vastrealconsole_docker/vaststat_replay.bash
	make backup_results foo=$label
	echo "Run $i completed $HOSTNAME $label"
	echo "Run $i completed $HOSTNAME $label" | nc 10.10.11.237 12345 -q 1
	./run\_plotresults.bash $HOME/Development/VAST-0.4.6/bin/logs/results/results1.txt $label

done
