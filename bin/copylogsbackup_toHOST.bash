#!/usr/bin/env bash

if [ -z "$1" ]
then
	echo "Usage: copylogsbackup_to_HOST.bash <toHOST>"
	exit 1
fi

TOHOST=$1
echo $HOSTNAME

rsync -avrz --delete --progress ~/Development/VAST-0.4.6/bin/logs/logs_backup/ dschoonwinkel@$TOHOST:~/Development/VAST-0.4.6/bin/logs/logs_$HOSTNAME
