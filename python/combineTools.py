#!/usr/bin/env python

import ROOT as r
import re
import numpy as np
import subprocess
from collections import defaultdict
from random import gauss
#import ExoAnalysis.cmsWR.backgroundFits as bgfits
#import ExoAnalysis.cms-WR-extentions.cross_sections as xs
import math
import datetime
import os
import sys

datafolder = os.environ['LOCALRT'] + "/src/ExoAnalysis/cms-WR-extensions/data/"
configfolder = os.environ['LOCALRT'] + "/src/ExoAnalysis/cms-WR-extensions/configs/"
##
# @brief creates a datacard for combine for signal and background
#
# @param outfile String of filename
# @param bin_name Name for bin
# @param nObs number of observed
# @param signal_tuple (sig_name, signal_rate)
# @param background_tuples list of (name, rate) for backgrounds
#
# @return signal_rate, tuple of bg rates
def makeDataCardSingleBin(outfile, bin_name, nObs, signal_tuple, background_tuples, systematics):

	nBGs = len(background_tuples)
	ns = "  ".join([str(i) for i in range(nBGs+1)])
	sig_name = signal_tuple[0]
	signal_rate = "%.4g" % (signal_tuple[1])

	bg_names = ""
	bg_rates = ""
	name_lookup = {sig_name:0}
	for i, (bg_name, bg_rate) in enumerate(background_tuples):
		name_lookup[bg_name] = i+1
		bg_names += bg_name + "  "
		bg_rates   += "%.4g" % bg_rate + "  "
	names = sig_name + "  " + bg_names
	rates = signal_rate + '  ' + bg_rates
	try:
		os.mkdir(os.path.dirname(outfile))
	except os.error as e:
		if e.errno != 17:
			raise
	with open(outfile, "w") as out:
		out.write("imax 1  number of channels\n")
		out.write("jmax %d  number of backgrounds\n" % nBGs)
		if systematics:
			out.write("kmax %d  number of nuisance parameters\n" % systematics.n_nuisance)
		out.write("bin " + bin_name + "\n")
		out.write("observation %d\n" % round(nObs))
		out.write("------------\n")
		out.write("bin" + ("    " + bin_name)* (nBGs + 1) + "\n")
		out.write("process  " + names + "\n")
		out.write("process  " + ns + "\n")
		out.write("rate  " + rates + "\n")
		out.write("------------\n")
		if systematics:
			out.write(str(systematics))
	return signal_rate, tuple(bg_rates.split())

class AnalysisResultsInterface:
	def __init__(self,
			base="./ROOTfiles",
			):

		self.filefmt_dict = {"base":base}
		self.filefmt = "{base}/WR_M-{MWR}_ToLNu_M-{MNR}_Analysis_MuMuJJ_000.root" ###CHANGE

		self.results = {}

	def GetNEvents(self, MWR, MNR, process, systematics, scale = 1.0):
		key = process+"_" + str(MWR) + "_" + str(MNR)
		r=[[str(MWR),str(MNR)]]
		stat = 0
		alle = 0
		central = 0
		xs = 0
		statSB = 0
		centralSB = 0
		SF_relstat = 0
		if "DY" == process:
			r=[["50","100"],["100","250"],["250","400"],["400","650"],["650","Inf"]]
		elif "WJets" == process:
			r=[["100","250"],["250","400"],["400","600"],["600","Inf"]]
		for i in range(len(r)):
			if "DY" in process or "WJets" in process:
				key = process+"_" + str(MWR) + "_" + str(MNR)+"_" + str(r[i][0]) + "_" + str(r[i][1])
                        if key not in self.results:
				self.GetResult(key, process, r[i][0], r[i][1], MWR, MNR)
			if "DY" in process and key+"_ZSB" not in self.results:
				self.GetResult(key+"_ZSB", process, r[i][0], r[i][1], MWR, MNR)
			stat    += self.results[key]["stat"]
			central += self.results[key]["central"]
			alle    += self.results[key]["all"]
			xs      += self.results[key]["xs"]
			if "DY" in process:
				statSB    += self.results[key+"_ZSB"]["stat"]
				centralSB += self.results[key+"_ZSB"]["central"]
		if "DY" in process or "WJets" in process:
			key = process+"_" + str(MWR) + "_" + str(MNR)
			if key not in self.results:
				self.results[key] = {
					"stat": stat,
					"central": central,
					"all": alle,
					"xs": xs#{
				     }
				if "DY" in process:
					self.results[key+"_ZSB"] = {
						"stat": statSB,
						"central": centralSB,
						"all": alle,
						"xs": xs#{
						}
					self.GetResult("data_ZSB", "data_ZSB", MWR, MNR, MWR, MNR)
					#SF = self.results["data_ZSB"]["central"]/self.results[key+"_ZSB"]["central"] 
					#SF_relstat = pow(pow(self.results["data_ZSB"]["stat"]/self.results["data_ZSB"]["central"],2)+pow(self.results[key+"_ZSB"]["stat"]/self.results[key+"_ZSB"]["central"],2),0.5)					
					SF=self.results[key+"_ZSB"]["central"]/self.results[key+"_ZSB"]["central"] #TOCHANGE
					scale = scale*SF  #TOCHANGE
					SF_relstat = pow(pow(self.results[key+"_ZSB"]["stat"]/self.results[key+"_ZSB"]["central"],2)+pow(self.results[key+"_ZSB"]["stat"]/self.results[key+"_ZSB"]["central"],2),0.5) #TOCHANGE
					print "DY SF is " + str(SF) + " with relative statistical uncertainty of " + str(SF_relstat)


		rate=self.results[key]["central"]*scale	
		stat=self.results[key]["stat"]*scale
		stat=rate*pow(pow(stat/rate,2)+pow(SF_relstat,2),0.5)
		# changing the number of events we need to put in the datacards to get the correct statistical uncertainty and rate.
		Nevents=pow(rate/stat,2)
		alpha=rate/Nevents
		systematics.add(process, process+"_unc", (Nevents,alpha))
		return rate

	def GetResult(self, key, process, m0, m1, MWR, MNR):
       		xs=1.0
		allevents=10000.0
		if "signal" not in key and "data" not in key:
			xs=self.GetBGCrossSection(process, m0, m1)
			allevents=self.GetAllEvents(process, m0, m1)
		f = self.OpenFile(process, m0, m1, MWR, MNR)
		self.ProcessFile(key, f, xs, allevents)			

	def GetBGCrossSection(self, process, m0, m1):
		backgroundsList = "samples/backgrounds/backgroundStack/backgroundsList.txt"
		xsec="0"
		with open(backgroundsList) as f:
			lines = f.read().splitlines()
		for line in lines:
			if ("TT" in process and "TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8.txt" in line) or (process in line and m0 in line and m1 in line):
				xsec=line.split(':')[1].strip()
				xsec=xsec.split('+')[0].strip()
				break
		return float(xsec)

	def GetAllEvents(self, process, m0, m1):
		f = self.OpenSkimmingFile(process, m0, m1)
		h_all= f.Get("skim/allEvents/eventsWeight")
		allevents= h_all.GetBinContent(1)
		return allevents

 	def OpenSkimmingFile(self, process, m0, m1):
		base=self.filefmt_dict["base"]

 		if "TT" in process:
			filename=base+"/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_eventsWeight.root"
 		elif "DY" in process:
			filename=base+"/DYJetsToLL_Pt-"+m0+"To"+m1+"_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_eventsWeight.root"
 		elif "WJets" in process:
 			filename=base+"/WJetsToLNu_Pt-"+m0+"To"+m1+"_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_eventsWeight.root"
 		else:
 			return None

 		print "Opening File ", filename
 		f = r.TFile.Open(filename)
 		if not f:
			raise IOError(filename)
 		return f


	def ProcessFile(self, key, f, xs, allevents):
		if key in self.results: return

		self.currentkey = key
		directory="demo/eventsPassingExtensionRECO2016VETO/"
		if "signal" not in key:
			directory="analyze/eventsPassingExtensionRECO2016VETO/"
		if "_ZSB" in key:
			directory="analyze/eventsPassingExtensionRECO2016VETOZMASS/"
		h = f.Get(directory+"eventsWeight")
		central_value=h.GetBinContent(1)
		central_error=h.GetBinError(1)

		if "data" in key:
			xs=1.0
			allevents=35.9
		if "signal" in key:
			h_all= f.Get("demo/allEvents/eventsWeight")
			allevents= h_all.GetBinContent(1)
 		self.results[key] = {
# 				"syst": {
# 					"mean":syst_means.tolist(),
# 					"std":syst_stds.tolist(),
# 					"unweighted_mean": syst_unweighted_means.tolist(),
# 					"stat_err": stat_err.tolist(),
# 					},
 				"stat": central_error * xs / allevents,
 				"central": central_value * xs / allevents,
				"all": allevents,
				"xs": xs#{
# 					"weighted": central_value.tolist(),
# 					"unweighted": central_unweighted.tolist(),
# 					},
# 				"mean":[0]*len(self.masses)
				}

 	def OpenFile(self, process, m0, m1, MWR, MNR):
		base=self.filefmt_dict["base"]
       		if process == "signal":
			self.filefmt_dict["MWR"] = str(m0)
			self.filefmt_dict["MNR"] = str(m1)
 			filename=self.filefmt.format(**self.filefmt_dict)
 		elif "TT" in process:
			filename=base+"/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_WR_M-"+str(MWR)+"_LNu_M-"+str(MNR)+".root"
 		elif "DY" in process:
			filename=base+"/DYJetsToLL_Pt-"+m0+"To"+m1+"_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_WR_M-"+str(MWR)+"_LNu_M-"+str(MNR)+".root"
 		elif "WJets" in process:
 			filename=base+"/WJetsToLNu_Pt-"+m0+"To"+m1+"_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_WR_M-"+str(MWR)+"_LNu_M-"+str(MNR)+".root"
		elif "data_ZSB" in process:
			filename=base+"/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_WR_M-"+str(MWR)+"_LNu_M-"+str(MNR)+".root" #TOCHANGE
 		else:
 			return None

 		print "Opening File ", filename
 		f = r.TFile.Open(filename)
 		if not f:
			raise IOError(filename)
 		return f

		
class Systematics:
 	#for formatting. if a string format as string otherwise use fmt_spec
 	#class nuisance_value:
 	#	def __init__(self, value):
 	#		self.value = value
 	#	def __format__(self, spec):
 	#		if(isinstance(self.value, str)): return str(self.value)
 	#		else: return format(self.value, spec)

	def __init__(self, channel_names, nuisance_params):
		self.channel_names = channel_names
		self.rows = nuisance_params
		self.n_nuisance = len(self.rows)
 		self.values = {}
 	def add(self, channel, syst, value):
 		key = channel+syst
 		self.values[key] = value
 	def __str__(self):
 		maxlen = max([len(name) for name, s_type in self.rows])
 		floatsize = 10
 		prefix = "{name:{maxlen}} {type} {N:6}"
 		fmt = " {:{floatsize}.4e}"
 		s = ""
 		for name, s_type in self.rows:
 			line = prefix 
 			N = ""
 			for channel in self.channel_names:
 				key = channel + name
 				try:
 					N, v = self.values[key]
 					N = int(round(N))
 					line += fmt.format(v, floatsize=floatsize)
 				except KeyError as e:
 					line += " " * floatsize + "-"
 				except TypeError:
 					v = self.values[key]
 					line += fmt.format(v, floatsize=floatsize )
 			s += line.format(name=name, type=s_type, N=N, maxlen=maxlen) + "\n"
 		return s
 	def __len__(self):
 		return len(self.values)

		




# ##
# # @brief calls and parses command for `combine'
# #
# # @param command a list to pass to subprocess.check_output
# #
# # @return (mean, meanError), (onesig_minus, onesig_plus), (twosig_minus, twosig_plus)
# def runCombine(command, ID):
# 	name  = "combine" + ID
# 	out_file = open(name + ".out", "w+")
# 	err_file = open(name + ".err", "w")
# 	command = "unbuffer " + command
# 	command = command.split(' ')
# 	jobname = command[command.index('-n') + 1]
# 	method = command[command.index('-M') + 1]
# 	print method, "HybridNew" in method, jobname
# 	try:
# 		seed = command[command.index('-s') + 1]
# 	except ValueError:
# 		seed = "123456"
# 	try:
# 		mass = command[command.index('-m') + 1]
# 	except ValueError:
# 		mass = "120"
		
# 	try:
# 		if "HybridNew" in method:
# 			rs = []
# 			print "do different quantiles"
# 			for q in [.025, .16, 0.5, .84, .975]:
# 				#print q
# 				run_command = command + ["--expectedFromGrid=%f" % q]
# 				#print " ".join(run_command)
# 				subprocess.call(run_command, stdout=out_file, stderr=err_file)
# 				out_file.seek(0)
# 				output = out_file.read()
# 				p = re.compile(r'Limit: r < ([0-9.]*)')
#  				matches  = p.findall(output)
# 				rs.append(matches[-1])
# 			twosig_minus, onesig_minus, median, onesig_plus, twosig_plus = tuple(rs)
# 		else:
# 			#print " ".join(command)
# 			subprocess.call(command, stdout=out_file, stderr=err_file)
# 			out_file.seek(0)
# 			output = out_file.read()
# 			if not "--toys" in command and not "-t" in command:
# 				p = re.compile(r'Limit: r < ([0-9.]*)')
#  				matches  = p.findall(output)
#  				if not matches: raise RuntimeError
# 				return matches[-1]
			
			
# 			outfile = r.TFile.Open("higgsCombine" + jobname + "." + method + ".mH" + mass + "." + seed + ".root")
# 			limitTree = outfile.Get("limit")
# 			limits = []
# 			for toy in limitTree:
# 				if not np.isnan(toy.limit):
# 					limits.append(toy.limit)
# 			#limitTree.Draw("limit>>tmphist")
# 			#h = r.gDirectory.Get("tmphist")
# 			q = [2.5, 16, 50, 84, 97.5]
# 			twosig_minus, onesig_minus, median, onesig_plus, twosig_plus = np.percentile(np.array(limits), q)

# 		if not all([median, onesig_minus, onesig_plus, twosig_minus, twosig_plus]):
# 			print "combine parse failed"
# 			print median, onesig_minus, onesig_plus, twosig_minus, twosig_plus
# 			raise TypeError
# 		return median, (onesig_minus, onesig_plus), (twosig_minus, twosig_plus)
# 	except Exception as e:
# 		raise e
# 		return None

# mass_cut = {"ee":{},
# 		      "mumu":{}
# 		     }
# with open(configfolder + "mass_cuts.txt") as mc_file:
# 	for line in mc_file:
# 		if line[0] == "#": continue
# 		ch, mass, low, hi = line.split()
# 		mass_cut[ch.lower()][int(mass)] = (int(low), int(hi))

# import sys
# if __name__ == '__main__':
# 	ID = sys.argv[1]
# 	command = " ".join(sys.argv[2:])
# 	print ID, command
# 	result = runCombine(command, ID)
# 	print ("COMBINE", ID, result)
