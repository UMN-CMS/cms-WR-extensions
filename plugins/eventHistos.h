#ifndef eventHistos_h
#define eventHistos_h

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
#include "TH1D.h"
#include "TH2D.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class eventHistos {
  public:
  eventHistos();
  void book(TFileDirectory histoFolder, uint16_t flavor);
  void fill(eventBits& event);



  private:
  void fillGen(eventBits& event);
  void fillReco(eventBits& event);

 
  TFileDirectory m_histoFolder;
  uint32_t m_flavor;

  //GEN PLOTS
  TH1D* m_eventsWeight;

  TH1D* m_parton1Et;
  TH1D* m_parton2Et;
  TH1D* m_muon1Et;
  TH1D* m_muon2Et;
  TH1D* m_firstPartonJetEt;
  TH1D* m_secondPartonJetEt;
  TH1D* m_firstPartonAK8JetEt;
  TH1D* m_secondPartonAK8JetEt;

  TH1D* m_parton1Eta;
  TH1D* m_parton2Eta;
  TH1D* m_muon1Eta;
  TH1D* m_muon2Eta;
  TH1D* m_firstPartonJetEta;
  TH1D* m_secondPartonJetEta;
  TH1D* m_firstPartonAK8JetEta;
  TH1D* m_secondPartonAK8JetEta;

  TH1D* m_parton1Phi;
  TH1D* m_parton2Phi;
  TH1D* m_muon1Phi;
  TH1D* m_muon2Phi;
  TH1D* m_firstPartonJetPhi;
  TH1D* m_secondPartonJetPhi;
  TH1D* m_firstPartonAK8JetPhi;
  TH1D* m_secondPartonAK8JetPhi;

  TH1D* m_dRparton1parton2;
  TH1D* m_dRmuon1muon2;
  TH1D* m_dRparton1muon2;
  TH1D* m_dRparton1muon1;
  TH1D* m_dRparton2muon2;
  TH1D* m_dRparton2muon1;
  TH1D* m_dRparton1jet;
  TH1D* m_dRparton2jet;
  TH1D* m_dRparton1AK8jet;
  TH1D* m_dRparton2AK8jet;

  TH1D* m_firstPartonJetEtHadronic;
  TH1D* m_secondPartonJetEtHadronic;
  TH1D* m_firstPartonJetEtEM;
  TH1D* m_secondPartonJetEtEM;
  TH1D* m_firstPartonJetEtInvisible;
  TH1D* m_secondPartonJetEtInvisible;

  TH1D* m_firstPartonAK8JetEtHadronic;
  TH1D* m_secondPartonAK8JetEtHadronic;
  TH1D* m_firstPartonAK8JetEtEM;
  TH1D* m_secondPartonAK8JetEtEM;
  TH1D* m_firstPartonAK8JetEtInvisible;
  TH1D* m_secondPartonAK8JetEtInvisible;

  TH1D* m_leadSubleadingJetsMuonsMass ;
  TH1D* m_leadSubleadingAK8JetsMuonsMass ;
  TH1D* m_leadSubleadingPartonsMuonsMass ;
  TH1D* m_leadAK8JetMuonMass;

  TH1D* m_leadSubleadingJetsMuonsPt ;
  TH1D* m_leadSubleadingAK8JetsMuonsPt ;
  TH1D* m_leadSubleadingPartonsMuonsPt ;
  TH1D* m_leadAK8JetMuonPt;

  TH1D* m_leadSubleadingJetsMuonsEta ;
  TH1D* m_leadSubleadingAK8JetsMuonsEta ;
  TH1D* m_leadSubleadingPartonsMuonsEta ;
  TH1D* m_leadAK8JetMuonEta;

  TH1D* m_leadAK8JetMuonPhi;

  TH1D* m_leadAK8JetMuonJetMuonEnergyFraction;
  //RECO PLOTS

  TH1D* m_EtPlacementMuon2;
  TH1D* m_nJets;
  TH1D* m_nAK8Jets;
  TH1D* m_nMuonCands;
  TH1D* m_nMuons40;
  TH1D* m_nAK8Jets40;

};

#endif
