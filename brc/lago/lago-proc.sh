#!/bin/bash
source ${LAGO_DAQ}/lago-configs
cd ${work}
files=$(find ${work} -iname "*.dat" -type f -mmin +30)
for i in ${files}; do
  echo $i
  u=$(basename $i)
  ./process -c -t -n $u
  echo
  bzip2 -1v $i
  bzip2 -1v ${work}/lp_v0r1_*
  mv -v ${work}/*.bz2 ${usb}/
done
