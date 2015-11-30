#!/bin/bash

source ${LAGO_DAQ}/lago-configs
cd ${work}
crontab ${LAGO_DAQ}/crontab.run
daq=$(ps aux | grep lago | grep SCREEN | awk '{print $2}')
if [ "X${daq}" == "X" ]; then 
	screen -d -m -a -S lago ${work}/lago-run.sh
else
	echo "DAQ is running"
fi
