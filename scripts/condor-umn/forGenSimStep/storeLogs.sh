#!/bin/bash

WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)
logsDir="/data/whybee0b/user/aevans/prodLogs/"

eval 'mkdir -p ${logsDir}'

for h in ${!WrMasses[*]}
do
    eval 'gzip -d /data/whybee0b/user/aevans/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ/log/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ_001.log.gz'
    eval 'cp /data/whybee0b/user/aevans/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ/log/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_GEN-SIM_MuMuJJ_001.log ${logsDir}'
done
