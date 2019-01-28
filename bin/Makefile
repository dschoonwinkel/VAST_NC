clean:
	-rm *.log
	-rm *.pos
	-rm *.stat
	-rm logs/*.txt
	-rm logs/results/results1.txt

clean_binaries:
	-rm demo_chatva_Qt
	-rm demo_console
	-rm my_demo
	-rm myVASTsim
	-rm random_walkertalker
	-rm SimNode_demo
	-rm test_console
	-rm vastplayground
	-rm VASTsim_console
	-rm VASTsim_Qt

backup_results:
	$(eval date_string := $(shell date +%d_%m_%Y-%H:%M))
	mkdir logs/logs_backup/logs_$(date_string)
	cp logs/*.txt logs/logs_backup/logs_$(date_string)/
	mkdir logs/logs_backup/logs_$(date_string)/results
	cp logs/results/results1.txt logs/logs_backup/logs_$(date_string)/results/

restore_results:
	$(backup_results)
	cp logs/logs_backup/logs_$(foo)/*.txt ./logs/
	echo $(foo)
