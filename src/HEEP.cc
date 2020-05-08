#include "ExoAnalysis/cmsWRextensions/interface/HEEP.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//C++ CLASSES
#include <iostream>
////LOCAL CLASSES
#include "ExoAnalysis/cmsWRextensions/interface/tools.h"




HEEP::HEEP () {


}

std::vector<double> HEEP::ScaleFactor(double ElectronEt, double ElectronEta, std::string era) {

  double scaleFactor = 1.0;
  double scaleFactor_Up = 0.;
  double scaleFactor_Down = 0.;

  std::cout << "ElectronEta: " << ElectronEta << std::endl;
  if(era == "2016"){
    if(abs(ElectronEta) < 1.4442){
      scaleFactor = 0.971;
      double error = sqrt(pow(0.001,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0022,.03),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }else{
      scaleFactor = 0.983;
      double error = sqrt(pow(0.001,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0143,.04),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }
  }else if(era == "2017"){
    if(abs(ElectronEta) < 1.4442){
      scaleFactor = 0.967;
      double error = sqrt(pow(0.001,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0022,.03),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }else{
      scaleFactor = 0.973;
      double error = sqrt(pow(0.002,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0143,.04),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }
  }else if(era == "2018"){
//Using 2017 for now
    if(abs(ElectronEta) < 1.4442){
      scaleFactor = 0.967;
      double error = sqrt(pow(0.001,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0022,.03),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }else{
      scaleFactor = 0.973;
      double error = sqrt(pow(0.002,2) + pow(TMath::Min(1.+(ElectronEt-90.)*0.0143,.04),2));
      scaleFactor_Up = scaleFactor + error;
      scaleFactor_Down = scaleFactor - error;
    }
  }

  std::vector<double> HEEPweights;
  HEEPweights.push_back(scaleFactor);
  HEEPweights.push_back(scaleFactor_Up);
  HEEPweights.push_back(scaleFactor_Down);

  return HEEPweights;
}

