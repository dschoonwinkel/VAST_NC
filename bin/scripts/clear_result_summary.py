#!/usr/bin/env python3

from os.path import expanduser

home_dir = expanduser("~")
print("Home Dir: ", home_dir)

# Reset results_summary.py
with open('%s/Development/VAST-0.4.6/bin/results_summary/results_summary.txt' % home_dir, 'w') as csvfile:
    csvfile.write('timestamp, NET_MODEL, NODECOUNT, BW, DELAY, LOSS, STEPS, PLATFORM, ACTIVE_NODES, ACTIVE_MATCHERS, TOPO_CONS, DRIFTDISTANCE, SEND_BW, RECV_BW, RAWMC_RECV, USEDMC_RECV, NICSEND_BYTES, NICRECV_BYTES, LATENCY\n')