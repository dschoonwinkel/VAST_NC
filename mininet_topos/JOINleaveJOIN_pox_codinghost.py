#!/usr/bin/python

from mininet.net import Mininet
from mininet.node import Controller, RemoteController, OVSController
from mininet.node import CPULimitedHost, Host, Node
from mininet.node import OVSKernelSwitch, UserSwitch
from mininet.node import IVSSwitch
from mininet.cli import CLI
from mininet.log import setLogLevel, info
from mininet.link import TCLink, Intf
from subprocess import call

import sys
import os, time

hosts = list()

TIMESTEP_DURATION = 0.1 #seconds

with open("../bin/VASTreal.ini", 'r') as config:
    data = config.readlines()
    TIMESTEP_DURATION = float(data[-1])/1000
    print(TIMESTEP_DURATION)
    SIMULATION_STEPS = (int)(data[data.index('#TIME_STEPS;    // number of steps\r\n')+1])
    print (SIMULATION_STEPS)


# SIMULATION_STEPS = 1000
TOTAL_SIMULATION_TIME = TIMESTEP_DURATION * SIMULATION_STEPS
# TOTAL_SIMULATION_TIME = 0
AUTO = True


def myNetwork():

    loss_perc = 10
#    BW = 1000
    Node_count = 10
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
    CLI(net)

    for i in range(1,Node_count+1):
        try:
            print("Setting up node %d" % i)
            hosts[i-1].cmd("route add 239.255.0.1 h%d-eth0" % i)
            if AUTO:
                # hosts[i-1].cmd("xterm -hold -fg black -bg green -geometry 80x60+%d+0 -e   \"./VASTreal_console %d 0 1037 10.0.0.1 \" &" % (200+i*40, i-1))    
                hosts[i-1].cmd("./VASTreal_console %d 0 1037 10.0.0.1 &> output_dump/node_10.0.0.%d.txt &" % (i-1, i))

                #DELAY before starting profiling = TIMESTEP_DURATION * (10 STEPS * NODE_COUNT + 100 STEPS) * 1000 ms
                DELAY = TIMESTEP_DURATION * (10 * Node_count + 100) * 1000
                # hosts[i-1].cmd("perf record --delay %d --freq 100 --call-graph dwarf -o ./perf/perf%d.data ./VASTreal_console %d 0 1037 10.0.0.1 &> output_dump/node%d.txt &" % (DELAY, i-1, i-1, i-1))
            time.sleep(1 + TIMESTEP_DURATION * 10)
        except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;

    if not AUTO:
        CLI(net)

    for i in range(1, int(TIMESTEP_DURATION * 1000)/10):
            print("Connection wait sleep 10 seconds, %d to go" % (int(TIMESTEP_DURATION * 1000) - i*10))
            try:
                time.sleep(10)
            except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;
        

    #print(net.links)
    
    # print("\n*** Applying loss to links")

    # for i in range(1, Node_count+1):
    #     links = hosts[i-1].connectionsTo(s1)
    #     srcLink = links[0][0]
    #     dstLink = links[0][1]
    #     dstLink.config(**{'loss' : loss_perc})



    for i in range(2,Node_count+1):
        try:
            print("Killing node %d" % i)
            if AUTO:
                hosts[i-1].cmd("kill $(ps a | grep \"VASTreal_console %d 0 1037 10.0.0.1\" | head -c 5)" % (i-1))

        except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;

    print("Sleeping between leave and join")
    try:
        time.sleep(1 + TIMESTEP_DURATION * 50)
    except KeyboardInterrupt:
        print("Sleep interrupted, exiting")

    for i in range(2,Node_count+1):
        try:
            print("Setting up node %d" % i)
            hosts[i-1].cmd("route add 239.255.0.1 h%d-eth0" % i)
            if AUTO:
                # hosts[i-1].cmd("xterm -hold -fg black -bg green -geometry 80x60+%d+0 -e   \"./VASTreal_console %d 0 1037 10.0.0.1 \" &" % (200+i*40, i-1))    
                hosts[i-1].cmd("echo \"\n\n\n*******************\nRestarting node\n*******************\n\" >> output_dump/node_10.0.0.%d.txt &" % (i))
                hosts[i-1].cmd("./VASTreal_console %d 0 1037 10.0.0.1 &>> output_dump/node_10.0.0.%d.txt &" % (i-1, i))

                #DELAY before starting profiling = TIMESTEP_DURATION * (10 STEPS * NODE_COUNT + 100 STEPS) * 1000 ms
                DELAY = TIMESTEP_DURATION * (10 * Node_count + 100) * 1000
                # hosts[i-1].cmd("perf record --delay %d --freq 100 --call-graph dwarf -o ./perf/perf%d.data ./VASTreal_console %d 0 1037 10.0.0.1 &> output_dump/node%d.txt &" % (DELAY, i-1, i-1, i-1))
            time.sleep(1 + TIMESTEP_DURATION * 10)
        except KeyboardInterrupt:
                print("Sleep interrupted, exiting")
                break;

    CLI(net)


    # if AUTO:
    #     for i in range(1, int(TOTAL_SIMULATION_TIME) / 10):
    #         print("Sleeping 10 seconds, %d to go" % (TOTAL_SIMULATION_TIME - i*10))
    #         try:
    #             time.sleep(10)
    #         except KeyboardInterrupt:
    #             print("Sleep interrupted, exiting")
    #             break;
        
    os.system("killall -s SIGINT VASTreal_console")
    os.system("killall -s SIGINT coding_host")

    # time.sleep(1)
    
    net.stop()

    os.system("killall -s SIGKILL VASTreal_console")
    os.system("killall xterm")
    os.system("killall -s SIGKILL coding_host")
    os.system("killall -s SIGKILL tcpdump")

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

