#!/usr/bin/env bash

rsync -av results_extracted/* schoonwi@10.10.11.81:/Users/schoonwi/Development/VAST-0.4.6/bin/logs/logs_$(hostname)/result_extracted/
