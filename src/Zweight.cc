#include "ExoAnalysis/cmsWRextensions/interface/Zweight.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>

Zweight::Zweight() {

}

void Zweight::setup(const std::string& era) {
  std::cout << "INITIALIZING Z WEIGHT: " <<era<< std::endl;
  std::string Zcorr = "";

  if(era == "2016") {
    std::cout << "HERE 2016" << std::endl;
    Zcorr="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2016/Zpt_weights_2016.root";
  }
  if(era == "2017") {
    Zcorr="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2017/Zpt_weights_2017.root";
    std::cout << "HERE 2017" << std::endl;
  }
  if(era == "2018") {
    Zcorr="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/2018/Zpt_weights_2018.root";
    std::cout << "HERE 2018" << std::endl;
  }
  std::cout << "OPENING: " << Zcorr<< std::endl;

  TFile *lFile_Z = TFile::Open(Zcorr.c_str());
  m_Zweights = (TH2D*) lFile_Z->Get("zptmass_weights");

  std::cout << "OPENED" << std::endl;

  m_Zweights->SetDirectory(0);

  lFile_Z->Close();
  std::cout << "DONE" << std::endl;

}

double Zweight::getZweight(double zm, double zpt) {

  int xbin = m_Zweights->GetXaxis()->FindBin(zm);
  int ybin = m_Zweights->GetYaxis()->FindBin(zpt);
  if (xbin==0) xbin = 1;
  else if (xbin > m_Zweights->GetXaxis()->GetNbins()) xbin -= 1;
  if (ybin==0) ybin = 1;
  else if (ybin > m_Zweights->GetYaxis()->GetNbins()) ybin -= 1;

  double weight = m_Zweights->GetBinContent(xbin,ybin);

  return weight;
}
