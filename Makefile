
VASTnet := VASTnet
VAST    := VAST 
VASTsim := VASTsim
VASTreal := VASTreal
common  := common
#VASTATE := VASTATE
#VASTATEsim := VASTATEsim
demo_console := Demo/demo_console
test_console := Demo/test_console
VASTsim_Qt := Demo/VASTsim_Qt
my_demo := Demo/my_demo
VASTsim_console := Demo/VASTsim_console
myVASTsim := Demo/myVASTsim
random_walkertalker := Demo/random_walkertalker
demo_chatva_Qt := Demo/demo_chatva_Qt
vaststat_replay := Demo/VASTStatReplay
vaststat_replay_Qt := Demo/VASTStatReplayQt
VASTreal_console := Demo/VASTreal_console
coding_host := coding_host

# tells make that the following labels are make targets, not filenames
# .PHONY: all clean $(VASTnet) $(VAST) $(VASTsim) $(common) $(test_console) $(VASTsim_Qt) $(demo_console) $(my_demo) $(VASTsim_console) $(myVASTsim) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay)
.PHONY: all clean $(VASTnet) $(VAST) $(VASTsim) $(common) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay) $(vaststat_replay_Qt) $(VASTsim_Qt) $(VASTreal) $(VASTreal_console) $(coding_host)

#all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(test_console) $(VASTsim_Qt)
#all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(test_console) $(demo_console) $(my_demo) $(demo_chatva_Qt)
# all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(my_demo) $(VASTsim_console) $(VASTsim_Qt) $(myVASTsim) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay)
#all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(VASTreal) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay) $(vaststat_replay_Qt) $(VASTsim_Qt) $(VASTreal_console)
all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(VASTreal) $(random_walkertalker) $(vaststat_replay) $(VASTreal_console) $(coding_host)

noqt: $(common) $(VASTnet) $(VAST) $(VASTsim) $(VASTreal) $(random_walkertalker) $(VASTreal_console) $(coding_host)

VASTreal_console: $(common) $(VASTnet) $(VAST) $(VASTsim) $(VASTreal) $(VASTreal_console)

# $(VAST) $(VASTsim) $(common) $(VASTnet) $(test_console) $(VASTsim_Qt) $(demo_console) $(my_demo) $(VASTsim_console) $(myVASTsim) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay):
$(common) $(VAST) $(VASTnet) $(VASTsim) $(VASTreal) $(random_walkertalker) $(demo_chatva_Qt) $(vaststat_replay) $(vaststat_replay_Qt) $(VASTsim_Qt) $(VASTreal_console) $(coding_host):
	$(MAKE) --directory=$@ $(TARGET)

$(VASTnet) : $(common)
$(VAST) : $(VASTnet) 
$(VASTsim): $(VAST)
$(VASTreal): $(VAST)
$(test_console) : $(VASTsim)
$(demo_console) : $(VASTsim)
$(VASTsim_console) : $(VASTsim)
$(my_demo) : $(VASTsim)
$(random_walkertalker) : $(VASTsim)
$(demo_chatva_Qt) : $(VASTsim)
$(vaststat_replay) : $(VASTsim)
$(vaststat_replay_Qt) : $(VASTsim)
$(VASTreal_console) : $(VASTreal)
$(coding_host) : $(VAST)

clean: 
	make TARGET=clean

noace:
	make TARGET=noace
