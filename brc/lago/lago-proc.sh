#!/bin/bash
source ${LAGO_DAQ}/lago-configs
cd ${work}
# we should check if we will use usb, and it is mounted

# # let's try with automount
# if (${hasUSB}); then
# 	if [ -d ${usb}/lost+found/ ]; then
# 		echo "USB is mounted"
# 	else
# 		mount -t ext4 /dev/sda1 ${usb}
# 	fi
# fi

files=$(find ${work} -iname "*.dat" -type f -mmin +30)
for i in ${files}; do
	echo $i
	# You can add here as many as process you need to do. 
	# First level process from ANNA
	if ($analyzeRaw); then
		if [ -e raw ]; then
			u=$(basename $i)
			./raw -c -t -n $u
			echo
			bzip2 -v ${work}/lp_*
		fi
	fi
	bzip2 -1v $i
done

if ($hasUSB); then
	mv -v ${work}/*.bz2 ${usb}/
	mv -v ${work}/*.mtd ${usb}/
fi
