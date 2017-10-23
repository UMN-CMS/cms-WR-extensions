import ExoAnalysis.cmsWRextensions.condorTools as condorTools
import os
import re
import subprocess
import sys

tag, toys = sys.argv[1:]

thisdir = os.getcwd()
#proddir = "/local/cms/user/phansen/limits/"
proddir = "/data/whybee0b/user/aevans/thesis/limits/"

datacardfolder = "/home/aevans/CMS/thesis/CMSSW_8_0_25/src/ExoAnalysis/cmsWRextensions/datacards/"
datacards = os.listdir(datacardfolder)
#WRmumujj_MWR2400_MNR0800.txt
pattern = re.compile("WR(.*)jj_MWR(.*)_MNR(.*).txt")

mode = "BayesianToyMC"
job = condorTools.Job(thisdir + "/scripts/batch_run", "_WRv05", prodSpace=proddir)
for datacard in datacards:
	m = pattern.match(datacard)
	if not m: continue
	channel, MWR, MNR = m.groups()
	if channel not in ["ee","mumu"]: continue
	if not MWR.isdigit(): continue

	print datacard
	datacard_file = datacardfolder + datacard

	jobname = channel + "_" + MWR + "_"
	systematics = True
#TODO: Make hybrid new work for observed
	if "Hybrid" in mode:
		jobid = jobname + "EXPECTED"
		command = "combine -M HybridNew --frequentist --testStat LHC -H ProfileLikelihood -S%d %s -n %s -T 5000 " %(systematics, datacard_file, datacard)
		prefix  = thisdir + "/python/combineTools.py " + jobid
		job.addJob( prefix + " " + command, jobid)
	elif "Bayes" in mode:
		jobid = jobname + "OBSERVED"
		command = "combine -M BayesianToyMC -H ProfileLikelihood -S%d %s -n %s " %(systematics, datacard_file, datacard)
		prefix  = thisdir + "/python/combineTools.py " + jobid
		job.addJob( prefix + " " + command, jobid)
		jobid = jobname + "EXPECTED"
		command = "combine -M BayesianToyMC -H ProfileLikelihood -S%d %s -n %s --toys %s" %(systematics, datacard_file, datacard, toys)
		prefix  = thisdir + "/python/combineTools.py " + jobid
#		job.addJob( prefix + " " + command, jobid)

job.submit(mode = "condor")
