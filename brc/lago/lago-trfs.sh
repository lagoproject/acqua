#!/bin/bash
source ${LAGO_DAQ}/lago-configs
cd ${work}
datadir=${work}
if ($hasUSB); then
	datadir=${usb}
fi
rsync -aPv $datadir/*.bz2 ${remoteUser}@${remoteIP}:~/lago/

if (${eraseOldFiles}); then
	files=$(find ${work} -iname "*.bz2" -type f -mtime +30)
	rm ${files}
fi
