#!/bin/bash
filename='TTbar_powheg_datasets.txt'
rootFiles=`cat $filename`
for rootFile in $rootFiles
do
  echo $rootFile
  eval 'IFS='/' read -ra sections <<< $rootFile'
  if [ -f $PWD/${sections[1]}.txt ]
  then
    eval 'dasgoclient --query="file dataset=$rootFile"  >> $PWD/${sections[1]}.txt'
  else
    eval 'dasgoclient --query="file dataset=$rootFile"  > $PWD/${sections[1]}.txt'
  fi
done
