#!/bin/bash

echo $1

find . | grep $1 > /dev/null

if [[ $? > 0 ]]; then
	echo "file not found"
	exit
fi

file1=$(find . | grep $1 | grep output_dump | grep node_10.0.0.0)
echo $file1
cat $file1 | grep "net_udp::constructor: _self_addr:"
# cat $file1
