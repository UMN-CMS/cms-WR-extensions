import ROOT as r
import sys
import datetime
import subprocess
import os
import copy
import sets
import collections
import math
import tdrstyle


def SignalRegionWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, Blind, weights):

    histoDict = {}

    for sample in sampleNames:
	print "sample: ", sample
	tfile = r.TFile.Open(samplesLocation+sample+'.root')
	bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
	systs = ['JEC','JER','PU','MuHPt','MuL']

	if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':
	    
	    histoDict['TT'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
	    histoDict['TT'].SetDirectory(0)
	    histoDict['TT'].SetName('TT')
	    histoDict['TT'].Scale(weights[sample])

	    for syst in systs:
		histoDict['TT_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		histoDict['TT_%sUp'%(syst)].SetDirectory(0)
		histoDict['TT_%sUp'%(syst)].SetName('TT_%sUp'%(syst))
		histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
        	histoDict['TT_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		histoDict['TT_%sDown'%(syst)].SetDirectory(0)
        	histoDict['TT_%sDown'%(syst)].SetName('TT_%sDown'%(syst))
		histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

	elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

	    if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
		histoDict['DiBoson'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['DiBoson'].SetDirectory(0)
		histoDict['DiBoson'].SetName('DiBoson')
            	histoDict['DiBoson'].Scale(weights[sample])
		for syst in systs:
		    histoDict['DiBoson_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
	            histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
	            histoDict['DiBoson_%sUp'%(syst)].SetName('DiBoson_%sUp'%(syst))
	            histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].SetName('DiBoson_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

	    else:
		histoDict['DiBoson'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'),weights[sample])
		for syst in systs:
		    histoDict['DiBoson_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)),weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

	elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
	    if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
		histoDict['ST'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].SetName('ST')
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['ST_%sUp'%(syst)].SetName('ST_%sUp'%(syst))
                    histoDict['ST_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].SetName('ST_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['ST'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'),weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)),weights[sample])
                    histoDict['ST_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['WJets'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].SetName('WJets')
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].SetName('WJets_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['WJets_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].SetName('WJets_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['WJets'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'),weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)),weights[sample])
                    histoDict['WJets_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['DY'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].SetName('DY')
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].SetName('DY_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DY_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].SetName('DY_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['DY'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'),weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)),weights[sample])
                    histoDict['DY_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
                histoDict['QCD'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].SetName('QCD')
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
                    histoDict['QCD_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].SetName('QCD_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['QCD_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].SetName('QCD_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['QCD'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'),weights[sample])
                for syst in systs:
                    histoDict['QCD_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)),weights[sample])
                    histoDict['QCD_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

	elif 'WR' in sample:
	    wrMass = sample.split('_')[1][2:]
	    nuMass = sample.split('_')[3][2:]
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetName('WR_%s_NR_%s'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

        elif 'SingleMuon' in sample:
            if sample == 'SingleMuon--Run2016B-03Feb2017_ver2-v2':
                histoDict['data_obs'] = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
                histoDict['data_obs'].SetDirectory(0)
                histoDict['data_obs'].SetName('data_obs')
            else:
                histoDict['data_obs'].Add(tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass'))

	tfile.Close()


    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_SignalRegion.root'    
    outputWorkspace = r.TFile(outputFile, "RECREATE")

    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
    MuMuJJ.cd()

    histoDict['TT'].Write()
    histoDict['DiBoson'].Write()
    histoDict['ST'].Write()
    histoDict['WJets'].Write()
    histoDict['DY'].Write()
    histoDict['QCD'].Write()
    if Blind == "False":
	print "Including SR data"
	histoDict['data_obs'].Write()
    for syst in systs:
	histoDict['TT_%sUp'%(syst)].Write()
        histoDict['TT_%sDown'%(syst)].Write()
	histoDict['DiBoson_%sUp'%(syst)].Write()
        histoDict['DiBoson_%sDown'%(syst)].Write()
        histoDict['ST_%sUp'%(syst)].Write()
        histoDict['ST_%sDown'%(syst)].Write()
        histoDict['WJets_%sUp'%(syst)].Write()
        histoDict['WJets_%sDown'%(syst)].Write()	
        histoDict['DY_%sUp'%(syst)].Write()
        histoDict['DY_%sDown'%(syst)].Write()
        histoDict['QCD_%sUp'%(syst)].Write()
        histoDict['QCD_%sDown'%(syst)].Write()

    signalMasses = {(5000,1000),(5000,500),(4500,900),(4500,450),(4000,800),(4000,400),(3500,700),(3500,350),(3000,600),(3000,300),(2500,500),(2500,250),(2000,400),(2000,200),(1500,300),(1500,150),(1000,200),(1000,100)}
    for WRmass,NRmass in signalMasses:
    	histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
	for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()

    outputWorkspace.Write()
    outputWorkspace.Close()

def ZPeakWorksapce(sampleNames,samplesLocation,workspaceOutputDirectory, weights):

    histoDict = {}

    for sample in sampleNames:
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
        systs = ['JEC','JER','PU','MuHPt','MuL']

        if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':

            histoDict['TT'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
            histoDict['TT'].SetDirectory(0)
            histoDict['TT'].SetName('TT')
            histoDict['TT'].Scale(weights[sample])

            for syst in systs:
                histoDict['TT_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                histoDict['TT_%sUp'%(syst)].SetDirectory(0)
                histoDict['TT_%sUp'%(syst)].SetName('TT_%sUp'%(syst))
                histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
                histoDict['TT_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)].SetDirectory(0)
                histoDict['TT_%sDown'%(syst)].SetName('TT_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

        elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

            if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
                histoDict['DiBoson'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['DiBoson'].SetDirectory(0)
                histoDict['DiBoson'].SetName('DiBoson')
                histoDict['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    histoDict['DiBoson_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sUp'%(syst)].SetName('DiBoson_%sUp'%(syst))
                    histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].SetName('DiBoson_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
                histoDict['DiBoson'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'),weights[sample])
                for syst in systs:
                    histoDict['DiBoson_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)),weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)),weights[sample])

        elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
            if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
                histoDict['ST'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].SetName('ST')
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].SetName('ST_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['ST_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].SetName('ST_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['ST'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'),weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)),weights[sample])
                    histoDict['ST_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)),weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['WJets'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].SetName('WJets')
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].SetName('WJets_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['WJets_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].SetName('WJets_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['WJets'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'),weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)),weights[sample])
                    histoDict['WJets_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)),weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['DY'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].SetName('DY')
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].SetName('DY_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DY_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].SetName('DY_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['DY'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'),weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)),weights[sample])
                    histoDict['DY_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)),weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
                histoDict['QCD'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].SetName('QCD')
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
                    histoDict['QCD_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].SetName('QCD_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['QCD_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].SetName('QCD_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['QCD'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'),weights[sample])
                for syst in systs:
                    histoDict['QCD_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)),weights[sample])
                    histoDict['QCD_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_%sDown'%(syst)),weights[sample])

        elif 'WR' in sample:
            wrMass = sample.split('_')[1][2:]
            nuMass = sample.split('_')[3][2:]
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetName('WR_%s_NR_%s'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

	elif 'SingleMuon' in sample:
            if sample == 'SingleMuon--Run2016B-03Feb2017_ver2-v2':
                histoDict['data_obs'] = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['data_obs'].SetDirectory(0)
                histoDict['data_obs'].SetName('data_obs')
            else:
                histoDict['data_obs'].Add(tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF'))

        tfile.Close()


    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_ZPeak.root'
    outputWorkspace = r.TFile(outputFile, "RECREATE")

    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
    MuMuJJ.cd()

    histoDict['TT'].Write()
    histoDict['DiBoson'].Write()
    histoDict['ST'].Write()
    histoDict['WJets'].Write()
    histoDict['DY'].Write()
    histoDict['QCD'].Write()
    histoDict['data_obs'].Write()
    for syst in systs:
        histoDict['TT_%sUp'%(syst)].Write()
        histoDict['TT_%sDown'%(syst)].Write()
        histoDict['DiBoson_%sUp'%(syst)].Write()
        histoDict['DiBoson_%sDown'%(syst)].Write()
        histoDict['ST_%sUp'%(syst)].Write()
        histoDict['ST_%sDown'%(syst)].Write()
        histoDict['WJets_%sUp'%(syst)].Write()
        histoDict['WJets_%sDown'%(syst)].Write()
        histoDict['DY_%sUp'%(syst)].Write()
        histoDict['DY_%sDown'%(syst)].Write()
        histoDict['QCD_%sUp'%(syst)].Write()
        histoDict['QCD_%sDown'%(syst)].Write()

    signalMasses = {(5000,1000),(5000,500),(4500,900),(4500,450),(4000,800),(4000,400),(3500,700),(3500,350),(3000,600),(3000,300),(2500,500),(2500,250),(2000,400),(2000,200),(1500,300),(1500,150),(1000,200),(1000,100)}
    for WRmass,NRmass in signalMasses:
        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
        for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()

    outputWorkspace.Write()
    outputWorkspace.Close()

def FSBWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights):
    histoDict = {}

    for sample in sampleNames:
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
        systs = ['JEC','JER','PU','MuL','HEEP']

        if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':

            histoDict['TT'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
            histoDict['TT'].SetDirectory(0)
            histoDict['TT'].SetName('TT')
            histoDict['TT'].Scale(weights[sample])

            for syst in systs:
                histoDict['TT_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                histoDict['TT_%sUp'%(syst)].SetDirectory(0)
                histoDict['TT_%sUp'%(syst)].SetName('TT_%sUp'%(syst))
                histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
                histoDict['TT_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)].SetDirectory(0)
                histoDict['TT_%sDown'%(syst)].SetName('TT_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

        elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

            if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
                histoDict['DiBoson'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['DiBoson'].SetDirectory(0)
                histoDict['DiBoson'].SetName('DiBoson')
                histoDict['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    histoDict['DiBoson_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sUp'%(syst)].SetName('DiBoson_%sUp'%(syst))
                    histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].SetName('DiBoson_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
                histoDict['DiBoson'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'),weights[sample])
                for syst in systs:
                    histoDict['DiBoson_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)),weights[sample])
                    histoDict['DiBoson_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)),weights[sample])

        elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
            if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
                histoDict['ST'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].SetName('ST')
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].SetName('ST_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['ST_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].SetName('ST_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['ST'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'),weights[sample])
                for syst in systs:
                    histoDict['ST_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)),weights[sample])
                    histoDict['ST_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)),weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['WJets'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].SetName('WJets')
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].SetName('WJets_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['WJets_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].SetName('WJets_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['WJets'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'),weights[sample])
                for syst in systs:
                    histoDict['WJets_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)),weights[sample])
                    histoDict['WJets_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)),weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                histoDict['DY'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].SetName('DY')
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].SetName('DY_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['DY_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].SetName('DY_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['DY'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'),weights[sample])
                for syst in systs:
                    histoDict['DY_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)),weights[sample])
                    histoDict['DY_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)),weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
                histoDict['QCD'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].SetName('QCD')
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
		    print "syst: ", syst
                    histoDict['QCD_%sUp'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].SetName('QCD_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    histoDict['QCD_%sDown'%(syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].SetName('QCD_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
                histoDict['QCD'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'),weights[sample])
                for syst in systs:
                    histoDict['QCD_%sUp'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)),weights[sample])
                    histoDict['QCD_%sDown'%(syst)].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)),weights[sample])

        elif 'WR' in sample:
            wrMass = sample.split('_')[1][2:]
            nuMass = sample.split('_')[3][2:]
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetName('WR_%s_NR_%s'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetName('WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

        elif 'SingleElectron' in sample:
            if sample == 'SingleElectron--Run2016B-03Feb2017_ver2-v2':
                histoDict['data_obs'] = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['data_obs'].SetDirectory(0)
                histoDict['data_obs'].SetName('data_obs')
            else:
                histoDict['data_obs'].Add(tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass'))

        tfile.Close()

    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_FSB.root'
    outputWorkspace = r.TFile(outputFile, "RECREATE")

    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
    MuMuJJ.cd()

    histoDict['TT'].Write()
    histoDict['DiBoson'].Write()
    histoDict['ST'].Write()
    histoDict['WJets'].Write()
    histoDict['DY'].Write()
    histoDict['QCD'].Write()
    histoDict['data_obs'].Write()
    for syst in systs:
        histoDict['TT_%sUp'%(syst)].Write()
        histoDict['TT_%sDown'%(syst)].Write()
        histoDict['DiBoson_%sUp'%(syst)].Write()
        histoDict['DiBoson_%sDown'%(syst)].Write()
        histoDict['ST_%sUp'%(syst)].Write()
        histoDict['ST_%sDown'%(syst)].Write()
        histoDict['WJets_%sUp'%(syst)].Write()
        histoDict['WJets_%sDown'%(syst)].Write()
        histoDict['DY_%sUp'%(syst)].Write()
        histoDict['DY_%sDown'%(syst)].Write()
        histoDict['QCD_%sUp'%(syst)].Write()
        histoDict['QCD_%sDown'%(syst)].Write()

    signalMasses = {(5000,1000),(5000,500),(4500,900),(4500,450),(4000,800),(4000,400),(3500,700),(3500,350),(3000,600),(3000,300),(2500,500),(2500,250),(2000,400),(2000,200),(1500,300),(1500,150),(1000,200),(1000,100)}
    for WRmass,NRmass in signalMasses:
        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
        for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()

    outputWorkspace.Write()
    outputWorkspace.Close()

def getEventsWeight(file,directory="",prefix="",filter="",inFolder = False):
    r.gROOT.SetBatch(True)
    hists1d = ["TH1D", "TH1F", "TH1"]
    hists2d = ["TH2D", "TH2F", "TH2"]
    histObjectNames = hists1d + hists2d
    print "Looking for eventsWeight histogram"
    if inFolder:
        for key in file.GetListOfKeys():
            print key.GetName()
            if key.GetClassName() in histObjectNames and filter in prefix and key.GetName() == "eventsWeight":
                print "Found events weight"
                return float(file.Get(key.GetName()).GetBinContent(1))
    else:
        for key in file.GetListOfKeys():
            if key.IsFolder():
                if key.GetName() == "allEvents":
                    inFolder = True
                dir = file.Get(key.GetName())
                newDir=directory+"/"+key.GetName()
                return getEventsWeight(dir,directory=newDir, prefix=prefix,filter=filter, inFolder = inFolder)
    print "UH OH! NO EVENTS WEIGHT PLOT FOUND!"
    return 1.0

############################################################################


if len(sys.argv) == 2 and (sys.argv[1] == "--help" or sys.argv[1] == "-h"):
    print "=========="
    print "This program takes the hadded histogram ROOT files and places the histograms needed for Higgs Combine in a single output ROOT file"
    print "In order, please specify:"
    print "Text file containing all samples (background+sig+data)"
    print "Directory of the samples"
    print "Directory for the output workspace"
    print "True/False for whether blind or not"

sampleList = sys.argv[1]
samplesLocation = sys.argv[2]
workspaceOutputDirectory = sys.argv[3]
Blind = sys.argv[4]
integratedLuminosity = 35900.0

with open(sampleList) as f:
    lines = f.read().splitlines()

xsecs = collections.OrderedDict()
sampleNames = []
lineNum = 0
for line in lines:
    if lineNum < 2 :
        lineNum+=1
        continue
    if ("SingleMuon" in line.split()[0] or "SingleElectron" in line.split()[0]):
        sampleNames.append(line.split()[0].strip().split("/")[1]+"--"+line.split()[0].strip().split("/")[2])
    else:
	sampleNames.append(line.split()[0].strip().split("/")[1])
	xsecLine = line.split()[3].strip().split("+")[0]
        if "*" in xsecLine:
            xsec = float(xsecLine.split("*")[0])*float(xsecLine.split("*")[1])
        else:
            xsec = float(xsecLine)
        if xsec == 0.0:
            print "ERROR MALFORMED XSEC: "
            print xsecLine
            exit(1)
        name = line.split()[0].strip().split("/")[1]
	xsecs[name] = xsec

weights = collections.OrderedDict()
for sample,xsec in xsecs.items():
	print "sample: ", sample
	backgroundEventsWeight = samplesLocation+sample+".root"
	weight = 1.0
	if "WR_M" in sample:
	    weight *= integratedLuminosity/1000
	else:
            weight *= integratedLuminosity
        weight *= xsecs[sample]
        eventsWeight = 0
        eventsWeight = getEventsWeight(r.TFile.Open(backgroundEventsWeight, "read"),directory=samplesLocation)
        if (eventsWeight == 0):
            print "BACKGROUND HAS ZERO EVENTS WEIGHT"
            continue
        print "Found # events:"+str(eventsWeight)
        weight /= eventsWeight
	print "Scale: "+str(weight)
	weights[sample] = weight


print "MAKING SIGNAL REGION WORKSPACE"
SignalRegionWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory,Blind, weights)

print "MAKING ZPEAK REGION WORKSPACE"
ZPeakWorksapce(sampleNames,samplesLocation,workspaceOutputDirectory, weights)

print "MAKING FSB REGION WORKSPACE"
FSBWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights)
