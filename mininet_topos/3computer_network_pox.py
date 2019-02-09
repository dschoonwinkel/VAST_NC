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

def myNetwork():

    loss_perc = 0
    BW = 1000
    Node_count = 40

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

    info( '*** Add hosts\n')
    for i in range(1,Node_count+1):
        h = net.addHost('h%d' % i, cls=Host, ip='10.0.0.%d' %i, defaultRoute=None)
        net.addLink(h, s1, bw=BW, loss=loss_perc)
        hosts.append(h)

    info( '*** Starting network\n')
    net.build()
    info( '*** Starting controllers\n')
    for controller in net.controllers:
        controller.start()

    info( '*** Starting switches\n')
    net.get('s1').start([c0])

    info( '*** Post configure switches and hosts\n')

    # for i in range(1,Node_count+1):
    #     # hosts[i-1].cmd("xterm -fg black -bg green -geometry 80x60+%d+0 -e   \"./VASTreal_console %d 0 1037 10.0.0.1 \" &" % (200+i*40, i-1))    
    #     hosts[i-1].cmd("./VASTreal_console %d 0 1037 10.0.0.1 &" % (i-1))
    #     time.sleep(2)

    hosts[0].cmd("iperf -s &")
    hosts[1].cmd("iperf -s &")
    hosts[2].cmd("iperf -s &")
    hosts[37].cmd("xterm -hold -fg black -bg green -geometry 80x60+20+0 -e \"iperf -c 10.0.0.3 \" &")
    hosts[38].cmd("xterm -hold -fg black -bg green -geometry 80x60+20+0 -e \"iperf -c 10.0.0.1 \" &")
    hosts[39].cmd("xterm -hold -fg black -bg green -geometry 80x60+40+0 -e \"iperf -c 10.0.0.2 \" &")

    # time.sleep(5)
    # os.system("killall -STOP VASTreal_console")

    # time.sleep(1)
    CLI(net)
    net.stop()

    os.system("killall VASTreal_console")
    os.system("killall xterm")

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

