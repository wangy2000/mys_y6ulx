#!/bin/bash


PWD=`pwd`
EXE=`echo $0 | sed s=$PWD==`
EXEPATH="$PWD"/"$EXE"


echo $PWD $EXE $EXEPATH
PARSEPATH=`echo $EXEPATH|grep -o '.*test.sh'`
echo $PARSEPATH

# /datadisk/github/test 
# ./test.sh 
# /datadisk/github/test/./test.sh


# PARSEPATH=

PARTITION_TEST=`cat /proc/partitions | grep '\<sd.\>\|\<sd..\>'`
echo $PARTITION_TEST
PARTITION_TEST=`cat /proc/partitions | grep '\<sd.\>\|\<mmcblk.\>' | grep -n ''`
echo $PARTITION_TEST