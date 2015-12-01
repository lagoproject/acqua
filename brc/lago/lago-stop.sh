#!/bin/bash

source ${LAGO_DAQ}/lago-configs
cd ${work}
crontab ${LAGO_DAQ}/crontab.stop

daq=$(ps aux | grep "lago" | grep SCREEN | grep -v grep | awk '{print $2}')
kill ${daq}
daq=$(ps aux | grep "lago" | grep SCREEN | grep -v grep | awk '{print $2}')
if [ "X${daq}" != "X" ]; then 
  kill -9 ${daq}
fi
 
./lago -s t1 1000
./lago -s t2 1000
./lago -s t3 1000
./lago -s hv1 0
./lago -s hv2 0
./lago -s hv3 0

echo "Waiting 10 sec for PMT..."
sleep 10
echo "... done."

./lago -a
