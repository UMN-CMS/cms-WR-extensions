import os
import re

#upon execution, pipe the output of this script into a file named crosssections.txt
#move this txt file into the data/ directory

print "#M_WR M_NU XS(pb) XS_ERR(pb)"
xspattern = re.compile("Before Filtrer: total cross section = (.*) \+- (.*) pb")

# logdirs = "/dir/where/lsfJobLogs/are/stored/ExoAnalysis/cmsWR/scripts/genAndFullOfflineAnalysisJobs/batchSubmFilesAndLogDirs/"
logdirs = "/data/whybee0b/user/aevans/WR_M-4000_ToLNu_M-400_GEN-SIM_MuMuJJ/log/"

status = 0
with open("/data/whybee0b/user/aevans/WR_M-3200_ToLNu_M-320_GEN-SIM_MuMuJJ/log/WR_M-3200_ToLNu_M-320_GEN-SIM_MuMuJJ_001.log") as log:
    for line in log:
        print "hi!"
        split = line.split()
        if not split: continue
        if split[0] == "9900012":
            mnu = int(float(split[5]))
            status += 1
        elif split[0] == "9900024":
            mwr = int(float(split[6]))
            status += 2
        m = xspattern.match(line)
        if m:
            xs = m.group(1)
            xserr = m.group(2)
            status += 4

        if status == 1 + 2 + 4:

            print mwr, mnu, xs, xserr
            break
    if status != 1 + 2 + 4:
        print "Issues here"
        print "MWR: "+str(mwr)
        print "MNU: "+str(mnu)
        print "Cross-section: "+str(xs)
        print "Cross-section error: "+str(xserr)

