#!/bin/bash
source ${LAGO_DAQ}/lago-configs
cd ${work}
remoteDir=/dspace/sites/${remoteUser}/
datadir=${work}
if ($hasUSB); then
	datadir=${usb}
fi
rsync -aPv $datadir/*.bz2 ${remoteUser}@${remoteIP}:${remoteDir}
rsync -aPv $datadir/*.mtd ${remoteUser}@${remoteIP}:${remoteDir}

if (${eraseOldFiles}); then
	files=$(find ${work} -iname "*.bz2" -type f -mtime +30)
	rm ${files}
fi
if (${eraseOldFiles}); then
	files=$(find ${work} -iname "*.mtd" -type f -mtime +30)
	rm ${files}
fi
