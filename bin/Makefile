clean:
	-rm *.log
	#-rm *.pos
	-rm *.stat
	-rm logs/*.txt
	-rm logs/results/results1.txt
	-rm output_dump/*.txt
	-rm perf/*.data
	-rm pcaps/*.pcap

clean_binaries:
	-rm demo_chatva_Qt
	-rm demo_console
	-rm my_demo
	-rm myVASTsim
	-rm random_walkertalker
	-rm SimNode_demo
	-rm test_console
	-rm vastplayground
	-rm VASTStatReplayQt
	-rm VASTsim_console
	-rm VASTsim_Qt


backup_results:
	$(eval date_string := $(shell date +%d_%m_%Y-%H:%M))
	mkdir logs/logs_backup/logs_$(foo)_$(date_string)
	cp logs/*.txt logs/logs_backup/logs_$(foo)_$(date_string)/
	mkdir logs/logs_backup/logs_$(foo)_$(date_string)/results
	cp logs/results/results1.txt logs/logs_backup/logs_$(foo)_$(date_string)/results/
	mkdir logs/logs_backup/logs_$(foo)_$(date_string)/output_dump
	cp output_dump/*.txt logs/logs_backup/logs_$(foo)_$(date_string)/output_dump/

restore_results:
	$(backup_results)
	cp logs/logs_backup/logs_$(foo)/*.txt ./logs/
	echo $(foo)

save_plots:
	echo "Saving plots as: $(foo).png"
	cp logs/results/active_nodes_topo_cons_.png logs/results/$(foo).png

