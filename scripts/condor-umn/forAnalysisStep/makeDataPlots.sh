#!/bin/bash

WrMasses=(2400 2400 2400)
NuMasses=( 240  800  480)
#WrMasses=(800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000 800 1600 2400 3200 4000 6000)
#NuMasses=( 80  160  240  320  400  600 160  320  480  640  800 1200 233  533  800 1067 1333 2000)
DataSetsDirectory="/data/whybee0b/user/aevans/"
echo 'CLEANING UP'
eval '/home/aevans/public_html/plots/data/sideband/archiver.sh'
#eval '/home/aevans/public_html/plots/data/search/archiver.sh'
date=$(date +%d_%b_%Y_%H-%M-%S-%Z)
echo 'MAKING NEW FOLDER'
eval 'mkdir /home/aevans/public_html/plots/data/sideband/${date}'
#eval 'mkdir /home/aevans/public_html/plots/data/search/${date}'
for h in ${!WrMasses[*]}
do
	## MuMuJJ
	#make python config file for MiniAOD step
    #/data/whybee0b/user/aevans/thesis/data/SingleMuon_WR_M-4000_LNu_M-1333.root
    #eval 'echo "python thePlottingScriptToEndAllPlottingScripts.py /data/whybee1b/user/aevans/thesis/data/SingleMuon_WR_M-${WrMasses[$h]}_LNu_M-${NuMasses[$h]}.root /home/aevans/public_html/plots/data/search/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds data"'
    #eval 'python thePlottingScriptToEndAllPlottingScripts.py /data/whybee0b/user/aevans/thesis/data/SingleMuon_WR_M-${WrMasses[$h]}_LNu_M-${NuMasses[$h]}.root /home/aevans/public_html/plots/data/search/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds data'
    eval 'echo "python thePlottingScriptToEndAllPlottingScripts.py /data/whybee1b/user/aevans/thesis/data/SingleElectron_WR_M-${WrMasses[$h]}_LNu_M-${NuMasses[$h]}.root /home/aevans/public_html/plots/data/sideband/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds data"'
    eval 'python thePlottingScriptToEndAllPlottingScripts.py /data/whybee0b/user/aevans/thesis/data/SingleElectron_WR_M-${WrMasses[$h]}_LNu_M-${NuMasses[$h]}.root /home/aevans/public_html/plots/data/sideband/${date}/ WR_M-${WrMasses[$h]}_ToLNu_M-${NuMasses[$h]}_Analysis_MuMuJJ backgrounds data'

done
eval 'find /home/aevans/public_html/plots/data/ -type d -exec ln /home/aevans/public_html/plots/data/index.php {}/index.php \;'
eval 'chmod -R g-w /home/aevans/public_html/plots/data'
