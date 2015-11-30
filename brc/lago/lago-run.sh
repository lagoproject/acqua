#!/bin/bash

source ${LAGO_DAQ}/lago-configs

if [ -d ${usb}/lost+found/ ]; then
  echo "USB is mounted"
else
  mount -t ext4 /dev/sda1 ${usb}
fi

cd ${work}
#check for 1443:0020 is not needed anymore in the new DAQ version
#flash=$(lsusb -d 1443:0020)
#if [ "X$flash" == "X" ]; then
./lago -x lago_fpga_vhdl_ram_${fpgaGates}k.xsvf
sleep 10
g=true
# Loop to wait for GPS to start working and configue true date
while ($g); do 
  d0=$(date) 
  date -u -s "$(./lago -g  | head -5 | tail -1 | awk '{print $3}') $(./lago -g  | head -5 | tail -1 | awk '{print $6}')"
  d1=$(date)
  if [ "X$d0" == "X$d1" ]; then 
    g=false
  fi
done
#fi

./lago -s hv1 ${detector1HV}
./lago -s hv2 ${detector2HV}
./lago -s hv3 ${detector3HV}
./lago -s t1 ${detector1Trigger}
./lago -s t2 ${detector2Trigger}
./lago -s t3 ${detector3Trigger}
./lago -a
echo "Waiting 10 seconds to start..."
sleep 10
echo "... done."

while(true); do
  ./lago -f ${siteName}
done
