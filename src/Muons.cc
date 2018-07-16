#include "ExoAnalysis/cmsWRextensions/interface/Muons.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"




Muons::Muons () {

  std::string iMuon_ID_BCDEF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/EfficienciesAndSF_BCDEF.root";
  TFile *lFile_BCDEF = TFile::Open(iMuon_ID_BCDEF.c_str());
  Muon_LooseID_BCDEF = (TH2D*) lFile_BCDEF->Get("MC_NUM_LooseID_DEN_genTracks_PAR_pt_eta/efficienciesDATA/pt_abseta_DATA");
  Muon_LooseID_BCDEF->SetDirectory(0);
  lFile_BCDEF->Close();

  std::string iMuon_ID_GH="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/EfficienciesAndSF_BCDEF.root";
  TFile *lFile_GH = TFile::Open(iMuon_ID_GH.c_str());
  Muon_LooseID_GH = (TH2D*) lFile_GH->Get("MC_NUM_LooseID_DEN_genTracks_PAR_pt_eta/efficienciesDATA/pt_abseta_DATA");
  Muon_LooseID_GH->SetDirectory(0);
  lFile_GH->Close();

  float lumi_GH = 16.146;
  float lumi_BCDEF = 19.721;
  float lumi_total = lumi_GH + lumi_BCDEF; 

  Muon_LooseID_eff = (TH2D*) Muon_LooseID_GH->Clone("pt_abseta_DATA_muid_ave");
  Muon_LooseID_eff->Scale(lumi_GH / lumi_total);
  Muon_LooseID_eff->Add(Muon_LooseID_BCDEF, lumi_BCDEF/lumi_total);

}

double Muons::MuonLooseIDweight(double MuonPt, double MuonEta) {

  double muPtForId = 0.;
  double muEtaForId = 0.;

  if(MuonPt > 100){
    muPtForId = 100.;
  }else if(MuonPt < 20.){
    muPtForId = 20.;
  }
  else{
    muPtForId = MuonPt;
  }

  if(std::abs(MuonEta) > 2.3){
    muEtaForId = 2.3;
  }else{
    muEtaForId = std::abs(MuonEta);
  }

  muidweight = Muon_LooseID_eff->GetBinContent(Muon_LooseID_eff->FindBin(muPtForId, muEtaForId));
  muidweightUp = muidweight + Muon_LooseID_eff->GetBinError(Muon_LooseID_eff->FindBin(muPtForId, muEtaForId));
  muidweightDown = muidweight - Muon_LooseID_eff->GetBinError(Muon_LooseID_eff->FindBin(muPtForId, muEtaForId));

  std::cout << "muidweight: " << muidweight << std::endl;
  std::cout << "muidweightUp: " << muidweightUp << std::endl;
  std::cout << "muidweightDown: " << muidweightDown << std::endl;

  return muidweight;
}

