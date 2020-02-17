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

binBoundaries = []
#binBoundaries = [800, 1200, 1600, 2000, 8000]
binBoundaries = [800, 1000, 1200, 1500, 1800, 8000]
#binBoundaries = [800, 1000, 1200, 1500, 1800, 2100, 8000]
#binBoundaries = [800, 1200, 1600, 2000, 2400, 8000]
#binBoundaries = [800, 1000, 1200, 1500, 1800, 2100, 2400, 3000,4000,8000]
#binBoundaries = [800, 1000, 1200, 1400, 1700, 2000]
#steps = 400
#for ibin in range(2400, 8001,steps):
#    binBoundaries.append(ibin)

#steps = 400
#for ibin in range(800, 8001,steps):
#    binBoundaries.append(ibin)
#print binBoundaries
#    binBoundaries = [800, 1000, 1200, 1600, 2000, 8000]
#    binBoundaries = [200, 300, 400, 500, 600, 700, 800, 900, 1000, 1200, 1400, 1600, 1800, 2000, 6000]
binBoundariesArray = array.array('d', binBoundaries)
Nbins = len(binBoundaries) - 1

def SignalRegionWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights, numberOfEvents, ZPeakSF,doSignal, normToOne, year):
    print "ZPeakSF: ", ZPeakSF
    histoDict = {}

    LSFSF = 0.87
    LSFSFUp = 0.95
    LSFSFDown = 0.8


#    ttbarSF = 1.51
    if '2016' in sampleList:
	ttbarSF = 1.307
    elif '2017' in sampleList:
        ttbarSF = 1.310
    elif '2018' in sampleList:
        ttbarSF = 1.267

    for sample in sampleNames:
	print "sample: ", sample
	tfile = r.TFile.Open(samplesLocation+sample+'.root')
	bkgs = ['TT','ST','WJets','DY','DiBoson','TriBoson']
        systs = ['JEC','JER','PU','MuID','MuResol','MuTrig']
	systs_pdf = ['Scale_muR2_muF1','Scale_muRp5_muF1','Scale_muR1_muF2','Scale_muR2_muF2','Scale_muR1_muFp5','Scale_muRp5_muFp5','Error_1047','Error_1048','Error_1049','Error_1050','Error_1051','Error_1052','Error_1053','Error_1054','Error_1055','Error_1056','Error_1057','Error_1058','Error_1059','Error_1060','Error_1061','Error_1062','Error_1063','Error_1064','Error_1065','Error_1066','Error_1067','Error_1068','Error_1069','Error_1070','Error_1071','Error_1072','Error_1073','Error_1074','Error_1075','Error_1076','Error_1077','Error_1078','Error_1079','Error_1080','Error_1081','Error_1082','Error_1083','Error_1084','Error_1085','Error_1086','Error_1087','Error_1088','Error_1089','Error_1090','Error_1091','Error_1092','Error_1093','Error_1094','Error_1095','Error_1096','Error_1097','Error_1098','Error_1099','Error_1100','Error_1101','Error_1102','Error_1103','Error_1104','Error_1105','Error_1106','Error_1107','Error_1108','Error_1109','Error_1110','Error_1111','Error_1112','Error_1113','Error_1114','Error_1115','Error_1116','Error_1117','Error_1118','Error_1119','Error_1120','Error_1121','Error_1122','Error_1123','Error_1124','Error_1125','Error_1126','Error_1127','Error_1128','Error_1129','Error_1130','Error_1131','Error_1132','Error_1133','Error_1134','Error_1135','Error_1136','Error_1137','Error_1138','Error_1139','Error_1140','Error_1141','Error_1142','Error_1143','Error_1144','Error_1145','Error_1146','Error_alphasUp','Error_alphasDown']

        if ('WW_Tune' in sample or 'WZ_Tune' in sample or 'ZZ_Tune' in sample) and not 'WWZ_Tune' in sample and not 'ZZZ_Tune' in sample and not 'WZZ_Tune' in sample:
	    print "Diboson samples: ", sample

	    if 'WW_Tune' in sample:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
		histoDict['DiBoson'] = temp.Rebin(Nbins, 'DiBoson', binBoundariesArray)
		histoDict['DiBoson'].SetDirectory(0)
            	histoDict['DiBoson'].Scale(weights[sample])
		for syst in systs:
		    print "syst: ", syst
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['DiBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%s_%sUp'%(syst,year), binBoundariesArray)
	            histoDict['DiBoson_%sUp'%(syst)].SetDirectory(0)
	            histoDict['DiBoson_%sUp'%(syst)].Scale(weights[sample])
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['DiBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DiBoson_%sDown'%(syst)].Scale(weights[sample])

                histoDict['DiBoson_LSFUp'] = histoDict['DiBoson'].Clone("DiBoson_LSF_%sUp"%(year))
		histoDict['DiBoson_LSFUp'].SetDirectory(0)

                histoDict['DiBoson_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['DiBoson_LSFDown'] = histoDict['DiBoson'].Clone("DiBoson_LSF_%sDown"%(year))
                histoDict['DiBoson_LSFDown'].SetDirectory(0)
                histoDict['DiBoson_LSFDown'].Scale(LSFSFDown/LSFSF)

	    else:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
		histoDict['DiBoson'].Add(temp,weights[sample])
		histoDict['DiBoson_LSFUp'].Add(temp,weights[sample]*LSFSFUp/LSFSF)
                histoDict['DiBoson_LSFDown'].Add(temp,weights[sample]*LSFSFDown/LSFSF)

		for syst in systs:
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
		    histoDict['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                    histoDict['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'WWW_4F_Tune' in sample or 'WWZ_Tune' in sample or 'WZZ_Tune' in sample or 'ZZZ_Tune' in sample or 'WWZ_4F_Tune' in sample:
	    print "Triboson sample: ", sample
	    if 'WWW_4F_Tune' in sample:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
                histoDict['TriBoson'] = temp.Rebin(Nbins, 'TriBoson', binBoundariesArray)
                histoDict['TriBoson'].SetDirectory(0)
                histoDict['TriBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['TriBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%s_%sUp'%(syst,year), binBoundariesArray)
                    histoDict['TriBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDict['TriBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['TriBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['TriBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDict['TriBoson_%sDown'%(syst)].Scale(weights[sample])

                histoDict['TriBoson_LSFUp'] = histoDict['TriBoson'].Clone("TriBoson_LSF_%sUp"%(year))
                histoDict['TriBoson_LSFUp'].SetDirectory(0)
                histoDict['TriBoson_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['TriBoson_LSFDown'] = histoDict['TriBoson'].Clone("TriBoson_LSF_%sDown"%(year))
                histoDict['TriBoson_LSFDown'].SetDirectory(0)
                histoDict['TriBoson_LSFDown'].Scale(LSFSFDown/LSFSF)

            else:
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                histoDict['TriBoson'].Add(temp,weights[sample])
                histoDict['TriBoson_LSFUp'].Add(temp,weights[sample]*LSFSFUp/LSFSF)
                histoDict['TriBoson_LSFDown'].Add(temp,weights[sample]*LSFSFDown/LSFSF)
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDict['TriBoson_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDict['TriBoson_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'TTWJetsToLNu' in sample or 'ttWJets' in sample or 'TTWJetsToQQ' in sample or 'ttZJets' in sample:
            if 'TTWJetsToLNu' in sample or 'ttWJets' in sample:
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
                histoDict['ttV'] = temp.Rebin(Nbins, 'ttV', binBoundariesArray)
                histoDict['ttV'].SetDirectory(0)
                histoDict['ttV'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
                    histoDict['ttV_%sUp'%(syst)] = temp.Rebin(Nbins, 'ttV_%s_%sUp'%(syst,year), binBoundariesArray)
                    histoDict['ttV_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ttV_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
                    histoDict['ttV_%sDown'%(syst)] = temp.Rebin(Nbins, 'ttV_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['ttV_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ttV_%sDown'%(syst)].Scale(weights[sample])

                histoDict['ttV_LSFUp'] = histoDict['ttV'].Clone("ttV_LSF_%sUp"%(year))
                histoDict['ttV_LSFUp'].SetDirectory(0)
                histoDict['ttV_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['ttV_LSFDown'] = histoDict['ttV'].Clone("ttV_LSF_%sDown"%(year))
                histoDict['ttV_LSFDown'].SetDirectory(0)
                histoDict['ttV_LSFDown'].Scale(LSFSFDown/LSFSF)
            else:
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                histoDict['ttV'].Add(temp,weights[sample])
                histoDict['ttV_LSFUp'].Add(temp,weights[sample]*LSFSFUp/LSFSF)
                histoDict['ttV_LSFDown'].Add(temp,weights[sample]*LSFSFDown/LSFSF)
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDict['ttV_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDict['ttV_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'ST_s-channel' in sample or 'ST_t-channel_antitop' in sample or 'ST_t-channel_top' in sample or 'ST_tW_antitop' in sample or 'ST_tW_top' in sample:
	    if 'ST_s-channel' in sample:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
		histoDict['ST'] = temp.Rebin(Nbins, 'ST', binBoundariesArray)
                histoDict['ST'].SetDirectory(0)
                histoDict['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['ST_%sUp'%(syst)] = temp.Rebin(Nbins, 'ST_%s_%sUp'%(syst,year), binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDict['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['ST_%sDown'%(syst)] = temp.Rebin(Nbins, 'ST_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDict['ST_%sDown'%(syst)].Scale(weights[sample])

                histoDict['ST_LSFUp'] = histoDict['ST'].Clone("ST_LSF_%sUp"%(year))
                histoDict['ST_LSFUp'].SetDirectory(0)
                histoDict['ST_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['ST_LSFDown'] = histoDict['ST'].Clone("ST_LSF_%sDown"%(year))
                histoDict['ST_LSFDown'].SetDirectory(0)
                histoDict['ST_LSFDown'].Scale(LSFSFDown/LSFSF)

            else:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'ST_temp', binBoundariesArray)
                histoDict['ST'].Add(temp,weights[sample])
                histoDict['ST_LSFUp'].Add(temp,weights[sample]*LSFSFUp/LSFSF)
                histoDict['ST_LSFDown'].Add(temp,weights[sample]*LSFSFDown/LSFSF)
                for syst in systs:
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDict['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample and not 'TTWJets' in sample  and not 'ttWJets' in sample:
		temp =  tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
		histoDict['WJets'] = temp.Rebin(Nbins, 'WJets', binBoundariesArray)
                histoDict['WJets'].SetDirectory(0)
                histoDict['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['WJets_%sUp'%(syst)] = temp.Rebin(Nbins, 'WJets_%s_%sUp'%(syst,year), binBoundariesArray)
                    histoDict['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['WJets_%sDown'%(syst)] = temp.Rebin(Nbins, 'WJets_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDict['WJets_%sDown'%(syst)].Scale(weights[sample])

                histoDict['WJets_LSFUp'] = histoDict['WJets'].Clone("WJets_LSF_%sUp"%(year))
                histoDict['WJets_LSFUp'].SetDirectory(0)
                histoDict['WJets_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['WJets_LSFDown'] = histoDict['WJets'].Clone("WJets_LSF_%sDown"%(year))
                histoDict['WJets_LSFDown'].SetDirectory(0)
                histoDict['WJets_LSFDown'].Scale(LSFSFDown/LSFSF)

        elif 'DY' in sample:
            if 'DYJetsToLL_M-50_HT-100to200' in sample:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
		histoDict['DY'] = temp.Rebin(Nbins, 'DY', binBoundariesArray)
                histoDict['DY'].SetDirectory(0)
                histoDict['DY'].Scale(weights[sample]*ZPeakSF)
                for syst in systs:
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
		    histoDict['DY_%sUp'%(syst)] = temp.Rebin(Nbins, 'DY_%s_%sUp'%(syst,year), binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDict['DY_%sUp'%(syst)].Scale(weights[sample]*ZPeakSF)
                    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
		    histoDict['DY_%sDown'%(syst)] = temp.Rebin(Nbins, 'DY_%s_%sDown'%(syst,year), binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDict['DY_%sDown'%(syst)].Scale(weights[sample]*ZPeakSF)

                histoDict['DY_LSFUp'] = histoDict['DY'].Clone("DY_LSF_%sUp"%(year))
                histoDict['DY_LSFUp'].SetDirectory(0)
                histoDict['DY_LSFUp'].Scale(LSFSFUp/LSFSF)
                histoDict['DY_LSFDown'] = histoDict['DY'].Clone("DY_LSF_%sDown"%(year))
                histoDict['DY_LSFDown'].SetDirectory(0)
                histoDict['DY_LSFDown'].Scale(LSFSFDown/LSFSF)
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_ZweightUp')
		histoDict['DY_ZweightUp'] = temp.Rebin(Nbins, 'DY_Zweight_%sUp'%(year), binBoundariesArray)
                histoDict['DY_ZweightUp'].SetDirectory(0)
                histoDict['DY_ZweightUp'].Scale(weights[sample]*ZPeakSF)
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_ZweightDown')
                histoDict['DY_ZweightDown'] = temp.Rebin(Nbins, 'DY_Zweight_%sDown'%(year), binBoundariesArray)
                histoDict['DY_ZweightDown'].SetDirectory(0)
                histoDict['DY_ZweightDown'].Scale(weights[sample]*ZPeakSF)

            else:
		temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'DY_temp', binBoundariesArray)
                histoDict['DY'].Add(temp,weights[sample]*ZPeakSF)
                histoDict['DY_LSFUp'].Add(temp,weights[sample]*LSFSFUp/LSFSF)
                histoDict['DY_LSFDown'].Add(temp,weights[sample]*LSFSFDown/LSFSF)
                for syst in systs:
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sUp'%(syst)].Add(temp,weights[sample]*ZPeakSF)
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDict['DY_%sDown'%(syst)].Add(temp,weights[sample]*ZPeakSF)
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_ZweightUp').Rebin(Nbins, 'DY_temp', binBoundariesArray)
		histoDict['DY_ZweightUp'].Add(temp,weights[sample]*ZPeakSF)
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_ZweightDown').Rebin(Nbins, 'DY_temp', binBoundariesArray)
                histoDict['DY_ZweightDown'].Add(temp,weights[sample]*ZPeakSF)


	elif 'WR' in sample:
	    wrMass = sample.split('_')[1][2:]
	    nuMass = sample.split('_')[2][1:]
            temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
            if(normToOne):
                norm = 1.0/float(temp.Integral(1, temp.GetNbinsX()))
            else:
                norm = 1.0
            weightScale = norm*weights[sample]
	    histoDict['WR_%s_NR_%s'%(wrMass,nuMass)] = temp.Rebin(Nbins, 'WR_%s_NR_%s'%(wrMass,nuMass), binBoundariesArray)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Scale(weightScale)
	    temp2 = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
	    histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)] = temp2.Rebin(Nbins, 'WR_%s_NR_%s_Numerator'%(wrMass,nuMass), binBoundariesArray)
	    histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].SetDirectory(0)
	    histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].Scale(1/numberOfEvents[sample])
	    print "Nominal Numerator: ", histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].Integral()
	    print "Nominal Weights: ", tfile.Get('analysis/allEvents/m_Scale_muR1_muF1').GetBinContent(1)

	    SignalDenominators = {}
	    SignalDenominators['WR_%s_NR_%s'%(wrMass,nuMass)] = tfile.Get('analysis/allEvents/m_Scale_muR1_muF1').GetBinContent(1)/numberOfEvents[sample]
            for syst in systs:
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sUp'%(syst))
		histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)] = temp.Rebin(Nbins, 'WR_%s_NR_%s_%s_%sUp'%(wrMass,nuMass,syst,year), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sUp'%(wrMass,nuMass,syst)].Scale(weightScale)
                temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%sDown'%(syst))
		histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)] = temp.Rebin(Nbins, 'WR_%s_NR_%s_%s_%sDown'%(wrMass,nuMass,syst,year), binBoundariesArray)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].SetDirectory(0)
                histoDict['WR_%s_NR_%s_%sDown'%(wrMass,nuMass,syst)].Scale(weightScale)

            histoDict['WR_%s_NR_%s_LSFUp'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_LSF_%sUp'%(wrMass,nuMass,year))
            histoDict['WR_%s_NR_%s_LSFUp'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s_LSFUp'%(wrMass,nuMass)].Scale(LSFSFUp/LSFSF)
            histoDict['WR_%s_NR_%s_LSFDown'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_LSF_%sDown'%(wrMass,nuMass,year))
            histoDict['WR_%s_NR_%s_LSFDown'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s_LSFDown'%(wrMass,nuMass)].Scale(LSFSFDown/LSFSF)
	    for syst in systs_pdf:

		if "alphas" in syst:
		    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%s'%(syst[6:]))
		else:
	    	    temp = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass_%s'%(syst))
		eventsWeight = tfile.Get('analysis/allEvents/m_%s'%(syst)).GetBinContent(1)
                print "syst: ", syst
                print "temp.Integral(): ", temp.Integral()
                print "eventsWeight: ", eventsWeight
                print "numberOfEvents: ", numberOfEvents[sample]
		print "Denominator: ", eventsWeight/numberOfEvents[sample]
		SignalDenominators['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)] =  eventsWeight/numberOfEvents[sample]
	    	histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)] = temp.Rebin(Nbins, 'WR_%s_NR_%s_%s'%(wrMass,nuMass,syst), binBoundariesArray)
		histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].SetDirectory(0)
		if "Scale" in syst:
		    histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(1/numberOfEvents[sample])
		    histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(1/SignalDenominators['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)])
#                    histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(weights[sample])
		else:
                    histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(1/numberOfEvents[sample])
#                histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(weights[sample])
		print "Numerator: ", histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Integral()
#		histoDict['WR_%s_NR_%s_%s'%(wrMass,nuMass,syst)].Scale(weights[sample]*numberOfEvents[sample]/eventsWeight)

	    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_PDFUp'%(wrMass,nuMass))
	    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetDirectory(0)
	    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].Reset()
            histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_PDFDown'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].Reset()

	    SquaredBinError = []
	    SquaredBinError.append(0.0)
	    for j in range(0,histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetNbinsX()+1):
		SquaredBinError.append(0.0)
	    for i in range(1047, 1146+1):
		for j in range(1,histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetNbinsX()+1):
		    if histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].Integral() == 0: continue
#                    if abs(histoDict['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)].Integral() - histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].Integral()) / histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].Integral() < 0.25:
#		    print "bin j: ", j
#		    print "Error: ", i
		    SquaredBinError[j] += (histoDict['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)].GetBinContent(j)/SignalDenominators['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)] - histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)/SignalDenominators['WR_%s_NR_%s'%(wrMass,nuMass)])**2
#		    if j == 5:
#	                print "bin j: ", j
#			print "Error: ", i
#		        print "SquaredBinError: ", (histoDict['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)].GetBinContent(j)/SignalDenominators['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)] - histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)/SignalDenominators['WR_%s_NR_%s'%(wrMass,nuMass)])**2
#			print "SignalDenominators['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)]: ", SignalDenominators['WR_%s_NR_%s_Error_%s'%(wrMass,nuMass,i)]
#			print "SignalDenominators['WR_%s_NR_%s'%(wrMass,nuMass)]: ", SignalDenominators['WR_%s_NR_%s'%(wrMass,nuMass)]
	    for j in range(1,histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetNbinsX()+1):
		print "Setting PDF error bin: ", j
		print "histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j): ", histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)
		print "math.sqrt(SquaredBinError[j]): ", math.sqrt(SquaredBinError[j])
		print "histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j): ", histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)
		if histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j) == 0:
		    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetBinContent(j,0)
		    histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetBinContent(j,0)
#		elif math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j) < 1:
                elif math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j) > 1:
                    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetBinContent(j, histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 +  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)))
                    histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetBinContent(j,0.000001)
                elif histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 -  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)) < 0:
                    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetBinContent(j, histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 +  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)))
                    histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetBinContent(j,0.000001)
		else:
                    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetBinContent(j, histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 +  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)))
                    histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetBinContent(j, histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 -  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)))
#		else:
#		    histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].SetBinContent(j, histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*(1 +  math.sqrt(SquaredBinError[j])/histoDict['WR_%s_NR_%s_Numerator'%(wrMass,nuMass)].GetBinContent(j)))
#                    histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].SetBinContent(j,0)


	    histoDict['WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass)].Reset()
            histoDict['WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass)] = histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].Clone('WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass))
            histoDict['WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass)].SetDirectory(0)
            histoDict['WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass)].Reset()

	    AlphaBinError = []
            AlphaBinError.append(0.0)
	    AlphaBinError[0] = (0.75*.5*(histoDict['WR_%s_NR_%s_Error_alphasUp'%(wrMass,nuMass)].Integral()/SignalDenominators['WR_%s_NR_%s_Error_alphasUp'%(wrMass,nuMass)] - histoDict['WR_%s_NR_%s_Error_alphasDown'%(wrMass,nuMass)].Integral()/SignalDenominators['WR_%s_NR_%s_Error_alphasDown'%(wrMass,nuMass)]))
	    for j in range(1,histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetNbinsX()+1):
		histoDict['WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass)].SetBinContent(j, (AlphaBinError[0]*histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j) + histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)))
                histoDict['WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass)].SetBinContent(j, (histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j) - histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetBinContent(j)*AlphaBinError[0]))

	    if(normToOne):
		histoDict['WR_%s_NR_%s_AlphaSUp'%(wrMass,nuMass)].Scale(norm)
		histoDict['WR_%s_NR_%s_AlphaSDown'%(wrMass,nuMass)].Scale(norm)
		histoDict['WR_%s_NR_%s_PDFUp'%(wrMass,nuMass)].Scale(norm)
		histoDict['WR_%s_NR_%s_PDFDown'%(wrMass,nuMass)].Scale(norm)
	

        elif 'SingleMuon' in sample:
            if 'SingleMuon--Run2016B' in sample or 'SingleMuon--Run2017B' in sample or 'SingleMuon--Run2018A' in sample:
		temp2 = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass')
		histoDict['SRdata'] = temp2.Rebin(Nbins, 'data_obs', binBoundariesArray)
		histoDict['SRdata'].SetDirectory(0)
            else:
		temp2 = tfile.Get('analysis/eventsFailResPassBoostRECO/leadAK8JetMuonMass').Rebin(Nbins, 'data_obs_temp', binBoundariesArray)
		histoDict['SRdata'].Add(temp2)

	elif 'SingleElectron' in sample or 'EGamma' in sample:
	    if 'SingleElectron--Run2016B' in sample or 'SingleElectron--Run2017B' in sample or 'EGamma--Run2018A' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDict['data_obs'] = temp.Rebin(Nbins, 'EMu', binBoundariesArray)
                histoDict['data_obs'].SetDirectory(0)
            else:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'data_obs_temp', binBoundariesArray)
                histoDict['data_obs'].Add(temp)


	tfile.Close()

    ttbarEst = histoDict['data_obs'].Clone('EMu')
    print "FSB data Integral: ", ttbarEst.Integral()
#    ttbarEst.Scale(137400./35900.)

    print "data FSB: ", ttbarEst.Integral()
    print "DiBoson FSB: ", histoDictFSB['DiBoson'].Integral()
    print "ST FSB: ", histoDictFSB['ST'].Integral()
    print "WJets FSB: ", histoDictFSB['WJets'].Integral()
    print "DY FSB: ", histoDictFSB['DY'].Integral()
    print "TriBoson FSB: ", histoDictFSB['TriBoson'].Integral()
    print "ttV FSB: ", histoDictFSB['ttV'].Integral()

    print "data FSB: ", ttbarEst.GetBinError(3)
    print "DiBoson FSB: ", histoDictFSB['DiBoson'].GetBinError(3)
    print "ST FSB: ", histoDictFSB['ST'].GetBinError(3)
    print "WJets FSB: ", histoDictFSB['WJets'].GetBinError(3)
    print "DY FSB: ", histoDictFSB['DY'].GetBinError(3)
    print "TriBoson FSB: ", histoDictFSB['TriBoson'].GetBinError(3)
    print "ttV FSB: ", histoDictFSB['ttV'].GetBinError(3)

    ttbarEst.Add(histoDictFSB['DiBoson'],-1)
    ttbarEst.Add(histoDictFSB['ST'],-1)
    ttbarEst.Add(histoDictFSB['WJets'],-1)
    ttbarEst.Add(histoDictFSB['DY'],-1)
    ttbarEst.Add(histoDictFSB['TriBoson'],-1)
    ttbarEst.Add(histoDictFSB['ttV'],-1)
    print "data FSB: ", ttbarEst.GetBinContent(3)

    ttbarEst.Scale(ttbarSF)

    ttbarEst_SystUp = ttbarEst.Clone('EMu_Syst_%sUp'%(year))
    ttbarEst_SystDown = ttbarEst.Clone('EMu_Syst_%sDown'%(year))

    ttbarEst_SystUp.Scale(1.2)
    ttbarEst_SystDown.Scale(0.8)

    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_BoostedSignalRegion_400GeVBins.root'    
    outputWorkspace = r.TFile(outputFile, "RECREATE")

    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
    MuMuJJ.cd()

    print "SR DY Integral: ", histoDict['DY'].Integral()

#    histoDict['TT'].Write()
    histoDict['DiBoson'].Write()
    histoDict['TriBoson'].Write()
    histoDict['ttV'].Write()
    histoDict['ST'].Write()
    histoDict['WJets'].Write()
    histoDict['DY'].Write()
    histoDict['SRdata'].Write()
    ttbarEst.Write()
    ttbarEst_SystUp.Write()
    ttbarEst_SystDown.Write()
#    histoDict['QCD'].Write()
#    histoDict['data_obs'].Write()
    for syst in systs:
#	histoDict['TT_%sUp'%(syst)].Write()
#        histoDict['TT_%sDown'%(syst)].Write()
        histoDict['TriBoson_%sUp'%(syst)].Write()
        histoDict['TriBoson_%sDown'%(syst)].Write()
	histoDict['DiBoson_%sUp'%(syst)].Write()
        histoDict['DiBoson_%sDown'%(syst)].Write()
        histoDict['ttV_%sUp'%(syst)].Write()
        histoDict['ttV_%sDown'%(syst)].Write()
        histoDict['ST_%sUp'%(syst)].Write()
        histoDict['ST_%sDown'%(syst)].Write()
        histoDict['WJets_%sUp'%(syst)].Write()
        histoDict['WJets_%sDown'%(syst)].Write()	
        histoDict['DY_%sUp'%(syst)].Write()
        histoDict['DY_%sDown'%(syst)].Write()
#        histoDict['QCD_%sUp'%(syst)].Write()
#        histoDict['QCD_%sDown'%(syst)].Write()
    histoDict['TriBoson_LSFUp'].Write()
    histoDict['TriBoson_LSFDown'].Write()
    histoDict['DiBoson_LSFUp'].Write()
    histoDict['DiBoson_LSFDown'].Write()
    histoDict['ttV_LSFUp'].Write()
    histoDict['ttV_LSFDown'].Write()
    histoDict['ST_LSFUp'].Write()
    histoDict['ST_LSFDown'].Write()
    histoDict['WJets_LSFUp'].Write()
    histoDict['WJets_LSFDown'].Write()
    histoDict['DY_LSFUp'].Write()
    histoDict['DY_LSFDown'].Write()
    histoDict['DY_ZweightUp'].Write()
    histoDict['DY_ZweightDown'].Write()

#    signalMasses = [(1000,100),(1500,100),(2000,100),(2500,100),(3000,100),(3500,100),(4000,100),(4500,100),(5000,100),(5500,100),(1000,300),(1500,300),(2000,300),(2500,300),(3000,300),(3500,300),(4000,300),(4500,300),(5000,300),(5500,300),(2000,500),(2500,500),(3000,500),(3500,500),(4000,500),(4500,500),(5000,500),(5500,500),(2000,700),(2500,700),(3000,700),(3500,700),(4000,700),(4500,700),(5000,700),(5500,700),(3000,900),(3500,900),(4000,900),(4500,900),(5000,900),(5500,900),(3500,1100),(4000,1100),(4500,1100),(5000,1100),(5500,1100),(4500,1300),(5000,1300),(5500,1300),(4500,1500),(5000,1500),(5500,1500),(5500,1700),(5500,1900),(5000,1000),(4500,450),(4000,800),(4000,400),(3500,350),(3000,600),(2500,250),(2000,400),(2000,200),(1500,150),(1000,200)] 

#    signalMasses = [(2000,1000),(3000,1500),(4000,2000)]
    if(doSignal):
        signalMasses = []

        signalMassFile = 'signalMassPoints.txt'

        with open(signalMassFile) as f:
            lines = f.read().splitlines()

        for line in lines:
            line = line.split(',')
            signalMasses.append(line)

        for WRmass,NRmass in signalMasses:
#            for j in range(1,histoDict['WR_%s_NR_%s'%(wrMass,nuMass)].GetNbinsX()+1):
#		if (histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].GetBinContent(j) < 0):
#		    histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].SetBinContent(j,0.)
#	    for i in range(1,5):
#		histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].SetBinContent(i,histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].GetBinContent(i)/2.)
#                histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].SetBinError(i,histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].GetBinError(i)/2.)
            histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_Numerator'%(WRmass,NRmass)].Write()
            for syst in systs:
#                for i in range(1,5):
#		    histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].SetBinContent(i,histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].GetBinContent(i)/2.)
#		    histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].SetBinError(i,histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].GetBinError(i)/2.)
#                    histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].SetBinContent(i,histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].GetBinContent(i)/2.)
#                    histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].SetBinError(i,histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].GetBinError(i)/2.)

                histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
                histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()
            for syst in systs_pdf:
                if 'Error' in syst: continue 
#           	if float(WRmass) != 5000.: continue
#               print "Saving: ", syst
                histoDict['WR_%s_NR_%s_%s'%(WRmass,NRmass,syst)].Write()

            histoDict['WR_%s_NR_%s_PDFUp'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_PDFDown'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_AlphaSUp'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_AlphaSDown'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_LSFUp'%(WRmass,NRmass)].Write()
            histoDict['WR_%s_NR_%s_LSFDown'%(WRmass,NRmass)].Write()

    
    outputWorkspace.Write()
    outputWorkspace.Close()

def ZPeakWorksapce(sampleNames,samplesLocation,workspaceOutputDirectory, weights):

#    histoDictZPeak = {}

    for sample in sampleNames:
        if "WRto" in sample: continue
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson']
        systs = ['JEC','JER','PU','MuID','MuResol','MuTrig']

	if 'TTTo' in sample:
          if 'TTToSemi' in sample:

            temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
	    histoDictZPeak['TT'] = temp.Rebin(Nbins, 'TT', binBoundariesArray)
            histoDictZPeak['TT'].SetDirectory(0)
            histoDictZPeak['TT'].Scale(weights[sample])

            for syst in systs:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
		histoDictZPeak['TT_%sUp'%(syst)] = temp.Rebin(Nbins, 'TT_%sUp'%(syst), binBoundariesArray)
                histoDictZPeak['TT_%sUp'%(syst)].SetDirectory(0)
                histoDictZPeak['TT_%sUp'%(syst)].Scale(weights[sample])
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                histoDictZPeak['TT_%sDown'%(syst)] = temp.Rebin(Nbins, 'TT_%sDown'%(syst), binBoundariesArray)
                histoDictZPeak['TT_%sDown'%(syst)].SetDirectory(0)
                histoDictZPeak['TT_%sDown'%(syst)].Scale(weights[sample])
	  else:
            temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'TT', binBoundariesArray)
            histoDictZPeak['TT'].Add(temp,weights[sample])


        elif ('WW_Tune' in sample or 'WZ_Tune' in sample or 'ZZ_Tune' in sample) and not 'WWZ_Tune' in sample and not 'ZZZ_Tune' in sample and not 'WZZ_Tune' in sample:

	    if 'WW_Tune' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
		histoDictZPeak['DiBoson'] = temp.Rebin(Nbins, 'DiBoson', binBoundariesArray)
                histoDictZPeak['DiBoson'].SetDirectory(0)
                histoDictZPeak['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
		    histoDictZPeak['DiBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
		    histoDictZPeak['DiBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
		temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                histoDictZPeak['DiBoson'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                    histoDictZPeak['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])	
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                    histoDictZPeak['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'ST_s-channel' in sample or 'ST_t-channel_antitop' in sample or 'ST_t-channel_top' in sample or 'ST_tW_antitop' in sample or 'ST_tW_top' in sample:
	    if 'ST_s-channel' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['ST'] = temp.Rebin(Nbins, 'ST', binBoundariesArray)
                histoDictZPeak['ST'].SetDirectory(0)
                histoDictZPeak['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDictZPeak['ST_%sUp'%(syst)] = temp.Rebin(Nbins, 'ST_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
		    histoDictZPeak['ST_%sDown'%(syst)] = temp.Rebin(Nbins, 'ST_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'ST_temp', binBoundariesArray)
                histoDictZPeak['ST'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDictZPeak['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDictZPeak['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample and not 'TTWJets' in sample and not 'ttWJets' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['WJets'] = temp.Rebin(Nbins, 'WJets', binBoundariesArray)
                histoDictZPeak['WJets'].SetDirectory(0)
                histoDictZPeak['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDictZPeak['WJets_%sUp'%(syst)] = temp.Rebin(Nbins, 'WJets_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDictZPeak['WJets_%sDown'%(syst)] = temp.Rebin(Nbins, 'WJets_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['WJets_%sDown'%(syst)].Scale(weights[sample])

        elif 'DY' in sample:
	    print "sample: ", sample
	    print "DY scale: ", weights[sample]
            if 'DYJetsToLL_M-50_HT-100to200' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['DY'] = temp.Rebin(Nbins, 'DY', binBoundariesArray)
                histoDictZPeak['DY'].SetDirectory(0)
                histoDictZPeak['DY'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDictZPeak['DY_%sUp'%(syst)] = temp.Rebin(Nbins, 'DY_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['DY_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDictZPeak['DY_%sDown'%(syst)] = temp.Rebin(Nbins, 'DY_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'DY_temp', binBoundariesArray)
                histoDictZPeak['DY'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDictZPeak['DY_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDictZPeak['DY_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WWW_4F_Tune' in sample or 'WWZ_Tune' in sample or 'WZZ_Tune' in sample or 'ZZZ_Tune' in sample or 'WWZ_4F_Tune' in sample:
	    if 'WWW_4F_Tune' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['TriBoson'] = temp.Rebin(Nbins, 'TriBoson', binBoundariesArray)
                histoDictZPeak['TriBoson'].SetDirectory(0)
                histoDictZPeak['TriBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDictZPeak['TriBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['TriBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['TriBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDictZPeak['TriBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['TriBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['TriBoson_%sDown'%(syst)].Scale(weights[sample])
            else:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                histoDictZPeak['TriBoson'].Add(temp,weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDictZPeak['TriBoson_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDictZPeak['TriBoson_%sDown'%(syst)].Add(temp,weights[sample])
	elif 'TTWJetsToLNu' in sample or 'ttWJets' in sample or 'TTWJetsToQQ' in sample or 'ttZJets' in sample:
	    print "ttV sample: ", sample
            if 'TTWJetsToLNu' in sample or 'ttWJets' in sample:
	        print "sample: ", sample
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['ttV'] = temp.Rebin(Nbins, 'ttV', binBoundariesArray)
                histoDictZPeak['ttV'].SetDirectory(0)
                histoDictZPeak['ttV'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst))
                    histoDictZPeak['ttV_%sUp'%(syst)] = temp.Rebin(Nbins, 'ttV_%sUp'%(syst), binBoundariesArray)
                    histoDictZPeak['ttV_%sUp'%(syst)].SetDirectory(0)
                    histoDictZPeak['ttV_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst))
                    histoDictZPeak['ttV_%sDown'%(syst)] = temp.Rebin(Nbins, 'ttV_%sDown'%(syst), binBoundariesArray)
                    histoDictZPeak['ttV_%sDown'%(syst)].SetDirectory(0)
                    histoDictZPeak['ttV_%sDown'%(syst)].Scale(weights[sample])
            else:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                histoDictZPeak['ttV'].Add(temp,weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sUp'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDictZPeak['ttV_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF_%sDown'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDictZPeak['ttV_%sDown'%(syst)].Add(temp,weights[sample])


	elif 'SingleMuon' in sample:
            if 'SingleMuon--Run2016B' in sample or 'SingleMuon--Run2017B' in sample or 'SingleMuon--Run2018A' in sample:
                temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF')
                histoDictZPeak['data_obs'] = temp.Rebin(Nbins, 'data_obs', binBoundariesArray)
                histoDictZPeak['data_obs'].SetDirectory(0)
            else:
		temp = tfile.Get('analysis/eventsPassBoostZMASSRECO/leadAK8JetMuonMass_noLSF').Rebin(Nbins, 'data_obs_temp', binBoundariesArray)
                histoDictZPeak['data_obs'].Add(temp)

        tfile.Close()

    data_DY_ZPeak = histoDictZPeak['data_obs'].Clone()
#    data_DY_ZPeak.Scale(80000./35900.)
    print "data_DY_ZPeak.Integral(): ", data_DY_ZPeak.Integral()


    print "DiBoson: ", histoDictZPeak['DiBoson'].Integral()
    print "ST: ", histoDictZPeak['ST'].Integral()
    print "WJets: ", histoDictZPeak['WJets'].Integral()
    print "TriBoson: ", histoDictZPeak['TriBoson'].Integral()
    print "ttV: ", histoDictZPeak['ttV'].Integral()
    print "TT: ", histoDictZPeak['TT'].Integral()

    data_DY_ZPeak.Add(histoDictZPeak['DiBoson'],-1)
    data_DY_ZPeak.Add(histoDictZPeak['ST'],-1)
    data_DY_ZPeak.Add(histoDictZPeak['WJets'],-1)
    data_DY_ZPeak.Add(histoDictZPeak['TriBoson'],-1)
    data_DY_ZPeak.Add(histoDictZPeak['ttV'],-1)
    data_DY_ZPeak.Add(histoDictZPeak['TT'],-1)

    print "data_DY_ZPeak.Integral(): ", data_DY_ZPeak.Integral()
    print "DY_ZPeak.Integral(): ", histoDictZPeak['DY'].Integral()

    print "DY SF: ", data_DY_ZPeak.Integral()/histoDictZPeak['DY'].Integral()

    ZPeakSF_tmp = data_DY_ZPeak.Integral()/histoDictZPeak['DY'].Integral()
    print "ZPeakSF_tmp: ", ZPeakSF_tmp

    data_unc = 0.0
    DY_unc = 0.0
    for i in range(1,data_DY_ZPeak.GetNbinsX()):
	data_unc += data_DY_ZPeak.GetBinError(i)**2
        DY_unc += histoDictZPeak['DY'].GetBinError(i)**2

    data_unc = math.sqrt(data_unc)
    DY_unc = math.sqrt(DY_unc)
    print "data_unc: ", data_unc
    print "DY_unc: ", DY_unc

    ZPeakSFunc = ZPeakSF_tmp*math.sqrt((data_unc/data_DY_ZPeak.Integral())**2 + (DY_unc/histoDictZPeak['DY'].Integral())**2)

    print "ZPeakSFunc: ", ZPeakSFunc

#    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_ResolvedZPeak.root'
#    outputWorkspace = r.TFile(outputFile, "RECREATE")

#    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
#    MuMuJJ.cd()

#    histoDict['TT'].Write()
#    histoDict['DiBoson'].Write()
#    histoDict['ST'].Write()
#    histoDict['WJets'].Write()
#    histoDict['DY'].Write()
#    histoDict['QCD'].Write()
#    histoDict['data_obs'].Write()
#    for syst in systs:
#        histoDict['TT_%sUp'%(syst)].Write()
#        histoDict['TT_%sDown'%(syst)].Write()
#        histoDict['DiBoson_%sUp'%(syst)].Write()
#        histoDict['DiBoson_%sDown'%(syst)].Write()
#        histoDict['ST_%sUp'%(syst)].Write()
#        histoDict['ST_%sDown'%(syst)].Write()
#        histoDict['WJets_%sUp'%(syst)].Write()
#        histoDict['WJets_%sDown'%(syst)].Write()
#        histoDict['DY_%sUp'%(syst)].Write()
#        histoDict['DY_%sDown'%(syst)].Write()
#        histoDict['QCD_%sUp'%(syst)].Write()
#        histoDict['QCD_%sDown'%(syst)].Write()

#    signalMasses = [(1000,100),(1500,100),(2000,100),(2500,100),(3000,100),(3500,100),(4000,100),(4500,100),(5000,100),(5500,100),(1000,300),(1500,300),(2000,300),(2500,300),(3000,300),(3500,300),(4000,300),(4500,300),(5000,300),(5500,300),(2000,500),(2500,500),(3000,500),(3500,500),(4000,500),(4500,500),(5000,500),(5500,500),(2000,700),(2500,700),(3000,700),(3500,700),(4000,700),(4500,700),(5000,700),(5500,700),(3000,900),(3500,900),(4000,900),(4500,900),(5000,900),(5500,900),(3500,1100),(4000,1100),(4500,1100),(5000,1100),(5500,1100),(4500,1300),(5000,1300),(5500,1300),(4500,1500),(5000,1500),(5500,1500),(5500,1700),(5500,1900),(5000,1000),(4500,450),(4000,800),(4000,400),(3500,350),(3000,600),(2500,250),(2000,400),(2000,200),(1500,150),(1000,200)]
    signalMasses = [(2000,1000),(3000,1500),(4000,2000)]

#    for WRmass,NRmass in signalMasses:
#        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
#        for syst in systs:
#            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
#            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()

#    outputWorkspace.Write()
#    outputWorkspace.Close()
    return ZPeakSF_tmp

def FSBWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights):

#    histoDict = {}

    for sample in sampleNames:
        if "WRto" in sample: continue
        print "sample: ", sample
        tfile = r.TFile.Open(samplesLocation+sample+'.root')
        bkgs = ['TT','ST','WJets','DY','DiBoson']
        systs = ['JEC','JER','PU','MuL','HEEP','ElHLT']

        if ('WW_Tune' in sample or 'WZ_Tune' in sample or 'ZZ_Tune' in sample) and not 'WWZ_Tune' in sample and not 'ZZZ_Tune' in sample and not 'WZZ_Tune' in sample:

	    if 'WW_Tune' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['DiBoson'] = temp.Rebin(Nbins, 'DiBoson', binBoundariesArray)
                histoDictFSB['DiBoson'].SetDirectory(0)
                histoDictFSB['DiBoson'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['DiBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['DiBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['DiBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['DiBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'DiBoson_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['DiBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['DiBoson_%sDown'%(syst)].Scale(weights[sample])

            else:
		temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                histoDictFSB['DiBoson'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                    histoDictFSB['DiBoson_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(Nbins, 'DiBoson_temp', binBoundariesArray)
                    histoDictFSB['DiBoson_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WWW_4F_Tune' in sample or 'WWZ_Tune' in sample or 'WZZ_Tune' in sample or 'ZZZ_Tune' in sample or 'WWZ_4F_Tune' in sample:
	    if 'WWW_4F_Tune' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['TriBoson'] = temp.Rebin(Nbins, 'TriBoson', binBoundariesArray)
                histoDictFSB['TriBoson'].SetDirectory(0)
                histoDictFSB['TriBoson'].Scale(weights[sample])
                for syst in systs:
		    print "syst: ", syst
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['TriBoson_%sUp'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['TriBoson_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['TriBoson_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['TriBoson_%sDown'%(syst)] = temp.Rebin(Nbins, 'TriBoson_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['TriBoson_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['TriBoson_%sDown'%(syst)].Scale(weights[sample])
            else:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                histoDictFSB['TriBoson'].Add(temp,weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDictFSB['TriBoson_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(Nbins, 'TriBoson_temp', binBoundariesArray)
                    histoDictFSB['TriBoson_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'TTWJetsToLNu' in sample or 'ttWJets' in sample or 'TTWJetsToQQ' in sample or 'ttZJets' in sample:
            if 'TTWJetsToLNu' in sample or 'ttWJets' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['ttV'] = temp.Rebin(Nbins, 'ttV', binBoundariesArray)
                histoDictFSB['ttV'].SetDirectory(0)
                histoDictFSB['ttV'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['ttV_%sUp'%(syst)] = temp.Rebin(Nbins, 'ttV_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['ttV_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['ttV_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['ttV_%sDown'%(syst)] = temp.Rebin(Nbins, 'ttV_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['ttV_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['ttV_%sDown'%(syst)].Scale(weights[sample])
            else:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                histoDictFSB['ttV'].Add(temp,weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDictFSB['ttV_%sUp'%(syst)].Add(temp,weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(Nbins, 'ttV_temp', binBoundariesArray)
                    histoDictFSB['ttV_%sDown'%(syst)].Add(temp,weights[sample])

	elif 'ST_s-channel' in sample or 'ST_t-channel_antitop' in sample or 'ST_t-channel_top' in sample or 'ST_tW_antitop' in sample or 'ST_tW_top' in sample:
	    if 'ST_s-channel' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['ST'] = temp.Rebin(Nbins, 'ST', binBoundariesArray)
                histoDictFSB['ST'].SetDirectory(0)
                histoDictFSB['ST'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['ST_%sUp'%(syst)] = temp.Rebin(Nbins, 'ST_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['ST_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['ST_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['ST_%sDown'%(syst)] = temp.Rebin(Nbins, 'ST_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['ST_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['ST_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'ST_temp', binBoundariesArray)
                histoDictFSB['ST'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDictFSB['ST_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(Nbins, 'ST_temp', binBoundariesArray)
                    histoDictFSB['ST_%sDown'%(syst)].Add(temp,weights[sample])

        elif 'WJets' in sample and not 'TTWJets' in sample and not 'ttWJets' in sample:
		temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['WJets'] = temp.Rebin(Nbins, 'WJets', binBoundariesArray)
                histoDictFSB['WJets'].SetDirectory(0)
                histoDictFSB['WJets'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['WJets_%sUp'%(syst)] = temp.Rebin(Nbins, 'WJets_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['WJets_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['WJets_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['WJets_%sDown'%(syst)] = temp.Rebin(Nbins, 'WJets_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['WJets_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['WJets_%sDown'%(syst)].Scale(weights[sample])

        elif 'DY' in sample:
            if 'DYJetsToLL_M-50_HT-100to200' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['DY'] = temp.Rebin(Nbins, 'DY', binBoundariesArray)
                histoDictFSB['DY'].SetDirectory(0)
                histoDictFSB['DY'].Scale(weights[sample])
                for syst in systs:
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst))
                    histoDictFSB['DY_%sUp'%(syst)] = temp.Rebin(Nbins, 'DY_%sUp'%(syst), binBoundariesArray)
                    histoDictFSB['DY_%sUp'%(syst)].SetDirectory(0)
                    histoDictFSB['DY_%sUp'%(syst)].Scale(weights[sample])
                    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst))
                    histoDictFSB['DY_%sDown'%(syst)] = temp.Rebin(Nbins, 'DY_%sDown'%(syst), binBoundariesArray)
                    histoDictFSB['DY_%sDown'%(syst)].SetDirectory(0)
                    histoDictFSB['DY_%sDown'%(syst)].Scale(weights[sample])
            else:
		temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'DY_temp', binBoundariesArray)
                histoDictFSB['DY'].Add(temp,weights[sample])
                for syst in systs:
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sUp'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDictFSB['DY_%sUp'%(syst)].Add(temp,weights[sample])
		    temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass_%sDown'%(syst)).Rebin(Nbins, 'DY_temp', binBoundariesArray)
                    histoDictFSB['DY_%sDown'%(syst)].Add(temp,weights[sample])



        elif 'SingleElectron' in sample or 'EGamma'in sample:
            if 'SingleElectron--Run2016B' in sample or 'SingleElectron--Run2017B' in sample or 'EGamma--Run2018A' in sample:
                temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass')
                histoDictFSB['data_obs'] = temp.Rebin(Nbins, 'data_obs', binBoundariesArray)
                histoDictFSB['data_obs'].SetDirectory(0)
            else:
		temp = tfile.Get('analysis/eventsPassBoostFSBRECO/leadAK8JetElectronMass').Rebin(Nbins, 'data_obs_temp', binBoundariesArray)
                histoDictFSB['data_obs'].Add(temp)

        tfile.Close()

#    outputFile = workspaceOutputDirectory+'WR_NR_MuMuJJ_ResolvedFSB.root'
#    outputWorkspace = r.TFile(outputFile, "RECREATE")

#    MuMuJJ = outputWorkspace.mkdir("MuMuJJ")
#    MuMuJJ.cd()

#    histoDict['TT'].Write()
#    histoDict['DiBoson'].Write()
#    histoDict['ST'].Write()
#    histoDict['WJets'].Write()
#    histoDict['DY'].Write()
#    histoDict['QCD'].Write()
#    histoDict['data_obs'].Write()
#    for syst in systs:
#        histoDict['TT_%sUp'%(syst)].Write()
 #       histoDict['TT_%sDown'%(syst)].Write()
 #       histoDict['DiBoson_%sUp'%(syst)].Write()
 #       histoDict['DiBoson_%sDown'%(syst)].Write()
 #       histoDict['ST_%sUp'%(syst)].Write()
 #       histoDict['ST_%sDown'%(syst)].Write()
 #       histoDict['WJets_%sUp'%(syst)].Write()
 #       histoDict['WJets_%sDown'%(syst)].Write()
 #       histoDict['DY_%sUp'%(syst)].Write()
 #       histoDict['DY_%sDown'%(syst)].Write()
#        histoDict['QCD_%sUp'%(syst)].Write()
#        histoDict['QCD_%sDown'%(syst)].Write()

#    signalMasses = [(1000,100),(1500,100),(2000,100),(2500,100),(3000,100),(3500,100),(4000,100),(4500,100),(5000,100),(5500,100),(1000,300),(1500,300),(2000,300),(2500,300),(3000,300),(3500,300),(4000,300),(4500,300),(5000,300),(5500,300),(2000,500),(2500,500),(3000,500),(3500,500),(4000,500),(4500,500),(5000,500),(5500,500),(2000,700),(2500,700),(3000,700),(3500,700),(4000,700),(4500,700),(5000,700),(5500,700),(3000,900),(3500,900),(4000,900),(4500,900),(5000,900),(5500,900),(3500,1100),(4000,1100),(4500,1100),(5000,1100),(5500,1100),(4500,1300),(5000,1300),(5500,1300),(4500,1500),(5000,1500),(5500,1500),(5500,1700),(5500,1900),(5000,1000),(4500,450),(4000,800),(4000,400),(3500,350),(3000,600),(2500,250),(2000,400),(2000,200),(1500,150),(1000,200)]
    signalMasses = [(2000,1000),(3000,1500),(4000,2000)]

#    for WRmass,NRmass in signalMasses:
#        histoDict['WR_%s_NR_%s'%(WRmass,NRmass)].Write()
#        for syst in systs:
#            histoDict['WR_%s_NR_%s_%sUp'%(WRmass,NRmass,syst)].Write()
#            histoDict['WR_%s_NR_%s_%sDown'%(WRmass,NRmass,syst)].Write()

#    outputWorkspace.Write()
#    outputWorkspace.Close()

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

global sampleList 
sampleList = sys.argv[1]
samplesLocation = sys.argv[2]
workspaceOutputDirectory = sys.argv[3]
lumi2016 = 35920.0
lumi2017 = 41530.0
lumi2018 = 59740.0
if '2016' in sampleList:
    integratedLuminosity = lumi2016
    year = '2016'
elif '2017' in sampleList:
    integratedLuminosity = lumi2017
    year = '2017'
elif '2018' in sampleList:
    integratedLuminosity = lumi2018
    year = '2018'
else:
    print "GIVE ME A SAMPLE LIST WITH YEAR IN NAME!!!"
    exit(0)

if(len(sys.argv) >= 5 ):
    doSignalStr = sys.argv[4]
    if(doSignalStr == "True"):
        doSignal = True
    else:
        doSignal = False
else:
    doSignal = True
if(len(sys.argv) == 6):
    normToOneStr = sys.argv[5]
    if(normToOneStr == "norm"):
        normToOne = True
    else:
        normToOne = False
else:
    normToOne = False

#integratedLuminosity = 137400.0
LSFSF = 0.87
#integratedLuminosity = 80000.0

with open(sampleList) as f:
    lines = f.read().splitlines()

xsecs = collections.OrderedDict()
sampleNames = []
lineNum = 0
for line in lines:
    if lineNum < 2 :
        lineNum+=1
        continue
    print "line.split()[0]: ", line.split()[0]
    if ("SingleMuon" in line.split()[0] or "SingleElectron" in line.split()[0] or "EGamma" in line.split()[0]):
        sampleNames.append(line.split()[0].strip().split("/")[1]+"--"+line.split()[0].strip().split("/")[2])
    elif ("WR" in line.split()[0]):
        if "/" in line.split()[0]:
            name = line.split()[0].strip().split("/")[1]
        else:
            name = line.split()[0].strip() 
	print "WR: ", name
	sampleNames.append(name)
#        xsecLine = line.split()[3].strip().split("+")[0]
#        xsec = float(xsecLine)
#        print "xsec: ", xsec
	xsec = float(1.0)
        xsecs[name] = xsec
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
weights_DY = collections.OrderedDict()
numberOfEvents = collections.OrderedDict()
for sample,xsec in xsecs.items():
	print "sample: ", sample
	backgroundEventsWeight = samplesLocation+sample+".root"
	weight = 1.0
        print "STARTING EVENT WEIGHT: "+str(weight)
	if "WRto" in sample:
	    weight *= integratedLuminosity/100
#	elif "DY" in sample:
#	    weight *= integratedLuminosity*0.937
	else:
            weight *= integratedLuminosity
        weight *= xsecs[sample]
        print "EVENT WEIGHT WITH XSEC: "+str(weight)
        eventsWeight = 0
        eventsWeight = getEventsWeight(r.TFile.Open(backgroundEventsWeight, "read"),directory=samplesLocation)
        if (eventsWeight == 0):
            print "BACKGROUND HAS ZERO EVENTS WEIGHT"
            continue
        print "Found # events:"+str(eventsWeight)
	numberOfEvents[sample] = eventsWeight
        weight /= eventsWeight
        print "EVENT WEIGHT WITH EVENTSWEIGHT: "+str(weight)
	print "Scale: "+str(weight)
	weights_DY[sample] = weight
        weight *= LSFSF
	if "WRto" in sample:
            if(normToOne):
                weights[sample] = 1.0
            else:
                weights[sample] = weight
	else:
	    weights[sample] = weight


histoDictFSB = {}
histoDictZPeak = {}
ZPeakSF = 1.0


print "Getting FSB background histos"
FSBWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights)

print "Getting ZPeak background histos"
ZPeakSF = ZPeakWorksapce(sampleNames,samplesLocation,workspaceOutputDirectory, weights_DY)
print "ZPeakSF: ", ZPeakSF

print "MAKING SIGNAL REGION WORKSPACE"
SignalRegionWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights, numberOfEvents, ZPeakSF, doSignal, normToOne, year)

#print "MAKING ZPEAK REGION WORKSPACE"
#ZPeakWorksapce(sampleNames,samplesLocation,workspaceOutputDirectory, weights)

#print "MAKING FSB REGION WORKSPACE"
#FSBWorkspace(sampleNames,samplesLocation,workspaceOutputDirectory, weights)
