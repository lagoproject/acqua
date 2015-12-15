#!/bin/bash
source ${LAGO_DAQ}/lago-configs
cd ${work}
datadir=${work}
if ($hasUSB); then
	datadir=${usb}
fi
rsync -aPv $datadir/*.bz2 ${remoteUser}@${remoteIP}:~/lago/
