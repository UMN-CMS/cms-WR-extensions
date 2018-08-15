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
#include "TObjString.h"
//C++ CLASSES
#include <iostream>
//LOCAL CLASSES
#include "eventHistos.h"
#include "eventBits.h"
#include "tools.h"

class eventHistos {
  public:
  eventHistos();
  void book(TFileDirectory histoFolder, uint16_t flavor, std::string tag, bool FSB);
  void fill(eventBits& event);



  private:
  void fillGen(eventBits& event);
  void fillReco(eventBits& event);
  void fillWeight(eventBits& event);
  void fillCutProgress(eventBits& event);

 
  TFileDirectory m_histoFolder;
  uint32_t m_flavor;
  bool m_FSB;

  //METADATA
  TH1D* m_metaData;

  //GEN PLOTS
  TH1D* m_eventsWeight;
  TH1D* m_finalEventWeight;

  TH1D* m_FSBfinalEventWeight_E;
  TH1D* m_FSBfinalEventWeight_B;

  TH1D* m_nLeptons     ;
  TH1D* m_nMuons       ;
  TH1D* m_nTaus        ;
  TH1D* m_nElectrons   ;
  TH1D* m_nLightPartons;
  TH1D* m_nTops        ;
  TH1D* m_nBs          ;
  TH1D* m_nPartons     ;
  TH1D* m_nAdditionalHEEP;

  TH1D* m_eventFlavor       ;

  TH1D* m_cutProgress;
  TH1D* m_FSBcutProgress;

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
  TH1D* m_leadAK8JetElectronMass;

  TH1D* m_leadSubleadingJetsMuonsPt ;
  TH1D* m_leadSubleadingAK8JetsMuonsPt ;
  TH1D* m_leadSubleadingPartonsMuonsPt ;
  TH1D* m_leadAK8JetMuonPt;
  TH1D* m_leadAK8JetElectronPt;

  TH1D* m_leadSubleadingJetsMuonsEta ;
  TH1D* m_leadSubleadingAK8JetsMuonsEta ;
  TH1D* m_leadSubleadingPartonsMuonsEta ;
  TH1D* m_leadAK8JetMuonEta;
  TH1D* m_leadAK8JetElectronEta;

  TH1D* m_leadAK8JetMuonPhi;
  TH1D* m_leadAK8JetElectronPhi;

  TH1D* m_leadAK8JetMuonJetMuonEnergyFraction;
  TH1D* m_leadAK8JetElectronJetMuonEnergyFraction;

  TH1D* m_electronTrigger;
  TH1D* m_muonTrigger;
  //RECO PLOTS
  TH1D* m_selectedMuonPt; 
  TH1D* m_selectedMuonPhi;
  TH1D* m_selectedMuonEta;

  TH1D* m_selectedElectronPt; 
  TH1D* m_selectedElectronPhi;
  TH1D* m_selectedElectronEta;

  TH1D* m_selectedJetPt;
  TH1D* m_selectedJetPhi;
  TH1D* m_selectedJetEta;
  TH1D* m_selectedJetMass;
  TH1D* m_selectedJetTau21;

  TH1D* m_subleadMuonEt;
  TH1D* m_subleadMuonEta;
  TH1D* m_subleadMuonPhi;

  TH1D* m_EtPlacementMuon2;
  TH1D* m_nJets;
  TH1D* m_nAK8Jets;
  TH1D* m_nMuonCands;
  TH1D* m_nElectronCands;
  TH1D* m_nMuons10;
  TH1D* m_nAK8Jets40;

  TH1D* m_subleadMuon_selJetdPhi;
  TH1D* m_subleadMuon_selElectronPhi;
  TH1D* m_subleadMuon_selElectronMass;
  TH1D* m_subleadMuon_selElectronZMass;
  TH1D* m_subleadMuon_selElectronPt;
  TH1D* m_subleadMuon_selMuondPhi;
  TH1D* m_subleadMuon_selMuonMass;
  TH1D* m_subleadMuon_selMuonZMass;
  TH1D* m_subleadMuon_selMuonPt;

  TH1D* m_MET;
  TH1D* m_MET_selJetdPhi;
  TH1D* m_MET_selElectrondPhi;
  TH1D* m_MET_selMuondPhi;
  TH1D* m_MET_selJetMass;
  TH1D* m_MET_selElectronMass;
  TH1D* m_MET_selMuonMass;
  TH1D* m_MET_selJetPt;
  TH1D* m_MET_selElectronPt;
  TH1D* m_MET_selMuonPt;

  TH1D* m_selectedJetTransMET;

  TH1D* m_nVertices;

  TH1D* m_MuonWeight;
  TH1D* m_PUWeight;

  TH1D* m_HEEP_SF;
  TH1D* m_HEEP_SF_E;
  TH1D* m_HEEP_SF_B;
  TH1D* m_Electron_Reco_SF;

};

#endif
