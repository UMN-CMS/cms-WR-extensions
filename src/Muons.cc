#include "ExoAnalysis/cmsWRextensions/interface/Muons.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "ExoAnalysis/cmsWRextensions/interface/GeneralizedEndpoint.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"
#include "ExoAnalysis/cmsWRextensions/interface/RoccoR.h"



Muons::Muons () {

  std::string iMuon_ID_BCDEF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/RunBCDEF_SYS_ID.root";
  TFile *lFile_BCDEF = TFile::Open(iMuon_ID_BCDEF.c_str());
  Muon_LooseID_BCDEF = (TH2D*) lFile_BCDEF->Get("NUM_LooseID_DEN_genTracks_eta_pt");
  Muon_LooseID_BCDEF->SetDirectory(0);

  Muon_HighPT_BCDEF = (TH2D*) lFile_BCDEF->Get("NUM_HighPtID_DEN_genTracks_eta_pair_newTuneP_probe_pt");
  Muon_HighPT_BCDEF->SetDirectory(0);

  lFile_BCDEF->Close();

  std::string iMuon_ID_GH="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/RunGH_SYS_ID.root";
  TFile *lFile_GH = TFile::Open(iMuon_ID_GH.c_str());
  Muon_LooseID_GH = (TH2D*) lFile_GH->Get("NUM_LooseID_DEN_genTracks_eta_pt");
  Muon_LooseID_GH->SetDirectory(0);

  Muon_HighPT_GH = (TH2D*) lFile_GH->Get("NUM_HighPtID_DEN_genTracks_eta_pair_newTuneP_probe_pt");
  Muon_HighPT_GH->SetDirectory(0);

  lFile_GH->Close();

  float lumi_GH = 16.146;
  float lumi_BCDEF = 19.721;
  float lumi_total = lumi_GH + lumi_BCDEF; 

  Muon_LooseID_eff2016 = (TH2D*) Muon_LooseID_GH->Clone("pt_abseta_DATA_muid_ave");
  Muon_LooseID_eff2016->Scale(lumi_GH / lumi_total);
  Muon_LooseID_eff2016->Add(Muon_LooseID_BCDEF, lumi_BCDEF/lumi_total);

  Muon_HighPT_eff2016 = (TH2D*) Muon_HighPT_GH->Clone("pt_abseta_DATA_muid_highPT_ave");
  Muon_HighPT_eff2016->Scale(lumi_GH / lumi_total);
  Muon_HighPT_eff2016->Add(Muon_HighPT_BCDEF, lumi_BCDEF/lumi_total);

//Muon ID SF files - 2017
  std::string iMuon_ID_BtoF2017="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/RunBCDEF_SF_ID_syst.root";
  TFile *lFile_BtoF2017 = TFile::Open(iMuon_ID_BtoF2017.c_str());
  Muon_LooseID_BtoF2017 = (TH2D*) lFile_BtoF2017->Get("NUM_LooseID_DEN_genTracks_pt_abseta");
  Muon_LooseID_BtoF2017->SetDirectory(0);

  Muon_HighPT_BtoF2017 = (TH2D*) lFile_BtoF2017->Get("NUM_HighPtID_DEN_genTracks_pair_newTuneP_probe_pt_abseta");
  Muon_HighPT_BtoF2017->SetDirectory(0);

  lFile_BtoF2017->Close();


//Muon ID SF files - 2018
  std::string iMuon_ID_AtoD2018="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/RunABCD_SF_ID.root";
  TFile *lFile_AtoD2018 = TFile::Open(iMuon_ID_AtoD2018.c_str());
  Muon_LooseID_AtoD2018 = (TH2D*) lFile_AtoD2018->Get("NUM_LooseID_DEN_genTracks_pt_abseta");
  Muon_LooseID_AtoD2018->SetDirectory(0);

  Muon_HighPT_AtoD2018 = (TH2D*) lFile_AtoD2018->Get("NUM_HighPtID_DEN_genTracks_pair_newTuneP_probe_pt_abseta");
  Muon_HighPT_AtoD2018->SetDirectory(0);

  lFile_AtoD2018->Close();

//Muon ISO SF files - 2016
  std::string iMuon_ISO_BCDEF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/RunBCDEF_SYS_ISO.root";
  TFile *lFile_ISO_BCDEF = TFile::Open(iMuon_ISO_BCDEF.c_str());
  Muon_HighPT_ISO_BCDEF = (TH2D*) lFile_ISO_BCDEF->Get("NUM_LooseRelTkIso_DEN_HighPtIDandIPCut_eta_pair_newTuneP_probe_pt");
  Muon_HighPT_ISO_BCDEF->SetDirectory(0);

  lFile_ISO_BCDEF->Close();

  std::string iMuon_ISO_GH="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/RunGH_SYS_ISO.root";
  TFile *lFile_ISO_GH = TFile::Open(iMuon_ISO_GH.c_str());
  Muon_HighPT_ISO_GH = (TH2D*) lFile_ISO_GH->Get("NUM_LooseRelTkIso_DEN_HighPtIDandIPCut_eta_pair_newTuneP_probe_pt");
  Muon_HighPT_ISO_GH->SetDirectory(0);

  lFile_ISO_GH->Close();


  Muon_HighPT_ISO_eff2016 = (TH2D*) Muon_HighPT_ISO_GH->Clone("pt_abseta_DATA_muid_highPT_ISO_ave");
  Muon_HighPT_ISO_eff2016->Scale(lumi_GH / lumi_total);
  Muon_HighPT_ISO_eff2016->Add(Muon_HighPT_ISO_BCDEF, lumi_BCDEF/lumi_total);

//Muon ISO SF files - 2017
  std::string iMuon_ISO_BtoF2017="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/RunBCDEF_SF_ISO_syst.root";
  TFile *lFile_ISO_BtoF2017 = TFile::Open(iMuon_ISO_BtoF2017.c_str());
  Muon_HighPT_ISO_BtoF2017 = (TH2D*) lFile_ISO_BtoF2017->Get("NUM_LooseRelTkIso_DEN_HighPtIDandIPCut_pair_newTuneP_probe_pt_abseta");
  Muon_HighPT_ISO_BtoF2017->SetDirectory(0);

  lFile_ISO_BtoF2017->Close();  

//Muon ISO SF files - 2018
  std::string iMuon_ISO_AtoD2018="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/RunABCD_SF_ISO.root";
  TFile *lFile_ISO_AtoD2018 = TFile::Open(iMuon_ISO_AtoD2018.c_str());
  Muon_HighPT_ISO_AtoD2018 = (TH2D*) lFile_ISO_AtoD2018->Get("NUM_LooseRelTkIso_DEN_HighPtIDandIPCut_pair_newTuneP_probe_pt_abseta");
  Muon_HighPT_ISO_AtoD2018->SetDirectory(0);

  lFile_ISO_AtoD2018->Close();



//Trigger SF files - 2016
  std::string iMuon_Trig_BCDEF = "${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/EfficienciesAndSF_RunBtoF.root";
  TFile *lFile_Trig_BCDEF = TFile::Open(iMuon_Trig_BCDEF.c_str());
  Muon_Trig_BCDEF = (TH2D*) lFile_Trig_BCDEF->Get("Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
  Muon_Trig_DataEff_BCDEF = (TH1F*) lFile_Trig_BCDEF->Get("Mu50_OR_TkMu50_PtBins/efficienciesDATA/histo_pt_DATA");
  Muon_Trig_BCDEF->SetDirectory(0);
  Muon_Trig_DataEff_BCDEF->SetDirectory(0);
  lFile_Trig_BCDEF->Close();

  std::string iMuon_Trig_GH = "${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/EfficienciesAndSF_RunGtoH.root";
  TFile *lFile_Trig_GH = TFile::Open(iMuon_Trig_GH.c_str());
  Muon_Trig_GH = (TH2D*) lFile_Trig_GH->Get("Mu50_OR_TkMu50_PtEtaBins/abseta_pt_ratio");
  Muon_Trig_DataEff_GH = (TH1F*) lFile_Trig_GH->Get("Mu50_OR_TkMu50_PtBins/efficienciesDATA/histo_pt_DATA");
  Muon_Trig_GH->SetDirectory(0);
  Muon_Trig_DataEff_GH->SetDirectory(0);
  lFile_Trig_GH->Close();

  Muon_Trig_SF2016 = (TH2D*) Muon_Trig_GH->Clone("pt_SF_trig_ave");
  Muon_Trig_SF2016->Scale(lumi_GH / lumi_total);
  Muon_Trig_SF2016->Add(Muon_Trig_BCDEF, lumi_BCDEF/lumi_total);

  for(int i=1;i<Muon_Trig_SF2016->GetNbinsX()+1; i++){
     for(int j=1;j<Muon_Trig_SF2016->GetNbinsY()+1; j++){
	std::cout << "Muon_Trig_SF2016.GetXaxis().GetBinCenter(i): " << Muon_Trig_SF2016->GetXaxis()->GetBinCenter(i) << std::endl;
	std::cout << "Muon_Trig_SF2016.GetYaxis().GetBinCenter(j): " << Muon_Trig_SF2016->GetYaxis()->GetBinCenter(j) << std::endl;
	std::cout << "Muon_Trig_SF2016.GetBinContent(i,j): " << Muon_Trig_SF2016->GetBinContent(i,j) << std::endl;
     }
  }

  Muon_Trig_DataEff2016 = (TH1F*) Muon_Trig_DataEff_GH->Clone("pt_dataEff_trig_ave");
  Muon_Trig_DataEff2016->Scale(lumi_GH / lumi_total);
  Muon_Trig_DataEff2016->Add(Muon_Trig_DataEff_BCDEF, lumi_BCDEF/lumi_total);

//Trigger SF files - 2017
  std::string iMuon_Trig_BtoF = "${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root";
  TFile *lFile_Trig_BtoF = TFile::Open(iMuon_Trig_BtoF.c_str());
  Muon_Trig_SF2017 = (TH2D*) lFile_Trig_BtoF->Get("Mu50_PtEtaBins/abseta_pt_ratio");
  Muon_Trig_DataEff2017 = (TH1F*) lFile_Trig_BtoF->Get("Mu50_PtBins/efficienciesDATA/histo_pt_DATA");
  Muon_Trig_SF2017->SetDirectory(0);
  Muon_Trig_DataEff2017->SetDirectory(0);
  lFile_Trig_BtoF->Close();

//Trigger SF files - 2018
  std::string iMuon_Trig_2018 = "${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root";
  TFile *lFile_Trig_2018 = TFile::Open(iMuon_Trig_2018.c_str());
  Muon_Trig_SF2018 = (TH2D*) lFile_Trig_2018->Get("Mu50_OR_OldMu100_OR_TkMu100_PtEtaBins/abseta_pt_ratio");
  Muon_Trig_DataEff2018 = (TH1F*) lFile_Trig_2018->Get("Mu50_OR_OldMu100_OR_TkMu100_PtBins/efficienciesDATA/histo_pt_DATA");
  Muon_Trig_SF2018->SetDirectory(0);
  Muon_Trig_DataEff2018->SetDirectory(0);
  lFile_Trig_2018->Close(); 

//Trigger Data Efficiency files (for weighting signal samples) - 2016


//Rochester Method
  rc2016.init(edm::FileInPath("ExoAnalysis/cmsWRextensions/data/2016/RoccoR2016.txt").fullPath());
  rc2017.init(edm::FileInPath("ExoAnalysis/cmsWRextensions/data/2017/RoccoR2017.txt").fullPath());
  rc2018.init(edm::FileInPath("ExoAnalysis/cmsWRextensions/data/2018/RoccoR2018.txt").fullPath());

//Generalized Endpoint Method


}

std::vector<double> Muons::MuonLooseIDweight(double MuonPt, double MuonEta, std::string era) {

  double muPtForId = 0.;
  double muEtaForId = 0.;

  if(MuonPt > 100){
    muPtForId = 100.;
  }else if(MuonPt < 20.){
    muPtForId = 21.;
  }
  else{
    muPtForId = MuonPt;
  }

  if(std::abs(MuonEta) > 2.3){
    muEtaForId = 2.29;
  }else{
    muEtaForId = std::abs(MuonEta);
  }

  if(era == "2016"){
    muidweight = Muon_LooseID_eff2016->GetBinContent(Muon_LooseID_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightUp = muidweight + Muon_LooseID_eff2016->GetBinError(Muon_LooseID_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightDown = muidweight - Muon_LooseID_eff2016->GetBinError(Muon_LooseID_eff2016->FindBin(muEtaForId, muPtForId));
  }else if(era == "2017"){
    muidweight = Muon_LooseID_BtoF2017->GetBinContent(Muon_LooseID_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_LooseID_BtoF2017->GetBinError(Muon_LooseID_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_LooseID_BtoF2017->GetBinError(Muon_LooseID_BtoF2017->FindBin(muPtForId, muEtaForId));
  }else if(era == "2018"){
    muidweight = Muon_LooseID_AtoD2018->GetBinContent(Muon_LooseID_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_LooseID_AtoD2018->GetBinError(Muon_LooseID_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_LooseID_AtoD2018->GetBinError(Muon_LooseID_AtoD2018->FindBin(muPtForId, muEtaForId));
  }

  std::cout << "muidweight: " << muidweight << std::endl;
  std::cout << "muidweightUp: " << muidweightUp << std::endl;
  std::cout << "muidweightDown: " << muidweightDown << std::endl;

  std::vector<double> muWeights;
  muWeights.push_back(muidweight);
  muWeights.push_back(muidweightUp);
  muWeights.push_back(muidweightDown);

  return muWeights;
}

std::vector<double> Muons::MuonHighPTIDweight(double MuonPt, double MuonEta, std::string era) {

  double muPtForId = 0.;
  double muEtaForId = 0.;

  if(MuonPt > 120){
    muPtForId = 119.;
  }else if(MuonPt < 20.){
    muPtForId = 21.;
  }
  else{
    muPtForId = MuonPt;
  }

  if(std::abs(MuonEta) > 2.3){
    muEtaForId = 2.29;
  }else{
    muEtaForId = std::abs(MuonEta);
  }

  if(era == "2016"){
    muidweight = Muon_HighPT_eff2016->GetBinContent(Muon_HighPT_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightUp = muidweight + Muon_HighPT_eff2016->GetBinError(Muon_HighPT_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightDown = muidweight - Muon_HighPT_eff2016->GetBinError(Muon_HighPT_eff2016->FindBin(muEtaForId, muPtForId));
  }else if(era == "2017"){
    muidweight = Muon_HighPT_BtoF2017->GetBinContent(Muon_HighPT_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_HighPT_BtoF2017->GetBinError(Muon_HighPT_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_HighPT_BtoF2017->GetBinError(Muon_HighPT_BtoF2017->FindBin(muPtForId, muEtaForId));
  }else if(era == "2018"){
    muidweight = Muon_HighPT_AtoD2018->GetBinContent(Muon_HighPT_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_HighPT_AtoD2018->GetBinError(Muon_HighPT_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_HighPT_AtoD2018->GetBinError(Muon_HighPT_AtoD2018->FindBin(muPtForId, muEtaForId));
  }

  std::vector<double> muWeights;
  muWeights.push_back(muidweight);
  muWeights.push_back(muidweightUp);
  muWeights.push_back(muidweightDown);

  return muWeights;
}

std::vector<double> Muons::MuonLooseTkIso(double MuonPt, double MuonEta, std::string era){

  double muPtForId = 0.;
  double muEtaForId = 0.;

  if(MuonPt > 120){
    muPtForId = 119.;
  }else if(MuonPt < 20.){
    muPtForId = 21.;
  }
  else{
    muPtForId = MuonPt;
  }

  if(std::abs(MuonEta) > 2.3){
    muEtaForId = 2.29;
  }else{
    muEtaForId = std::abs(MuonEta);
  }

  if(era == "2016"){
    muidweight = Muon_HighPT_ISO_eff2016->GetBinContent(Muon_HighPT_ISO_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightUp = muidweight + Muon_HighPT_ISO_eff2016->GetBinError(Muon_HighPT_ISO_eff2016->FindBin(muEtaForId, muPtForId));
    muidweightDown = muidweight - Muon_HighPT_ISO_eff2016->GetBinError(Muon_HighPT_ISO_eff2016->FindBin(muEtaForId, muPtForId));
  }else if(era == "2017"){
    muidweight = Muon_HighPT_ISO_BtoF2017->GetBinContent(Muon_HighPT_ISO_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_HighPT_ISO_BtoF2017->GetBinError(Muon_HighPT_ISO_BtoF2017->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_HighPT_ISO_BtoF2017->GetBinError(Muon_HighPT_ISO_BtoF2017->FindBin(muPtForId, muEtaForId));
  }else if(era == "2018"){
    muidweight = Muon_HighPT_ISO_AtoD2018->GetBinContent(Muon_HighPT_ISO_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightUp = muidweight + Muon_HighPT_ISO_AtoD2018->GetBinError(Muon_HighPT_ISO_AtoD2018->FindBin(muPtForId, muEtaForId));
    muidweightDown = muidweight - Muon_HighPT_ISO_AtoD2018->GetBinError(Muon_HighPT_ISO_AtoD2018->FindBin(muPtForId, muEtaForId));
  }

  std::cout << "muisoweight: " << muidweight << std::endl;
  std::cout << "muisoweightUp: " << muidweightUp << std::endl;
  std::cout << "muisoweightDown: " << muidweightDown << std::endl;

  std::vector<double> muWeights;
  muWeights.push_back(muidweight);
  muWeights.push_back(muidweightUp);
  muWeights.push_back(muidweightDown);

  return muWeights;

}

std::vector<double> Muons::MuonTriggerWeight(double MuonPt, double MuonEta, std::string era, bool isSignal){

  double muPtForId = 0.;
  double muEtaForId = 0.;

  if(MuonPt > 800){
    muPtForId = 799;
  }else{
    muPtForId = MuonPt;
  }

  if(std::abs(MuonEta) > 2.4){
    muEtaForId = 2.39;
  }else{
    muEtaForId = std::abs(MuonEta);
  }


  if(era == "2016"){
    if(isSignal){
      muTrigWeight = Muon_Trig_DataEff2016->GetBinContent(Muon_Trig_DataEff2016->FindBin(muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_DataEff2016->GetBinError(Muon_Trig_DataEff2016->FindBin(muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_DataEff2016->GetBinError(Muon_Trig_DataEff2016->FindBin(muPtForId));
    }else{
      muTrigWeight = Muon_Trig_SF2016->GetBinContent(Muon_Trig_SF2016->FindBin(muEtaForId,muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_SF2016->GetBinError(Muon_Trig_SF2016->FindBin(muEtaForId,muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_SF2016->GetBinError(Muon_Trig_SF2016->FindBin(muEtaForId,muPtForId));
    }
  }else if(era == "2017"){
    if(isSignal){
      muTrigWeight = Muon_Trig_DataEff2017->GetBinContent(Muon_Trig_DataEff2017->FindBin(muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_DataEff2017->GetBinError(Muon_Trig_DataEff2017->FindBin(muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_DataEff2017->GetBinError(Muon_Trig_DataEff2017->FindBin(muPtForId));
    }else{
      muTrigWeight = Muon_Trig_SF2017->GetBinContent(Muon_Trig_SF2017->FindBin(muEtaForId,muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_SF2017->GetBinError(Muon_Trig_SF2017->FindBin(muEtaForId,muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_SF2017->GetBinError(Muon_Trig_SF2017->FindBin(muEtaForId,muPtForId));
    }
  }else if(era == "2018"){
    if(isSignal){
      muTrigWeight = Muon_Trig_DataEff2018->GetBinContent(Muon_Trig_DataEff2018->FindBin(muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_DataEff2018->GetBinError(Muon_Trig_DataEff2018->FindBin(muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_DataEff2018->GetBinError(Muon_Trig_DataEff2018->FindBin(muPtForId));
    }else{
      muTrigWeight = Muon_Trig_SF2018->GetBinContent(Muon_Trig_SF2018->FindBin(muEtaForId,muPtForId));
      muTrigWeightUp = muTrigWeight + Muon_Trig_SF2018->GetBinError(Muon_Trig_SF2018->FindBin(muEtaForId,muPtForId));
      muTrigWeightDown = muTrigWeight - Muon_Trig_SF2018->GetBinError(Muon_Trig_SF2018->FindBin(muEtaForId,muPtForId));
    }
  }

  std::cout << "muTrigWeight: " << muTrigWeight << std::endl;
  std::cout << "muPtForId: " << muPtForId << std::endl;
  std::cout << "muEtaForId: " << muEtaForId << std::endl;

  std::vector<double> muTrigWeights;
  muTrigWeights.push_back(muTrigWeight);
  muTrigWeights.push_back(muTrigWeightUp);
  muTrigWeights.push_back(muTrigWeightDown);

  return muTrigWeights;
}

std::vector<double> Muons::RochesterMethod_DataScale(const pat::Muon* Mu, std::string era){

  double dtSF = 0.;
  double rndm = gRandom->Rndm();

  if(era == "2016"){
    dtSF = rc2016.kScaleDT(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), 0, 0);
  }else if(era == "2017"){
    dtSF = rc2017.kScaleDT(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), 0, 0);
  }else if(era == "2018"){
    dtSF = rc2018.kScaleDT(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), 0, 0);
  }

  std::cout << "dtSF: " <<  dtSF << std::endl;

  std::vector<double> Weights;
  Weights.push_back(dtSF);
  Weights.push_back(0.);
  Weights.push_back(0.);

  return Weights;


}

std::vector<double> Muons::RochesterMethod_MCSmear(const pat::Muon* Mu, std::string era){

  double mcSF = 0.;
  double Stat = 0.;
  double Zpt = 0.;
  double Ewk = 0.;
  double deltaM = 0.;
  double Ewk2 = 0.;
  double TotalError = 0.;
  double rndm = gRandom->Rndm();

  if(era == "2016"){
    mcSF = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 0, 0);
    Stat = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 1, 99);
    Zpt = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 2, 0);
    Ewk = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 3, 0);
    deltaM = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 4, 0);
    Ewk2 = rc2016.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 5, 0);
  }else if(era == "2017"){
    mcSF = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 0, 0);
    Stat = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 1, 99);
    Zpt = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 2, 0);
    Ewk = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 3, 0);
    deltaM = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 4, 0);
    Ewk2 = rc2017.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 5, 0);
  }else if(era == "2018"){
    mcSF = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 0, 0);
    Stat = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 1, 99);
    Zpt = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 2, 0);
    Ewk = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 3, 0);
    deltaM = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 4, 0);
    Ewk2 = rc2018.kSmearMC(Mu->charge(), Mu->pt(), Mu->eta(), Mu->phi(), Mu->innerTrack()->hitPattern().trackerLayersWithMeasurement(), rndm, 5, 0);
  }

  std::cout << "mcSF: " <<  mcSF << std::endl;
  std::cout << "Stat: " << Stat << " Zpt: " << Zpt << " Ewk: " << Ewk << " deltaM: " << deltaM << " Ewk2: " << Ewk2 << std::endl;
  TotalError = sqrt(pow(mcSF-Stat,2)+pow(mcSF-Zpt,2)+pow(mcSF-Ewk,2)+pow(mcSF-deltaM,2)+pow(mcSF-Ewk2,2));
   
  std::vector<double> Weights;
  Weights.push_back(mcSF);
  Weights.push_back(mcSF+TotalError);
  Weights.push_back(mcSF-TotalError);

  return Weights;


}

std::vector<double> Muons::GeneralizedEndpointMethod(const pat::Muon* Mu){

  double CorrectpT = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), 0, 1);
  double CorrectpT_Up = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), 1, 1);
  double CorrectpT_Down = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), 2, 1);

  std::cout << "CorrectpT/Mu->pt(): " << CorrectpT/Mu->pt() << std::endl;

  std::vector<double> Weights;
  Weights.push_back(CorrectpT/Mu->pt());
  Weights.push_back(CorrectpT_Up/Mu->pt());
  Weights.push_back(CorrectpT_Down/Mu->pt());

  return Weights;    

}

std::vector<double> Muons::GeneralizedEndpointMethod_Data(const pat::Muon* Mu){

  double CorrectpT = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), -1, 1);
  double CorrectpT_Up = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), -2, 1);
  double CorrectpT_Down = myGeneralizedEndpoint.GeneralizedEndpointPt(Mu->pt(), Mu->charge(), Mu->eta(), Mu->phi(), -3, 1);

  std::cout << "CorrectpT/Mu->pt(): " << CorrectpT/Mu->pt() << std::endl;

  std::vector<double> Weights;
  Weights.push_back(CorrectpT/Mu->pt());
  Weights.push_back(CorrectpT_Up/Mu->pt());
  Weights.push_back(CorrectpT_Down/Mu->pt());

  return Weights;

}

