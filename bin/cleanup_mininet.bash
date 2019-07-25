#!/usr/bin/env bash

sudo mn -c

killall -s SIGINT VASTreal_console
killall -s SIGKILL VASTreal_console
killall xterm
killall -s SIGKILL coding_host