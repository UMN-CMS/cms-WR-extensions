#ifndef HEEP_h
#define HEEP_h

//CMSSW CLASSES
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/JetReco/interface/GenJet.h"
//ROOT CLASSES
#include "TGraphAsymmErrors.h"
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class HEEP {
  public:
  HEEP();
  std::vector<double> ScaleFactor(double ElectronEt, double ElectronEta, std::string era);



  private:

};

#endif
