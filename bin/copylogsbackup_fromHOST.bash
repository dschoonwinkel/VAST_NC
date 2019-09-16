#!/usr/bin/env bash

if [ -z "$1" ]
then
	echo "Usage: copylogsbackup_fromHOST.bash <fromHOST>"
	exit 1
fi

FROMHOST=$1
echo $HOSTNAME

rsync -vrz --progress --dry-run dschoonwinkel@$FROMHOST:~/Development/VAST-0.4.6/logs/logs_$HOSTNAME ~/Development/VAST-0.4.6/bin/logs/logs_backup/