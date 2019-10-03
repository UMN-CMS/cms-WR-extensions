#ifndef Muons_h
#define Muons_h

//CMSSW CLASSES
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "ExoAnalysis/cmsWRextensions/interface/RoccoR.h"
#include "ExoAnalysis/cmsWRextensions/interface/GeneralizedEndpoint.h"
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
  std::vector<double> MuonTriggerWeight(double MuonPt, double MuonEta, std::string era, bool isSignal);
  std::vector<double> RochesterMethod_DataScale(const pat::Muon* Mu, std::string era);
  std::vector<double> RochesterMethod_MCSmear(const pat::Muon* Mu, std::string era);
  std::vector<double> GeneralizedEndpointMethod(const pat::Muon* Mu);
  std::vector<double> GeneralizedEndpointMethod_Data(const pat::Muon* Mu);

  double muidweight;
  double muidweightUp;
  double muidweightDown;

  double muTrigWeight;
  double muTrigWeightUp;
  double muTrigWeightDown;

  GeneralizedEndpoint myGeneralizedEndpoint;

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

  TH2D* Muon_Trig_BCDEF;
  TH2D* Muon_Trig_GH;
  TH2D* Muon_Trig_SF2016;

  TH1F* Muon_Trig_DataEff_BCDEF;
  TH1F* Muon_Trig_DataEff_GH;
  TH1F* Muon_Trig_DataEff2016;

  TH2D* Muon_Trig_SF2017;
  TH2D* Muon_Trig_SF2018;

  TH1F* Muon_Trig_DataEff2017;
  TH1F* Muon_Trig_DataEff2018;

  RoccoR rc2016;
  RoccoR rc2017;
  RoccoR rc2018;

};

#endif
