import ExoAnalysis.cmsWRextensions.combineTools as combineTools
#import ExoAnalysis.cms-WR.cross_sections as xs 

import argparse

parser = argparse.ArgumentParser(description='Make datacards')
parser.add_argument('-d', '--dir', dest='basedir',
                    default="./",
                    help='base dir for analysis tree files')
parser.add_argument('-t', '--tag', dest='tag',
                    default="",
                    help='tag name for analysis tree files')
parser.add_argument('-o', '--outdir', dest='outdir',
                    default="datacards/",
                    help='where to store the datacards')
parser.add_argument('--no-syst', dest='nosyst', action='store_true',
                    help='do not write systematics to datacards')
parser.add_argument('--draw-plots', dest='drawplots', action='store_true',
                    help='draw plots')
#parser.add_argument('-s', dest='scale', action='store_true',
#                    help='if present, rescale signal to .001 fb XS')

args = parser.parse_args()

#minitrees = combineTools.AnalysisResultsInterface(
# base=args.basedir,
# tag =args.tag,
# makeplots=args.drawplots
#)

nuisance_params = []
nuisance_params.append(("lumi",        "lnN"))
nuisance_params.append(("signal_unc",  "gmN"))
nuisance_params.append(("TT_unc",      "gmN"))
nuisance_params.append(("DY_unc",       "gmN"))
nuisance_params.append(("Wjets_unc",   "gmN"))

sig_name = "WR_mumujj"
MWR = []
signal = []
bg = []
systematics_list = []

for mass in (2000,4000):
	print mass
	datacard = "WRmumujj_MASS%04d" % (mass)
	datacard_file = args.outdir + "/" + datacard + ".txt"
	try:
	       	systematics = combineTools.Systematics(["signal", "TT", "DY", "Wjets"], nuisance_params)
		systematics.add("signal", "lumi", 1.025)
		systematics.add("signal", "signal_unc", (10,1))
		systematics.add("TT", "TT_unc", (100,1))
		systematics.add("DY", "DY_unc", (200,1))
		systematics.add("Wjets", "Wjets_unc", (0,1))
	except (IOError,KeyError,TypeError) as e:
	       	print mass, "not found"
	       	print e
	signal_tuple = (sig_name, 10)
	bg_names = ["TTBar", "DY", "Wjets"]
	bg =[100,200,0]
	bg_tuples = zip(bg_names, bg)
	nBG=sum(bg)
	sig, bgs = combineTools.makeDataCardSingleBin(datacard_file, "1", nBG,
				signal_tuple, bg_tuples, systematics)



#		try:
#			systematics = combineTools.Systematics(["signal", "TT", "DYAMCPT", "Other"], nuisance_params)
#			if args.scale:
#				scale =  .001/xs.WR_jj[channel][mass]
#			else:
#				scale = 1.0
#			signalNevents = minitrees.getNEvents(mass, channel, "signal", systematics, scale = scale)
#			TTBar = minitrees.getNEvents(mass, channel, "TT", systematics)
#			DY = minitrees.getNEvents(mass, channel, "DYAMCPT", systematics)
#			Other = minitrees.getNEvents(mass, channel, "Other", systematics)
#                       print 'tt',TTBar
#			MWR.append(mass)
#			signal.append(signalNevents)
#			bg.append([TTBar, DY, Other])
#
#			if args.nosyst: systematics = None
#			systematics_list.append(systematics)
#		except (IOError,KeyError,TypeError) as e:
#			print mass, "not found"
#			print e
#
#	bg_names = ["TTBar", "DY", "Other"]
#
#	for i in range(len(MWR)):
#		print channel, MWR[i], signal[i]/sum(bg[i])
#		signal_tuple = (sig_name, signal[i])
#		bg_tuples = zip(bg_names, bg[i])
#		nBG = sum(bg[i])
#
#		datacard = "WR%sjj_MASS%04d" % (channel, MWR[i])
#		datacard_file = args.outdir + "/" + datacard + ".txt"
#		sig, bgs = combineTools.makeDataCardSingleBin(datacard_file, channel + "jj", nBG,
#				signal_tuple, bg_tuples, systematics=systematics_list[i])
