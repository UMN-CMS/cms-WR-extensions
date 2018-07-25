#ifndef Muons_h
#define Muons_h

//CMSSW CLASSES
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
//ROOT CLASSES
#include "TH2D.h"
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "tools.h"

class Muons {
  public:
  Muons();
  double MuonLooseIDweight(double MuonPt, double MuonEta);
  double MuonHighPTIDweight(double MuonPt, double MuonEta);

  double muidweight;
  double muidweightUp;
  double muidweightDown;

  private:
/*  void fillGen(eventBits& event);
  void fillReco(eventBits& event);
  void fillWeight(eventBits& event);
  void fillCutProgress(eventBits& event);*/

  TH2D* Muon_LooseID_BCDEF;
  TH2D* Muon_LooseID_GH;
  TH2D* Muon_LooseID_eff;
  TH2D* Muon_HighPT_BCDEF;
  TH2D* Muon_HighPT_GH;
  TH2D* Muon_HighPT_eff;

};

#endif
