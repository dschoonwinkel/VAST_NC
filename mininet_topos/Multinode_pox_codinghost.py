#!/usr/bin/env python2

from mininet.net import Mininet
from mininet.node import Controller, RemoteController, OVSController
from mininet.node import CPULimitedHost, Host, Node
from mininet.node import OVSKernelSwitch, UserSwitch
from mininet.node import IVSSwitch
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.link import TCLink, Intf
from subprocess import call
from os.path import expanduser


import sys
import os, time

hosts = list()

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

TIMESTEP_DURATION = 0.1 #seconds

with open("%s/Development/VAST-0.4.6/bin/VASTreal.ini" % home_dir, 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])/1000
    print(TIMESTEP_DURATION)
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\n')+1])
    print (SIMULATION_STEPS)

with open("%s/Development/VAST-0.4.6/bin/Mininet.ini" % home_dir, 'r') as config:
    data = config.readlines()
    NODE_COUNT = int(data[data.index('#NODE_COUNT;    // Nodes started in simulation\n')+1])
    print("NODE_COUNT", NODE_COUNT)
    BW = (float)(data[data.index('#BW;            // Bandwidth limit [Mbps], 0 if inifinte\n')+1])
    print ("BW", BW)
    DELAY = (int)(data[data.index('#DELAY;         // Delay in MS\n')+1])
    print ("DELAY", DELAY)
    LOSS_PERC = (int)(data[data.index('#LOSS_PERC;     // Percentages of packets dropped on downstream link. Upstream link unaffected\n')+1])
    print ("LOSS_PERC", LOSS_PERC)


# SIMULATION_STEPS = 1000
TOTAL_SIMULATION_TIME = TIMESTEP_DURATION * SIMULATION_STEPS
# TOTAL_SIMULATION_TIME = 0
AUTO = True


def myNetwork():

    loss_perc = LOSS_PERC
    Node_count = NODE_COUNT
    delay = DELAY
    bw = BW
    run_codinghost = True

    if (len(sys.argv) > 1):
        print(sys.argv[1])
        loss_perc = float(sys.argv[1])


    net = Mininet( topo=None,
                   build=False,
                   ipBase='10.0.0.0/8',
                   link=TCLink,)

    info( '*** Adding controller\n' )
    c0=net.addController(name='c0',
                      controller=RemoteController,
                      ip='127.0.0.1',
                      protocol='tcp',
                      port=6633)

    info( '*** Add switches\n')
    s1 = net.addSwitch('s1', cls=OVSKernelSwitch)

    # Set up coding_host seperately
    coding_host = net.addHost('codinghost', cls=Host, ip='10.0.0.254', defaultRoute=None)
    net.addLink(coding_host, s1)

    info( '*** Add hosts\n')
    for i in range(1,Node_count+1):
        h = net.addHost('h%d' % i, cls=Host, ip='10.0.0.%d' %i, defaultRoute=None)
        net.addLink(h, s1)
        hosts.append(h)

    hosts.append(coding_host)


    info( '*** Starting network\n')
    net.build()
    info( '*** Starting controllers\n')
    for controller in net.controllers:
        controller.start()

    info( '*** Starting switches\n')
    net.get('s1').start([c0])

    info( '*** Post configure switches and hosts\n')

    coding_host.cmd("route add 239.255.0.1 codinghost-eth0")

    h1 = net.get('h1')
    h1.cmd("tcpdump -i h1-eth0 -w pcaps/h1.pcap &> pcaps/h1_pcapout.txt & ")

    if (run_codinghost):
        # coding_host.cmd("xterm -hold -fg black -bg green -geometry 80x10+200+600 -e \"./coding_host \" &")
        coding_host.cmd("./coding_host &> output_dump/codinghost.txt &")
        pass
    #CLI(net)

    for i in range(1,Node_count+1):
        try:
            print("Setting up node %d" % i)
            hosts[i-1].cmd("route add 239.255.0.1 h%d-eth0" % i)
            hosts[i-1].cmd("tshark -T fields -e frame.number -e frame.time -e frame.len -e ip.src -e ip.dst -e ip.proto -E separator=, -E quote=d port 1037 > pcaps/h%d_pcapout.csv &" % i)
            if AUTO:
                # hosts[i-1].cmd("xterm -hold -fg black -bg green -geometry 80x60+%d+0 -e   \"./VASTreal_console %d 0 1037 10.0.0.1 \" &" % (200+i*40, i-1))    
                hosts[i-1].cmd("ASAN_OPTIONS=alloc_dealloc_mismatch=0 ./VASTreal_console %d 0 1037 10.0.0.1 > output_dump/node_10.0.0.%d.txt 2> output_dump/node_10.0.0.%d_err.txt &" % (i-1, i, i))

                #PROFILING_DELAY before starting profiling = TIMESTEP_DURATION * (10 STEPS * NODE_COUNT + 100 STEPS) * 1000 ms
                PROFILING_DELAY = TIMESTEP_DURATION * (10 * Node_count + 100) * 1000
                # hosts[i-1].cmd("perf record --delay %d --freq 100 --call-graph dwarf -o ./perf/perf%d.data ./VASTreal_console %d 0 1037 10.0.0.1 &> output_dump/node%d.txt &" % (PROFILING_DELAY, i-1, i-1, i-1))
            time.sleep(1 + TIMESTEP_DURATION * 10)
        except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;

    # if not AUTO:
        # CLI(net)

    for i in range(0, int(TIMESTEP_DURATION * 1000)/10):
            print("Connection wait sleep 10 seconds, %d to go" % (int(TIMESTEP_DURATION * 1000) - i*10))
            try:
                time.sleep(10)
            except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;
        

    #print(net.links)
    
    print("\n*** Applying loss to links")

    for i in range(1, Node_count+1):
        links = hosts[i-1].connectionsTo(s1)
        srcLink = links[0][0]
        dstLink = links[0][1]
        prop_dict = dict()
        if loss_perc > 0:
            prop_dict['loss'] = loss_perc
        if delay > 0:
            prop_dict['delay'] = '%dms' % delay
        if bw > 0:
            prop_dict['bw'] = bw
        if len(prop_dict.keys()) > 0:
            dstLink.config(**prop_dict)


    #h1 = net.get('h1')
    #links = h1.connectionsTo(s1)

    #srcLink = links[0][0]
    #dstLink = links[0][1]

    #dstLink.config(**{ 'bw' : 1, 'loss' : loss_perc})
    # srcLink.config(**{ 'bw' : 1, 'loss' : 10})

    #print(srcLink)
    #print(dstLink)
    # hosts[0].cmd("iperf -s &")
    # hosts[1].cmd("iperf -s &")
    # hosts[2].cmd("iperf -s &")
    # hosts[37].cmd("xterm -hold -fg black -bg green -geometry 80x60+20+0 -e \"iperf -c 10.0.0.3 \" &")
    # hosts[38].cmd("xterm -hold -fg black -bg green -geometry 80x60+20+0 -e \"iperf -c 10.0.0.1 \" &")
    # hosts[39].cmd("xterm -hold -fg black -bg green -geometry 80x60+40+0 -e \"iperf -c 10.0.0.2 \" &")

    if AUTO:
        for i in range(1, int(TOTAL_SIMULATION_TIME) / 10):
            print("Sleeping 10 seconds, %d to go" % (TOTAL_SIMULATION_TIME - i*10))
            try:
                time.sleep(10)
            except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;
        
    os.system("killall -s SIGINT VASTreal_console")
    os.system("killall -s SIGINT coding_host")
    os.system("killall -s SIGINT tshark")

    time.sleep(1)
    # CLI(net)
    net.stop()

    os.system("killall -s SIGKILL VASTreal_console")
    os.system("killall xterm")
    os.system("killall -s SIGKILL coding_host")
    os.system("killall -s SIGKILL tcpdump")
    os.system("killall -s SIGKILL tshark")

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

