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
import array

def SignalRegionWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, Blind, weights):

    histoDict = {}

    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000, 6000]
    binBoundariesArray = array.array('d', binBoundaries)

    for sample in sampleNames:
	print "sample: ", sample
	tfile = r.TFile.Open(samplesLocation+sample+'.root')
	bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
	systs = ['JEC','JER','PU','MuHPt','MuL']

	if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':
	    
            temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
	    histoDict['TT'] = temp.Rebin(14, 'TT', binBoundariesArray)
            histoDict['TT'].SetDirectory(0)
	    histoDict['TT'].Scale(weights[sample])

	    for syst in systs:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		histoDict['TT_%sUp'%(syst)] = temp.Rebin(14, 'TT_%sUp'%(syst), binBoundariesArray)
                histoDict['TT_%sUp'%(syst)].SetDirectory(0)
		histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
        	temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)] = temp.Rebin(14, 'TT_%sDown'%(syst), binBoundariesArray)
		histoDict['TT_%sDown'%(syst)].SetDirectory(0)
		histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

	elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

	    if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['DiBoson'] = temp.Rebin(14, 'DiBoson', binBoundariesArray)
		histoDict['DiBoson'].SetDirectory(0)
            	histoDict['DiBoson'].Scale(weights[sample])
		for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['DiBoson_%sUp'%(syst)] = temp.Rebin(14, 'DiBoson_%sUp'%(syst), binBoundariesArray)
	            histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
	            histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['DiBoson_%sDown'%(syst)] = temp.Rebin(14, 'DiBoson_%sDown'%(syst), binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

	    else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'DiBoson_temp', binBoundariesArray)
		histoDict['DiBoson'].Add(temp,weights[sample])
		for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
		    histoDict['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

	elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
	    if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['ST'] = temp.Rebin(14, 'ST', binBoundariesArray)
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['ST_%sUp'%(syst)] = temp.Rebin(14, 'ST_%sUp'%(syst), binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['ST_%sDown'%(syst)] = temp.Rebin(14, 'ST_%sDown'%(syst), binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'ST_temp', binBoundariesArray)
                histoDict['ST'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
		temp =  tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['WJets'] = temp.Rebin(14, 'WJets', binBoundariesArray)
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['WJets_%sUp'%(syst)] = temp.Rebin(14, 'WJets_%sUp'%(syst), binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['WJets_%sDown'%(syst)] = temp.Rebin(14, 'WJets_%sDown'%(syst), binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'WJets_temp', binBoundariesArray)
                histoDict['WJets'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['DY'] = temp.Rebin(14, 'DY', binBoundariesArray)
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['DY_%sUp'%(syst)] = temp.Rebin(14, 'DY_%sUp'%(syst), binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['DY_%sDown'%(syst)] = temp.Rebin(14, 'DY_%sDown'%(syst), binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'DY_temp', binBoundariesArray)
                histoDict['DY'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['QCD'] = temp.Rebin(14, 'QCD', binBoundariesArray)
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['QCD_%sUp'%(syst)] = temp.Rebin(14, 'QCD_%sUp'%(syst), binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['QCD_%sDown'%(syst)] = temp.Rebin(14, 'QCD_%sDown'%(syst), binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'QCD_temp', binBoundariesArray)
                histoDict['QCD'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'WR' in sample:
	    wrMass = sample.split('_')[1][2:]
	    nuMass = sample.split('_')[3][2:]
            temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
	    histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = temp.Rebin(14, 'WR_%s_NR_%s'%(wrMass,nuMass), binBoundariesArray)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
                temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sUp'%(syst))
		histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass_%sDown'%(syst))
		histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

        elif 'SingleMuon' in sample:
            if sample == 'SingleMuon--Run2016B-03Feb2017_ver2-v2':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass')
		histoDict['data_obs'] = temp.Rebin(14, 'data_obs', binBoundariesArray)
                histoDict['data_obs'].SetDirectory(0)
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO/leadAK8JetMuonMass').Rebin(14, 'data_obs_temp', binBoundariesArray)
                histoDict['data_obs'].Add(temp)

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

    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000, 6000]
#    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1150, 1500, 6000]
    binBoundariesArray = array.array('d', binBoundaries)

    for sample in sampleNames:
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
        systs = ['JEC','JER','PU','MuHPt','MuL']

        if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':

            temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
	    histoDict['TT'] = temp.Rebin(14, 'TT', binBoundariesArray)
            histoDict['TT'].SetDirectory(0)
            histoDict['TT'].Scale(weights[sample])

            for syst in systs:
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
		histoDict['TT_%sUp'%(syst)] = temp.Rebin(14, 'TT_%sUp'%(syst), binBoundariesArray)
                histoDict['TT_%sUp'%(syst)].SetDirectory(0)
                histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)] = temp.Rebin(14, 'TT_%sDown'%(syst), binBoundariesArray)
                histoDict['TT_%sDown'%(syst)].SetDirectory(0)
                histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

        elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

            if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
		histoDict['DiBoson'] = temp.Rebin(14, 'DiBoson', binBoundariesArray)
                histoDict['DiBoson'].SetDirectory(0)
                histoDict['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
		    histoDict['DiBoson_%sUp'%(syst)] = temp.Rebin(14, 'DiBoson_%sUp'%(syst), binBoundariesArray)
                    histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
		    histoDict['DiBoson_%sDown'%(syst)] = temp.Rebin(14, 'DiBoson_%sDown'%(syst), binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'DiBoson_temp', binBoundariesArray)
                histoDict['DiBoson'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])	
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

        elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
            if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['ST'] = temp.Rebin(14, 'ST', binBoundariesArray)
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)] = temp.Rebin(14, 'ST_%sUp'%(syst), binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
		    histoDict['ST_%sDown'%(syst)] = temp.Rebin(14, 'ST_%sDown'%(syst), binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'ST_temp', binBoundariesArray)
                histoDict['ST'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['WJets'] = temp.Rebin(14, 'WJets', binBoundariesArray)
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)] = temp.Rebin(14, 'WJets_%sUp'%(syst), binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)] = temp.Rebin(14, 'WJets_%sDown'%(syst), binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'WJets_temp', binBoundariesArray)
		histoDict['WJets'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['DY'] = temp.Rebin(14, 'DY', binBoundariesArray)
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)] = temp.Rebin(14, 'DY_%sUp'%(syst), binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)] = temp.Rebin(14, 'DY_%sDown'%(syst), binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'DY_temp', binBoundariesArray)
                histoDict['DY'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['QCD'] = temp.Rebin(14, 'QCD', binBoundariesArray)
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)] = temp.Rebin(14, 'QCD_%sUp'%(syst), binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)] = temp.Rebin(14, 'QCD_%sDown'%(syst), binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'QCD_temp', binBoundariesArray)
                histoDict['QCD'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WR' in sample:
            wrMass = sample.split('_')[1][2:]
            nuMass = sample.split('_')[3][2:]
            temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = temp.Rebin(14, 'WR_%s_NR_%s'%(wrMass,nuMass), binBoundariesArray)	    
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst), binBoundariesArray)		
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

	elif 'SingleMuon' in sample:
            if sample == 'SingleMuon--Run2016B-03Feb2017_ver2-v2':
                temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF')
                histoDict['data_obs'] = temp.Rebin(14, 'data_obs', binBoundariesArray)
                histoDict['data_obs'].SetDirectory(0)
            else:
		temp = tfile.Get('analysis/eventsPassingExtensionRECO2016VETOZMASS/leadAK8JetMuonMass_noLSF').Rebin(14, 'data_obs_temp', binBoundariesArray)
                histoDict['data_obs'].Add(temp)

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

    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000, 6000]
#    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1150, 1500, 6000]
    binBoundariesArray = array.array('d', binBoundaries)

    for sample in sampleNames:
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
        systs = ['JEC','JER','PU','MuL','HEEP']

        if sample == 'TT_TuneCUETP8M2T4_13TeV-powheg-pythia8':

            temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
            histoDict['TT'] = temp.Rebin(14, 'TT', binBoundariesArray)
            histoDict['TT'].SetDirectory(0)
            histoDict['TT'].Scale(weights[sample])

            for syst in systs:
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                histoDict['TT_%sUp'%(syst)] = temp.Rebin(14, 'TT_%sUp'%(syst), binBoundariesArray)
                histoDict['TT_%sUp'%(syst)].SetDirectory(0)
                histoDict['TT_%sUp'%(syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                histoDict['TT_%sDown'%(syst)] = temp.Rebin(14, 'TT_%sDown'%(syst), binBoundariesArray)
                histoDict['TT_%sDown'%(syst)].SetDirectory(0)
                histoDict['TT_%sDown'%(syst)].Scale(weights[sample])

        elif sample == 'WW_TuneCUETP8M1_13TeV-pythia8' or sample == 'WZ_TuneCUETP8M1_13TeV-pythia8' or sample == 'ZZ_TuneCUETP8M1_13TeV-pythia8':

            if sample == 'WW_TuneCUETP8M1_13TeV-pythia8':
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['DiBoson'] = temp.Rebin(14, 'DiBoson', binBoundariesArray)
                histoDict['DiBoson'].SetDirectory(0)
                histoDict['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['DiBoson_%sUp'%(syst)] = temp.Rebin(14, 'DiBoson_%sUp'%(syst), binBoundariesArray)
                    histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['DiBoson_%sDown'%(syst)] = temp.Rebin(14, 'DiBoson_%sDown'%(syst), binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'DiBoson_temp', binBoundariesArray)
                histoDict['DiBoson'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(14, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

        elif sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8' or sample == 'ST_t-channel_antitop_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_t-channel_top_4f_inclusiveDecays_TuneCUETP8M2T4_13TeV-powhegV2-madspin' or sample == 'ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4' or sample == 'ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4':
            if sample == 'ST_s-channel_4f_InclusiveDecays_13TeV-amcatnlo-pythia8':
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['ST'] = temp.Rebin(14, 'ST', binBoundariesArray)
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['ST_%sUp'%(syst)] = temp.Rebin(14, 'ST_%sUp'%(syst), binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['ST_%sDown'%(syst)] = temp.Rebin(14, 'ST_%sDown'%(syst), binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'ST_temp', binBoundariesArray)
                histoDict['ST'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(14, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample:
            if sample == 'WJetsToLNu_Wpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['WJets'] = temp.Rebin(14, 'WJets', binBoundariesArray)
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['WJets_%sUp'%(syst)] = temp.Rebin(14, 'WJets_%sUp'%(syst), binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['WJets_%sDown'%(syst)] = temp.Rebin(14, 'WJets_%sDown'%(syst), binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'WJets_temp', binBoundariesArray)
                histoDict['WJets'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(14, 'WJets_temp', binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'DY' in sample:
            if sample == 'DYJetsToLL_Zpt-0To50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8':
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['DY'] = temp.Rebin(14, 'DY', binBoundariesArray)
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['DY_%sUp'%(syst)] = temp.Rebin(14, 'DY_%sUp'%(syst), binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['DY_%sDown'%(syst)] = temp.Rebin(14, 'DY_%sDown'%(syst), binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'DY_temp', binBoundariesArray)
                histoDict['DY'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(14, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'QCD' in sample:
            if sample == 'QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8':
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['QCD'] = temp.Rebin(14, 'QCD', binBoundariesArray)
                histoDict['QCD'].SetDirectory(0)
                histoDict['QCD'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDict['QCD_%sUp'%(syst)] = temp.Rebin(14, 'QCD_%sUp'%(syst), binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDict['QCD_%sDown'%(syst)] = temp.Rebin(14, 'QCD_%sDown'%(syst), binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].SetDirectory(0)
                    histoDict['QCD_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'QCD_temp', binBoundariesArray)
                histoDict['QCD'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sUp'%(syst)].Add(temp,weights[sample])	
		    temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(14, 'QCD_temp', binBoundariesArray)
                    histoDict['QCD_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WR' in sample:
            wrMass = sample.split('_')[1][2:]
            nuMass = sample.split('_')[3][2:]
            temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = temp.Rebin(14, 'WR_%s_NR_%s'%(wrMass,nuMass), binBoundariesArray)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weights[sample])

            for syst in systs:
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sUp'%(syst))
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass_%sDown'%(syst))
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = temp.Rebin(14, 'WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weights[sample])

        elif 'SingleElectron' in sample:
            if sample == 'SingleElectron--Run2016B-03Feb2017_ver2-v2':
                temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass')
                histoDict['data_obs'] = temp.Rebin(14, 'data_obs', binBoundariesArray)
                histoDict['data_obs'].SetDirectory(0)
            else:
		temp = tfile.Get('analysis/eventsPassingFlavorSidebandRECOelePt200/leadAK8JetElectronMass').Rebin(14, 'data_obs_temp', binBoundariesArray)
                histoDict['data_obs'].Add(temp)

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
