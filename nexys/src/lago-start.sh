#!/bin/bash

source ${LAGO_DAQ}/lago-configs
cd ${work}
daq=$(ps aux | grep "lago-" | grep SCREEN | grep -v grep | awk '{print $2}')
if [ "X${daq}" == "X" ]; then
	echo "Starting DAQ"
	screen -d -m -a -S lago ${work}/lago-run.sh
	sleep 10
	daq=$(ps aux | grep "lago-" | grep SCREEN | grep -v grep | awk '{print $2}')
	echo "DAQ is running on process $daq"
else
	echo "DAQ is running on process $daq"
fi
crontab ${LAGO_DAQ}/crontab.run
