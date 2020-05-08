#ifndef Zweight_h
#define Zweight_h

//CMSSW CLASSES
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
//ROOT CLASSES
#include "TH2D.h"
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "tools.h"

class Zweight {
  public:
  Zweight();
  
  void setup(const std::string& era);
  
  std::vector<double> getZweight(double zm, double zpt);

  private:
  TH2D* m_Zweights; 

};

#endif
