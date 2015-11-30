#!/bin/bash
detector1HV=0
detector2HV=0
detector3HV=0
detector1Trigger=1000
detector2Trigger=1000
detector3Trigger=1000

source ${LAGO_DAQ}/lago-configs
cd ${work}

#check for 1443:0020 is not needed anymore in the new DAQ version
#flash=$(lsusb -d 1443:0020)
#if [ "X$flash" == "X" ]; then
./lago -x lago_fpga_vhdl_ram_${fpgaGates}k.xsvf
sleep 10
#fi
# Loop to wait for GPS to start working and configue true date
gps=${hasGPS} 
# this is highly experimental and should not be used.
# if you are using a computer, please check the time is correctly setted 
# if you are using a raspberry pi, install NTP to get the correct time
# if you have no network, you should consider to buy a RTC module
# https://learn.adafruit.com/adding-a-real-time-clock-to-raspberry-pi/overview
if (false); then # if you want to test, change false for true and don't complain
	while (${gps}); do 
		d0=$(date)
		date -u -s "$(./lago -g | head -5 | tail -1 | awk '{print $3}') $(./lago -g | head -5 | tail -1 | awk '{print $6}')"
		d1=$(date)
		if [ "X${d0}" == "X${d1}" ]; then 
			gps=false
		fi
	done
fi

# setting daq
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
