import os
print "HI!"
import re

#upon execution, pipe the output of this script into a file named crosssections.txt
#move this txt file into the data/ directory

print "#M_WR M_NU XS(pb) XS_ERR(pb)"
#xspattern = re.compile("Before Filtrer: total cross section = (.*) \+- (.*) pb")
#xspattern = re.compile("f fbar' -> W_R^+-                             (.*) |        (.*)       (.*)       (.*) |   (.*)  (.*)")

# logdirs = "/dir/where/lsfJobLogs/are/stored/ExoAnalysis/cmsWR/scripts/genAndFullOfflineAnalysisJobs/batchSubmFilesAndLogDirs/"
logdirs = "/data/whybee0b/user/aevans/prodLogs/"
os.listdir(logdirs)
status = 0

for logfile in os.listdir(logdirs):
   # print "OPENING: "+logdirs+logfile
    lines = []
    with open(logdirs+logfile) as log:
        for line in log:
            if "W_R^+-" in line:
                lines.append(line)
            split = line.split()
            if not split: continue
            if split[0] == "9900012":
                mnu = int(float(split[5]))
                status += 1
            elif split[0] == "9900024":
                mwr = int(float(split[6]))
                status += 2
        if len(lines) < 1 : continue
        xsecLine = lines[-1]
        xsecLineSegments = xsecLine.split("|")
        #print xsecLineSegments
        if len(xsecLineSegments) < 2 : continue
        xsecAndError=xsecLineSegments[-2].split()
        xsec = xsecAndError[0]
        xsecError = xsecAndError[1]
        print "M_WR "+str(mwr)+" M_NU "+str(mnu)+" xsec "+str(xsec)+" xsec_error "+str(xsecError)
                
        #        split = line.split()
        #        if not split: continue
        #        if split[0] == "9900012":
        #            mnu = int(float(split[5]))
        #            status += 1
        #        elif split[0] == "9900024":
        #            mwr = int(float(split[6]))
        #            status += 2
        #        m = xspattern.match(line)
        #        if m:
        #            xs = m.group(1)
        #            xserr = m.group(2)
        #            status += 4

        #        if status == 1 + 2 + 4:

        #            print mwr, mnu, xs, xserr
        #            break
        #if status != 1 + 2 + 4:
        #    print "Issues here"
        #    print "MWR: "+str(mwr)
        #    print "MNU: "+str(mnu)
        #    print "Cross-section: "+str(xs)
        #    print "Cross-section error: "+str(xserr)

