
VASTnet := VASTnet
VAST    := VAST 
VASTsim := VASTsim
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

# tells make that the following labels are make targets, not filenames
.PHONY: all clean $(VASTnet) $(VAST) $(VASTsim) $(common) $(test_console) $(VASTsim_Qt) $(demo_console) $(my_demo) $(VASTsim_console) $(myVASTsim) $(random_walkertalker)

#all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(test_console) $(VASTsim_Qt)
#all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(test_console) $(demo_console) $(my_demo)
all: $(common) $(VASTnet) $(VAST) $(VASTsim) $(my_demo) $(VASTsim_console) $(VASTsim_Qt) $(myVASTsim) $(random_walkertalker)


$(VAST) $(VASTsim) $(common) $(VASTnet) $(test_console) $(VASTsim_Qt) $(demo_console) $(my_demo) $(VASTsim_console) $(myVASTsim) $(random_walkertalker):
	$(MAKE) --directory=$@ $(TARGET)

$(VASTnet) : $(common)
$(VAST) : $(VASTnet) 
$(VASTsim): $(VAST)
$(test_console) : $(VASTsim)
$(demo_console) : $(VASTsim)
$(VASTsim_console) : $(VASTsim)
$(my_demo) : $(VASTsim)
$(random_walkertalker) : $(VASTsim)

clean: 
	make TARGET=clean

noace:
	make TARGET=noace
