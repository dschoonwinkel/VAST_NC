#!python3
import sys
from os.path import expanduser

# print("Usage: ./generate_label - will print out label derived from VASTreal.ini and Mininet.ini")

home_dir = expanduser("~")
# print("Home Dir: ", home_dir)

NET_MODEL_STRINGS = ['net_emu', 'net_ace', 'net_udp', 'net_udpNC']

with open("%s/Development/VAST-0.4.6/bin/VASTreal.ini" % home_dir, 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])
    # print("TIMESTEP_DURATION", TIMESTEP_DURATION, "[ms]")
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\n')+1])
    # print ("SIMULATION_STEPS", SIMULATION_STEPS)
    NET_MODEL = (int)(data[data.index('#NET_MODEL;     // 1: Net emulated 2: Net emulated with bandwidth limitation\n')+1])
    # print ("NET_MODEL", NET_MODEL)


with open("%s/Development/VAST-0.4.6/bin/Mininet.ini" % home_dir, 'r') as config:
    data = config.readlines()
    NODE_COUNT = int(data[data.index('#NODE_COUNT;    // Nodes started in simulation\n')+1])
    # print("NODE_COUNT", NODE_COUNT)
    BW = (int)(data[data.index('#BW;            // Bandwidth limit [Mbps], 0 if inifinte\n')+1])
    # print ("BW", BW)
    DELAY = (int)(data[data.index('#DELAY;         // Delay in MS\n')+1])
    # print ("DELAY", DELAY)
    LOSS_PERC = (int)(data[data.index('#LOSS_PERC;     // Percentages of packets dropped on downstream link. Upstream link unaffected\n')+1])
    # print ("LOSS_PERC", LOSS_PERC)

print(NET_MODEL_STRINGS[NET_MODEL-1] + "_NODES" + str(NODE_COUNT) + "_BW" + str(BW) + "_DELAY" + str(DELAY) + "_LOSS_PERC" + str(LOSS_PERC))