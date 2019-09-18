#include "ExoAnalysis/cmsWRextensions/interface/egammaEffi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"




egammaEffi::egammaEffi () {

  std::string iegammaEffi_SF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/egammaEffi.txt_EGM2D.root";
  TFile *lFile = TFile::Open(iegammaEffi_SF.c_str());
  m_egamma_sf = (TH2D*) lFile->Get("EGamma_SF2D");
  lFile->Close();

  std::string iegammaEffi_SF2017="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
  TFile *lFile_2017 = TFile::Open(iegammaEffi_SF2017.c_str());
  m_egamma_sf2017 = (TH2D*) lFile_2017->Get("EGamma_SF2D");
  lFile_2017->Close();

  std::string iegammaEffi_SF2018="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/egammaEffi.txt_EGM2D_updatedAll.root";
  TFile *lFile_2018 = TFile::Open(iegammaEffi_SF2018.c_str());
  m_egamma_sf2018 = (TH2D*) lFile_2018->Get("EGamma_SF2D");
  lFile_2018->Close();

//Electron Trigger SF's
  std::string iegammaEffi_HLT_SF2016="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/egammaEffi.txt_EGM2D_HLT2016.root";
  TFile *lFile_HLT2016 = TFile::Open(iegammaEffi_HLT_SF2016.c_str());
  m_egamma__HLT_sf2016_barrel = (TH2D*) lFile_HLT2016->Get("SF_TH2F_Barrel");
  m_egamma__HLT_sf2016_endcap = (TH2D*) lFile_HLT2016->Get("SF_TH2F_Endcap");
  lFile_HLT2016->Close();

  std::string iegammaEffi_HLT_SF2017="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/egammaEffi.txt_EGM2D_HLT2017.root";
  TFile *lFile_HLT2017 = TFile::Open(iegammaEffi_HLT_SF2017.c_str());
  m_egamma__HLT_sf2017_barrel = (TH2D*) lFile_HLT2017->Get("SF_TH2F_Barrel");
  m_egamma__HLT_sf2017_endcap = (TH2D*) lFile_HLT2017->Get("SF_TH2F_Endcap");
  lFile_HLT2017->Close();

  std::string iegammaEffi_HLT_SF2018="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/egammaEffi.txt_EGM2D_HLT2018.root";
  TFile *lFile_HLT2018 = TFile::Open(iegammaEffi_HLT_SF2018.c_str());
  m_egamma__HLT_sf2018_barrel = (TH2D*) lFile_HLT2018->Get("SF_TH2F_Barrel");
  m_egamma__HLT_sf2018_endcap = (TH2D*) lFile_HLT2018->Get("SF_TH2F_Endcap");
  lFile_HLT2018->Close();



}
void egammaEffi::Initialize(std::string era) {

  if(era == "2016"){
    m_nEtaBins = m_egamma_sf->GetXaxis()->GetNbins();
    m_nPtBins  = m_egamma_sf->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_barrel = m_egamma__HLT_sf2016_barrel->GetXaxis()->GetNbins();
    m_nPtBins_HLT_barrel  = m_egamma__HLT_sf2016_barrel->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_endcap = m_egamma__HLT_sf2016_endcap->GetXaxis()->GetNbins();
    m_nPtBins_HLT_endcap  = m_egamma__HLT_sf2016_endcap->GetYaxis()->GetNbins();
  }else if(era == "2017"){
    m_nEtaBins = m_egamma_sf2017->GetXaxis()->GetNbins();
    m_nPtBins  = m_egamma_sf2017->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_barrel = m_egamma__HLT_sf2017_barrel->GetXaxis()->GetNbins();
    m_nPtBins_HLT_barrel  = m_egamma__HLT_sf2017_barrel->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_endcap = m_egamma__HLT_sf2017_endcap->GetXaxis()->GetNbins();
    m_nPtBins_HLT_endcap  = m_egamma__HLT_sf2017_endcap->GetYaxis()->GetNbins();
  }else if(era == "2018"){
    m_nEtaBins = m_egamma_sf2018->GetXaxis()->GetNbins();
    m_nPtBins  = m_egamma_sf2018->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_barrel = m_egamma__HLT_sf2018_barrel->GetXaxis()->GetNbins();
    m_nPtBins_HLT_barrel  = m_egamma__HLT_sf2018_barrel->GetYaxis()->GetNbins();
    m_nEtaBins_HLT_endcap = m_egamma__HLT_sf2018_endcap->GetXaxis()->GetNbins();
    m_nPtBins_HLT_endcap  = m_egamma__HLT_sf2018_endcap->GetYaxis()->GetNbins();
  }
  
}

std::vector<double> egammaEffi::ScaleFactor(double ElectronEta, double ElectronPt, std::string era) {

  double scaleFactor = 1.0;
  double scaleFactor_Up = 0;
  double scaleFactor_Down = 0.;

  int etaBin = -1;
  int ptBin = -1;

  if(ElectronPt > 500){
    ElectronPt = 499;
  }

  std::vector<double> egammaWeights;

  if(era == "2016"){
    etaBin = m_egamma_sf->GetXaxis()->FindBin(ElectronEta);
    ptBin = m_egamma_sf->GetYaxis()->FindBin(ElectronPt);
  }else if(era == "2017"){
    etaBin = m_egamma_sf2017->GetXaxis()->FindBin(ElectronEta);
    ptBin = m_egamma_sf2017->GetYaxis()->FindBin(ElectronPt);
  }else if(era == "2018"){
    etaBin = m_egamma_sf2018->GetXaxis()->FindBin(ElectronEta);
    ptBin = m_egamma_sf2018->GetYaxis()->FindBin(ElectronPt);
  }

  if(etaBin == 0 || ptBin == 0) {
    std::cout << "egammaEffi::ScaleFactor : WARNING PT OR ETA IN UNDERFLOW : ETA : "<<etaBin<<" : PT : "<<ptBin << std::endl;
    egammaWeights.push_back(scaleFactor);
    egammaWeights.push_back(scaleFactor_Up);
    egammaWeights.push_back(scaleFactor_Down);
    return egammaWeights;
  }
  if(etaBin == (m_nEtaBins+1) || ptBin == (m_nEtaBins+1) ){
    std::cout << "egammaEffi::ScaleFactor : WARNING PT OR ETA IN OVERFLOW  : ETA : "<<etaBin<<" : PT : "<<ptBin << std::endl;
    egammaWeights.push_back(scaleFactor);
    egammaWeights.push_back(scaleFactor_Up);
    egammaWeights.push_back(scaleFactor_Down);
    return egammaWeights;
  }

  if(era == "2016"){
    scaleFactor = m_egamma_sf->GetBinContent(etaBin, ptBin);
    scaleFactor_Up = m_egamma_sf->GetBinContent(etaBin, ptBin) + m_egamma_sf->GetBinError(etaBin, ptBin);
    scaleFactor_Down = m_egamma_sf->GetBinContent(etaBin, ptBin) - m_egamma_sf->GetBinError(etaBin, ptBin);
  }else if(era == "2017"){
    scaleFactor = m_egamma_sf2017->GetBinContent(etaBin, ptBin);
    scaleFactor_Up = m_egamma_sf2017->GetBinContent(etaBin, ptBin) + m_egamma_sf2017->GetBinError(etaBin, ptBin);
    scaleFactor_Down = m_egamma_sf2017->GetBinContent(etaBin, ptBin) - m_egamma_sf2017->GetBinError(etaBin, ptBin);
  }else if(era == "2018"){
    scaleFactor = m_egamma_sf2018->GetBinContent(etaBin, ptBin);
    scaleFactor_Up = m_egamma_sf2018->GetBinContent(etaBin, ptBin) + m_egamma_sf2018->GetBinError(etaBin, ptBin);
    scaleFactor_Down = m_egamma_sf2018->GetBinContent(etaBin, ptBin) - m_egamma_sf2018->GetBinError(etaBin, ptBin);
  }


  egammaWeights.push_back(scaleFactor);
  egammaWeights.push_back(scaleFactor_Up);
  egammaWeights.push_back(scaleFactor_Down);
  return egammaWeights;

}

std::vector<double> egammaEffi::HLT_ScaleFactor(double ElectronEta, double ElectronPt, std::string era) {

  double scaleFactor = 1.0;
  double scaleFactor_Up = 0;
  double scaleFactor_Down = 0.;

  int etaBin = -1;
  int ptBin = -1;

  if(ElectronPt > 500){
    ElectronPt = 499;
  }

  std::vector<double> egammaWeights;

  if (abs(ElectronEta) <= 1.479){
    if(era == "2016"){
      etaBin = m_egamma__HLT_sf2016_barrel->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2016_barrel->GetYaxis()->FindBin(ElectronPt);
    }else if(era == "2017"){
      etaBin = m_egamma__HLT_sf2017_barrel->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2017_barrel->GetYaxis()->FindBin(ElectronPt);
    }else if(era == "2018"){
      etaBin = m_egamma__HLT_sf2018_barrel->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2018_barrel->GetYaxis()->FindBin(ElectronPt);
    }
  }else if (abs(ElectronEta) > 1.479){
    if(era == "2016"){
      etaBin = m_egamma__HLT_sf2016_endcap->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2016_endcap->GetYaxis()->FindBin(ElectronPt);
    }else if(era == "2017"){
      etaBin = m_egamma__HLT_sf2017_endcap->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2017_endcap->GetYaxis()->FindBin(ElectronPt);
    }else if(era == "2018"){
      etaBin = m_egamma__HLT_sf2018_endcap->GetXaxis()->FindBin(ElectronEta);
      ptBin = m_egamma__HLT_sf2018_endcap->GetYaxis()->FindBin(ElectronPt);
    }
  }


  if(etaBin == 0 || ptBin == 0) {
    std::cout << "egammaEffi::ScaleFactor : WARNING PT OR ETA IN UNDERFLOW : ETA : "<<etaBin<<" : PT : "<<ptBin << std::endl;
    egammaWeights.push_back(scaleFactor);
    egammaWeights.push_back(scaleFactor_Up);
    egammaWeights.push_back(scaleFactor_Down);
    return egammaWeights;
  }
  if (abs(ElectronEta) <= 1.479){
    if(etaBin == (m_nEtaBins_HLT_barrel+1) || ptBin == (m_nEtaBins_HLT_barrel+1) ){
      std::cout << "egammaEffi::ScaleFactor : WARNING PT OR ETA IN OVERFLOW  : ETA : "<<etaBin<<" : PT : "<<ptBin << std::endl;
      egammaWeights.push_back(scaleFactor);
      egammaWeights.push_back(scaleFactor_Up);
      egammaWeights.push_back(scaleFactor_Down);
      return egammaWeights;
    }
  }else if (abs(ElectronEta) > 1.479){
    if(etaBin == (m_nEtaBins_HLT_endcap+1) || ptBin == (m_nEtaBins_HLT_endcap+1) ){
      std::cout << "egammaEffi::ScaleFactor : WARNING PT OR ETA IN OVERFLOW  : ETA : "<<etaBin<<" : PT : "<<ptBin << std::endl;
      egammaWeights.push_back(scaleFactor);
      egammaWeights.push_back(scaleFactor_Up);
      egammaWeights.push_back(scaleFactor_Down);
      return egammaWeights;
    }
  }

  if (abs(ElectronEta) <= 1.479){
    if(era == "2016"){
      scaleFactor = m_egamma__HLT_sf2016_barrel->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2016_barrel->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2016_barrel->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2016_barrel->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2016_barrel->GetBinError(etaBin, ptBin);
    }else if(era == "2017"){
      scaleFactor = m_egamma__HLT_sf2017_barrel->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2017_barrel->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2017_barrel->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2017_barrel->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2017_barrel->GetBinError(etaBin, ptBin);
    }else if(era == "2018"){
      scaleFactor = m_egamma__HLT_sf2018_barrel->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2018_barrel->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2018_barrel->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2018_barrel->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2018_barrel->GetBinError(etaBin, ptBin);
    }
  }else if (abs(ElectronEta) > 1.479){
    if(era == "2016"){
      scaleFactor = m_egamma__HLT_sf2016_endcap->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2016_endcap->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2016_endcap->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2016_endcap->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2016_endcap->GetBinError(etaBin, ptBin);
    }else if(era == "2017"){
      scaleFactor = m_egamma__HLT_sf2017_endcap->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2017_endcap->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2017_endcap->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2017_endcap->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2017_endcap->GetBinError(etaBin, ptBin);
    }else if(era == "2018"){
      scaleFactor = m_egamma__HLT_sf2018_endcap->GetBinContent(etaBin, ptBin);
      scaleFactor_Up = m_egamma__HLT_sf2018_endcap->GetBinContent(etaBin, ptBin) + m_egamma__HLT_sf2018_endcap->GetBinError(etaBin, ptBin);
      scaleFactor_Down = m_egamma__HLT_sf2018_endcap->GetBinContent(etaBin, ptBin) - m_egamma__HLT_sf2018_endcap->GetBinError(etaBin, ptBin);
    }
  }


  egammaWeights.push_back(scaleFactor);
  egammaWeights.push_back(scaleFactor_Up);
  egammaWeights.push_back(scaleFactor_Down);
  return egammaWeights;

}
