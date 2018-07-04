#include "ExoAnalysis/cmsWRextensions/interface/HEEP.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"




HEEP::HEEP () {

  std::string iHEEP_SF="${CMSSW_BASE}/src/ExoAnalysis/cmsWRextensions/data/HEEP_SF.root";
  TFile *lFile = TFile::Open(iHEEP_SF.c_str());
  HEEPsf = (TGraphAsymmErrors*) lFile->Get("SF_Eta");
  lFile->Close();

}

void HEEP::Initialize() {

  for(int i=0; i < HEEPsf->GetN(); i++){
     HEEPsf->GetPoint(i, eta[i], sf[i]);

     sfEhigh[i] = HEEPsf->GetErrorYhigh(i);
     sfElow[i] = HEEPsf->GetErrorYlow(i);

     etaEhigh[i] = HEEPsf->GetErrorXhigh(i);
     etaElow[i] = HEEPsf->GetErrorXlow(i);

     std::cout << "eta: " << eta[i] << " +" << etaEhigh[i] << " -" << etaElow[i] << std::endl;
  }
}

double HEEP::ScaleFactor(double ElectronEta) {

  double scaleFactor = 0.;

  for(int i=0; i<HEEPsf->GetN(); i++){
     if((eta[i] - etaElow[i]) < ElectronEta && ElectronEta < (eta[i] + etaEhigh[i])){
	scaleFactor = sf[i];
	break;
     }
  }

  return scaleFactor;
}

