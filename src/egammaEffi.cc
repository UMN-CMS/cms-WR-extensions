#include "ExoAnalysis/cmsWRextensions/interface/egammaEffi.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"




egammaEffi::egammaEffi () {

  std::string iegammaEffi_SF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/egammaEffi.txt_EGM2D.root";
  TFile *lFile = TFile::Open(iegammaEffi_SF.c_str());
  m_egamma_sf = (TH2D*) lFile->Get("EGamma_SF2D");
  lFile->Close();

}
void egammaEffi::Initialize() {
  
  m_nEtaBins = m_egamma_sf->GetXaxis()->GetNbins();
  m_nPtBins  = m_egamma_sf->GetYaxis()->GetNbins();
  
}

std::vector<double> egammaEffi::ScaleFactor(double ElectronEta, double ElectronPt) {

  double scaleFactor = 0.;
  double scaleFactor_Up = 0;
  double scaleFactor_Down = 0.;

  int etaBin = -1;
  int ptBin = -1;

  if(ElectronPt > 500){
    ElectronPt = 499;
  }

  std::vector<double> egammaWeights;

  etaBin = m_egamma_sf->GetXaxis()->FindBin(ElectronEta);
  ptBin = m_egamma_sf->GetYaxis()->FindBin(ElectronPt);

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

  scaleFactor = m_egamma_sf->GetBinContent(etaBin, ptBin);
  scaleFactor_Up = m_egamma_sf->GetBinContent(etaBin, ptBin) + m_egamma_sf->GetBinError(etaBin, ptBin);
  scaleFactor_Down = m_egamma_sf->GetBinContent(etaBin, ptBin) - m_egamma_sf->GetBinError(etaBin, ptBin);


  egammaWeights.push_back(scaleFactor);
  egammaWeights.push_back(scaleFactor_Up);
  egammaWeights.push_back(scaleFactor_Down);
  return egammaWeights;

}

