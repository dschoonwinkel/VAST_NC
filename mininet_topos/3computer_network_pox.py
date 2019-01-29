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

import os, time

def myNetwork():

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
    h1 = net.addHost('h1', cls=Host, ip='10.0.0.1', defaultRoute=None)
    h2 = net.addHost('h2', cls=Host, ip='10.0.0.2', defaultRoute=None)
    h3 = net.addHost('h3', cls=Host, ip='10.0.0.3', defaultRoute=None)
    h4 = net.addHost('h4', cls=Host, ip='10.0.0.4', defaultRoute=None)
    h5 = net.addHost('h5', cls=Host, ip='10.0.0.5', defaultRoute=None)
    h6 = net.addHost('h6', cls=Host, ip='10.0.0.6', defaultRoute=None)

    info( '*** Add links\n')
    # net.addLink(h1, s1, bw=10, delay='50ms', loss=0)
    net.addLink(h1, s1, bw=1, loss=4)
    net.addLink(s1, h2, bw=1, loss=4)
    net.addLink(s1, h3, bw=1, loss=4)
    net.addLink(s1, h4, bw=1, loss=4)
    net.addLink(s1, h5, bw=1, loss=4)
    net.addLink(s1, h6, bw=1, loss=4)

    info( '*** Starting network\n')
    net.build()
    info( '*** Starting controllers\n')
    for controller in net.controllers:
        controller.start()

    info( '*** Starting switches\n')
    net.get('s1').start([c0])

    info( '*** Post configure switches and hosts\n')

    h1.cmd("xterm -fg black -bg green -geometry 80x60+300+0 -e   \"./VASTreal_console 0 0 1037 10.0.0.1 \" &")
    time.sleep(2)
    h2.cmd("xterm -fg black -bg yellow -geometry 80x60+800+0 -e \"./VASTreal_console 1 0 1037 10.0.0.1 \" &")
    time.sleep(2)
    h3.cmd("xterm -fg black -bg cyan -geometry 80x12+1300+0 -e \"./VASTreal_console 2 0 1037 10.0.0.1 \" &")
    time.sleep(2)
    h4.cmd("xterm -fg black -bg cyan -geometry 80x12+1300+300 -e \"./VASTreal_console 3 0 1037 10.0.0.1 \" &")
    time.sleep(2)
    h5.cmd("xterm -fg black -bg cyan -geometry 80x12+1300+500 -e \"./VASTreal_console 4 0 1037 10.0.0.1 \" &")
    time.sleep(2)
    h6.cmd("xterm -fg black -bg cyan -geometry 80x12+1300+700 -e \"./VASTreal_console 5 0 1037 10.0.0.1 \"")

    # time.sleep(5)
    # os.system("killall -STOP VASTreal_console")

    time.sleep(1)
    # CLI(net)
    net.stop()

    os.system("killall VASTreal_console")
    os.system("killall xterm")

if __name__ == '__main__':
    setLogLevel( 'info' )
    myNetwork()

