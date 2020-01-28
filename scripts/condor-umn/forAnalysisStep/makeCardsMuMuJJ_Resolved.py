import ROOT as r,sys,math,array,os
from ROOT import TFile, TTree, TChain, gPad, gDirectory
from multiprocessing import Process
from optparse import OptionParser
from operator import add
import math
import sys
import time
import array

def SignalRegionCard(workspaceDirectory):

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_ResolvedSignalRegion.root'

    tfile = r.TFile.Open(workspace)

    bkgs = ['ST','WJets','DY','DiBoson','TriBoson','ttV']
    Allbkgs = ['ST','WJets','DY','DiBoson','TriBoson','ttV','EMu']
    #bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
    systs_pdf = ['Scale_muR2_muF1','Scale_muRp5_muF1','Scale_muR1_muF2','Scale_muR2_muF2','Scale_muR1_muFp5','Scale_muRp5_muFp5','Numerator']
#    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']
#    sigs = ['2000,1000','3000,1500','4000,2000']

    signalMasses = []

    signalMassFile = 'signalMassPoints.txt'

    with open(signalMassFile) as f:
        lines = f.read().splitlines()

    for line in lines:
        line = line.split(',')
        signalMasses.append(line)


    histoDict = {}

    for bkg in bkgs:
        histoDict['%s'%(bkg)] = tfile.Get('MuMuJJ/%s'%(bkg))
	for syst in systs:
	    histoDict['%s_%sUp'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sUp'%(bkg,syst))
            histoDict['%s_%sDown'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sDown'%(bkg,syst))

    for WRmass,NRmass in signalMasses:
	histoDict['WR_%s_NR_%s'%(WRmass,NRmass)] = tfile.Get('MuMuJJ/WR_%s_NR_%s'%(WRmass,NRmass))
	for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst))
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst))
        for syst in systs_pdf:
            histoDict['WR_%s_NR_%s_%s'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%s'%(WRmass,NRmass,syst))


    jecErrs = {}
    jerErrs = {}
    puErrs = {}
    muResIsoErrs = {}
    muResIDErrs = {}
    muResolErrs = {}
    muTrigErrs = {}
    ZweightsErrs = {}
    ScaleErrs = {}

    for bkg in bkgs:
	print "bkg: ", bkg
	rate = histoDict['%s'%(bkg)].Integral()
	rateJECUp = histoDict['%s_JECUp'%(bkg)].Integral()
	rateJECDown = histoDict['%s_JECDown'%(bkg)].Integral()
	rateJERUp = histoDict['%s_JERUp'%(bkg)].Integral()
	rateJERDown = histoDict['%s_JERDown'%(bkg)].Integral()
	ratePUUp = histoDict['%s_PUUp'%(bkg)].Integral()
	ratePUDown = histoDict['%s_PUDown'%(bkg)].Integral()


	if rate > 0:
	    jecErrs['%s'%(bkg)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
	    jerErrs['%s'%(bkg)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
	    puErrs['%s'%(bkg)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
	else:
            jecErrs['%s'%(bkg)] = 1.0
            jerErrs['%s'%(bkg)] = 1.0
            puErrs['%s'%(bkg)] = 1.0

        jecErrs['%s'%(bkg)] = 1.0
        jerErrs['%s'%(bkg)] = 1.0
        puErrs['%s'%(bkg)] = 1.0
	muResIsoErrs['%s'%(bkg)] = 1.0
	muResIDErrs['%s'%(bkg)] = 1.0
        muResolErrs['%s'%(bkg)] = 1.0
        muTrigErrs['%s'%(bkg)] = 1.0
        ZweightsErrs['%s'%(bkg)] = 1.0

    for WRmass,NRmass in signalMasses:
        rate = histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        rateJECUp = histoDict['WR_%s_NR_%s_JECUp'%(WRmass,NRmass)].Integral()
        rateJECDown = histoDict['WR_%s_NR_%s_JECDown'%(WRmass,NRmass)].Integral()
        rateJERUp = histoDict['WR_%s_NR_%s_JERUp'%(WRmass,NRmass)].Integral()
        rateJERDown = histoDict['WR_%s_NR_%s_JERDown'%(WRmass,NRmass)].Integral()
        ratePUUp = histoDict['WR_%s_NR_%s_PUUp'%(WRmass,NRmass)].Integral()
        ratePUDown = histoDict['WR_%s_NR_%s_PUDown'%(WRmass,NRmass)].Integral()

        rateNominal = histoDict['WR_%s_NR_%s_Numerator'%(WRmass,NRmass)].Integral()
        rateScale = []
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muR2_muF1'%(WRmass,NRmass)].Integral())
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muRp5_muF1'%(WRmass,NRmass)].Integral())
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muR1_muF2'%(WRmass,NRmass)].Integral())
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muR2_muF2'%(WRmass,NRmass)].Integral())
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muR1_muFp5'%(WRmass,NRmass)].Integral())
        rateScale.append(histoDict['WR_%s_NR_%s_Scale_muRp5_muFp5'%(WRmass,NRmass)].Integral())

        rateScale.sort()

        if rate > 0:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
            ScaleErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateScale[5]-rateNominal) + abs(rateScale[0]-rateNominal))/(2*rateNominal)
        else:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            ScaleErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

        jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muResIsoErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
	muResIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for WRmass,NRmass in signalMasses:

    	divider = '------------------------------------------------------------\n'
    	datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
	    'shapes * ResolvedSignalRegion WR_NR_MuMuJJ_ResolvedSignalRegion.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
	    divider + \
	    'bin             ResolvedSignalRegion\n' + \
            'observation     -1.0\n' + \
            divider

    	binString = 'bin'
    	processString = 'process'
    	processNumberString = 'process'
    	rateString = 'rate'
    	lumiString = 'lumi\tlnN'
    	jecString = 'JEC\tshapeN2'
    	jerString = 'JER\tshapeN2'
    	puString = 'PU\tshapeN2'
	muResIDString = 'MuResID\tshapeN2'
	muResolString = 'MuResol\tshapeN2'
	muTrigString = 'MuTrig\tshapeN2'
        ScaleUncString = 'Scale\tlnN'
        PDFUncString = 'PDF\tshapeN2'
        AlphaSUncString = 'AlphaS\tshapeN2'
        ZweightString = 'Zweight\tlnN'

	print "WR_%s_NR_%s%(WRmass,NRmass): ", 'WR_%s_NR_%s'%(WRmass,NRmass)
	print "histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral(): ", histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        if histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral() > 0:
	    print "INSIDE HERE"
            binString += " ResolvedSignalRegion"
            processString += ' WR_%s_NR_%s'%(WRmass,NRmass)
            processNumberString += ' 0'
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            jerString += ' %.3f'%jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            puString += ' %.3f'%puErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muResIDString += '\t%.3f'%muResIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muResolString += '\t%.3f'%muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            ZweightString += '\t-'
	    muTrigString += '\t%.3f'%muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            ScaleUncString += '\t%.3f'%(ScaleErrs['WR_%s_NR_%s'%(WRmass,NRmass)])
            PDFUncString += '\t1'
            AlphaSUncString += '\t1'

    	sampleNumber = 0
    	for bkg in Allbkgs:
	  if bkg == 'EMu':
	    sampleNumber += 1
            binString += " ResolvedSignalRegion"
            processString += ' %s'%(bkg)
            processNumberString += ' %s'%(sampleNumber)
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' -'
            jerString += ' -'
            puString += ' -'
            muResIDString += '\t-'
            muResolString += '\t-'
            muTrigString += '\t-'
            ScaleUncString += '\t-'
            PDFUncString += '\t-'
            AlphaSUncString += '\t-'
            ZweightString += '\t-'
	  else:
#	    print "%s%(bkg): ", '%s'%(bkg)
#	    print "histoDict['%s'%(bkg)].Integral(): ", histoDict['%s'%(bkg)].Integral()
            if histoDict['%s'%(bkg)].Integral() <= 0.: continue
	    sampleNumber += 1
    	    binString += " ResolvedSignalRegion"
    	    processString += ' %s'%(bkg)
    	    processNumberString += ' %s'%(sampleNumber)
    	    rateString += ' -1'
    	    lumiString += ' 1.025'
    	    jecString += ' %.3f'%jecErrs['%s'%(bkg)]
    	    jerString += ' %.3f'%jerErrs['%s'%(bkg)]
    	    puString += ' %.3f'%puErrs['%s'%(bkg)]
	    muResIDString += '\t%.3f'%muResIDErrs['%s'%(bkg)]
	    muResolString += '\t%.3f'%muResolErrs['%s'%(bkg)]
	    muTrigString += '\t%.3f'%muTrigErrs['%s'%(bkg)]
            ScaleUncString += '\t-'
            PDFUncString += '\t-'
            AlphaSUncString += '\t-'
            if bkg == 'DY':
                ZweightString += '\t%.3f'%ZweightsErrs['%s'%(bkg)]
            else:
                ZweightString += '\t-'

	outputFile = workspaceDirectory+"WR_%s_NR_%s_ResolvedSignalRegion.txt"%(WRmass,NRmass)
	print "processNumberString: ", processNumberString
    	dctmp = open(outputFile,'w')
    	binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
    	jecString+='\n'; jerString+='\n'; puString+='\n'; muResIDString+='\n'; muResolString+='\n'; muTrigString+='\n'; ScaleUncString+='\n'; PDFUncString+='\n'; AlphaSUncString+='\n'; ZweightString+='\n'

    	datacard+=binString+processString+processNumberString+rateString+divider

    	datacard+=lumiString+jecString+jerString+puString+muResIDString+muResolString+muTrigString+ZweightString+ScaleUncString+PDFUncString+AlphaSUncString

	datacard+='DYNorm lnN - - 1.03 - - - -\n'
	datacard+='Syst shapeN2 - - - - - - 1\n'
#        datacard+='topNorm rateParam ResolvedSignalRegion TT 0.674 [0.5392,0.8088]\n' + \
#        'DYNorm rateParam ResolvedSignalRegion DY 1.069 [1.061,1.077]\n' + \
        datacard+='* autoMCStats 0\n'

    	dctmp.write(datacard)

def ZPeakCard(workspaceDirectory):

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_ResolvedZPeak.root'

    tfile = r.TFile.Open(workspace)

    bkgs = ['TT','ST','WJets','DY','DiBoson']
#    bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
#    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']
    sigs = ['2000,1000','3000,1500','4000,2000']

    histoDict = {}

    for bkg in bkgs:
        histoDict['%s'%(bkg)] = tfile.Get('MuMuJJ/%s'%(bkg))
        for syst in systs:
            histoDict['%s_%sUp'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sUp'%(bkg,syst))
            histoDict['%s_%sDown'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sDown'%(bkg,syst))

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]
        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)] = tfile.Get('MuMuJJ/WR_%s_NR_%s'%(WRmass,NRmass))
        for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst))
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst))


    jecErrs = {}
    jerErrs = {}
    puErrs = {}
    muResIsoErrs = {}
    muResIDErrs = {}
    muResolErrs = {}
    muTrigErrs = {}

    for bkg in bkgs:
        rate = histoDict['%s'%(bkg)].Integral()
        rateJECUp = histoDict['%s_JECUp'%(bkg)].Integral()
        rateJECDown = histoDict['%s_JECDown'%(bkg)].Integral()
        rateJERUp = histoDict['%s_JERUp'%(bkg)].Integral()
        rateJERDown = histoDict['%s_JERDown'%(bkg)].Integral()
        ratePUUp = histoDict['%s_PUUp'%(bkg)].Integral()
        ratePUDown = histoDict['%s_PUDown'%(bkg)].Integral()


        if rate > 0:
            jecErrs['%s'%(bkg)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
            jerErrs['%s'%(bkg)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
            puErrs['%s'%(bkg)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
        else:
            jecErrs['%s'%(bkg)] = 1.0
            jerErrs['%s'%(bkg)] = 1.0
            puErrs['%s'%(bkg)] = 1.0

        muResIsoErrs['%s'%(bkg)] = 1.0
        muResIDErrs['%s'%(bkg)] = 1.0
        muResolErrs['%s'%(bkg)] = 1.0
        muTrigErrs['%s'%(bkg)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]
        rate = histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        rateJECUp = histoDict['WR_%s_NR_%s_JECUp'%(WRmass,NRmass)].Integral()
        rateJECDown = histoDict['WR_%s_NR_%s_JECDown'%(WRmass,NRmass)].Integral()
        rateJERUp = histoDict['WR_%s_NR_%s_JERUp'%(WRmass,NRmass)].Integral()
        rateJERDown = histoDict['WR_%s_NR_%s_JERDown'%(WRmass,NRmass)].Integral()
        ratePUUp = histoDict['WR_%s_NR_%s_PUUp'%(WRmass,NRmass)].Integral()
        ratePUDown = histoDict['WR_%s_NR_%s_PUDown'%(WRmass,NRmass)].Integral()


        if rate > 0:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
        else:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

        muResIsoErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muResIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]

        divider = '------------------------------------------------------------\n'
        datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
            'shapes * ZPeak WR_NR_MuMuJJ_ResolvedZPeak.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
            divider + \
            'bin             ZPeak\n' + \
            'observation     -1.0\n' + \
            divider

        binString = 'bin'
        processString = 'process'
        processNumberString = 'process'
        rateString = 'rate'
        lumiString = 'lumi\tlnN'
        jecString = 'JEC\tlnN'
        jerString = 'JER\tlnN'
        puString = 'Pu\tlnN'
        muResIDString = 'MuResID\tshape'
        muResolString = 'MuResol\tshape'
        muTrigString = 'MuTrig\tshape'

        print "WR_%s_NR_%s%(WRmass,NRmass): ", 'WR_%s_NR_%s'%(WRmass,NRmass)
        print "histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral(): ", histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
	if histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral() > 0:
            binString += " ZPeak"
            processString += ' WR_%s_NR_%s'%(WRmass,NRmass)
            processNumberString += ' 0'
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            jerString += ' %.3f'%jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            puString += ' %.3f'%puErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muResIDString += '\t%.3f'%muResIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muResolString += '\t%.3f'%muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muTrigString += '\t%.3f'%muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)]

        sampleNumber = 0
        for bkg in bkgs:
            print "%s%(bkg): ", '%s'%(bkg)
            print "histoDict['%s'%(bkg)].Integral(): ", histoDict['%s'%(bkg)].Integral()
	    if histoDict['%s'%(bkg)].Integral() <= 0.: continue
            sampleNumber += 1
            binString += " ZPeak"
            processString += ' %s'%(bkg)
            processNumberString += ' %s'%(sampleNumber)
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['%s'%(bkg)]
            jerString += ' %.3f'%jerErrs['%s'%(bkg)]
            puString += ' %.3f'%puErrs['%s'%(bkg)]
            muResIDString += '\t%.3f'%muResIDErrs['%s'%(bkg)]
            muResolString += '\t%.3f'%muResolErrs['%s'%(bkg)]
            muTrigString += '\t%.3f'%muTrigErrs['%s'%(bkg)]

        outputFile = workspaceDirectory+"WR_%s_NR_%s_ResolvedZPeak.txt"%(WRmass,NRmass)
        dctmp = open(outputFile,'w')
        binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
        jecString+='\n'; jerString+='\n'; puString+='\n'; muResIDString+='\n'; muResolString+='\n'; muTrigString+='\n';

        datacard+=binString+processString+processNumberString+rateString+divider

        datacard+=lumiString+jecString+jerString+puString+muResIDString+muResolString+muTrigString
        datacard+='DYNorm rateParam ZPeak DY 1.069 [1.061,1.077]\n' #+ \
        '* autoMCStats 0\n'

        dctmp.write(datacard)

def FSBCarb(workspaceDirectory):

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_ResolvedFSB.root'

    tfile = r.TFile.Open(workspace)

    bkgs = ['TT','ST','WJets','DY','DiBoson']
#    bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
#    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']
    sigs = ['2000,1000','3000,1500','4000,2000']

    histoDict = {}

    for bkg in bkgs:
        histoDict['%s'%(bkg)] = tfile.Get('MuMuJJ/%s'%(bkg))
        for syst in systs:
            histoDict['%s_%sUp'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sUp'%(bkg,syst))
            histoDict['%s_%sDown'%(bkg,syst)] = tfile.Get('MuMuJJ/%s_%sDown'%(bkg,syst))

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]
        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)] = tfile.Get('MuMuJJ/WR_%s_NR_%s'%(WRmass,NRmass))
        for syst in systs:
            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst))
            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)] = tfile.Get('MuMuJJ/WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst))


    jecErrs = {}
    jerErrs = {}
    puErrs = {}
    HEEPErrs = {}
    muIsoErrs = {}
    muIDErrs = {}
    muResolErrs = {}
    muTrigErrs =  {}

    for bkg in bkgs:
        rate = histoDict['%s'%(bkg)].Integral()
        rateJECUp = histoDict['%s_JECUp'%(bkg)].Integral()
        rateJECDown = histoDict['%s_JECDown'%(bkg)].Integral()
        rateJERUp = histoDict['%s_JERUp'%(bkg)].Integral()
        rateJERDown = histoDict['%s_JERDown'%(bkg)].Integral()
        ratePUUp = histoDict['%s_PUUp'%(bkg)].Integral()
        ratePUDown = histoDict['%s_PUDown'%(bkg)].Integral()


        if rate > 0:
            jecErrs['%s'%(bkg)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
            jerErrs['%s'%(bkg)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
            puErrs['%s'%(bkg)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
        else:
            jecErrs['%s'%(bkg)] = 1.0
            jerErrs['%s'%(bkg)] = 1.0
            puErrs['%s'%(bkg)] = 1.0

        HEEPErrs['%s'%(bkg)] = 1.0
        muIsoErrs['%s'%(bkg)] = 1.0
        muIDErrs['%s'%(bkg)] = 1.0
        muResolErrs['%s'%(bkg)] = 1.0
        muTrigErrs['%s'%(bkg)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]
        rate = histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        rateJECUp = histoDict['WR_%s_NR_%s_JECUp'%(WRmass,NRmass)].Integral()
        rateJECDown = histoDict['WR_%s_NR_%s_JECDown'%(WRmass,NRmass)].Integral()
        rateJERUp = histoDict['WR_%s_NR_%s_JERUp'%(WRmass,NRmass)].Integral()
        rateJERDown = histoDict['WR_%s_NR_%s_JERDown'%(WRmass,NRmass)].Integral()
        ratePUUp = histoDict['WR_%s_NR_%s_PUUp'%(WRmass,NRmass)].Integral()
        ratePUDown = histoDict['WR_%s_NR_%s_PUDown'%(WRmass,NRmass)].Integral()


        if rate > 0:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJECUp-rate)+abs(rateJECDown-rate))/(2.*rate)
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(rateJERUp-rate)+abs(rateJERDown-rate))/(2.*rate)
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0+(abs(ratePUUp-rate)+abs(ratePUDown-rate))/(2.*rate)
        else:
            jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
            puErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

        HEEPErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muIsoErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]

        divider = '------------------------------------------------------------\n'
        datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
            'shapes * ResolvedFSB WR_NR_MuMuJJ_ResolvedFSB.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
            divider + \
            'bin             ResolvedFSB\n' + \
            'observation     -1.0\n' + \
            divider

        binString = 'bin'
        processString = 'process'
        processNumberString = 'process'
        rateString = 'rate'
        lumiString = 'lumi\tlnN'
        jecString = 'JEC\tlnN'
        jerString = 'JER\tlnN'
        puString = 'Pu\tlnN'
        HEEPString = 'HEEP\tshape'
        muIsoString = 'MuIso\tshape'
	muIDString = 'MuID\tshape'
	muResolString = 'MuResol\tshape'
	muTrigString ='MuTrig\tshape'

        print "WR_%s_NR_%s%(WRmass,NRmass): ", 'WR_%s_NR_%s'%(WRmass,NRmass)
        print "histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral(): ", histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        if histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral() > 0:
            binString += " ResolvedFSB"
            processString += ' WR_%s_NR_%s'%(WRmass,NRmass)
            processNumberString += ' 0'
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            jerString += ' %.3f'%jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            puString += ' %.3f'%puErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            HEEPString += '\t%.3f'%HEEPErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muIsoString += '\t%.3f'%muIsoErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muIDString += '\t%.3f'%muIDErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muResolString += '\t%.3f'%muResolErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muTrigString += '\t%.3f'%muTrigErrs['WR_%s_NR_%s'%(WRmass,NRmass)]

        sampleNumber = 0
        for bkg in bkgs:
            print "%s%(bkg): ", '%s'%(bkg)
            print "histoDict['%s'%(bkg)].Integral(): ", histoDict['%s'%(bkg)].Integral()
            if histoDict['%s'%(bkg)].Integral() <= 0.: continue
            sampleNumber += 1
            binString += " ResolvedFSB"
            processString += ' %s'%(bkg)
            processNumberString += ' %s'%(sampleNumber)
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['%s'%(bkg)]
            jerString += ' %.3f'%jerErrs['%s'%(bkg)]
            puString += ' %.3f'%puErrs['%s'%(bkg)]
            HEEPString += '\t%.3f'%HEEPErrs['%s'%(bkg)]
            muIsoString += '\t%.3f'%muIsoErrs['%s'%(bkg)]
	    muIDString += '\t%.3f'%muIDErrs['%s'%(bkg)]
	    muResolString += '\t%.3f'%muResolErrs['%s'%(bkg)]
	    muTrigString += '\t%.3f'%muTrigErrs['%s'%(bkg)]

        outputFile = workspaceDirectory+"WR_%s_NR_%s_ResolvedFSB.txt"%(WRmass,NRmass)
        dctmp = open(outputFile,'w')
        binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
        jecString+='\n'; jerString+='\n'; puString+='\n'; HEEPString+='\n'; muIsoString+='\n'; muIDString+='\n'; muResolString+='\n'; muTrigString+='\n';

        datacard+=binString+processString+processNumberString+rateString+divider

#### Temporarily removing HEEP unc. because of plot filling issue
#### Put back after rerunning
        datacard+=lumiString+jecString+jerString+puString+muIsoString+muIDString+muResolString+muTrigString
#        datacard+=lumiString+jecString+jerString+puString+HEEPString+muIsoString+muIDString+muResolString+muTrigString
#        datacard+='topNorm extArg 0.674 [0.5392,0.8088]' + \
#	'rateTop rateParam ResolvedFSB TT @0 topNorm' + \
	datacard+='topNorm rateParam ResolvedFSB TT 0.674 [0.5392,0.8088]\n' + \
	'* autoMCStats 0\n'

        dctmp.write(datacard)

##########################################################################

if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="

workspaceDirectory = sys.argv[1]

SignalRegionCard(workspaceDirectory)

#ZPeakCard(workspaceDirectory)

#FSBCarb(workspaceDirectory)

