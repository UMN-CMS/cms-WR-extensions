import ExoAnalysis.cmsWRextensions.combineTools as combineTools
#import ExoAnalysis.cms-WR.cross_sections as xs 

import argparse

parser = argparse.ArgumentParser(description='Make datacards')
parser.add_argument('-d', '--dir', dest='basedir',
                    default="./ROOTfiles",
                    help='base dir for analysis tree files')
#parser.add_argument('-t', '--tag', dest='tag',
#                    default="",
#                    help='tag name for analysis tree files')
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

minitrees = combineTools.AnalysisResultsInterface(
 base=args.basedir #,
# tag =args.tag,
# makeplots=args.drawplots
)

nuisance_params = []
nuisance_params.append(("lumi",        "lnN"))
nuisance_params.append(("signal_unc",  "gmN"))
nuisance_params.append(("TT_unc",      "gmN"))
nuisance_params.append(("DY_unc",      "gmN"))
nuisance_params.append(("WJets_unc",   "gmN"))

sig_name = "WR_mumujj"
MWR = []
signal = []
bg = []
systematics_list = []

masses=[[2400,240],[2400,480],[2400,800]]
#for mass in ((2400,240),(2400,480),(2400,800)):
for i in range(len(masses)):
	print masses[i]
	datacard = "WRmumujj_MWR%04d_MNR%04d" % (masses[i][0],masses[i][1])
	print datacard
	datacard_file = args.outdir + "/" + datacard + ".txt"
	try:
	       	systematics = combineTools.Systematics(["signal", "TT", "DY", "WJets"], nuisance_params)
		signalNevents = minitrees.GetNEvents(masses[i][0], masses[i][1], "signal", systematics, scale = 1.0)
		TTNevents = minitrees.GetNEvents(masses[i][0], masses[i][1], "TT", systematics, scale = 1.0)
		DYNevents = minitrees.GetNEvents(masses[i][0], masses[i][1], "DY", systematics, scale = 1.0)
		WJetsNevents = minitrees.GetNEvents(masses[i][0], masses[i][1], "WJets", systematics, scale = 1.0)
		systematics.add("signal", "lumi", 1.025)
		systematics.add("DY", "lumi", 1.025)
	except (IOError,KeyError,TypeError) as e:
	       	print masses[i], "not found"
	       	print e
	signal_tuple = (sig_name, signalNevents)
	bg_names = ["TTBar", "DY", "WJets"]
	bg =[TTNevents,DYNevents,WJetsNevents]
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
