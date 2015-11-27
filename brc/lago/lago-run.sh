#!/bin/bash

source configs

if [ -d ${usb}/lost+found/ ]; then
  echo "USB is mounted"
else
  mount -t ext4 /dev/sda1 ${usb}
fi

crontab crontab.run

cd ${work}
flash=$(lsusb -d 1443:0020)

if [ "X$flash" == "X" ]; then
  ./lago -x lago.xsvf
  sleep 10
  g=true
  while ($g); do 
    d0=$(date) 
    date -u -s "$(./lago -g  | head -5 | tail -1 | awk '{print $3}') $(./lago -g  | head -5 | tail -1 | awk '{print $6}')"
    d1=$(date)
    if [ "X$d0" == "X$d1" ]; then 
      g=false
    fi
  done
fi

./lago -s hv1 ${hv1}
./lago -s hv2 ${hv2}
./lago -s hv3 ${hv3}
./lago -s t1 ${t1}
./lago -s t2 ${t2}
./lago -s t3 ${t3}
./lago -a
echo "Waiting 10 seconds to start..."
sleep 10
echo "... done."

while(true); do
  ./lago -f ${name}
done
