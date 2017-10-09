#!/bin/bash

WrMasses=(2400 2400 2400)
NuMasses=( 240  800  480)
#WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
#NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)
DataSetsDirectory="/data/whybee0b/user/aevans/"
echo 'CLEANING UP'
eval '/home/aevans/public_html/plots/data/archiver.sh'
date=$(date +%d_%b_%Y_%H-%M-%S-%Z)
echo 'MAKING NEW FOLDER'
eval 'mkdir /home/aevans/public_html/plots/date/${date}'
for h in ${!WrMasses[*]}
do
	## MuMuJJ
	#make python config file for MiniAOD step
    eval 'echo "python thePlottingScriptToEndAllPlottingScripts.py /data/whybee0b/user/aevans/data_cfg_SingleMuon_WR_M-${WrMasses[$h]}_LNu_M-${NuMasses[$h]}/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root /home/aevans/public_html/plots/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds"'
    eval 'cp /data/whybee0b/user/aevans/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root /data/whybee0b/user/aevans/thesis/signals/'
    eval 'python thePlottingScriptToEndAllPlottingScripts.py /data/whybee0b/user/aevans/thesis/signals/WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ_000.root /home/aevans/public_html/plots/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds'

done
eval 'find /home/aevans/public_html/plots/ -type d -exec ln /home/aevans/public_html/plots/index.php {}/index.php \;'
eval 'chmod -R g-w /home/aevans/public_html/plots'
