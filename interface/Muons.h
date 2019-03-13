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
  std::vector<double> MuonLooseIDweight(double MuonPt, double MuonEta, std::string era);
  std::vector<double> MuonHighPTIDweight(double MuonPt, double MuonEta, std::string era);
  std::vector<double> MuonLooseTkIso(double MuonPt, double MuonEta, std::string era);
  std::vector<double> MuonTriggerWeight(double MuonPt, std::string era);

  double muidweight;
  double muidweightUp;
  double muidweightDown;

  double muTrigWeight;
  double muTrigWeightUp;
  double muTrigWeightDown;

  private:
/*  void fillGen(eventBits& event);
  void fillReco(eventBits& event);
  void fillWeight(eventBits& event);
  void fillCutProgress(eventBits& event);*/

  TH2D* Muon_LooseID_BCDEF;
  TH2D* Muon_LooseID_GH;
  TH2D* Muon_LooseID_eff2016;
  TH2D* Muon_HighPT_BCDEF;
  TH2D* Muon_HighPT_GH;
  TH2D* Muon_HighPT_eff2016;

  TH2D* Muon_LooseID_BtoF2017;
  TH2D* Muon_HighPT_BtoF2017;
  TH2D* Muon_LooseID_AtoD2018;
  TH2D* Muon_HighPT_AtoD2018;

  TH2D* Muon_HighPT_ISO_BCDEF;
  TH2D* Muon_HighPT_ISO_GH;
  TH2D* Muon_HighPT_ISO_eff2016;

  TH2D* Muon_HighPT_ISO_BtoF2017;
  TH2D* Muon_HighPT_ISO_AtoD2018;

  TH1F* Muon_Trig_BCDEF;
  TH1F* Muon_Trig_GH;
  TH1F* Muon_Trig_SF2016;

  TH1F* Muon_Trig_SF2017;
  TH1F* Muon_Trig_SF2018;
};

#endif
