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

TOTAL_SIMULATION_TIME = 150


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
    if (run_codinghost):
        coding_host.cmd("xterm -hold -fg black -bg green -geometry 80x10+200+600 -e \"./coding_host \" &")
    CLI(net)

    for i in range(2,Node_count+1):
        hosts[i-1].cmd("route add 239.255.0.1 h%d-eth0" % i)
        # hosts[i-1].cmd("xterm -hold -fg black -bg green -geometry 80x60+%d+0 -e   \"./VASTreal_console %d 0 1037 10.0.0.1 \" &" % (200+i*40, i-1))    
        hosts[i-1].cmd("./VASTreal_console %d 0 1037 10.0.0.1 &> output_dump/node%d.txt &" % (i-1, i-1))
        time.sleep(1)

    #print(net.links)
    
    print("\n*** Applying loss to links")

    for i in range(1, Node_count+1):
        links = hosts[i-1].connectionsTo(s1)
        srcLink = links[0][0]
        dstLink = links[0][1]
        dstLink.config(**{'loss' : loss_perc})


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

    for i in range(1, TOTAL_SIMULATION_TIME / 10):
        print("Sleeping 10 seconds, %d to go" % (TOTAL_SIMULATION_TIME - i*10))
        time.sleep(10)
    
    os.system("killall -STOP VASTreal_console")

    # time.sleep(1)
    CLI(net)
    net.stop()

    os.system("killall -s SIGKILL VASTreal_console")
    os.system("killall xterm")
    os.system("killall -s SIGKILL coding_host")

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

