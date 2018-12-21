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

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_SignalRegion.root'

    tfile = r.TFile.Open(workspace)

    bkgs = ['TT','ST','WJets','DY','DiBoson']
    bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']

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
    muHiPTidErrs = {}
    muLooseIdErrs = {}

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

	muHiPTidErrs['%s'%(bkg)] = 1.0
	muLooseIdErrs['%s'%(bkg)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]
	print "WRmass: ", WRmass
	print "NRmass: ", NRmass
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

        muHiPTidErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
	muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]

    	divider = '------------------------------------------------------------\n'
    	datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
	    'shapes * SignalRegion WR_NR_MuMuJJ_SignalRegion.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
	    divider + \
	    'bin             SignalRegion\n' + \
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
    	muHiPTidString = 'MuHPt\tshape'
	muLooseIdString = 'MuL\tshape'

	print "WR_%s_NR_%s%(WRmass,NRmass): ", 'WR_%s_NR_%s'%(WRmass,NRmass)
	print "histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral(): ", histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        if histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral() > 0:
	    print "INSIDE HERE"
            binString += " SignalRegion"
            processString += ' WR_%s_NR_%s'%(WRmass,NRmass)
            processNumberString += ' 0'
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            jerString += ' %.3f'%jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            puString += ' %.3f'%puErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muHiPTidString += '\t%.3f'%muHiPTidErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
	    muLooseIdString += '\t%.3f'%muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)]

    	sampleNumber = 0
    	for bkg in bkgs:
#	    print "%s%(bkg): ", '%s'%(bkg)
#	    print "histoDict['%s'%(bkg)].Integral(): ", histoDict['%s'%(bkg)].Integral()
            if histoDict['%s'%(bkg)].Integral() <= 0.: continue
	    sampleNumber += 1
    	    binString += " SignalRegion"
    	    processString += ' %s'%(bkg)
    	    processNumberString += ' %s'%(sampleNumber)
    	    rateString += ' -1'
    	    lumiString += ' 1.025'
    	    jecString += ' %.3f'%jecErrs['%s'%(bkg)]
    	    jerString += ' %.3f'%jerErrs['%s'%(bkg)]
    	    puString += ' %.3f'%puErrs['%s'%(bkg)]
    	    muHiPTidString += '\t%.3f'%muHiPTidErrs['%s'%(bkg)]
	    muLooseIdString += '\t%.3f'%muLooseIdErrs['%s'%(bkg)]

	outputFile = workspaceDirectory+"WR_%s_NR_%s_SignalRegion.txt"%(WRmass,NRmass)
	print "processNumberString: ", processNumberString
    	dctmp = open(outputFile,'w')
    	binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
    	jecString+='\n'; jerString+='\n'; puString+='\n'; muHiPTidString+='\n'; muLooseIdString+='\n';

    	datacard+=binString+processString+processNumberString+rateString+divider

    	datacard+=lumiString+jecString+jerString+puString+muHiPTidString+muLooseIdString
        datacard+='topNorm rateParam SignalRegion TT 1\n' + \
        'topNorm rateParam SignalRegion ST 1\n' + \
	'DYNorm rateParam SignalRegion DY 1\n' #+ \
        '* autoMCStats 0\n'

    	dctmp.write(datacard)

def ZPeakCard(workspaceDirectory):

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_ZPeak.root'

    tfile = r.TFile.Open(workspace)

#    bkgs = ['TT','ST','WJets','DY','DiBoson']
    bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']

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
    muHiPTidErrs = {}
    muLooseIdErrs = {}

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

        muHiPTidErrs['%s'%(bkg)] = 1.0
        muLooseIdErrs['%s'%(bkg)] = 1.0

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

        muHiPTidErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0
        muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]

        divider = '------------------------------------------------------------\n'
        datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
            'shapes * ZPeak WR_NR_MuMuJJ_ZPeak.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
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
        muHiPTidString = 'MuHPt\tshape'
        muLooseIdString = 'MuL\tshape'

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
            muHiPTidString += '\t%.3f'%muHiPTidErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muLooseIdString += '\t%.3f'%muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)]

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
            muHiPTidString += '\t%.3f'%muHiPTidErrs['%s'%(bkg)]
            muLooseIdString += '\t%.3f'%muLooseIdErrs['%s'%(bkg)]

        outputFile = workspaceDirectory+"WR_%s_NR_%s_ZPeak.txt"%(WRmass,NRmass)
        dctmp = open(outputFile,'w')
        binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
        jecString+='\n'; jerString+='\n'; puString+='\n'; muHiPTidString+='\n'; muLooseIdString+='\n';

        datacard+=binString+processString+processNumberString+rateString+divider

        datacard+=lumiString+jecString+jerString+puString+muHiPTidString+muLooseIdString
        datacard+='DYNorm rateParam ZPeak DY 1\n' #+ \
        '* autoMCStats 0\n'

        dctmp.write(datacard)

def FSBCarb(workspaceDirectory):

    workspace = workspaceDirectory+'WR_NR_MuMuJJ_FSB.root'

    tfile = r.TFile.Open(workspace)

    bkgs = ['TT','ST','WJets','DY','DiBoson','QCD']
    systs = ['JEC','JER','PU']
    sigs = ['1000,100','1500,100','2000,100','2500,100','3000,100','3500,100','4000,100','4500,100','5000,100','5500,100','1000,300','1500,300','2000,300','2500,300','3000,300','3500,300','4000,300','4500,300','5000,300','5500,300','2000,500','2500,500','3000,500','3500,500','4000,500','4500,500','5000,500','5500,500','2000,700','2500,700','3000,700','3500,700','4000,700','4500,700','5000,700','5500,700','3000,900','3500,900','4000,900','4500,900','5000,900','5500,900','3500,1100','4000,1100','4500,1100','5000,1100','5500,1100','4500,1300','5000,1300','5500,1300','4500,1500','5000,1500','5500,1500','5500,1700','5500,1900','5000,1000','4500,450','4000,800','4000,400','3500,350','3000,600','2500,250','2000,400','2000,200','1500,150','1000,200']

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
    muLooseIdErrs = {}

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
        muLooseIdErrs['%s'%(bkg)] = 1.0

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
        muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)] = 1.0

    for sig in sigs:
        WRmass = sig.split(',')[0]
        NRmass = sig.split(',')[1]

        divider = '------------------------------------------------------------\n'
        datacard = 'imax 1 number of channels\n' + \
            'jmax * number of processes minus 1\n' + \
            'kmax * number of nuisance parameters\n' + \
            divider + \
            'shapes * FSB WR_NR_MuMuJJ_FSB.root MuMuJJ/$PROCESS MuMuJJ/$PROCESS_$SYSTEMATIC\n' + \
            divider + \
            'bin             FSB\n' + \
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
        muLooseIdString = 'MuL\tshape'

        print "WR_%s_NR_%s%(WRmass,NRmass): ", 'WR_%s_NR_%s'%(WRmass,NRmass)
        print "histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral(): ", histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral()
        if histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Integral() > 0:
            binString += " FSB"
            processString += ' WR_%s_NR_%s'%(WRmass,NRmass)
            processNumberString += ' 0'
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            jerString += ' %.3f'%jerErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            puString += ' %.3f'%puErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            HEEPString += '\t%.3f'%HEEPErrs['WR_%s_NR_%s'%(WRmass,NRmass)]
            muLooseIdString += '\t%.3f'%muLooseIdErrs['WR_%s_NR_%s'%(WRmass,NRmass)]

        sampleNumber = 0
        for bkg in bkgs:
            print "%s%(bkg): ", '%s'%(bkg)
            print "histoDict['%s'%(bkg)].Integral(): ", histoDict['%s'%(bkg)].Integral()
            if histoDict['%s'%(bkg)].Integral() <= 0.: continue
            sampleNumber += 1
            binString += " FSB"
            processString += ' %s'%(bkg)
            processNumberString += ' %s'%(sampleNumber)
            rateString += ' -1'
            lumiString += ' 1.025'
            jecString += ' %.3f'%jecErrs['%s'%(bkg)]
            jerString += ' %.3f'%jerErrs['%s'%(bkg)]
            puString += ' %.3f'%puErrs['%s'%(bkg)]
            HEEPString += '\t%.3f'%HEEPErrs['%s'%(bkg)]
            muLooseIdString += '\t%.3f'%muLooseIdErrs['%s'%(bkg)]

        outputFile = workspaceDirectory+"WR_%s_NR_%s_FSB.txt"%(WRmass,NRmass)
        dctmp = open(outputFile,'w')
        binString+='\n'; processString+='\n'; processNumberString+='\n'; rateString +='\n'; lumiString+='\n';
        jecString+='\n'; jerString+='\n'; puString+='\n'; HEEPString+='\n'; muLooseIdString+='\n';

        datacard+=binString+processString+processNumberString+rateString+divider

        datacard+=lumiString+jecString+jerString+puString+HEEPString+muLooseIdString
	datacard+='topNorm rateParam FSB TT 1\n' + \
	'topNorm rateParam FSB ST 1\n' #+ \
	'* autoMCStats 0\n'

        dctmp.write(datacard)

##########################################################################

if len(sys.argv) == 2 and (sys.argv[1] == "help" or sys.argv[1] == "h"):
    print "=========="

workspaceDirectory = sys.argv[1]

SignalRegionCard(workspaceDirectory)

ZPeakCard(workspaceDirectory)

FSBCarb(workspaceDirectory)

