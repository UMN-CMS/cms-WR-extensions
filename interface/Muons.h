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
  std::vector<double> MuonTriggerWeight(double MuonPt, double MuonEta, double MuonPt_2, double MuonEta_2, std::string era, bool isSignal, std::string analysis);
  std::vector<double> MuonTriggerWeight_ResFSB(double MuonPt, double MuonEta, std::string era, bool isSignal);
  std::vector<double> MuonRECOweight(double MuonP, double MuonEta, std::string era);
  std::vector<double> RochesterMethod_DataScale(const pat::Muon* Mu, std::string era);
  std::vector<double> RochesterMethod_MCSmear(const pat::Muon* Mu, std::string era);
  std::vector<double> GeneralizedEndpointMethod(const pat::Muon* Mu);
  std::vector<double> GeneralizedEndpointMethod_Data(const pat::Muon* Mu);

  double muidweight;
  double muidweightUp;
  double muidweightDown;

  double muTrigMCEff_1;
  double muTrigMCEffUp_1;
  double muTrigMCEffDown_1;

  double muTrigMCEff_2;
  double muTrigMCEffUp_2;
  double muTrigMCEffDown_2;

  double muTrigDATAEff_1;
  double muTrigDATAEffUp_1;
  double muTrigDATAEffDown_1;

  double muTrigDATAEff_2;
  double muTrigDATAEffUp_2;
  double muTrigDATAEffDown_2;

  double muTrigWeight;
  double muTrigWeightUp;
  double muTrigWeightDown;

  double muRecoWeight;
  double muRecoWeightUp;
  double muRecoWeightDown;

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

  TH2D* Muon_Trig_MCEff_BCDEF;
  TH2D* Muon_Trig_MCEff_GH;
  TH2D* Muon_Trig_MCEff2016;
  TH2D* Muon_Trig_MCEff2016Up;
  TH2D* Muon_Trig_MCEff2016Down;

  TH2D* Muon_Trig_DataEff_BCDEF;
  TH2D* Muon_Trig_DataEff_GH;
  TH2D* Muon_Trig_DataEff2016;
  TH2D* Muon_Trig_DataEff2016Up;
  TH2D* Muon_Trig_DataEff2016Down;

  TH2D* Muon_Trig_MCEff2017;
  TH2D* Muon_Trig_MCEff2018;
  TH2D* Muon_Trig_MCEff2017Up;
  TH2D* Muon_Trig_MCEff2018Up;
  TH2D* Muon_Trig_MCEff2017Down;
  TH2D* Muon_Trig_MCEff2018Down;

  TH2D* Muon_Trig_DataEff2017;
  TH2D* Muon_Trig_DataEff2018;
  TH2D* Muon_Trig_DataEff2017Up;
  TH2D* Muon_Trig_DataEff2018Up;
  TH2D* Muon_Trig_DataEff2017Down;
  TH2D* Muon_Trig_DataEff2018Down;

  RoccoR rc2016;
  RoccoR rc2017;
  RoccoR rc2018;

};

#endif
