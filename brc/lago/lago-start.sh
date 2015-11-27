#!/bin/bash

source /root/lago/configs
cd ${work}
daq=$(ps aux | grep lago | grep SCREEN | awk '{print $2}')
if [ "X${daq}" == "X" ]; then 
  screen -d -m -a -S lago ${work}/lago-run.sh
fi
