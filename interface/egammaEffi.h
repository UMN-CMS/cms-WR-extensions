#ifndef EGAMMAEFF_h
#define EGAMMAEFF_h

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
#include "TH2D.h"
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class egammaEffi {
  public:
  egammaEffi();
  void Initialize(std::string era);
  std::vector<double> ScaleFactor(double ElectronEta, double ElectronPt, std::string era);
  std::vector<double> HLT_ScaleFactor(double ElectronEta, double ElectronPt, std::string era);



  private:
  TH2D* m_egamma_sf;
  TH2D* m_egamma_sf2017;
  TH2D* m_egamma_sf2018;

  TH2D* m_egamma__HLT_sf2016_barrel;
  TH2D* m_egamma__HLT_sf2016_endcap;
  TH2D* m_egamma__HLT_sf2017_barrel;
  TH2D* m_egamma__HLT_sf2017_endcap;
  TH2D* m_egamma__HLT_sf2018_barrel;
  TH2D* m_egamma__HLT_sf2018_endcap;

  int m_nEtaBins;
  int m_nPtBins;
  int m_nEtaBins_HLT_barrel;
  int m_nPtBins_HLT_barrel;
  int m_nEtaBins_HLT_endcap;
  int m_nPtBins_HLT_endcap;


};

#endif
