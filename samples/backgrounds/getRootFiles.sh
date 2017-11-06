#!/bin/bash
filename='tSingle_datasets.txt'
rootFiles=`cat $filename`
for rootFile in $rootFiles
do
  echo $rootFile
  eval 'IFS='/' read -ra sections <<< $rootFile'
  if [ -f $PWD/${sections[1]}.txt ]
  then
    eval 'das_client.py --query="file dataset=$rootFile" --limit=1000 >> $PWD/${sections[1]}.txt'
  else
    eval 'das_client.py --query="file dataset=$rootFile" --limit=1000 > $PWD/${sections[1]}.txt'
  fi
done
